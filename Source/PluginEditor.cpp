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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (620, 400);

    m_delayLengthSlider.setSliderStyle(juce::Slider::Rotary);
    m_delayLengthSlider.setTextValueSuffix("s");
    m_delayLengthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 30);
    addAndMakeVisible(&m_delayLengthSlider);
    m_delayLengthLabel.attachToComponent(&m_delayLengthSlider, false);
    m_delayLengthAttachment = std::make_unique<SliderAttachment>(p.m_valueTreeState, "delayLength", m_delayLengthSlider);

    m_feedbackSlider.setSliderStyle(juce::Slider::Rotary);
    m_feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 30);
    addAndMakeVisible(&m_feedbackSlider);
    m_feedbackLabel.attachToComponent(&m_feedbackSlider, false);
    m_feedbackAttachment = std::make_unique<SliderAttachment>(p.m_valueTreeState, "feedback", m_feedbackSlider);

    m_mixSlider.setSliderStyle(juce::Slider::Rotary);
    m_mixSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 30);
    addAndMakeVisible(&m_mixSlider);
    m_mixLabel.attachToComponent(&m_mixSlider, false);
    m_mixAttachment = std::make_unique<SliderAttachment>(p.m_valueTreeState, "mix", m_mixSlider);
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
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    m_delayLengthSlider.setBounds(20, 80, 180, 180);
    m_feedbackSlider.setBounds(220, 80, 180, 180);
    m_mixSlider.setBounds(420, 80, 180, 180);
}
