#pragma once
#include <JuceHeader.h>

class AudioPluginAudioProcessor;

/**
 * @class GUI
 * @brief Implements the graphical user interface for the audio plugin.
 *
 * This class inherits from juce::AudioProcessorEditor and provides
 * the visual interface for controlling the AudioPluginAudioProcessor.
 *
 * It includes controls for waveform selection, velocity adjustment,
 * and an on-screen MIDI keyboard for note triggering.
 *
 * The GUI communicates with the processor through the
 * AudioProcessorValueTreeState attachments, ensuring automatic
 * synchronization between UI components and internal parameters.
 */
class GUI : public juce::AudioProcessorEditor
{
public:
    /**
     * @brief Constructs the GUI and initializes all interface components.
     * @param processor Reference to the associated AudioPluginAudioProcessor.
     *
     * Initializes the waveform selector, velocity slider, and MIDI keyboard.
     * Connects each control to the processor’s parameter tree using
     * JUCE attachments.
     */
    GUI(AudioPluginAudioProcessor &processor);

    /**
     * @brief Default destructor.
     */
    ~GUI() override = default;

    /**
     * @brief Paints the background and static text of the interface.
     * @param g Reference to the Graphics context used for drawing.
     *
     */
    void paint(juce::Graphics &g) override;

    /**
     * @brief Defines the layout and positions of all child components.
     *
     * Called whenever the window is resized to adjust the bounds of
     * the ComboBox, Slider, and MIDI keyboard components.
     */
    void resized() override;

private:
    /** @brief Reference to the owning AudioProcessor. */
    AudioPluginAudioProcessor &audioProcessor;

    // === UI Components ===

    /** @brief Drop-down selector for choosing the waveform type. */
    juce::ComboBox waveSelector;

    /** @brief Slider for controlling note velocity or output amplitude. */
    juce::Slider velocitySlider;

    // === Parameter Attachments ===

    /** @brief Links the waveform ComboBox to the processor’s parameter tree. */
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveAttachment;

    /** @brief Links the velocity Slider to the processor’s parameter tree. */
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> velocityAttachment;

    // === MIDI Keyboard ===

    /**
     * @brief On-screen MIDI keyboard for triggering notes visually. (This is for testing outside of host, in the standalone plugin)
     *
     * Uses the processor’s MidiKeyboardState for note tracking.
     */
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI)
};
