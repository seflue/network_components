#include "UserEquipment.h"
#include "DataHandler.h"
#include "Logging.h"
#include "String.h"

#include <Poco/Util/HelpFormatter.h>

#include <grpcpp/grpcpp.h>
#include <regex>
#include <spdlog/spdlog.h>
#include <string>

namespace ue = user_equipment;

using Poco::Util::HelpFormatter;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::Application::EXIT_OK;

int ue::UserEquipment::main([[maybe_unused]] const std::vector<std::string>& args)
{
    if (_helpRequested) {
        displayHelp();
        return EXIT_OK;
    }

    while (true) {
        updateBsInfo();
        handleUserData();
        if (handleShutdown())
            break;
    }
    return EXIT_OK;
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

void ue::UserEquipment::initialize(Poco::Util::Application& self)
{
    loadConfiguration(); // load default configuration files, if present
    Application::initialize(self);

    initializeGrpcChannels();
    initializePeriodicScanning();
    initializeDataHandling();
    // further initialization code ...
}
void ue::UserEquipment::initializeDataHandling()
{
    _dataHandler = std::make_unique<DataHandler>(_ueid);
}
void ue::UserEquipment::initializePeriodicScanning()
{
    _timer = std::make_unique<Timer>();
}
void ue::UserEquipment::initializeGrpcChannels()
{
    for (const auto& bs : _baseStations)
        _controlBand.addChannel(bs);
}
void ue::UserEquipment::uninitialize()
{
    // further uninitialization code ...
    Application::uninitialize();
}

void ue::UserEquipment::reinitialize(Poco::Util::Application& self)
{
    // further reinitialization code ...
    Application::reinitialize(self);
}
void ue::UserEquipment::defineOptions(Poco::Util::OptionSet& options)
{
    Application::defineOptions(options);

    options.addOption(Option("help", "h", "display help information on command line arguments")
                          .required(false)
                          .repeatable(false)
                          .callback(OptionCallback(this, &UserEquipment::handleHelp)));

    options.addOption(Option("ueid", "i", "unique identifier for the user equipment")
                          .required(true)
                          .repeatable(false)
                          .argument("ueid", true)
                          .callback(OptionCallback(this, &UserEquipment::handleUeid)));

    options.addOption(Option("name", "n", "name of the user equipment")
                          .required(true)
                          .repeatable(false)
                          .argument("name", true)
                          .callback(OptionCallback(this, &UserEquipment::handleName)));

    options.addOption(Option("base-station", "b", "visible base station")
                          .required(true)
                          .repeatable(true)
                          .argument("baseStation", true)
                          .callback(OptionCallback(this, &UserEquipment::handleBaseStation)));
}
void ue::UserEquipment::handleHelp([[maybe_unused]] const std::string& name,
                                               [[maybe_unused]] const std::string& value)
{
    _helpRequested = true;
    displayHelp();
    stopOptionsProcessing();
}

void ue::UserEquipment::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A sample application that demonstrates some of the features of the "
                            "Poco::Util::Application class.");
    helpFormatter.format(std::cout);
}

void ue::UserEquipment::handleUeid([[maybe_unused]] const std::string& name,
                                               const std::string& value)
{
    _ueid = value;
}
void ue::UserEquipment::handleName([[maybe_unused]] const std::string& name,
                                               const std::string& value)
{
    _name = value;
}
void ue::UserEquipment::handleBaseStation([[maybe_unused]] const std::string& name,
                                                      const std::string& value)
{
    auto tokens = utils::string::split(value, ':');
    auto address = tokens.size() > 0 ? tokens[0] : "127.0.0.1";
    uint32_t port = tokens.size() > 1 ? std::stoi(tokens[1]) : 50051;
    auto quality = tokens.size() > 2 ? std::stoi(tokens[2]) : 0;
    _baseStations.push_back({{address, port}, quality});
}

auto ue::Timer::now() -> tp { return std::chrono::system_clock::now(); }

void ue::Timer::reset() { start_time = now(); }

auto ue::Timer::isExceeded() -> bool { return now() - duration > start_time; }

