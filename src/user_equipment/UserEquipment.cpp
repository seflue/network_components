#include "UserEquipment.h"
#include "ControlClient.h"
#include "DataHandler.h"
#include "Logging.h"

#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>

user_equipment::UserEquipment::UserEquipment(const std::string &name, const std::string &ueid,
                                             const std::vector<BaseStation> &baseStations)
{
    this->_name = name;
    this->_ueid = ueid;
    for (const auto &bs : baseStations)
        _controlBand.addChannel(bs);
    _timer = std::make_unique<Timer>();
    _dataHandler = std::make_unique<DataHandler>(ueid);
}

void user_equipment::UserEquipment::start()
{
    while (true) {
        updateBsInfo();
        handleUserData();
        if (handleShutdown())
            break;
    }
}

void user_equipment::UserEquipment::shutdown()
{
    _isShutdownSent = true;
    LOG_DEBUG("Gracefully shutdown {{ {} : {} }}", _name, _ueid);
}

void user_equipment::UserEquipment::updateBsInfo()
{
    if (!_timer->isExceeded())
        return;

    _timer->reset();
    _controlBand.scan(_ueid);
}

bool user_equipment::UserEquipment::handleShutdown() { return _isShutdownSent; }

void user_equipment::UserEquipment::handleUserData()
{
    auto userData = _userdataGenerator.generate();
    if (userData)
        _dataHandler->queueData(*userData);
    _dataHandler->handleData(_controlBand);
}

auto user_equipment::UserEquipment::toString() -> std::string
{
    return std::format("{} : {}", _name, _ueid);
}

auto user_equipment::Timer::now() -> tp { return std::chrono::system_clock::now(); }

void user_equipment::Timer::reset() { start_time = now(); }

auto user_equipment::Timer::isExceeded() -> bool { return now() - duration > start_time; }
