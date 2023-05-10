#include <memory>
#include <string>
#include <vector>
#include <signal.h>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

#include "UserEquipment.h"

using user_equipment::UserEquipment;
using user_equipment::BaseStation;

ABSL_FLAG(std::string, target, "localhost:50051", "Server address");

std::function<void(int)> shutdown_handler;

/**
 * This wrapper is needed, because signal is not able to handle a lambda with
 * captures directly.
 */
void signal_handler(int signum) {
    shutdown_handler(signum);
}

int main(int argc, char** argv) {
    std::vector<BaseStation> stations = {
      {"127.0.0.1:50051", 10},
      {"127.0.0.1:50052", 8},
    };
    std::vector<std::unique_ptr<UserEquipment>> users;
    users.push_back(std::make_unique<UserEquipment>("Alice", "0001", stations));
    users.push_back(std::make_unique<UserEquipment>("Bob", "0002", stations));

    shutdown_handler = [&] (int signum){
        for (auto&& user : users)
            user->shutdown();
        exit(signum);
    };
    signal(SIGINT, signal_handler);

    std::vector<std::thread> workers;

    for (auto&& user: users) {
        workers.push_back(std::thread([&]{ user->start();}));
    }
    for (auto&& worker : workers)
        worker.join();
    return 0;
}
