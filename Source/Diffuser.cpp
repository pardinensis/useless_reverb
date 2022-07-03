#include "Diffuser.h"

#include "MatrixUtils.h"
#include <chrono>
#include <random>

static constexpr float DELAY_TIME_MAX = 0.050f;

Diffuser::Diffuser(double sampleRate) {
    // create delays with a randomized delay time
    auto seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 rng(seed);
    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        const float delayTimeMin = DELAY_TIME_MAX / NUM_CHANNELS * channel;
        const float delayTimeMax = DELAY_TIME_MAX / NUM_CHANNELS * (channel + 1);
        float delayTime = std::uniform_real_distribution<float>(delayTimeMin, delayTimeMax)(rng);
        int numSamples = std::max(1, static_cast<int>(delayTime * sampleRate));
        m_delays.push_back(std::make_unique<Delay>(numSamples));
    }

    // create flip polarity flags
    std::uniform_int_distribution<int> binary_distribution(0, 1);
    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        m_flipPolarity[channel] = binary_distribution(rng) * 2.f - 1.f;
    }

    m_mixMatrix = MatrixUtils::createRandomHouseholderMatrix<float, NUM_CHANNELS>();
}

void Diffuser::processSample(std::array<float, NUM_CHANNELS>& slice) {
    float tmp[NUM_CHANNELS];
    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        tmp[channel] = m_delays[channel]->read() * m_flipPolarity[channel];
        m_delays[channel]->write(slice[channel]);
        m_delays[channel]->next();
    }

    float mixed[NUM_CHANNELS];
    for (int row = 0; row < NUM_CHANNELS; ++row) {
        mixed[row] = 0;
        for (int column = 0; column < NUM_CHANNELS; ++column) {
            mixed[row] += tmp[column] * m_mixMatrix[row * NUM_CHANNELS + column];
        }
    }

    for (int channel = 0; channel < NUM_CHANNELS; ++channel) {
        slice[channel] = mixed[channel];
    }
}
