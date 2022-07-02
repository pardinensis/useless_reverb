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
    m_delayLengthSlider.addListener(this);
    m_delayLengthSlider.setRange(0.01f, 2.0f, 0.01f);
    m_delayLengthSlider.setValue(1.0f);
    m_delayLengthSlider.setTextValueSuffix("s");
    m_delayLengthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 30);
    addAndMakeVisible(&m_delayLengthSlider);
    m_delayLengthLabel.attachToComponent(&m_delayLengthSlider, false);

    m_feedbackSlider.setSliderStyle(juce::Slider::Rotary);
    m_feedbackSlider.addListener(this);
    m_feedbackSlider.setRange(0.0, 1.0, 0.01);
    m_feedbackSlider.setValue(0.5f);
    m_feedbackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 30);
    addAndMakeVisible(&m_feedbackSlider);
    m_feedbackLabel.attachToComponent(&m_feedbackSlider, false);

    m_wetSlider.setSliderStyle(juce::Slider::Rotary);
    m_wetSlider.addListener(this);
    m_wetSlider.setRange(0.0, 1.0, 0.01);
    m_wetSlider.setValue(0.5f);
    m_wetSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 30);
    addAndMakeVisible(&m_wetSlider);
    m_wetLabel.attachToComponent(&m_wetSlider, false);
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
    m_wetSlider.setBounds(420, 80, 180, 180);
}

void UselessReverbAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    const float value = slider->getValue();
    if (slider == &m_delayLengthSlider) {
        audioProcessor.m_delayLength.setValueNotifyingHost(value);
    }
    else if (slider == &m_feedbackSlider) {
        audioProcessor.m_feedback.setValueNotifyingHost(value);
    }
    else if (slider == &m_wetSlider) {
        audioProcessor.m_wet.setValueNotifyingHost(value);
    }
}
