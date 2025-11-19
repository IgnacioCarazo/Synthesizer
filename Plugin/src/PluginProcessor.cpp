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
    for (int i = 0; i < 8; ++i) // 8 voces = polifonía estándar
        synth.addVoice(new SynthVoice());

    synth.clearSounds();
    synth.addSound(new SynthSound());
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

//==============================================================================
// PREPARACIÓN DE AUDIO
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);
    synth.setCurrentPlaybackSampleRate(sampleRate);

    // Propagar sampleRate a voces si es necesario (las voces en su constructor pueden usar este valor)
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto *v = dynamic_cast<SynthVoice *>(synth.getVoice(i)))
        {
            // Si en el futuro hace falta pasar sampleRate explícitamente se haría aquí
            v->prepare(sampleRate);
        }
    }
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

    // Oscillator / UI
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "WAVE", "Wave Type",
        juce::StringArray{"Sine", "Saw", "Square", "Triangle"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "VELOCITY", "Velocity", 0.0f, 1.0f, 1.0f));

    // ADSR parameters (attack, decay in seconds; sustain 0..1; release seconds)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ATTACK", "Attack", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f), 0.01f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "DECAY", "Decay", juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f), 0.10f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "SUSTAIN", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f), 0.80f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "RELEASE", "Release", juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f), 0.20f));
    // Filtro: tipo y cutoff
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "FILTER_TYPE", "Filter Type",
        juce::StringArray{"Lowpass", "Highpass", "Bandpass"}, 0));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "FILTER_CUTOFF", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.5f), 1000.0f));
    return {params.begin(), params.end()};
}

//==============================================================================
// PROPAGACIÓN DE PARÁMETROS A LAS VOCES
void AudioPluginAudioProcessor::updateVoicesParameters()
{
    // Leer parámetros de oscilador
    int waveIndex = static_cast<int>(*apvts.getRawParameterValue("WAVE"));
    float velocity = *apvts.getRawParameterValue("VELOCITY");

    // Leer parámetros ADSR
    float attack = *apvts.getRawParameterValue("ATTACK");
    float decay = *apvts.getRawParameterValue("DECAY");
    float sustain = *apvts.getRawParameterValue("SUSTAIN");
    float release = *apvts.getRawParameterValue("RELEASE");

    // Leer parámetros del filtro
    int filterTypeIndex = static_cast<int>(*apvts.getRawParameterValue("FILTER_TYPE"));
    juce::String filterTypeStr = filterTypeIndex == 0 ? "lowpass" : filterTypeIndex == 1 ? "highpass"
                                                                                         : "bandpass";
    float filterCutoff = *apvts.getRawParameterValue("FILTER_CUTOFF");

    // Propagar parámetros a todas las voces activas
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice *>(synth.getVoice(i)))
        {
            // Oscillator
            voice->setWaveType(waveIndex);
            voice->setAmplitude(velocity);

            // ADSR
            voice->setEnvelopeParameters(attack, decay, sustain, release);

            // Filtro
            voice->setFilterParameters(filterCutoff, filterTypeStr);
        }
    }
}