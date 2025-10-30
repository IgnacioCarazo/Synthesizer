#include "Square.h"
#include <cmath>
#include <algorithm>

/**
 * @brief Constructor that initializes the square oscillator.
 * @param sampleRate The system sample rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Initializes the frequency banks and calls generateWavetableBanks()
 * to compute the band-limited square waveforms.
 */
Square::Square(double sampleRate, float velocity)
    : WavetableOscillator({}, {}), banks({20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125,
                                          140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558,
                                          627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187,
                                          2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720,
                                          8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000})
{
    name = "Square";
    generateWavetableBanks(sampleRate, velocity);
}

/**
 * @brief Generates the square wave wavetables with harmonic band-limiting.
 * @param fs Sampling rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Each table is built using a Fourier series approximation of a square wave:
 *
 * \f[
 * x(t) = \sum_{k=1,3,5...}^{N} \frac{1}{k} \sin(2 \pi k t)
 * \f]
 *
 * The number of harmonics included per bank depends on the fundamental
 * frequency to ensure that no partials exceed the Nyquist frequency.
 *
 * The resulting waveform is normalized to unity amplitude and scaled by
 * the velocity parameter before being stored.
 */
void Square::generateWavetableBanks(double fs, float velocity)
{
    waveTables.clear();
    waveTables.reserve(banks.size());

    const int desiredNumberOfHarmonics = 128;
    double fNyquist = fs / 2.0;

    for (auto bankFreq : banks)
    {
        int Ns = static_cast<int>(std::round(fs / bankFreq));
        std::vector<float> waveform(Ns, 0.0f);
        // Compute maximum number of odd harmonics allowed below Nyquist
        int numHarmonics = (bankFreq > 10000.0) ? 32 : desiredNumberOfHarmonics;
        int maxHarmonics = static_cast<int>(0.9 * fNyquist / bankFreq);
        numHarmonics = std::min(numHarmonics, maxHarmonics);

        // Build the waveform using only odd harmonics
        for (int k = 1; k <= 2 * numHarmonics; k += 2)
        {
            float amplitude = 1.0f / k;
            for (int n = 0; n < Ns; ++n)
                waveform[n] += amplitude * std::sin(2.0 * M_PI * k * n / Ns);
        }

        // normalize
        float maxVal = *std::max_element(waveform.begin(), waveform.end(),
                                         [](float a, float b)
                                         { return std::abs(a) < std::abs(b); });
        for (auto &v : waveform)
            v = velocity * v / maxVal;

        waveTables.push_back(waveform);
    }

    bankFreqs = banks;
}
