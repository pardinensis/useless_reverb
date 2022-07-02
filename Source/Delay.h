#pragma once

#include <JuceHeader.h>

class Delay {
private:
	juce::AudioBuffer<float> m_delayBuffer;
	const int m_delayBufferSamples;
	int m_currentDelayBufferIndex;

public:
	Delay(int numSamples);
	~Delay() = default;

	Delay(const Delay&) = delete;
	Delay(Delay&&) = delete;
	Delay& operator=(const Delay&) = delete;
	Delay& operator=(Delay&&) = delete;

public:
	float read() const;
	void write(float value);
	void next();
};
