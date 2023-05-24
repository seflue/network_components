#include "DataObjects.h"
#include <Poco/FileStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <memory>
#include <string>

namespace user_equipment {
class FileSender;

class FileSender {
  public:
    explicit FileSender(const std::string& file,
                        const user_equipment::UserConnection& userConnection);

    auto sendNext() -> bool;
    auto userConnection() -> UserConnection { return _userConnection; }

  private:
    UserConnection _userConnection;
    std::string _file;
    std::unique_ptr<Poco::FileInputStream> _fis;
    std::unique_ptr<Poco::Net::SocketAddress> _socketAddress;
    std::unique_ptr<Poco::Net::DatagramSocket> _socket;
    const int _bufferSize = 1024;
    std::unique_ptr<std::vector<char>> _buffer = std::make_unique<std::vector<char>>(_bufferSize);
};
} // namespace user_equipment
