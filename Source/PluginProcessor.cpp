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
    const int NUM_DELAY_CHANNELS = 4;
    const float DELAY_TIME_MIN = 0.10f;
    const float DELAY_TIME_MAX = 0.20f;
    if (m_delays.empty()) {
        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution(DELAY_TIME_MIN, DELAY_TIME_MAX);

        for (int channel = 0; channel < NUM_DELAY_CHANNELS; ++channel) {
            float delayTime = distribution(generator);
            int numSamples = std::max(1, static_cast<int>(delayTime * sampleRate));
            m_delays.push_back(std::make_unique<Delay>(numSamples));
        }

        m_mixMatrix = std::vector<float>{
             0.7408,  0.3456, -0.3456,  0.4608,
             0.3456,  0.5392,  0.4608, -0.6144,
            -0.3456,  0.4608,  0.5392,  0.6144,
             0.4608, -0.6144,  0.6144,  0.1808
        };
    }
}

void UselessReverbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    m_delays.clear();
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

void UselessReverbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    if (m_delays.empty() || m_mixMatrix.empty()) {
        return;
    }

    const int numIOChannels = buffer.getNumChannels();
    const int numDelayChannels = m_delays.size();
    const int numSamples = buffer.getNumSamples();

    std::vector<float> mixedValues(numDelayChannels, 0.f);

    for (int sample = 0; sample < numSamples; ++sample) {
        // calculate input value (avg of all input channels)
        float inputValue = 0;
        for (int channel = 0; channel < numIOChannels; ++channel) {
            inputValue += buffer.getSample(channel, sample);
        }
        inputValue /= numIOChannels;

        // clear the mixedValues
        std::fill(mixedValues.begin(), mixedValues.end(), 0.f);

        float outputValue = 0;

        for (int channel = 0; channel < numDelayChannels; ++channel) {
            float delayedValue = m_delays[channel]->read();
            outputValue += delayedValue;

            float dampenedValue = delayedValue * 0.85;
            for (int mixChannel = 0; mixChannel < numDelayChannels; ++mixChannel) {
                const float mixFactor = m_mixMatrix[channel * numDelayChannels + mixChannel];
                mixedValues[mixChannel] += dampenedValue * mixFactor;
            }
        }

        for (int channel = 0; channel < numDelayChannels; ++channel) {
            float sum = inputValue + mixedValues[channel];
            m_delays[channel]->write(sum);
            m_delays[channel]->next();
        }

        for (int channel = 0; channel < numIOChannels; ++channel) {
            buffer.setSample(channel, sample, outputValue);
        }
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
