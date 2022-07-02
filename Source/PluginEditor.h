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
class UselessReverbAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener
{
public:
    UselessReverbAudioProcessorEditor (UselessReverbAudioProcessor&);
    ~UselessReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    juce::Label m_delayLengthLabel {"", "Delay"};
    juce::Slider m_delayLengthSlider;

    juce::Label m_feedbackLabel {"", "Feedback"};
    juce::Slider m_feedbackSlider;

    juce::Label m_wetLabel {"", "Mix"};
    juce::Slider m_wetSlider;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UselessReverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UselessReverbAudioProcessorEditor)
};
