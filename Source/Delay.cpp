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
