//
// Created by seflue on 20.05.2023.
//

#ifndef NETWORK_COMPONENTS_CONNECTION_H
#define NETWORK_COMPONENTS_CONNECTION_H

#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <fstream>
#include <optional>
#include <string>
#include <thread>

namespace base_station {
using Notification = Poco::AutoPtr<Poco::Net::ReadableNotification>;

class Connection {
  public:
    // explicit Connection(uint32_t udpPort);
    explicit Connection() : _file(), _reactor(nullptr), _socket(nullptr) {}
    void connect(std::string ueid, std::string filename);
    void disconnect();
    bool isAvailable() const;
    std::string ueid() const { return _ueid; }
    uint32_t udpPort() const { return _udpPort; }
    std::string toString() const { return std::format("{}:{}:{}", _ueid, _addr, _udpPort); }

  private:
    void receiveData();
    void start();
    auto openFile() -> bool;
    void closeFile();
    void onSocketReadable(const Notification &n);
    void runReactor();
    void stop();

    std::string _filename;
    std::ofstream _file;
    std::string _ueid;
    uint32_t _udpPort;
    std::thread _receiver;
    std::unique_ptr<Poco::Net::SocketReactor> _reactor;
    std::unique_ptr<Poco::Net::DatagramSocket> _socket;
    std::thread _reactorThread;
    std::atomic<bool> _connected;
    std::atomic<bool> waitingForTimeout_;
    std::atomic<bool> _allowDisconnect;
    std::string _addr;

    void openSocket();

    void startReactor();
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_CONNECTION_H
