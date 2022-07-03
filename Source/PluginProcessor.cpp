/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>
#include <random>
#include <vector>

//==============================================================================
UselessReverbAudioProcessor::UselessReverbAudioProcessor()
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
}

UselessReverbAudioProcessor::~UselessReverbAudioProcessor()
{
}

//==============================================================================
const juce::String UselessReverbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool UselessReverbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool UselessReverbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool UselessReverbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double UselessReverbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int UselessReverbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int UselessReverbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void UselessReverbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String UselessReverbAudioProcessor::getProgramName (int index)
{
    return {};
}

void UselessReverbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void UselessReverbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    for (int diffuserStep = 0; diffuserStep < m_diffusers.size(); ++diffuserStep) {
        m_diffusers[diffuserStep] = std::make_unique<Diffuser>(sampleRate);
    }
    m_feedbackLoop = std::make_unique<FeedbackLoop>(sampleRate);
    m_reverbBuffer.setSize(NUM_REVERB_CHANNELS, samplesPerBlock);
    m_reverbBuffer.clear();
}

void UselessReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    m_feedbackLoop.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UselessReverbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void UselessReverbAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (!m_feedbackLoop) {
        return;
    }

    const int numSamples = buffer.getNumSamples();
    const int numIOChannels = buffer.getNumChannels();

    // split the signal into multiple channels
    for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
        // TODO: use all channels of the input buffer, not just the first one
        int sourceChannel = 0;
        m_reverbBuffer.copyFrom(channel, 0, buffer, sourceChannel, 0, numSamples);
    }

    // do stuff with the m_reverbBuffer
    std::array<float, NUM_REVERB_CHANNELS> slice;
    for (int sample = 0; sample < numSamples; ++sample) {
        for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
            slice[channel] = m_reverbBuffer.getSample(channel, sample);
        }
        for (auto& diffuser : m_diffusers) {
            diffuser->processSample(slice);
        }
        m_feedbackLoop->processSample(slice);
        for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
            m_reverbBuffer.setSample(channel, sample, slice[channel]);
        }
    }

    // merge the signal
    const float gain = 1.f / NUM_REVERB_CHANNELS;
    for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
        buffer.addFrom(0, 0, m_reverbBuffer, channel, 0, numSamples, gain);
        buffer.addFrom(1, 0, m_reverbBuffer, channel, 0, numSamples, gain);
    }
}


//==============================================================================
bool UselessReverbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UselessReverbAudioProcessor::createEditor()
{
    return new UselessReverbAudioProcessorEditor (*this);
}

//==============================================================================
void UselessReverbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void UselessReverbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new UselessReverbAudioProcessor();
}
