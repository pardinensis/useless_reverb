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

public:
    juce::AudioParameterFloat m_delayLength {
        "delayLength",
        "Delay",
        0.01f,
        2.0f,
        1.00f
    };
    juce::AudioParameterFloat m_feedback {
        "feedback",
        "Feedback",
        0.00f,
        1.00f,
        0.50f
    };
    juce::AudioParameterFloat m_wet {
        "wet",
        "Mix",
        0.00f,
        1.00f,
        0.50f
    };
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UselessReverbAudioProcessor)
};
