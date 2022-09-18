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
    std::unique_ptr<juce::Label> m_decayTimeLabel;
    std::unique_ptr<juce::Slider> m_decayTimeSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_decayTimeAttachment;

    std::unique_ptr<juce::Label> m_mixLabel;
    std::unique_ptr<juce::Slider> m_mixSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> m_mixAttachment;

    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    UselessReverbAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (UselessReverbAudioProcessorEditor)
};
