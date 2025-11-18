#pragma once
#include <JuceHeader.h>

/**
 * @class Envelope
 * @brief Implements a simple linear ADSR envelope generator.
 *
 * The Envelope class provides a per-voice ADSR amplitude envelope,
 * updated at audio rate on a per-sample basis. It supports linear
 * Attack, Decay, Sustain, and Release phases, designed to match
 * the behavior of the reference MATLAB implementation.
 *
 * The envelope is driven manually through:
 *  - enterAttack()   — Called on note-on events.
 *  - enterRelease()  — Called on note-off events.
 *  - processSample() — Returns the amplitude multiplier for each sample.
 *
 * Timing is internally converted from seconds to sample counts,
 * ensuring stable behavior across arbitrary sample rates.
 */
class Envelope
{
public:
    /**
     * @brief Enumeration of the internal ADSR states.
     */
    enum class Stage
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    /**
     * @brief Constructs a new Envelope with default ADSR parameters.
     *
     * Default values:
     *  - Attack  = 0.01 s
     *  - Decay   = 0.10 s
     *  - Sustain = 0.80 (normalized)
     *  - Release = 0.20 s
     *
     * The sample rate defaults to 44100 Hz until updated externally.
     */
    Envelope();

    /**
     * @brief Sets the sample rate used to convert ADSR times to samples.
     * @param sr Sample rate in Hz.
     */
    void setSampleRate(double sr);

    /**
     * @brief Sets the ADSR envelope parameters.
     * @param attackSec  Attack time in seconds.
     * @param decaySec   Decay time in seconds.
     * @param sustainLvl Sustain level (0.0–1.0).
     * @param releaseSec Release time in seconds.
     *
     * Converts the time-domain parameters into sample counts.
     */
    void setParameters(float attackSec, float decaySec,
                       float sustainLvl, float releaseSec);

    /**
     * @brief Begins the attack phase of the envelope.
     *
     * Called by the voice on MIDI Note-On events. Resets phase counters
     * and prepares the envelope to rise from 0.0 to 1.0.
     */
    void enterAttack();

    /**
     * @brief Begins the release phase of the envelope.
     *
     * Called by the voice on MIDI Note-Off. Stores the current level
     * and transitions toward 0.0 over the configured release time.
     */
    void enterRelease();

    /**
     * @brief Forces the envelope into the idle state and resets internal values.
     */
    void reset();

    /**
     * @brief Processes a single audio sample's amplitude.
     * @return A linear gain factor (0.0–1.0) depending on the envelope stage.
     *
     * This method advances the internal state machine and must be called
     * once per rendered audio sample.
     */
    float processSample();

    /**
     * @brief Indicates whether the envelope is active.
     * @return True if the envelope is not in the Idle stage.
     *
     * Useful for determining when a voice should be cleared.
     */
    bool isActive() const;

private:
    Stage stage;       ///< Current ADSR stage.
    double sampleRate; ///< System sample rate.

    float attackTime;   ///< Attack time in seconds.
    float decayTime;    ///< Decay time in seconds.
    float sustainLevel; ///< Sustain level (0–1).
    float releaseTime;  ///< Release time in seconds.

    int stageCounter;        ///< Samples elapsed in the current stage.
    float currentLevel;      ///< Current envelope output level.
    float releaseStartLevel; ///< Level at the moment of entering Release.

    int attackSamples;  ///< Attack duration in samples.
    int decaySamples;   ///< Decay duration in samples.
    int releaseSamples; ///< Release duration in samples.
};
