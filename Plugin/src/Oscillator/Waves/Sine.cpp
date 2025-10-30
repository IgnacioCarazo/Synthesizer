#include "Sine.h"
#include <algorithm>
#include <cmath>

/**
 * @brief Constructor that initializes the sine oscillator.
 * @param sampleRate The system sample rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Initializes the list of frequency banks and calls
 * generateWavetableBanks() to precompute the single-cycle sine waveforms.
 */
Sine::Sine(double sampleRate, float velocity)
    : WavetableOscillator({}, {}), banks({20, 25, 30, 35, 40, 45, 50, 56, 63, 70, 79, 88, 99, 111, 125,
                                          140, 157, 176, 198, 222, 249, 279, 313, 352, 395, 443, 497, 558,
                                          627, 704, 790, 886, 992, 1111, 1244, 1393, 1560, 1747, 1955, 2187,
                                          2450, 2740, 3070, 3440, 3860, 4330, 4860, 5450, 6120, 6870, 7720,
                                          8680, 9760, 10960, 12320, 13840, 15530, 17440, 19550, 20000})
{
    name = "Sine";
    generateWavetableBanks(sampleRate, velocity);
}

/**
 * @brief Generates all single-cycle sine wavetables for each frequency bank.
 * @param fs Sampling rate in Hz.
 * @param velocity Amplitude scaling factor.
 *
 * Each wavetable is computed as a pure sine function sampled over one period.
 * The resulting waveform is normalized and scaled according to the velocity parameter.
 *
 * Although sine waves are naturally alias-free, maintaining multiple banks
 * allows consistent integration with the same wavetable framework used
 * by other oscillator types (Saw, Square, Triangle, etc.).
 */
void Sine::generateWavetableBanks(double fs, float velocity)
{
    waveTables.clear();
    std::vector<std::vector<float>> tables;
    tables.reserve(banks.size());

    for (auto bankFreq : banks)
    {
        int N = static_cast<int>(std::round(fs / bankFreq));
        std::vector<float> waveform(N);
        for (int n = 0; n < N; ++n)
        {
            waveform[n] = std::sin(2.0 * M_PI * n / N);
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
