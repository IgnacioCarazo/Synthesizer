#include "SynthVoice.h"
#include "Filter/RCFilter.h"

// Constructor
SynthVoice::SynthVoice()
{
    oscillator.setSampleRate(44100.0); // default, luego se ajusta en prepareToPlay
    env.setSampleRate(44100.0);
    voiceFilter.setSampleRate(44100.0); // inicializa el filtro con sample rate
}

// Verifica si puede reproducir un sonido
bool SynthVoice::canPlaySound(juce::SynthesiserSound *sound)
{
    return dynamic_cast<juce::SynthesiserSound *>(sound) != nullptr;
}

// Nota activada
void SynthVoice::startNote(int midiNoteNumber, float velocity,
                           juce::SynthesiserSound * /*sound*/, int /*currentPitchWheelPosition*/)
{
    currentVelocity = velocity;

    oscillator.setNote(midiNoteNumber);
    oscillator.setAmplitude(currentVelocity);
    oscillator.setWaveType(currentWaveIndex);

    // Reset de envelope y filtro para la nueva voz (requerido para polifonía)
    env.enterAttack();
    voiceFilter.reset();

    isNoteActive = true;
}

// Nota desactivada
void SynthVoice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
{
    // Para polifonía nunca apagamos la voz inmediatamente
    // Iniciamos la fase de Release del envelope
    env.enterRelease();
}

// Actualiza parámetros de ADSR
void SynthVoice::setEnvelopeParameters(float a, float d, float s, float r)
{
    env.setParameters(a, d, s, r);
}

// Actualiza parámetros de filtro
void SynthVoice::setFilterParameters(float cutoff, const juce::String &type)
{
    RCFilterType filterType = RCFilterType::Lowpass; // default
    if (type == "lowpass")
        filterType = RCFilterType::Lowpass;
    else if (type == "highpass")
        filterType = RCFilterType::Highpass;
    else if (type == "bandpass")
        filterType = RCFilterType::Bandpass;

    voiceFilter.setParameters(filterType, cutoff);
}

// Pitch wheel (no usado)
void SynthVoice::pitchWheelMoved(int /*newPitchWheelValue*/) {}

// Controladores (no usados)
void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) {}

// Renderiza el bloque de audio
void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                 int startSample, int numSamples)
{
    // Si el envelope ya terminó, esta voz debe apagarse
    if (!env.isActive())
    {
        clearCurrentNote();
        return;
    }

    oscillator.setWaveType(currentWaveIndex);
    oscillator.setAmplitude(currentVelocity);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float oscValue = oscillator.getNextSample();
        float envValue = env.processSample();
        float finalValue = oscValue * envValue;

        // Aplicar filtro al sample generado
        finalValue = voiceFilter.processSample(finalValue);

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addSample(channel, startSample + sample, finalValue);
    }
}

// Cambia el tipo de onda desde GUI
void SynthVoice::setWaveType(int waveIndex)
{
    currentWaveIndex = waveIndex;
    oscillator.setWaveType(currentWaveIndex);
}

// Cambia amplitud desde GUI o AudioProcessor
void SynthVoice::setAmplitude(float amp)
{
    currentVelocity = amp;
    oscillator.setAmplitude(currentVelocity);
}

void SynthVoice::prepare(double sampleRate)
{
    oscillator.setSampleRate(sampleRate);
    env.setSampleRate(sampleRate);
    voiceFilter.setSampleRate(sampleRate);
}