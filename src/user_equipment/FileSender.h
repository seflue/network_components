#include "DataObjects.h"
#include <Poco/FileStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <memory>
#include <string>

namespace user_equipment {
class FileSender;
class Socket;

class FileSender {
  public:
    explicit FileSender(const std::string &file,
                        const user_equipment::UserConnection &userConnection);

    auto sendNext() -> bool;
    auto userConnection() -> UserConnection { return userConnection_; }

  private:
    std::string file_;
    std::unique_ptr<Poco::Net::DatagramSocket> socket_;
    std::unique_ptr<Poco::FileInputStream> fis_;
    std::unique_ptr<Poco::Net::SocketAddress> socketAddress_;
    UserConnection userConnection_;
    const size_t bufferSize = 1024;
    std::unique_ptr<std::vector<char>> buffer_ = std::make_unique<std::vector<char>>(bufferSize);
};
} // namespace user_equipment
