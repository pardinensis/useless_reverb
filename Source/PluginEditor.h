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
class UselessReverbAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    UselessReverbAudioProcessorEditor (UselessReverbAudioProcessor&);
    ~UselessReverbAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    juce::Label m_delayLengthLabel {"", "Delay"};
    juce::Slider m_delayLengthSlider;
    std::unique_ptr<SliderAttachment> m_delayLengthAttachment;

    juce::Label m_feedbackLabel {"", "Feedback"};
    juce::Slider m_feedbackSlider;
    std::unique_ptr<SliderAttachment> m_feedbackAttachment;

    juce::Label m_mixLabel {"", "Mix"};
    juce::Slider m_mixSlider;
    std::unique_ptr<SliderAttachment> m_mixAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UselessReverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UselessReverbAudioProcessorEditor)
};
