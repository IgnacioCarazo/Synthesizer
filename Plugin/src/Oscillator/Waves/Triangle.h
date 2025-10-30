#pragma once
#include "Oscillator.h"

/**
 * @class Triangle
 * @brief Implements a band-limited triangle wave oscillator using wavetable synthesis.
 *
 * The Triangle class generates multiple precomputed wavetable banks containing
 * single-cycle, band-limited triangle waveforms. Each bank corresponds to a
 * different base frequency range and restricts its harmonic content to avoid
 * aliasing above the Nyquist frequency.
 *
 * The class extends WavetableOscillator, which handles phase accumulation,
 * table interpolation, and signal output.
 */
class Triangle : public WavetableOscillator
{
public:
    /**
     * @brief Constructs a triangle oscillator and generates its wavetable banks.
     * @param sampleRate Sampling rate in Hz.
     * @param velocity Output amplitude scaling factor (default = 1.0f).
     *
     * Initializes the frequency bank centers and builds the triangle waveforms
     * by summing odd harmonics with amplitude and sign decay.
     */
    Triangle(double sampleRate, float velocity = 1.0f);

private:
    /**
     * @brief Generates the band-limited triangle wavetable banks.
     * @param fs Sampling rate in Hz.
     * @param velocity Amplitude scaling factor.
     *
     * Each wavetable is computed as a Fourier series with odd harmonics whose
     * amplitudes decay as 1/n² and alternate in sign. The number of harmonics
     * included depends on the bank’s fundamental frequency to maintain band-limiting.
     */
    void generateWavetableBanks(double fs, float velocity);

    /** @brief Center frequencies (in Hz) defining each wavetable bank. */
    std::vector<double> banks;
};
