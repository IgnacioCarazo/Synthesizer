#pragma once
#include "Oscillator.h"

class Sine : public WavetableOscillator
{
public:
    /**
     * @brief Constructs a sine oscillator and generates its wavetable banks.
     * @param sampleRate The system sample rate in Hz.
     * @param velocity Output amplitude scaling factor (default = 1.0f).
     *
     * Initializes the oscillator with a list of frequency banks and precomputes
     * the corresponding single-cycle sine waveforms for each band.
     */
    Sine(double sampleRate, float velocity = 1.0f);

private:
    /**
     * @brief Generates all wavetable banks for the sine waveform.
     * @param fs Sampling rate in Hz.
     * @param velocity Amplitude scaling factor.
     *
     * Creates one-cycle sine waves for each bank frequency, normalizes them,
     * and scales the amplitude based on the input velocity.
     */
    void generateWavetableBanks(double fs, float velocity);

    /** @brief Center frequencies (in Hz) defining each wavetable bank. */
    std::vector<double> banks;
};
