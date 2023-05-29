#include "Poco/Util/Application.h"
#include "UserEquipment.h"
#include "Logging.h"
#include <csignal>

using user_equipment::BaseStation;
using user_equipment::UserEquipment;

std::function<void()> shutdownHandler;

void signalHandler([[maybe_unused]] int signum)
{
    shutdownHandler();
}

int main(int argc, char** argv)
{
    utils::logging::initializeLogger();
    auto pApp = std::make_unique<UserEquipment>();
    shutdownHandler = [&](){
        pApp->shutdown();
    };
    signal(SIGINT, signalHandler);
    try {
        pApp->init(argc, argv);
    }
    catch (Poco::Exception& exc) {
        pApp->logger().log(exc);
        return Poco::Util::Application::EXIT_CONFIG;
    }
    return pApp->run();
};
