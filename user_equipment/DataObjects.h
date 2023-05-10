//
// Created by seflue on 17.05.2023.
//

#ifndef NETWORK_COMPONENTS_DATAOBJECTS_H
#define NETWORK_COMPONENTS_DATAOBJECTS_H

#include <string>
namespace user_equipment{
    struct BaseStation {
        std::string address;
        int quality;
    };
}

#endif //NETWORK_COMPONENTS_DATAOBJECTS_H
