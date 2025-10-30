#pragma once
#include "Oscillator.h"

/**
 * @class Saw
 * @brief Implements a band-limited sawtooth wave oscillator using wavetable synthesis.
 *
 * This class generates multiple wavetable banks at different frequency ranges
 * to minimize aliasing and ensure spectral accuracy across the full frequency spectrum.
 * Each wavetable bank contains a precomputed, band-limited sawtooth waveform.
 *
 * The oscillator inherits from WavetableOscillator, which handles sample
 * interpolation, phase accumulation, and waveform lookup.
 */
class Saw : public WavetableOscillator
{
public:
    /**
     * @brief Constructs a Saw oscillator and generates its wavetable banks.
     * @param sampleRate The system sample rate in Hz.
     * @param velocity Output amplitude scaling factor (default = 1.0f).
     *
     * The constructor initializes the base class and precomputes
     * the harmonic-limited wavetables for multiple frequency bands.
     */
    Saw(double sampleRate, float velocity = 1.0f);

private:
    /**
     * @brief Generates all band-limited wavetable banks for the sawtooth waveform.
     * @param fs Sampling frequency in Hz.
     * @param velocity Amplitude scaling factor.
     *
     * For each frequency bank, this function calculates the sawtooth waveform
     * using an additive synthesis approach (sum of harmonics with amplitude 1/k).
     * The number of harmonics is limited by the Nyquist frequency to prevent aliasing.
     */
    void generateWavetableBanks(double fs, float velocity);

    /** @brief Center frequencies (in Hz) defining each wavetable bank. */
    std::vector<double> banks;
};
