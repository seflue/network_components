#include "Logging.h"
#include <Poco/NObserver.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAcceptor.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>

#include "Connection.h"

using namespace base_station;
namespace fs = std::filesystem;

// Connection::Connection(uint32_t udpPort)
//     : _filename(),
//       _file(),
//       _reactor(nullptr),
//       _socket(nullptr)
//{ }

void Connection::connect(std::string ueid, std::string filename)
{
    if (_connected)
        return;
    _ueid = ueid;
    _filename = filename;
    start();
    //_connected = true;
    //_receiver = std::thread(&Connection::receiveData, this);
}

void Connection::start()
{
    try {
        openFile();
        openSocket();
        startReactor();
        _connected = true;
    }
    catch (const std::exception &ex) {
        spdlog::get("stderr")->error("Error starting receiver: {}", ex.what());
    }
}

void Connection::startReactor()
{
    _reactor = std::make_unique<Poco::Net::SocketReactor>();
    _reactor->addEventHandler(*_socket,
                              Poco::NObserver<Connection, Poco::Net::ReadableNotification>(
                                  *this,
                                  &Connection::onSocketReadable));
    _reactorThread = std::thread(&Connection::runReactor, this);
    LOG_DEBUG("Reactor started");
}

void Connection::openSocket()
{
    _socket = std::make_unique<Poco::Net::DatagramSocket>();
    Poco::Net::SocketAddress recipient(Poco::Net::IPAddress(), 0);
    _socket->bind(recipient);
    LOG_DEBUG("Socket bound to {} ", _socket->address().toString());
}

void Connection::disconnect()
{
    if (!_connected)
        return;
    stop();
    // if(!_receiver.joinable())
    //     return;
    //_receiver.join();
}

bool Connection::isAvailable() const { return !_connected && !_receiver.joinable(); }

void Connection::receiveData()
{
    auto sa = Poco::Net::SocketAddress(Poco::Net::IPAddress(), _udpPort);
    auto dgs = Poco::Net::DatagramSocket(sa, true);

    std::vector<char> buffer(1024);

    const std::chrono::milliseconds timeout(5000);
    auto startTime = std::chrono::steady_clock::now();

    try {
        while (true) {
            Poco::Net::SocketAddress sender;
            std::size_t bytesRead;
            try {
                bytesRead = dgs.receiveFrom(buffer.data(), buffer.size() - 1, sender);
            }
            catch (const Poco::Exception &ex) {
                LOG_ERROR("Error receiving data: {}", ex.displayText());
                break;
            }
            if (!_connected) {
                if (waitingForTimeout_) {
                    auto currentTime = std::chrono::steady_clock::now();
                    if (currentTime - startTime >= timeout) {
                        LOG_DEBUG("Timeout exceeded, stop reception.");
                        break; // Timeout exceeded, stop receiving
                    }
                }

                LOG_DEBUG("Received disconnect, waiting for timeout.");
                startTime = std::chrono::steady_clock::now();
                waitingForTimeout_ = true;
            }
            if (bytesRead == 0) {
                LOG_DEBUG("No more data to receive");
                break; // No more data to receive
            }

            LOG_DEBUG("{} bytes received from {}", bytesRead, sender.toString());
            _file.write(buffer.data(), bytesRead);
            LOG_DEBUG("{} written to file {}", buffer.data(), _filename);
        }

        LOG_DEBUG("File received successfully.");
    }
    catch (const Poco::Exception &ex) {
        LOG_ERROR("Error receiving data: {}", ex.displayText());
    }

    _file.close();
}

auto Connection::openFile() -> bool
{
    auto filePath = fs::path(_filename);
    auto directoryPath = filePath.parent_path();
    auto fullDirPath = _ueid + "/" + directoryPath.string();

    if (!fs::exists(fullDirPath) && !fs::create_directories(fs::path(fullDirPath))) {
        LOG_ERROR("Failed to create directory: {}", fullDirPath);
        return false;
    }

    std::string fullFilePath = fullDirPath + "/" + filePath.filename().string();
    std::ofstream _file(fullFilePath, std::ios::binary);
    if (!_file) {
        LOG_ERROR("Failed to open file {}", fullFilePath);
        return false;
    }
    LOG_DEBUG("File {} successfully opened.", fullFilePath);
    return true;
}

void Connection::onSocketReadable(const Notification &n)
{
    Poco::Net::SocketAddress sender;
    std::vector<char> buffer(1024);

    try {
        LOG_DEBUG("About to read from buffer ...");
        std::size_t bytesRead = _socket->receiveFrom(buffer.data(), buffer.size(), sender);

        if (bytesRead > 0) {
            _file.write(buffer.data(), bytesRead);
            LOG_DEBUG("Received {} bytes from {}", bytesRead, sender.toString());
        }
        _allowDisconnect = true;
    }
    catch (const Poco::Exception &ex) {
        LOG_ERROR("Error receiving data: {}", ex.displayText());
    }
}

void Connection::runReactor()
{
    try {
        _reactor->run();
        LOG_DEBUG("{}: Reactor started.", toString());
    }
    catch (const Poco::Exception &ex) {
        LOG_ERROR("Error running reactor: {}", ex.displayText());
    }
}

void Connection::stop()
{
    const std::chrono::milliseconds timeout(5000);
    auto startTime = std::chrono::steady_clock::now();
    while(true)
    {
        if(_allowDisconnect)
            break;
        sleep(0.1);
        auto current = std::chrono::steady_clock::now();
        if(startTime + timeout < current) {
            LOG_DEBUG("Timed out.");
            break;
        }
    }
    if (_reactor) {
        LOG_DEBUG("{}: Stop reactor", toString());
        _reactor->stop();
        if (_reactorThread.joinable()) {
            LOG_DEBUG("{}: Join reactor thread", toString());
            _reactorThread.join();
        }
    }

    LOG_DEBUG("Close file.");
    _file.close();
    _connected = false;
}
