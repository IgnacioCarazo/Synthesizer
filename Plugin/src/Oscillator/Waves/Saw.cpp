#include "Saw.h"
#include <cmath>
#include <algorithm>

/**
 * @brief Constructor that initializes the sawtooth oscillator.
 * @param sampleRate The system sample rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Initializes the list of frequency banks and calls
 * generateWavetableBanks() to precompute the band-limited tables.
 */
Saw::Saw(double sampleRate, float velocity)
    : WavetableOscillator({}, {}), banks({20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125,
                                          140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558,
                                          627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187,
                                          2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720,
                                          8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000})
{
    name = "Saw";
    generateWavetableBanks(sampleRate, velocity);
}

/**
 * @brief Generates the full set of band-limited wavetable banks.
 * @param fs Sampling rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Each table corresponds to a specific frequency range and contains
 * a normalized sawtooth waveform computed by summing harmonics.
 * The harmonic count is adjusted based on the bank frequency and
 * Nyquist limit to avoid aliasing artifacts.
 */
void Saw::generateWavetableBanks(double fs, float velocity)
{
    waveTables.clear();
    std::vector<std::vector<float>> tables;
    tables.reserve(banks.size());

    const int desiredNumberOfHarmonics = 128;
    double fNyquist = fs / 2.0;

    for (auto bankFreq : banks)
    {
        int Ns = static_cast<int>(std::round(fs / bankFreq));
        std::vector<float> waveform(Ns, 0.0f);

        int numHarmonics = desiredNumberOfHarmonics;
        if (bankFreq > 10000.0)
            numHarmonics = 32;

        int maxHarmonics = static_cast<int>(0.9 * fNyquist / bankFreq);
        numHarmonics = std::min(numHarmonics, maxHarmonics);
        // Additive synthesis: sum of harmonics with amplitude 1/k
        for (int k = 1; k <= numHarmonics; ++k)
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

        tables.push_back(waveform);
    }

    waveTables = tables;
    bankFreqs = banks;
}
