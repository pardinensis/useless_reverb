#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <algorithm>
#include <random>
#include <vector>

UselessReverbAudioProcessor::UselessReverbAudioProcessor()
	: AudioProcessor(BusesProperties()
		.withInput("Input", juce::AudioChannelSet::stereo(), true)
		.withOutput("Output", juce::AudioChannelSet::stereo(), true)
	)
	, m_undoManager()
	, m_valueTreeState(*this, &m_undoManager, "parameters", createParameters())
{
	m_mixer.setMixingRule(juce::dsp::DryWetMixingRule::linear);
}

UselessReverbAudioProcessor::~UselessReverbAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout UselessReverbAudioProcessor::createParameters() const
{
	std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

	juce::NormalisableRange<float> decayTimeRange(0.10f, 10.00f, 0.01f);
	decayTimeRange.setSkewForCentre(2.00f);
	params.push_back(std::make_unique<juce::AudioParameterFloat>(
		/* parameterID */ "decayTime",
		/* parameterName */ "decayTime",
		/* normalizableRange */ decayTimeRange,
		/* defaultValue */ 2.00f
		));

	juce::NormalisableRange<float> mixRange(0, 100, 1);
	params.push_back(std::make_unique<juce::AudioParameterFloat>(
		/* parameterID */ "mix",
		/* parameterName */ "mix",
		/* normalizableRange */ mixRange,
		/* defaultValue */ 50.f
		));

	return { params.begin(), params.end() };
}

const juce::String UselessReverbAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool UselessReverbAudioProcessor::acceptsMidi() const
{
	return false;
}

bool UselessReverbAudioProcessor::producesMidi() const
{
	return false;
}

bool UselessReverbAudioProcessor::isMidiEffect() const
{
	return false;
}

double UselessReverbAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int UselessReverbAudioProcessor::getNumPrograms()
{
	return 1;
}

int UselessReverbAudioProcessor::getCurrentProgram()
{
	return 0;
}

void UselessReverbAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String UselessReverbAudioProcessor::getProgramName(int index)
{
	return {};
}

void UselessReverbAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

void UselessReverbAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	for (int diffuserStep = 0; diffuserStep < m_diffusers.size(); ++diffuserStep) {
		m_diffusers[diffuserStep] = std::make_unique<Diffuser>(sampleRate);
	}
	m_feedbackLoop = std::make_unique<FeedbackLoop>(sampleRate);
	m_reverbBuffer.setSize(NUM_REVERB_CHANNELS, samplesPerBlock);
	m_reverbBuffer.clear();

	m_outputBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

	juce::dsp::ProcessSpec spec;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();
	spec.sampleRate = sampleRate;

	m_mixer.prepare(spec);

	m_lowPassFilter = std::make_unique<Filter>(juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, 1000));
	m_lowPassFilter->prepare(spec);
	m_lowPassFilter->reset();
}

void UselessReverbAudioProcessor::releaseResources()
{
	m_feedbackLoop.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool UselessReverbAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	juce::ignoreUnused(layouts);
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

	// split the signal into multiple channels and store them in m_reverbBuffer
	for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
		// TODO: use all channels of the input buffer, not just the first one
		int sourceChannel = 0;
		m_reverbBuffer.copyFrom(channel, 0, buffer, sourceChannel, 0, numSamples);
	}

	// diffuse the signal in the m_reverbBuffer using m_diffusers
	const float decayTime = m_valueTreeState.getParameterAsValue("decayTime").getValue();
	const float decayGain = FeedbackLoop::calculateDecayGain(decayTime);
	std::array<float, NUM_REVERB_CHANNELS> slice;
	for (int sample = 0; sample < numSamples; ++sample) {
		for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
			slice[channel] = m_reverbBuffer.getSample(channel, sample);
		}
		for (auto& diffuser : m_diffusers) {
			diffuser->processSample(slice);
		}
		m_feedbackLoop->processSample(slice, decayGain);
		for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
			m_reverbBuffer.setSample(channel, sample, slice[channel]);
		}
	}

	// merge the signal from the m_reverbBuffer channels
	m_outputBuffer.clear();
	const float gain = 1.f / NUM_REVERB_CHANNELS;
	for (int channel = 0; channel < NUM_REVERB_CHANNELS; ++channel) {
		m_outputBuffer.addFrom(0, 0, m_reverbBuffer, channel, 0, numSamples, gain);
		m_outputBuffer.addFrom(1, 0, m_reverbBuffer, channel, 0, numSamples, gain);
	}

	// add a low pass filter
	// TODO: don't just filter the entire thing afterwards, but do it per-sample between the diffuser and the feedback loop
	juce::dsp::AudioBlock<float> reverbBlock(m_outputBuffer);
	m_lowPassFilter->process(juce::dsp::ProcessContextReplacing<float>(reverbBlock));

	// mix the dry and wet signals
	// in order to avoid copying the samples, the dry and wet signals are interchanged
	const float mixPercentage = m_valueTreeState.getParameterAsValue("mix").getValue();
	m_mixer.setWetMixProportion(1.f - mixPercentage * 0.01f);
	m_mixer.pushDrySamples(m_outputBuffer);
	m_mixer.mixWetSamples(buffer);
}


//==============================================================================
bool UselessReverbAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* UselessReverbAudioProcessor::createEditor()
{
	return new UselessReverbAudioProcessorEditor(*this);
}

//==============================================================================
void UselessReverbAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void UselessReverbAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
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