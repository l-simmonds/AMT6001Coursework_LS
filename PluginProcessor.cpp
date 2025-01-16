/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GuitarPluginAudioProcessor::GuitarPluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter (speed = new juce::AudioParameterFloat ({"speed", 1}, "Arpeggiator Speed", 0.0, 1.0, 0.5));
}

GuitarPluginAudioProcessor::~GuitarPluginAudioProcessor()
{
}

//==============================================================================
const juce::String GuitarPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GuitarPluginAudioProcessor::acceptsMidi() const
{
    return true;
}

bool GuitarPluginAudioProcessor::producesMidi() const
{
    return true;
}

bool GuitarPluginAudioProcessor::isMidiEffect() const
{
    return true;
}

double GuitarPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GuitarPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GuitarPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GuitarPluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GuitarPluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void GuitarPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GuitarPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //Setting Turbo to False
    turboEnabled = false;
    //Setting Chord as major
    setMajorChord = true;
    //clearing the stored notes
    chordNotes.clear(); 
    notes.clear();
    //resetting variables
    time = 0;
    rate = static_cast<float> (sampleRate);
    placeholder = -1;
    currentNote = -1;
    lastNoteValue = -1;
    
}

void GuitarPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GuitarPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GuitarPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    jassert(buffer.getNumChannels() == 0);
    auto numSamples = buffer.getNumSamples();
    auto noteDuration = static_cast<int> (std::ceil(rate * 0.25f * (0.1f + (1.0f - (*speed)))));
    
    for (auto metadata : midiMessages)
    {
        auto msg = metadata.getMessage();
        if (msg.isNoteOn())
        {
            for (int i : chordStructure)
            {
                chordNotes.push_back(msg.getNoteNumber() + i);
                notes.add (msg.getNoteNumber() + i);
            }
        }
        else if (msg.isNoteOff())
        {
            for (int i : chordStructure)
            {
                chordNotes.remove (msg.getNoteNumber() + i);
                notes.removeValue (msg.getNoteNumber() + i);
            }
        }
    }
    midiMessages.clear();
    
    if ((time + numSamples) > noteDuration) 
    {
        auto offset = juce::jmax(0, juce::jmin((int) (noteDuration - time), numSamples - 1));
        
        if (turboEnabled)
        {
            if (lastNoteValue > 0) 
            {
                midiMessages.addEvent (juce::MidiMessage::noteOff(1, lastNoteValue), offset);
                lastNoteValue = -1;
            } 
            
            if (notes.size() > 0) 
            {
                currentNote = (currentNote + 1) % notes.size();
                lastNoteValue = notes[currentNote];
                midiMessages.addEvent (juce::MidiMessage::noteOn(1, lastNoteValue, (juce::uint8) 127), offset);
            }
        }
        else
        {
            if (placeholder > 0)
            {
                for (int i : chordNotes)
                {
                    midiMessages.addEvent (juce::MidiMessage::noteOff (1, i), offset);
                }
                placeholder = -1;
            }
        
            if (chordNotes.size() > 0)
            {
                for (int i : chordNotes)
                {
                    midiMessages.addEvent (juce::MidiMessage::noteOn (1, i, (juce::uint8) 127), offset);
                }
                placeholder = 1;
            }
        }
            
    }
    
    time = (time + numSamples) % noteDuration;
            
}

//==============================================================================
bool GuitarPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GuitarPluginAudioProcessor::createEditor()
{
    return new GuitarPluginAudioProcessorEditor (*this);
}

//==============================================================================
void GuitarPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GuitarPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

float GuitarPluginAudioProcessor::getSpeed()
{
    return speed -> get();
}

void GuitarPluginAudioProcessor::setSpeed (float value)
{
    *speed = value;
}

void GuitarPluginAudioProcessor::updateTurbo()
{
    turboEnabled = !turboEnabled;
}

void GuitarPluginAudioProcessor::updateChordType()
{
    setMajorChord = !setMajorChord;
    if(setMajorChord)
    {
        chordStructure = {0, 4, 7, 11, 14};
    }
    else
    {
        chordStructure = {0, 3, 7, 10, 14};
    }
}
//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GuitarPluginAudioProcessor();
}
