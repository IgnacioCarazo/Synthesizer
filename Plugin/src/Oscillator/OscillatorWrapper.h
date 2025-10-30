#pragma once
#include "Oscillator.h"
#include "Waves/Sine.h"
#include "Waves/Saw.h"
#include "Waves/Square.h"
#include "Waves/Triangle.h"
#include <array>
#include <memory>

/**
 * @class OscillatorWrapper
 * @brief High-level wrapper managing multiple oscillator types in a single interface.
 *
 * Provides a unified interface to switch between Sine, Saw, Square, and Triangle
 * waveforms while maintaining frequency, amplitude, and sample rate consistency.
 *
 * Handles MIDI note input and converts it to frequency for all internal oscillators.
 */
class OscillatorWrapper
{
public:
    /** @brief Constructs the wrapper and initializes all oscillator instances. */
    OscillatorWrapper();

    /** @brief Sets the current MIDI note and updates oscillator frequency. */
    void setNote(int midiNote);

    /** @brief Sets the system sample rate (Hz) and updates oscillators. */
    void setSampleRate(double fs);

    /** @brief Sets the output amplitude (linear scaling). */
    void setAmplitude(float amp);

    /**
     * @brief Selects the active waveform.
     * @param waveIndex Index of the waveform: 0 = Sine, 1 = Saw, 2 = Square, 3 = Triangle.
     */
    void setWaveType(int waveIndex);

    /** @brief Retrieves the next sample from the currently active oscillator. */
    float getNextSample();

private:
    std::array<std::unique_ptr<OscillatorBase>, 4> oscillators; /**< Internal oscillators: Sine, Saw, Square, Triangle. */
    OscillatorBase *activeOsc = nullptr;                        /**< Pointer to currently active oscillator. */

    int currentWaveIndex = 0;    /**< Currently selected waveform index. */
    int midiNote = 69;           /**< Current MIDI note number. */
    double sampleRate = 44100.0; /**< Sample rate in Hz. */
    double frequency = 440.0;    /**< Frequency corresponding to MIDI note. */
    float amplitude = 1.0f;      /**< Output amplitude. */

    /** @brief Updates frequency based on current MIDI note and sample rate. */
    void updateFrequency();
};
