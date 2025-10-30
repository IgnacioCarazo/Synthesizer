#pragma once
#include <JuceHeader.h>
#include <vector>
#include <string>
#include <cmath>

/**
 * @class OscillatorBase
 * @brief Abstract base class defining the interface and core parameters for oscillators.
 *
 * Provides basic phase, frequency, and amplitude handling shared among all oscillator types.
 * This class defines a virtual interface for sample generation and frequency configuration.
 */
class OscillatorBase
{
public:
    /** Default constructor. */
    OscillatorBase() = default;

    /** Virtual destructor for safe inheritance. */
    virtual ~OscillatorBase() = default;

    /**
     * @brief Generates the next sample from the oscillator.
     * @return The next audio sample as a floating-point value.
     *
     * This function must be implemented by derived oscillator classes.
     */
    virtual float getNextSample() = 0;

    /** @brief Resets the oscillator’s phase to zero. */
    virtual void resetPhase() { phase = 0.0; }

    /**
     * @brief Sets the oscillator amplitude (output gain).
     * @param amp Linear amplitude value (typically between 0.0 and 1.0).
     */
    void setAmplitude(float amp) { amplitude = amp; }

    /**
     * @brief Sets the oscillator frequency and updates the phase increment.
     * @param newFreq Frequency in Hz.
     * @param sampleRate Sampling rate in Hz.
     *
     * Computes the normalized phase increment per sample:
     * \f[
     * \Delta \phi = \frac{f}{f_s}
     * \f]
     */
    virtual void setFrequency(double newFreq, double sampleRate)
    {
        frequency = newFreq;
        phaseDelta = frequency / sampleRate;
    }

    /** @brief Returns the oscillator’s name identifier. */
    const std::string &getName() const { return name; }

protected:
    double phase = 0.0;        /**< Current phase position in [0, 1). */
    double phaseDelta = 0.0;   /**< Phase increment per sample. */
    double frequency = 440.0;  /**< Current oscillator frequency in Hz. */
    float amplitude = 1.0f;    /**< Output amplitude (velocity scaling). */
    std::string name = "Base"; /**< Identifier name for the oscillator type. */
};

/**
 * @class WavetableOscillator
 * @brief Implements a band-limited wavetable oscillator using precomputed lookup tables.
 *
 * The WavetableOscillator manages a set of band-limited waveforms (banks), each covering
 * a frequency range optimized to minimize aliasing. The class performs table lookup and
 * linear interpolation between adjacent samples.
 */
class WavetableOscillator : public OscillatorBase
{
public:
    /**
     * @brief Constructs a WavetableOscillator.
     * @param tables 2D vector containing multiple wavetable banks.
     * @param tableFreqs Center frequency of each wavetable bank.
     * @param amp Initial amplitude (default = 1.0f).
     */
    WavetableOscillator(const std::vector<std::vector<float>> &tables,
                        const std::vector<double> &tableFreqs,
                        float amp = 1.0f);
    /**
     * @brief Retrieves the next interpolated sample from the current wavetable.
     * @return Next output sample (float).
     *
     * Performs:
     * 1. Bank selection based on closest frequency.
     * 2. Linear interpolation between two consecutive samples.
     * 3. Phase advancement with wrap-around.
     */
    float getNextSample() override;

    /**
     * @brief Sets oscillator frequency and computes the normalized phase increment.
     * @param freq Frequency in Hz.
     * @param sampleRate Sampling rate in Hz.
     */
    void setFrequency(double freq, double sampleRate) override;

protected:
    std::vector<std::vector<float>> waveTables; // banked wavetables
    std::vector<double> bankFreqs;              // center freqs for each bank
};
