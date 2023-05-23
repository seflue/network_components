#include "FileSender.h"
#include "DataObjects.h"

#include "Logging.h"

user_equipment::FileSender::FileSender(const std::string &file,
                                       const UserConnection &userConnection) :
    _userConnection(userConnection),
    _file(file),
    _fis(std::make_unique<Poco::FileInputStream>(file)),
    _socketAddress(std::make_unique<Poco::Net::SocketAddress>(userConnection.grpcSocket.ip,
                                                              userConnection.udpPort)),
    _socket(std::make_unique<Poco::Net::DatagramSocket>())
{
    _socket->connect(*_socketAddress);
}

auto user_equipment::FileSender::sendNext() -> bool
{
    if (_fis->eof()) {
        LOG_DEBUG("Reached end of file {}", _file);
        return false;
    }

    _fis->read(_buffer->data(), _bufferSize);
    try {
        _socket->sendBytes(_buffer->data(), _fis->gcount());
        LOG_DEBUG("Send {} bytes", _fis->gcount());
    }
    catch (const Poco::Exception &ex) {
        LOG_ERROR("Error sending file: {}", ex.displayText());
    }
    return true;
}
