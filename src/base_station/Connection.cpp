#include "Connection.h"
#include "FileConsumer.h"
#include "Logging.h"
#include <Poco/NObserver.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/Net/SocketNotification.h>
#include <Poco/Net/SocketReactor.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <utility>

namespace bs = base_station;
using SocketObserver = Poco::NObserver<bs::Connection, Poco::Net::ReadableNotification>;

void bs::Connection::connect(std::string ueid, std::unique_ptr<DataConsumer> dataConsumer)
{
    if (_connected) {
        LOG_ERROR("Connection from {} is already connected to {}",
                  ueid,
                  _socket->address().toString());
        return;
    }
    _ueid = std::move(ueid);
    _dataConsumer = std::move(dataConsumer);
    start();
}

void bs::Connection::disconnect()
{
    if (!_connected)
        return;
    stop();
    _connected = false;
}

void bs::Connection::start()
{
    try {
        openSocket();
        _dataConsumer->open(_ueid);
        startReactor();
        _connected = true;
    }
    catch (const std::exception& ex) {
        spdlog::get("stderr")->error("Error starting receiver: {}", ex.what());
    }
}

void bs::Connection::stop()
{
    stopReactor();
    _dataConsumer->close();
}

void bs::Connection::startReactor()
{
    _reactor = std::make_unique<Poco::Net::SocketReactor>();
    _observer = std::make_unique<SocketObserver>(*this, &Connection::onSocketReadable);
    _reactor->addEventHandler(*_socket, *_observer);
    _reactorThread = std::thread(&Connection::runReactor, this);
    LOG_DEBUG("{} Reactor started", toString());
}

void bs::Connection::openSocket()
{
    _socket = std::make_unique<Poco::Net::DatagramSocket>();
    Poco::Net::SocketAddress recipient(Poco::Net::IPAddress(), 0);
    _socket->bind(recipient);
    LOG_DEBUG("Socket bound to {} ", _socket->address().toString());
    _udpPort = _socket->address().port();
    LOG_DEBUG("Port set to {} ", _udpPort);
}

bool bs::Connection::isAvailable() const { return !_connected && !_receiver.joinable(); }

void bs::Connection::onSocketReadable(const Notification& n)
{
    LOG_DEBUG("Notification {} from Socket {}", n->socket().address().toString(), n->name());
    Poco::Net::SocketAddress sender;
    std::vector<char> buffer(1024);

    try {
        LOG_DEBUG("About to read from buffer ...");
        auto bytesRead =
            _socket->receiveFrom(buffer.data(), static_cast<int>(buffer.size()), sender);
        LOG_DEBUG("Received {} bytes from {}", bytesRead, sender.toString());
        LOG_DEBUG("Buffer data: {}", std::string(buffer.begin(), buffer.begin() + bytesRead));
        _dataConsumer->consume(buffer.data(), bytesRead);
    }
    catch (const Poco::Exception& ex) {
        LOG_ERROR("Error receiving data: {}", ex.displayText());
    }
}

void bs::Connection::runReactor()
{
    try {
        LOG_DEBUG("{}: About to run reactor", toString());
        _reactor->run();
        LOG_DEBUG("{}: Reactor stopped.", toString());
    }
    catch (const Poco::Exception& ex) {
        LOG_ERROR("Error running reactor: {}", ex.displayText());
    }
}

void bs::Connection::stopReactor()
{
    if (!_reactor) {
        LOG_ERROR("{}: Try to stop, but no reactor running.", toString());
        return;
    }

    LOG_DEBUG("{}: Stop reactor", toString());
    _reactor->stop();

    if (!_reactorThread.joinable())
        return;

    LOG_DEBUG("{}: Join reactor thread", toString());
    _reactorThread.join();
}
