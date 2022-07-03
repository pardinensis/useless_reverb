#pragma once

#include <JuceHeader.h>
#include "Delay.h"

class FeedbackLoop {
public:
	static const int NUM_CHANNELS = 8;

	FeedbackLoop(double sampleRate);

	void processSample(std::array<float, NUM_CHANNELS>& slice);

private:
	std::array<float, NUM_CHANNELS * NUM_CHANNELS> m_mixMatrix;
	std::vector<std::unique_ptr<Delay>> m_delays;
};
