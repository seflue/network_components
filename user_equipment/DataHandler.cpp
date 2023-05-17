//
// Created by seflue on 12.05.2023.
//

#include "DataHandler.h"
#include <Poco/Net/DatagramSocket.h>
#include <Poco/Net/SocketAddress.h>
#include <Poco/FileStream.h>

void user_equipment::DataHandler::handleData(const std::string& address) {
    if (nothingToSend())
        return;
    if(!sender_)
        sender_ = std::make_unique<user_equipment::FileSender>(nextItem(), address);
    if(!sender_->sendNext())
        sender_.reset();
}

void user_equipment::DataHandler::queueData(const std::string& file) {
    dataQueue_.push(file);
}

bool user_equipment::DataHandler::nothingToSend() {
    return dataQueue_.empty() && sender_;
}

std::string user_equipment::DataHandler::nextItem() {
    auto item = dataQueue_.front();
    dataQueue_.pop();
    return item;
}

user_equipment::FileSender::FileSender(
        const std::string& file,
        const std::string& address) :
        file_(file),
        address_(address),
        fis_(std::make_unique<Poco::FileInputStream>(file))
        {}

bool user_equipment::FileSender::sendNext() {
    if(fis_->eof())
        return false;

    fis_->read(buffer_, bufferSize);
    socket_->sendTo(buffer_, fis_->gcount(), *socketAddress_);
    return true;
}

