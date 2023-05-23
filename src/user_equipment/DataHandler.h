//
// Created by seflue on 12.05.2023.
//

#ifndef NETWORK_COMPONENTS_DATAHANDLER_H
#define NETWORK_COMPONENTS_DATAHANDLER_H

#include "CoinOrDice.h"
#include "ControlClient.h"
#include "FileSender.h"
#include <memory>
#include <optional>
#include <queue>

namespace Poco {
class FileInputStream;
namespace Net {
class DatagramSocket;
class SocketAddress;
} // namespace Net
} // namespace Poco

namespace user_equipment {
struct DataChannel {
    std::string fileName;
    Socket grpcSocket;
    uint32_t udpPort;
};

class DataHandler {
  public:
    DataHandler(const std::string &basicString);

    void handleData(user_equipment::ControlClient &controlClient);
    void queueData(const std::string &file);

  private:
    bool nothingToSend();
    std::string nextItem();
    std::unique_ptr<user_equipment::FileSender> sender_;
    std::queue<std::string> dataQueue_;
    std::string ueid;
};
} // namespace user_equipment

#endif // NETWORK_COMPONENTS_DATAHANDLER_H
