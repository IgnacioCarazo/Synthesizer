#pragma once
#include "Oscillator.h"

/**
 * @class Square
 * @brief Implements a band-limited square wave oscillator using wavetable synthesis.
 *
 * The Square class generates multiple wavetable banks, each containing a
 * band-limited single-cycle square waveform. Each bank is optimized for a
 * particular frequency range, limiting the number of harmonics included
 * to prevent aliasing above the Nyquist frequency.
 *
 * Inherits from WavetableOscillator, which provides phase management,
 * table interpolation, and playback.
 */
class Square : public WavetableOscillator
{
public:
    /**
     * @brief Constructs a square oscillator and generates its wavetable banks.
     * @param sampleRate The system sample rate in Hz.
     * @param velocity Output amplitude scaling factor (default = 1.0f).
     *
     * Initializes the oscillator with a predefined set of frequency banks and
     * precomputes the band-limited waveforms for each one.
     */
    Square(double sampleRate, float velocity = 1.0f);

private:
    /**
     * @brief Generates the band-limited square wavetable banks.
     * @param fs Sampling rate in Hz.
     * @param velocity Amplitude scaling factor.
     *
     * For each frequency bank, constructs a single-cycle square waveform by
     * summing odd harmonics of a sine series up to the Nyquist limit. Each
     * waveform is normalized and scaled by the velocity parameter.
     */
    void generateWavetableBanks(double fs, float velocity);

    /** @brief Center frequencies (in Hz) defining each wavetable bank. */
    std::vector<double> banks;
};
