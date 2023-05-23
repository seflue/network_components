#ifndef NETWORK_COMPONENTS_USEREQUIPMENT_H
#define NETWORK_COMPONENTS_USEREQUIPMENT_H

#include "ControlClient.h"
#include "DataHandler.h"
#include "DataObjects.h"
#include "UserdataGenerator.h"
#include "controlplane.grpc.pb.h"
#include <chrono>
#include <format>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

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

class UserEquipment {
  public:
    explicit UserEquipment(const std::string &name, const std::string &ueid,
                           const std::vector<BaseStation> &baseStations);

    void start();
    auto toString() -> std::string;
    void shutdown();

  private:
    std::string _name;
    std::string _ueid;
    std::unique_ptr<user_equipment::Timer> _timer;
    void updateBsInfo();
    bool handleShutdown();
    bool _isShutdownSent = false;
    ControlClient _controlBand;
    std::unique_ptr<user_equipment::DataHandler> _dataHandler;

    void handleUserData();
    user_equipment::UserdataGenerator _userdataGenerator;
};
} // namespace user_equipment

#endif // NETWORK_COMPONENTS_USEREQUIPMENT_H
