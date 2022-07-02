#include "Delay.h"

#include <cassert>

Delay::Delay(int numSamples) : m_delayBufferSamples(numSamples), m_currentDelayBufferIndex(0) {
	m_delayBuffer.setSize(1, numSamples);
	m_delayBuffer.clear();
}

float Delay::read() const {
	return m_delayBuffer.getSample(0, m_currentDelayBufferIndex);
}

void Delay::write(float value) {
	m_delayBuffer.setSample(0, m_currentDelayBufferIndex, value);
}

void Delay::next() {
	m_currentDelayBufferIndex = (m_currentDelayBufferIndex + 1) % m_delayBufferSamples;
}

/*
void Delay::processBlock(juce::AudioBuffer<float>& block) {
	const auto numSamples = block.getNumSamples();
	const auto numChannels = block.getNumChannels();
	assert(numChannels == m_delayBufferChannels);

	for (int channel = 0; channel < numChannels; ++channel) {
		float* blockData = block.getWritePointer(channel);
		float* delayData = m_delayBuffer.getWritePointer(channel);

		int delayBufferIndex = m_currentDelayBufferIndex;

		for (int sample = 0; sample < numSamples; ++sample) {
			float in = blockData[sample];
			float out = delayData[delayBufferIndex];
			delayData[delayBufferIndex] = m_feedback * in;
			blockData[sample] = out;

			delayBufferIndex = (delayBufferIndex + 1) % m_delayBufferSamples;
		}
	}

	m_currentDelayBufferIndex = (m_currentDelayBufferIndex + numSamples) % m_delayBufferSamples;
}
*/