#ifndef NETWORK_COMPONENTS_USEREQUIPMENT_H
#define NETWORK_COMPONENTS_USEREQUIPMENT_H

#include "ControlClient.h"
#include "DataHandler.h"
#include "DataObjects.h"
#include "UserdataGenerator.h"
#include "controlplane.grpc.pb.h"
#include <Poco/Util/Application.h>
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
    static tp now();

  public:
    void reset();
    bool isExceeded();
};

class UserEquipment : public Poco::Util::Application {
  public:
    void initialize(Application& self) override;
    void uninitialize() override;
    void reinitialize(Application& self) override;
    void defineOptions(Poco::Util::OptionSet& options) override;
    auto main(const std::vector<std::string>& args) -> int override;

    auto toString() -> std::string;
    void shutdown();

  private:
    void updateBsInfo();
    void displayHelp();
    void initializeGrpcChannels();
    void initializePeriodicScanning();
    void initializeDataHandling();
    void handleHelp(const std::string& name, const std::string& value);
    void handleUeid(const std::string& name, const std::string& value);
    void handleName(const std::string& name, const std::string& value);
    void handleBaseStation(const std::string& name, const std::string& value);
    [[nodiscard]] auto handleShutdown() const -> bool ;
    void handleUserData();

    std::string _name;
    std::string _ueid;
    std::unique_ptr<user_equipment::Timer> _timer;
    bool _isShutdownSent = false;
    ControlClient _controlBand;
    std::unique_ptr<user_equipment::DataHandler> _dataHandler;

    user_equipment::UserdataGenerator _userdataGenerator;
    bool _helpRequested;
    std::vector<BaseStation> _baseStations;
};
} // namespace user_equipment

#endif // NETWORK_COMPONENTS_USEREQUIPMENT_H
