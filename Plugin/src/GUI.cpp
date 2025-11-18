#include "GUI.h"
#include "PluginProcessor.h"

GUI::GUI(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      keyboardComponent(audioProcessor.getKeyboardState(),
                        juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(1000, 500);

    // Waveform selector

    waveSelector.addItem("Sine", 1);
    waveSelector.addItem("Saw", 2);
    waveSelector.addItem("Square", 3);
    waveSelector.addItem("Triangle", 4);
    addAndMakeVisible(waveSelector);

    waveAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.apvts, "WAVE", waveSelector);

    // Velocity slider

    velocitySlider.setRange(0.0, 1.0, 0.001);
    velocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    velocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(velocitySlider);

    velocityAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "VELOCITY", velocitySlider);

    velocityLabel.setText("Velocity", juce::dontSendNotification);
    velocityLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    velocityLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(velocityLabel);

    // ADSR sliders

    attackSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    attackSlider.setRange(0.001, 5.0, 0.001);
    addAndMakeVisible(attackSlider);
    attackAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "ATTACK", attackSlider);

    decaySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    decaySlider.setRange(0.001, 5.0, 0.001);
    addAndMakeVisible(decaySlider);
    decayAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "DECAY", decaySlider);

    sustainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    sustainSlider.setRange(0.0, 1.0, 0.001);
    addAndMakeVisible(sustainSlider);
    sustainAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "SUSTAIN", sustainSlider);

    releaseSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    releaseSlider.setRange(0.001, 10.0, 0.001);
    addAndMakeVisible(releaseSlider);
    releaseAttachment = std::make_unique<
        juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.apvts, "RELEASE", releaseSlider);

    // ADSR labels
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    attackLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(attackLabel);

    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    decayLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(decayLabel);

    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    sustainLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(sustainLabel);

    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    releaseLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(releaseLabel);

    // MIDI Keyboard

    addAndMakeVisible(keyboardComponent);
}

void GUI::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black);

    const int margin = 10;
    const int keyboardHeight = 100;
    auto area = getLocalBounds().reduced(margin);
    auto keyboardArea = area.removeFromBottom(keyboardHeight);
    auto controlArea = area;

    // Split into 3 sections
    auto oscArea = controlArea.removeFromLeft(controlArea.getWidth() / 3);
    auto envArea = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto filterArea = controlArea;

    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
    g.fillRect(oscArea);
    g.fillRect(envArea);
    g.fillRect(filterArea);

    g.setColour(juce::Colours::white);
    g.setFont(20.f);
    g.drawText("OSCILLATOR", oscArea, juce::Justification::centredTop);
    g.drawText("ENVELOPE", envArea, juce::Justification::centredTop);
    g.drawText("FILTER", filterArea, juce::Justification::centredTop);
    g.drawText("MIDI KEYBOARD", keyboardArea, juce::Justification::centredTop);

    // Borders
    g.setColour(juce::Colours::grey);
    g.drawRect(oscArea);
    g.drawRect(envArea);
    g.drawRect(filterArea);
    g.drawRect(keyboardArea);
}

void GUI::resized()
{
    const int margin = 10;
    const int keyboardHeight = 100;

    auto area = getLocalBounds().reduced(margin);
    auto keyboardArea = area.removeFromBottom(keyboardHeight);

    auto controlArea = area;
    auto oscArea = controlArea.removeFromLeft(controlArea.getWidth() / 3);
    auto envArea = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto filterArea = controlArea;

    const int topPadding = 40;

    //  OSC Layout
    auto oscContent = oscArea.reduced(20, 0);
    oscContent.removeFromTop(topPadding);

    waveSelector.setBounds(oscContent.removeFromTop(40).reduced(0, 5));
    velocityLabel.setBounds(oscContent.removeFromTop(20));
    velocitySlider.setBounds(oscContent.removeFromTop(40).reduced(0, 5));

    //  Envelope Layout
    auto envContent = envArea.reduced(20, 10);
    envContent.removeFromTop(topPadding);

    const int sliderHeight = 28;
    const int sliderGap = 8;

    attackLabel.setBounds(envContent.removeFromTop(18));
    attackSlider.setBounds(envContent.removeFromTop(sliderHeight).reduced(0, 2));
    envContent.removeFromTop(sliderGap);

    decayLabel.setBounds(envContent.removeFromTop(18));
    decaySlider.setBounds(envContent.removeFromTop(sliderHeight).reduced(0, 2));
    envContent.removeFromTop(sliderGap);

    sustainLabel.setBounds(envContent.removeFromTop(18));
    sustainSlider.setBounds(envContent.removeFromTop(sliderHeight).reduced(0, 2));
    envContent.removeFromTop(sliderGap);

    releaseLabel.setBounds(envContent.removeFromTop(18));
    releaseSlider.setBounds(envContent.removeFromTop(sliderHeight).reduced(0, 2));

    //  MIDI keyboard
    keyboardComponent.setBounds(keyboardArea);
}
