#include "GUI.h"
#include "PluginProcessor.h"

GUI::GUI(AudioPluginAudioProcessor &p)
    : AudioProcessorEditor(&p),
      audioProcessor(p),
      // Inicializar el teclado con el MidiKeyboardState del procesador
      keyboardComponent(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(600, 300); // más alto para el teclado

    // Wave Selector UI
    waveSelector.addItem("Sine", 1);
    waveSelector.addItem("Saw", 2);
    waveSelector.addItem("Square", 3);
    waveSelector.addItem("Triangle", 4);
    addAndMakeVisible(waveSelector);

    // Attach ComboBox a parametro
    waveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::
                                          ComboBoxAttachment>(audioProcessor.apvts, "WAVE", waveSelector);

    // Velocity UI
    velocitySlider.setRange(0.0, 1.0);
    velocitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    velocitySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    addAndMakeVisible(velocitySlider);

    // Attach Slider a parametro
    velocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::
                                              SliderAttachment>(audioProcessor.apvts, "VELOCITY", velocitySlider);

    // Agregar teclado MIDI
    addAndMakeVisible(keyboardComponent);
}

void GUI::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(18.f);
    g.drawFittedText("TEC Synth Controls", getLocalBounds(), juce::Justification::centredTop, 1);
}

void GUI::resized()
{
    waveSelector.setBounds(20, 20, 150, 30);
    velocitySlider.setBounds(200, 20, getWidth() - 220, 30);
    keyboardComponent.setBounds(10, 70, getWidth() - 20, 120);
}
