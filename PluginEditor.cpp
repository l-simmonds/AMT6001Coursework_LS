/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarPluginAudioProcessorEditor::GuitarPluginAudioProcessorEditor (GuitarPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
    //Slider
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    speedLabel.setText("Speed", juce::dontSendNotification);
    addAndMakeVisible(&speedLabel);
    
    speedSlider.setSliderStyle(juce::Slider::Rotary);
    speedSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkslateblue);
    speedSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    speedSlider.setRange(0.0, 1.0, 0.01);
    speedSlider.setValue(audioProcessor.getSpeed());
    speedSlider.addListener(this);
    addAndMakeVisible(&speedSlider);
    
    //Turbo Mode Button
    turboModeButton.setButtonText("Turbo Mode");
    turboModeButton.setClickingTogglesState(true);
    turboModeButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::darkseagreen);
    turboModeButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::darkslateblue);
    
    addAndMakeVisible(turboModeButton);
    turboModeButton.onClick = [this]
    {
        toggleTurboModeState (&turboModeButton);
    };
    
    //Chord type
    chordTypeButton.setButtonText("Set Chord");
    chordTypeButton.setClickingTogglesState(true);
    chordTypeButton.setColour(juce::TextButton::ColourIds::buttonOnColourId, juce::Colours::orange);
    chordTypeButton.setColour(juce::TextButton::ColourIds::textColourOffId, juce::Colours::pink);
    
    addAndMakeVisible(chordTypeButton);
    chordTypeButton.onClick = [this]
    {
        setChordType(&chordTypeButton);
        if (audioProcessor.setMajorChord)
        {
            chordTypeButton.setButtonText("Major");
        }
        else
        {
            chordTypeButton.setButtonText("Minor");
        }
    };
    
}

GuitarPluginAudioProcessorEditor::~GuitarPluginAudioProcessorEditor()
{
}

//==============================================================================
void GuitarPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
}

void GuitarPluginAudioProcessorEditor::resized()
{
    //Slider
    auto bgRect = getBounds();
    const int border = 20;
    int sliderWidth = 200;
    int sliderHeight = getHeight() - border - 100;
    int labelWidth = 50;
    int labelHeight = 20;
    int center_x = bgRect.getWidth() / 2;
    int center_y = bgRect.getHeight() / 2;
    int margin = 10;
    
    speedLabel.setBounds(center_x - ((labelWidth/2) + margin),
                         center_y - ((sliderHeight/2) + labelHeight + margin),
                         labelWidth,
                         labelHeight);
    
    speedSlider.setBounds(center_x - ((sliderWidth/2) + margin),
                          center_y - ((sliderHeight/2) + margin),
                          sliderWidth,
                          sliderHeight);
    
    //Turbo Mode
    int buttonWidth = getWidth() * 0.25;
    int buttonHeight = buttonWidth * 0.5;
    
    turboModeButton.setBounds(speedSlider.getX() - (margin/2), speedSlider.getY() + sliderHeight + margin, buttonWidth, buttonHeight);
    
    //Chord Type
    chordTypeButton.setBounds(speedSlider.getX() + (margin/2) + buttonWidth, speedSlider.getY() + sliderHeight + margin, buttonWidth, buttonHeight);
}

void GuitarPluginAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
        if (slider == &speedSlider)
    {
        audioProcessor.setSpeed(float(slider->getValue()));
    }
}

void GuitarPluginAudioProcessorEditor::toggleTurboModeState(juce::Button* turboButton)
{
    audioProcessor.updateTurbo();
}

void GuitarPluginAudioProcessorEditor::setChordType(juce::Button* chordButton)
{
    audioProcessor.updateChordType();
}
