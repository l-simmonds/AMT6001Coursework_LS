/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class GuitarPluginAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    GuitarPluginAudioProcessor();
    ~GuitarPluginAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    //Slider
    float getSpeed();
    void setSpeed (float value);
    
    //Turbo
    bool turboEnabled;
    void updateTurbo();
    
    //Chord Type
    std::list<int> chordStructure;
    bool setMajorChord;
    void updateChordType();

private:
    //==============================================================================
    juce::AudioParameterFloat* speed;
    int currentNote, lastNoteValue; 
    int time;
    int placeholder;
    float rate; 
    juce::SortedSet<int> notes; 
    std::list<int> chordNotes;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GuitarPluginAudioProcessor)
};
