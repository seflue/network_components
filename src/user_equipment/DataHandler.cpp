#include "DataHandler.h"
#include "Logging.h"
#include "Poco/Net/NetException.h"
#include <Poco/FileStream.h>
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <filesystem>
#include <fstream>
#include <iostream>

using namespace user_equipment;

DataHandler::DataHandler(const std::string &ueid) : ueid(ueid) {}

void DataHandler::handleData(ControlClient &controlClient)
{
    std::string file;
    if (nothingToSend())
        return;
    if (!sender_) {
        file = nextItem();
        if (!std::ifstream(file).good()) {
            LOG_ERROR("File {} not found!", file);
            return;
        }

        auto userConnection = controlClient.connect(ueid, file);
        if (!userConnection) {
            LOG_ERROR("Cannot establish userconnection.");
            return;
        }

        try {
            LOG_DEBUG("Try to connect to {} at {} ...",
                      userConnection->grpcSocket.toString(),
                      userConnection->udpPort);
            sender_ = std::make_unique<FileSender>(file, *userConnection);
        }
        catch (Poco::FileNotFoundException &e) {
            LOG_ERROR("Poco Exception: File {} not found!", file);
            return;
        }
        catch (Poco::Net::HostNotFoundException &e) {
            LOG_ERROR("File: {}, ", file);
            LOG_ERROR("Host not found for address {}:{}",
                      userConnection->grpcSocket.ip,
                      userConnection->udpPort);
            sleep(1);
            return;
        }
    }
    if (!sender_->sendNext()) {
        controlClient.disconnect(ueid, sender_->userConnection());
        sender_.reset();
        LOG_DEBUG("Disconnected {} and reset sender", ueid);
    }
}

void DataHandler::queueData(const std::string &file) { dataQueue_.push(file); }

bool DataHandler::nothingToSend() { return dataQueue_.empty() && !sender_; }

std::string DataHandler::nextItem()
{
    auto item = dataQueue_.front();
    dataQueue_.pop();
    LOG_DEBUG("Pop {} from queue.", item);
    return item;
}
