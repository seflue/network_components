#include "UserEquipment.h"
#include "ControlClient.h"
#include "DataHandler.h"

#include <grpcpp/grpcpp.h>

user_equipment::UserEquipment::UserEquipment(
        const std::string& name,
        const std::string& ueid,
        const std::vector<BaseStation>& baseStations) {
    this->name = name;
    this->ueid = ueid;
    for(const auto& bs : baseStations)
        controlBand_.addChannel(bs);
    timer = std::make_unique<Timer>();
    dataHandler = std::make_unique<DataHandler>();
}

void user_equipment::UserEquipment::start() {
    while(true)
    {
        updateBsInfo();
        handleUserData();
        if(handleShutdown())
            break;
    }
}


void user_equipment::UserEquipment::shutdown() {
    isShutdownSent = true;
    std::cout << "Gracefully shutdown { " << name << " : " << ueid << "}\n";
}

void user_equipment::UserEquipment::updateBsInfo() {
    if(!timer->isExceeded())
        return;

    timer->reset();
    controlBand_.scan(ueid);
}

bool user_equipment::UserEquipment::handleShutdown() {
    return isShutdownSent;
}

void user_equipment::UserEquipment::handleUserData() {
    dataHandler->handleData();
}

auto user_equipment::Timer::now() -> tp {
    return std::chrono::system_clock::now();
}

void user_equipment::Timer::reset() {
    start_time = now();
}

auto user_equipment::Timer::isExceeded() -> bool {
    return now() - duration > start_time;
}
