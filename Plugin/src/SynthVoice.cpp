#include "SynthVoice.h"

// Constructor
SynthVoice::SynthVoice()
{
    oscillator.setSampleRate(44100.0); // default, luego se ajusta en prepareToPlay
    env.setSampleRate(44100.0);
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
    // Oscillator setup
    currentVelocity = velocity;
    oscillator.setNote(midiNoteNumber);       // actualiza frecuencia
    oscillator.setAmplitude(currentVelocity); // actualiza volumen
    oscillator.setWaveType(currentWaveIndex); // asegura onda correcta
    isNoteActive = true;

    // Start envelope
    env.enterAttack();
    isNoteActive = true;
}

// Nota desactivada
void SynthVoice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
{
    env.enterRelease();
}

void SynthVoice::setEnvelopeParameters(float a, float d, float s, float r)
{
    env.setParameters(a, d, s, r);
}

// Pitch wheel (no usado)
void SynthVoice::pitchWheelMoved(int /*newPitchWheelValue*/) {}

// Controladores (no usados)
void SynthVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/) {}

// Renderiza el bloque de audio
void SynthVoice::renderNextBlock(juce::AudioBuffer<float> &outputBuffer,
                                 int startSample, int numSamples)
{
    if (!isNoteActive)
        return;

    oscillator.setWaveType(currentWaveIndex);
    oscillator.setAmplitude(currentVelocity);

    for (int sample = 0; sample < numSamples; ++sample)
    {
        float oscValue = oscillator.getNextSample();
        float envValue = env.processSample();
        float finalValue = oscValue * envValue;

        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            outputBuffer.addSample(channel, startSample + sample, finalValue);
    }

    // If the envelope has finished, the voice can be cleared.
    if (!env.isActive())
        clearCurrentNote();
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
