//
// Created by seflue on 17.05.2023.
//

#ifndef NETWORK_COMPONENTS_COINORDICE_H
#define NETWORK_COMPONENTS_COINORDICE_H

#include <random>
#include <vector>
#include <numeric>

namespace utils {

template <int N>
class CoinOrDice {
public:
    CoinOrDice() : distribution(1, N) {}

    void setDistribution(const std::vector<double>& probabilities) {
        if (probabilities.size() != N) {
            // Invalid probabilities vector size
            return;
        }

        // Normalize probabilities
        std::vector<double> normalizedProbabilities = probabilities;
        double sum = std::accumulate(normalizedProbabilities.begin(), normalizedProbabilities.end(), 0.0);
        if (sum > 0.0) {
            for (double& probability : normalizedProbabilities) {
                probability /= sum;
            }
        }

        distribution = std::discrete_distribution<>(probabilities.begin(), probabilities.end());
    }

    int getValue() {
        return distribution(generator);
    }

private:
    std::default_random_engine generator;
    std::discrete_distribution<> distribution;
};
} // utils

#endif //NETWORK_COMPONENTS_COINORDICE_H
