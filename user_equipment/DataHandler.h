//
// Created by seflue on 12.05.2023.
//

#ifndef NETWORK_COMPONENTS_DATAHANDLER_H
#define NETWORK_COMPONENTS_DATAHANDLER_H

#include <memory>
#include <queue>
#include <optional>
#include "../utils/CoinOrDice.h"
namespace Poco {
    class FileInputStream;
    namespace Net {
        class DatagramSocket;
        class SocketAddress;
    }
}

namespace user_equipment{
    class FileSender;

    class DataHandler {
    public:
        void handleData(const std::string& address);
        void queueData(const std::string& file);
    private:
        bool nothingToSend();
        std::string nextItem();
        std::unique_ptr<FileSender> sender_;
        std::queue<std::string> dataQueue_;
    };

    class FileSender {
    public:
        explicit FileSender(const std::string& file, const std::string& address);

        bool sendNext();

    private:
        std::string file_;
        std::string address_;
        std::unique_ptr<Poco::Net::DatagramSocket> socket_;
        std::unique_ptr<Poco::FileInputStream> fis_;
        std::unique_ptr<Poco::Net::SocketAddress> socketAddress_;
        const static int bufferSize=1024;
        char buffer_[bufferSize];
    };
}

#endif //NETWORK_COMPONENTS_DATAHANDLER_H
