#pragma once

#include <array>
#include <chrono>
#include <random>

namespace MatrixUtils {
	
	template<typename T, int numDimensions>
	std::array<T, numDimensions * numDimensions> createHouseholderMatrix(const std::array<T, numDimensions>& unitVector) {
		std::array<T, numDimensions * numDimensions> P;
		for (int i = 0; i < numDimensions; ++i) {
			for (int j = 0; j < numDimensions; ++j) {
				float identity = (i == j) ? 1.f : 0.f;
				P[i * numDimensions + j] = identity - 2 * unitVector[i] * unitVector[j];
			}
		}
		return P;
	}

	template<typename T, int numDimensions>
	std::array<T, numDimensions * numDimensions> createRandomHouseholderMatrix() {
		auto seed = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
		std::mt19937 rng(seed);
		std::normal_distribution<float> distribution;

		std::array<T, numDimensions> unitVector;
		float normSquared{ 0.f };
		for (int i = 0; i < numDimensions; ++i) {
			float value = distribution(rng);
			normSquared += value * value;
			unitVector[i] = value;
		}
		float invNorm = 1.f / std::sqrt(normSquared);
		for (int i = 0; i < numDimensions; ++i) {
			unitVector[i] *= invNorm;
		}

		return createHouseholderMatrix<T, numDimensions>(unitVector);
	}
}