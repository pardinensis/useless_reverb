/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
UselessReverbAudioProcessorEditor::UselessReverbAudioProcessorEditor (UselessReverbAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    // decay time
    m_decayTimeLabel = std::make_unique<juce::Label>("decayTimeLabel", "Decay Time");
    m_decayTimeSlider = std::make_unique<juce::Slider>(juce::Slider::Rotary, juce::Slider::TextBoxBelow);
    m_decayTimeSlider->setTextValueSuffix("s");
    addAndMakeVisible(m_decayTimeSlider.get());
    m_decayTimeLabel->attachToComponent(m_decayTimeSlider.get(), false);
    m_decayTimeAttachment = std::make_unique<SliderAttachment>(p.m_valueTreeState, "decayTime", *m_decayTimeSlider);

    // mix
    m_mixLabel = std::make_unique<juce::Label>("mixLabel", "Mix");
    m_mixSlider = std::make_unique<juce::Slider>(juce::Slider::Rotary, juce::Slider::TextBoxBelow);
    m_mixSlider->setTextValueSuffix("%");
    addAndMakeVisible(m_mixSlider.get());
    m_mixLabel->attachToComponent(m_mixSlider.get(), false);
    m_mixAttachment = std::make_unique<SliderAttachment>(p.m_valueTreeState, "mix", *m_mixSlider);

    setSize(620, 400);
}

UselessReverbAudioProcessorEditor::~UselessReverbAudioProcessorEditor()
{
}

//==============================================================================
void UselessReverbAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));


}

void UselessReverbAudioProcessorEditor::resized()
{
    m_decayTimeSlider->setBounds(20, 80, 180, 180);
    m_mixSlider->setBounds(220, 80, 180, 180);
}
