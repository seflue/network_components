#include "FileSender.h"
#include "DataObjects.h"

#include "Logging.h"

user_equipment::FileSender::FileSender(const std::string &file,
                                       const UserConnection &userConnection)
    : socket_(std::make_unique<Poco::Net::DatagramSocket>()), file_(file),
      userConnection_(userConnection), socketAddress_(std::make_unique<Poco::Net::SocketAddress>(
                                           userConnection.grpcSocket.ip, userConnection.udpPort)),
      fis_(std::make_unique<Poco::FileInputStream>(file))
{
    socket_->connect(*socketAddress_);
}

auto user_equipment::FileSender::sendNext() -> bool
{
    if (fis_->eof()) {
        LOG_DEBUG("Reached end of file {}", file_);
        return false;
    }

    fis_->read(buffer_->data(), bufferSize);
    try {
        socket_->sendBytes(buffer_->data(), fis_->gcount());
        LOG_DEBUG("Send {} bytes", fis_->gcount());
    }
    catch (const Poco::Exception ex) {
        LOG_ERROR("Error sending file: {}", ex.displayText());
    }
    return true;
}
