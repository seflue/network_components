#include <memory>
#include <signal.h>
#include <string>
#include <vector>

#include "Logging.h"

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "UserEquipment.h"

using user_equipment::BaseStation;
using user_equipment::UserEquipment;
using namespace utils::logging;

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

std::function<void(int)> shutdown_handler;

/**
 * This wrapper is needed, because signal is not able to handle a lambda with
 * captures directly.
 */
void signal_handler(int signum) { shutdown_handler(signum); }

int main()
{
    initializeLogger();
    LOG_INFO("Starting user equipment workers:");
    std::vector<BaseStation> stations = {
        {{"127.0.0.1", 50051}, 10},
        //{{"127.0.0.2", 50051}, 8},
    };
    std::vector<std::unique_ptr<UserEquipment>> users;
    users.push_back(std::make_unique<UserEquipment>("Alice", "0001", stations));
    // users.push_back(std::make_unique<UserEquipment>("Bob", "0002", stations));

    shutdown_handler = [&](int signum) {
        for (auto &&user : users)
            user->shutdown();
        exit(signum);
    };
    signal(SIGINT, signal_handler);

    std::vector<std::thread> workers;

    for (auto &&user : users) {
        workers.push_back(std::thread([&] { user->start(); }));
        LOG_INFO("Worker {} started.", user->toString());
    }
    for (auto &&worker : workers)
        worker.join();
    return 0;
}
