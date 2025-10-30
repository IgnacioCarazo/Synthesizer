#include "Triangle.h"
#include <cmath>
#include <algorithm>

/**
 * @brief Constructs a Triangle oscillator and initializes its wavetable banks.
 * @param sampleRate Sampling rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Populates the internal list of frequency banks and generates a series of
 * band-limited triangle waveforms for efficient playback.
 */
Triangle::Triangle(double sampleRate, float velocity)
    : WavetableOscillator({}, {}), banks({20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125,
                                          140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558,
                                          627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187,
                                          2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720,
                                          8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000})
{
    name = "Triangle";
    generateWavetableBanks(sampleRate, velocity);
}

/**
 * @brief Builds a series of band-limited triangle waveforms.
 * @param fs Sampling rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Each waveform is constructed using a truncated Fourier series containing
 * only odd harmonics, following:
 *
 * \f[
 * x(t) = \sum_{n=0}^{N} (-1)^n \frac{1}{(2n+1)^2} \sin(2 \pi (2n+1) t)
 * \f]
 *
 * The number of harmonics is limited by both a fixed maximum and the
 * Nyquist constraint. After generation, each waveform is normalized to
 * unity and scaled by the velocity factor.
 */
void Triangle::generateWavetableBanks(double fs, float velocity)
{
    waveTables.clear();
    waveTables.reserve(banks.size());

    const int desiredNumberOfHarmonics = 128;
    double fNyquist = fs / 2.0;

    for (auto bankFreq : banks)
    {
        int Ns = static_cast<int>(std::round(fs / bankFreq));
        std::vector<float> waveform(Ns, 0.0f);

        int numHarmonics = (bankFreq > 10000.0) ? 32 : desiredNumberOfHarmonics;
        int maxHarmonics = static_cast<int>(0.9 * fNyquist / bankFreq);
        numHarmonics = std::min(numHarmonics, maxHarmonics);

        // odd harmonics with alternating sign and 1/n^2 amplitude
        for (int n = 0; n < numHarmonics; ++n)
        {
            int k = 2 * n + 1;
            float amplitude = std::pow(-1.0f, n) / (k * k);
            for (int i = 0; i < Ns; ++i)
                waveform[i] += amplitude * std::sin(2.0 * M_PI * k * i / Ns);
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
