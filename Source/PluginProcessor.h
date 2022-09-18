/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Diffuser.h"
#include "FeedbackLoop.h"

//==============================================================================
/**
*/
class UselessReverbAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    UselessReverbAudioProcessor();
    ~UselessReverbAudioProcessor() override;
   
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters() const;

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
private:
    static const int NUM_REVERB_CHANNELS = 8;
    std::array<std::unique_ptr<Diffuser>, 4> m_diffusers;
    std::unique_ptr<FeedbackLoop> m_feedbackLoop;
    juce::AudioBuffer<float> m_reverbBuffer;
    juce::AudioBuffer<float> m_outputBuffer;
    juce::dsp::DryWetMixer<float> m_mixer;

    juce::UndoManager m_undoManager;

public:
    juce::AudioProcessorValueTreeState m_valueTreeState;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UselessReverbAudioProcessor)
};
