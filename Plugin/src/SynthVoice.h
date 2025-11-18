#pragma once
#include <JuceHeader.h>
#include "Oscillator/OscillatorWrapper.h"
#include "Oscillator/Oscillator.h"
#include "Envelope.h"

/**
 * @class SynthVoice
 * @brief Implements a monophonic synthesiser voice using JUCE.
 *
 * This class extends juce::SynthesiserVoice to handle playback
 * of a single synthesiser voice. It manages waveform generation,
 * amplitude, frequency (MIDI note), and audio rendering to the output buffer.
 *
 * It relies on the OscillatorWrapper class to handle waveform selection
 * and real-time sample generation.
 */
class SynthVoice : public juce::SynthesiserVoice
{
public:
    /**
     * @brief Default constructor.
     *
     * Initializes the voice with a default sample rate.
     * The final sample rate is set later via `prepareToPlay`.
     */
    SynthVoice();

    /**
     * @brief Determines whether this voice can play a given sound.
     * @param sound Pointer to a juce::SynthesiserSound object.
     * @return true if the sound is compatible with this voice.
     */
    bool canPlaySound(juce::SynthesiserSound *sound) override;

    /**
     * @brief Starts playback of a new MIDI note.
     * @param midiNoteNumber MIDI note number (0–127).
     * @param velocity MIDI velocity (0.0–1.0), used as amplitude.
     * @param sound Pointer to the associated sound (not used directly).
     * @param currentPitchWheelPosition Current pitch wheel value.
     *
     * Updates the oscillator’s frequency, amplitude, and waveform type
     * according to the current parameters, and marks the note as active.
     */
    void startNote(int midiNoteNumber, float velocity,
                   juce::SynthesiserSound *sound, int currentPitchWheelPosition) override;
    /**
     * @brief Stops playback of the active note.
     * @param velocity Note-off velocity (unused).
     * @param allowTailOff Whether to allow a natural tail-off (unused).
     */
    void stopNote(float velocity, bool allowTailOff) override;

    /**
     * @brief Handles pitch wheel movement (not implemented).
     * @param newPitchWheelValue New pitch wheel value.
     */
    void pitchWheelMoved(int newPitchWheelValue) override;

    /**
     * @brief Handles MIDI controller movement (not implemented).
     * @param controllerNumber Controller number.
     * @param newControllerValue New controller value.
     */
    void controllerMoved(int controllerNumber, int newControllerValue) override;

    /**
     * @brief Renders the next audio block for the active note.
     * @param outputBuffer Reference to the output buffer to write samples to.
     * @param startSample Starting sample index in the buffer.
     * @param numSamples Number of samples to generate.
     *
     * If the note is active, this method generates samples from the oscillator
     * and mixes them into each channel of the output buffer.
     */
    void renderNextBlock(juce::AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    /**
     * @brief Sets the waveform type used by the oscillator.
     * @param waveIndex Waveform index (e.g., 0 = sine, 1 = square, etc.).
     */
    void setWaveType(int waveIndex); // cambia tipo de onda desde GUI

    /**
     * @brief Sets the amplitude (volume) of the voice.
     * @param amp New amplitude value (normalized 0.0–1.0).
     */
    void setAmplitude(float amp); // cambia volumen desde GUI o plugin processor

    /**
     * @brief Updates ADSR parameters for the envelope.
     *
     * Called by the processor when GUI parameters change.
     */
    void setEnvelopeParameters(float attack, float decay,
                               float sustain, float release);

private:
    /** @brief Main oscillator used for waveform generation. */
    OscillatorWrapper oscillator;

    /** @brief Main oscillator used for waveform generation. */
    Envelope env;

    /** @brief Current waveform index. */
    int currentWaveIndex = 0;

    /** @brief Current amplitude (linked to velocity or volume). */
    float currentVelocity = 1.0f;

    /** @brief Indicates whether a note is currently active. */
    bool isNoteActive = false;
};
