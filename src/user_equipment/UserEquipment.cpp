#include "UserEquipment.h"
#include "ControlClient.h"
#include "DataHandler.h"
#include "Logging.h"

#include <grpcpp/grpcpp.h>
#include <spdlog/spdlog.h>

namespace ue = user_equipment;

ue::UserEquipment::UserEquipment(const std::string& name,
                                 const std::string& ueid,
                                 const std::vector<BaseStation>& baseStations)
{
    this->_name = name;
    this->_ueid = ueid;
    for (const auto& bs : baseStations)
        _controlBand.addChannel(bs);
    _timer = std::make_unique<Timer>();
    _dataHandler = std::make_unique<DataHandler>(ueid);
}

void ue::UserEquipment::start()
{
    while (true) {
        updateBsInfo();
        handleUserData();
        if (handleShutdown())
            break;
    }
}

void ue::UserEquipment::shutdown()
{
    _isShutdownSent = true;
    LOG_DEBUG("Gracefully shutdown {{ {} : {} }}", _name, _ueid);
}

void ue::UserEquipment::updateBsInfo()
{
    if (!_timer->isExceeded())
        return;

    _timer->reset();
    _controlBand.scan(_ueid);
}

bool ue::UserEquipment::handleShutdown() const { return _isShutdownSent; }

void ue::UserEquipment::handleUserData()
{
    auto userData = _userdataGenerator.generate();
    if (userData)
        _dataHandler->queueData(*userData);
    _dataHandler->handleData(_controlBand);
}

auto ue::UserEquipment::toString() -> std::string { return std::format("{} : {}", _name, _ueid); }

auto ue::Timer::now() -> tp { return std::chrono::system_clock::now(); }

void ue::Timer::reset() { start_time = now(); }

auto ue::Timer::isExceeded() -> bool { return now() - duration > start_time; }
