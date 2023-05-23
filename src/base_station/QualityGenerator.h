//
// Created by seflue on 22.05.2023.
//

#ifndef NETWORK_COMPONENTS_QUALITYGENERATOR_H
#define NETWORK_COMPONENTS_QUALITYGENERATOR_H

#include "CoinOrDice.h"
#include <optional>

namespace base_station {
class QualityGenerator {
  public:
    explicit QualityGenerator() { selector.setDistribution({1, 2, 3, 3, 2}); }
    uint32_t generate() { return selector.getValue(); }

  private:
    utils::CoinOrDice<5> selector;
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_QUALITYGENERATOR_H
