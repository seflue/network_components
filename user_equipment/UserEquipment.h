//
// Created by seflue on 11.05.2023.
//

#ifndef NETWORK_COMPONENTS_USEREQUIPMENT_H
#define NETWORK_COMPONENTS_USEREQUIPMENT_H

#include "controlplane.grpc.pb.h"
#include "DataHandler.h"
#include "ControlClient.h"
#include "DataObjects.h"
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <chrono>
#include <future>

namespace user_equipment {

    using tp = std::chrono::time_point<std::chrono::system_clock>;
    using ms = std::chrono::microseconds;
    using namespace std::chrono_literals;

    class Timer {
        tp start_time = std::chrono::system_clock::now();
        ms duration = 10s;
        tp now();
    public:
        void reset();
        bool isExceeded();
    };

    struct UserEquipment {
        std::string name;
        std::string ueid;
        explicit UserEquipment(
                const std::string& name,
                const std::string& ueid,
                const std::vector<BaseStation>& baseStations
        );

        void start();
        void shutdown();

    private:
        std::unique_ptr<user_equipment::Timer> timer;
        void updateBsInfo();
        bool handleShutdown();
        bool isShutdownSent = false;
        ControlClient controlBand_;
        std::unique_ptr<user_equipment::DataHandler> dataHandler;

        void handleUserData();
    };
}

#endif //NETWORK_COMPONENTS_USEREQUIPMENT_H
