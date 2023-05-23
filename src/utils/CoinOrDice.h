#ifndef NETWORK_COMPONENTS_COINORDICE_H
#define NETWORK_COMPONENTS_COINORDICE_H

#include <numeric>
#include <random>
#include <vector>

namespace utils {

template <uint32_t N> class CoinOrDice {
  public:
    CoinOrDice() : distribution(1u, N) {}

    void setDistribution(const std::vector<double> &probabilities)
    {
        if (probabilities.size() != N) {
            // Invalid probabilities vector size
            return;
        }

        // Normalize probabilities
        std::vector<double> normalizedProbabilities = probabilities;
        double sum =
            std::accumulate(normalizedProbabilities.begin(), normalizedProbabilities.end(), 0.0);
        if (sum > 0.0) {
            for (double &probability : normalizedProbabilities) {
                probability /= sum;
            }
        }

        distribution =
            std::discrete_distribution<uint32_t>(probabilities.begin(), probabilities.end());
    }

    uint32_t getValue() { return distribution(generator); }

  private:
    std::default_random_engine generator;
    std::discrete_distribution<uint32_t> distribution;
};
} // namespace utils

#endif // NETWORK_COMPONENTS_COINORDICE_H
