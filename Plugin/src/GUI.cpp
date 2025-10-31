#include "GUI.h"
#include "PluginProcessor.h"

GUI::GUI(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      keyboardComponent(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(1000, 500);

    // Wave Selector (Oscillator)
    waveSelector.addItem("Sine", 1);
    waveSelector.addItem("Saw", 2);
    waveSelector.addItem("Square", 3);
    waveSelector.addItem("Triangle", 4);
    addAndMakeVisible(waveSelector);

    waveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::
                                          ComboBoxAttachment>(audioProcessor.apvts, "WAVE", waveSelector);

    // Velocity
    velocitySlider.setRange(0.0, 1.0);
    velocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    velocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(velocitySlider);

    velocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::
                                              SliderAttachment>(audioProcessor.apvts, "VELOCITY", velocitySlider);

    // MIDI Keyboard
    addAndMakeVisible(keyboardComponent);
}

void GUI::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black);

    // === Layout general ===
    const int margin = 10;
    const int keyboardHeight = 100;
    auto area = getLocalBounds().reduced(margin);
    auto keyboardArea = area.removeFromBottom(keyboardHeight);
    auto controlArea = area; // zona superior para 3 secciones

    // Dividir zona superior en 3 partes iguales
    auto oscArea = controlArea.removeFromLeft(controlArea.getWidth() / 3);
    auto envArea = controlArea.removeFromLeft(controlArea.getWidth() / 2);
    auto filterArea = controlArea;

    // === Dibujar secciones ===
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

    // Marco visual
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
    auto oscContent = oscArea.reduced(20, 0);
    oscContent.removeFromTop(topPadding);

    waveSelector.setBounds(oscContent.removeFromTop(40).reduced(0, 5));
    velocitySlider.setBounds(oscContent.removeFromTop(40).reduced(0, 5));

    keyboardComponent.setBounds(keyboardArea);
}
