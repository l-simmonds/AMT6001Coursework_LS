/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GuitarPluginAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          public juce::Slider::Listener
{
public:
    GuitarPluginAudioProcessorEditor (GuitarPluginAudioProcessor&);
    ~GuitarPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //defining functions
    void toggleTurboModeState(juce::Button* turboButton);
    
    void sliderValueChanged(juce::Slider *slider) override;
    
    void setChordType(juce::Button* chordButton);
    
    GuitarPluginAudioProcessor& audioProcessor;
    
    //Turbo Mode
    juce::TextButton turboModeButton;
    
    //Slider
    juce::Label speedLabel;
    juce::Slider speedSlider;
    
    //Chord Button
    juce::TextButton chordTypeButton;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarPluginAudioProcessorEditor)
};
