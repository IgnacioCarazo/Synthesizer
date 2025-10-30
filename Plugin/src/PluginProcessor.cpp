#include "PluginProcessor.h"
#include "GUI.h"

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int /*midiNoteNumber*/) override { return true; }
    bool appliesToChannel(int /*midiChannel*/) override { return true; }
};

//==============================================================================
// CONSTRUCTOR
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
                         ),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
    synth.clearVoices();
    synth.addVoice(new SynthVoice());

    synth.clearSounds();
    synth.addSound(new SynthSound());
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

//==============================================================================
// PREPARACIÓN DE AUDIO
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

//==============================================================================
// LIBERAR RECURSOS
void AudioPluginAudioProcessor::releaseResources()
{
    // Aquí se pueden limpiar buffers si se desea
}

//==============================================================================
// PROCESAMIENTO DE AUDIO Y MIDI
void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float> &buffer,
                                             juce::MidiBuffer &midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    juce::MidiBuffer combinedMidi;
    combinedMidi.addEvents(midiMessages, 0, buffer.getNumSamples(), 0);
    keyboardState.processNextMidiBuffer(combinedMidi, 0, buffer.getNumSamples(), true);

    updateVoicesParameters(); // aplica cambios de GUI en tiempo real
    synth.renderNextBlock(buffer, combinedMidi, 0, buffer.getNumSamples());
}

//==============================================================================
// OTROS MÉTODOS MANTENIDOS
bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout &layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono() &&
        layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

//==============================================================================
// GUI
bool AudioPluginAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor *AudioPluginAudioProcessor::createEditor()
{
    return new GUI(*this);
}

//==============================================================================
// MÉTODOS REQUERIDOS POR JUCE
int AudioPluginAudioProcessor::getNumPrograms() { return 1; }
int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram(int) {}
const juce::String AudioPluginAudioProcessor::getProgramName(int) { return {}; }
void AudioPluginAudioProcessor::changeProgramName(int, const juce::String &) {}

const juce::String AudioPluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}
bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}
bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
// STATE MGMT
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock &destData)
{
    juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void *data, int sizeInBytes)
{
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// FACTORY
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

//==============================================================================
// PARAMETER SETUP
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "WAVE", "Wave Type",
        juce::StringArray{"Sine", "Saw", "Square", "Triangle"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "VELOCITY", "Velocity", 0.0f, 1.0f, 1.0f));

    return {params.begin(), params.end()};
}

void AudioPluginAudioProcessor::updateVoicesParameters()
{
    int waveIndex = static_cast<int>(*apvts.getRawParameterValue("WAVE"));
    float velocity = *apvts.getRawParameterValue("VELOCITY");

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice *>(synth.getVoice(i)))
        {
            voice->setWaveType(waveIndex);
            voice->setAmplitude(velocity);
        }
    }
}
