#include "Oscillator.h"
#include <algorithm>

/**
 * @brief Constructs a WavetableOscillator and initializes its tables.
 * @param tables Wavetable banks containing precomputed waveforms.
 * @param tableFreqs Center frequencies for each bank.
 * @param amp Initial amplitude.
 */
WavetableOscillator::WavetableOscillator(const std::vector<std::vector<float>> &tables,
                                         const std::vector<double> &tableFreqs,
                                         float amp)
    : waveTables(tables), bankFreqs(tableFreqs)
{
    name = "SineWavetable";
    amplitude = amp;
}

/**
 * @brief Sets oscillator frequency and updates the phase increment.
 * @param freq Frequency in Hz.
 * @param sampleRate Sampling rate in Hz.
 */
void WavetableOscillator::setFrequency(double freq, double sampleRate)
{
    frequency = freq;
    phaseDelta = frequency / sampleRate;
}

/**
 * @brief Returns the next interpolated sample from the current wavetable.
 * @return Next audio sample (float).
 *
 * This method performs the following:
 * 1. Selects the wavetable bank with the closest center frequency.
 * 2. Interpolates linearly between consecutive samples.
 * 3. Scales the sample by the current amplitude.
 * 4. Advances and wraps the oscillator phase.
 */
float WavetableOscillator::getNextSample()
{
    if (waveTables.empty())
        return 0.0f;

    // Select closest frequency bank
    size_t bankIndex = 0;
    double minDiff = std::abs(bankFreqs[0] - frequency);
    for (size_t i = 1; i < bankFreqs.size(); ++i)
    {
        double diff = std::abs(bankFreqs[i] - frequency);
        if (diff < minDiff)
        {
            minDiff = diff;
            bankIndex = i;
        }
    }

    auto &table = waveTables[bankIndex];

    // Linear interpolation between samples
    size_t idx0 = static_cast<size_t>(phase * table.size());
    size_t idx1 = (idx0 + 1) % table.size();
    float frac = static_cast<float>(phase * table.size() - idx0);

    float sample = (table[idx0] * (1.0f - frac) + table[idx1] * frac) * amplitude;

    // Advance and wrap phase
    phase += phaseDelta;
    if (phase >= 1.0)
        phase -= 1.0;

    return sample;
}
