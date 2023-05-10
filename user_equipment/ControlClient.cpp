#include "ControlClient.h"
#include <string>

void user_equipment::ControlClient::scan(const std::string& ueid) {
    for(const auto& [address, channel] : channels_) {
        control::ScanRequest request;
        request.set_ueid(ueid);
        control::ScanReply reply;
        grpc::ClientContext context;

        // The actual RPC.
        grpc::Status status = channel.service->Scan(&context, request, &reply);

        // Act upon its status.
        if (status.ok()) {
            std::cout << ueid << ":" << channel.address << ", Quality: " << reply.quality() << '\n';
        } else {
            std::cout << status.error_code() << ": " << status.error_message()
                      << std::endl;
        }
    }
}

void user_equipment::ControlClient::addChannel(const BaseStation &bs) {
    channels_[bs.address] = {control::ControlService::NewStub(grpc::CreateChannel(
            bs.address,
            grpc::InsecureChannelCredentials())), bs.address, bs.quality};
}
