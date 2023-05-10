//
// Created by seflue on 12.05.2023.
//

#ifndef NETWORK_COMPONENTS_DATAHANDLER_H
#define NETWORK_COMPONENTS_DATAHANDLER_H

#include <memory>
#include "../utils/CoinOrDice.h"

namespace user_equipment{
    class DataHandler {
    public:
        explicit DataHandler();
        bool isDataAvailable();
    private:
        std::unique_ptr<utils::CoinOrDice<2>> coin;
    };
}

#endif //NETWORK_COMPONENTS_DATAHANDLER_H
