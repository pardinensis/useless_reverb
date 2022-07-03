#include "FeedbackLoop.h"

#include <chrono>
#include <random>
#include "MatrixUtils.h"

static constexpr float DELAY_TIME_MIN = 0.10f;
static constexpr float DELAY_TIME_MAX = 0.20f;

FeedbackLoop::FeedbackLoop(double sampleRate) {
    // create delays with a randomized delay time
    auto seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> distribution(DELAY_TIME_MIN, DELAY_TIME_MAX);
    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        float delayTime = distribution(rng);
        int numSamples = std::max(1, static_cast<int>(delayTime * sampleRate));
        m_delays.push_back(std::make_unique<Delay>(numSamples));
    }

    // create a householder matrix to smear the delayed signal across all channels
    m_mixMatrix = MatrixUtils::createRandomHouseholderMatrix<float, NUM_CHANNELS>();
}

/*
void redistributeSample(const juce::dsp::Matrix<float>& in, juce::dsp::Matrix<float>& out) {
    jassert(in.getNumColumns() == 1);
    jassert(out.getNumColumns() == 1);
    const int numChannelsIn = in.getNumRows();
    const int numChannelsOut = out.getNumRows();
    
    int channelIn = 0;
    int channelOut = 0;
    int scanline = 0;
    while (scanline < numChannelsIn * numChannelsOut) {
        int nextChannelIn = channelIn + numChannelsOut;
        int nextChannelOut = channelOut + numChannelsIn;
        if (nextChannelIn < nextChannelOut) {
            float interpolationFactor = float(nextChannelIn - scanline) / numChannelsIn;
            out(channelOut / numChannelsIn, 0) = in(channelIn / numChannelsOut, 0) * interpolationFactor;
            channelIn = nextChannelIn;
            scanline = nextChannelIn;
        }
        else {
            float interpolationFactor = float(nextChannelOut - scanline) / numChannelsIn;
            out(channelOut / numChannelsIn, 0) = in(channelIn / numChannelsOut, 0) * interpolationFactor;
            channelOut = nextChannelOut;
            scanline = nextChannelOut;
        }
    }
}*/

void FeedbackLoop::processSample(std::array<float, NUM_CHANNELS>& slice) {
    const float delayGain = 0.85f;
    
    float out[NUM_CHANNELS];
    float decayed[NUM_CHANNELS];
    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        const float delayed = m_delays[channel]->read();
        out[channel] = delayed;
        decayed[channel] = delayed * delayGain;
    }

    float mixed[NUM_CHANNELS];
    for (int row = 0; row < NUM_CHANNELS; ++row) {
        mixed[row] = 0;
        for (int column = 0; column < NUM_CHANNELS; ++column) {
            mixed[row] += decayed[column] * m_mixMatrix[row * NUM_CHANNELS + column];
        }
    }

    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        m_delays[channel]->write(mixed[channel] + slice[channel]);
        m_delays[channel]->next();

        slice[channel] = out[channel];
    }
}
