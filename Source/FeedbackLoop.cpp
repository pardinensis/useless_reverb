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

float FeedbackLoop::calculateDecayGain(float decayTime) {
    // calculate the gain factor so that the signal fades to -60dB after $decayTime seconds
    // $targetGain is -60dB or 10^-6
    constexpr float targetGain = 0.000001f;

    // averageDelayTime is the mean delay time of all delay instances
    constexpr float averageDelayTime = 0.5f * (DELAY_TIME_MIN + DELAY_TIME_MAX);

    // the signal decays to -60dB after going through the delay $invIterations^-1 times
    const float invIterations = averageDelayTime / decayTime;

    // delayGain is the factor that the signal decays in every iteration
    // TODO: std::powf might be too expensive here
    return std::powf(targetGain, invIterations);
}

void FeedbackLoop::processSample(std::array<float, NUM_CHANNELS>& slice, float decayGain) {    
    float out[NUM_CHANNELS];
    float decayed[NUM_CHANNELS];
    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        const float delayed = m_delays[channel]->read();
        out[channel] = delayed;
        decayed[channel] = delayed * decayGain;
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
