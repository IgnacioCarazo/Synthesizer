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
 * It includes controls for waveform selection, amplitude (velocity),
 * ADSR envelope sliders, and an on-screen MIDI keyboard for note triggering.
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
     * Initializes the waveform selector, velocity slider, ADSR sliders,
     * labels, and MIDI keyboard. Connects each control to the processor’s
     * parameter tree using JUCE attachments.
     */
    GUI(AudioPluginAudioProcessor &processor);

    /**
     * @brief Default destructor.
     */
    ~GUI() override = default;

    /**
     * @brief Paints the background and static labels of the interface.
     * @param g Reference to the Graphics context used for drawing.
     */
    void paint(juce::Graphics &g) override;

    /**
     * @brief Defines the layout of all GUI components.
     *
     * Called when the window is resized; recalculates the positions of all
     * controls according to the defined layout rules.
     */
    void resized() override;

private:
    /** @brief Reference to the owning AudioProcessor. */
    AudioPluginAudioProcessor &audioProcessor;

    //
    // UI COMPONENTS
    //

    /** @brief Drop-down selector for choosing the oscillator waveform. */
    juce::ComboBox waveSelector;

    /** @brief Menu → APVTS attachment for waveform selection. */
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveAttachment;

    /** @brief Slider controlling global amplitude or note velocity. */
    juce::Slider velocitySlider;

    /** @brief Slider → APVTS attachment for amplitude. */
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> velocityAttachment;

    //  ADSR Sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    // ADSR attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    //  Labels
    juce::Label velocityLabel;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    /** @brief On-screen MIDI keyboard for manual note triggering. */
    juce::MidiKeyboardComponent keyboardComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GUI)
};
