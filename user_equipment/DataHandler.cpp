//
// Created by seflue on 12.05.2023.
//

#include "DataHandler.h"

user_equipment::DataHandler::DataHandler() {
    coin = std::make_unique<utils::CoinOrDice<2>>();
    coin->setDistribution({0.999, 0.001});
}

bool user_equipment::DataHandler::isDataAvailable() {
    return coin->getValue();
}
