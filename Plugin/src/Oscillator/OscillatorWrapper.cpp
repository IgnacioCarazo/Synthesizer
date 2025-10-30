#include "OscillatorWrapper.h"
#include <cmath>

/**
 * @brief Constructs the OscillatorWrapper and initializes all waveform oscillators.
 *
 * Initializes Sine, Saw, Square, and Triangle oscillators and sets the default
 * active waveform to Sine.
 */
OscillatorWrapper::OscillatorWrapper()
{
    oscillators[0] = std::make_unique<Sine>(sampleRate, 1.0f);
    oscillators[1] = std::make_unique<Saw>(sampleRate, 1.0f);
    oscillators[2] = std::make_unique<Square>(sampleRate, 1.0f);
    oscillators[3] = std::make_unique<Triangle>(sampleRate, 1.0f);

    currentWaveIndex = 0;
    activeOsc = oscillators[currentWaveIndex].get();
}

/** @brief Sets the sample rate for all oscillators and updates frequency. */
void OscillatorWrapper::setSampleRate(double fs)
{
    sampleRate = fs;
    updateFrequency();
}

/** @brief Sets the MIDI note and updates oscillator frequency accordingly. */
void OscillatorWrapper::setNote(int midiNoteIn)
{
    midiNote = midiNoteIn;
    updateFrequency();
}

/** @brief Sets the amplitude of the active oscillator. */
void OscillatorWrapper::setAmplitude(float amp)
{
    amplitude = amp;
    if (activeOsc)
        activeOsc->setAmplitude(amplitude);
}

/**
 * @brief Switches the active waveform.
 * @param waveIndex Index of the desired waveform.
 *
 * Updates amplitude and frequency of the newly selected oscillator.
 */
void OscillatorWrapper::setWaveType(int waveIndex)
{
    if (waveIndex != currentWaveIndex && waveIndex >= 0 && waveIndex < (int)oscillators.size())
    {
        currentWaveIndex = waveIndex;
        activeOsc = oscillators[currentWaveIndex].get();
        activeOsc->setAmplitude(amplitude);
        activeOsc->setFrequency(frequency, sampleRate);
    }
}

/** @brief Updates the frequency of all oscillators based on the current MIDI note. */
void OscillatorWrapper::updateFrequency()
{
    frequency = 440.0 * pow(2.0, (midiNote - 69) / 12.0);
    for (auto &osc : oscillators)
        if (osc)
            osc->setFrequency(frequency, sampleRate);
}

/** @brief Returns the next sample from the currently active oscillator. */
float OscillatorWrapper::getNextSample()
{
    return activeOsc ? activeOsc->getNextSample() : 0.0f;
}
