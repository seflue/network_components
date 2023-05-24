#include "ControlClient.h"
#include "DataObjects.h"
#include "Logging.h"
#include <grpcpp/create_channel.h>
#include <ranges>
#include <string>

namespace rv = std::ranges::views;
using std::ranges::max_element;

void user_equipment::ControlClient::scan(const std::string& ueid)
{
    for (auto& channel : std::views::values(channels_)) {
        control::ScanRequest request;
        request.set_ueid(ueid);
        control::ScanReply reply;
        grpc::ClientContext context;

        auto status = channel.service->Scan(&context, request, &reply);

        if (status.ok()) {
            LOG_DEBUG("{} : {} : {}, Quality: {}",
                      ueid,
                      channel.grpcSocket.ip,
                      channel.grpcSocket.port,
                      reply.quality());
            channel.quality = reply.quality();
        }
        else {
            LOG_ERROR("{}: {}", static_cast<int>(status.error_code()), status.error_message());
        }
    }
}

void user_equipment::ControlClient::addChannel(const BaseStation& bs)
{
    channels_[bs.grpcSocket] = Channel{
        control::ControlService::NewStub(
            grpc::CreateChannel(bs.grpcSocket.toString(), grpc::InsecureChannelCredentials())),
        bs.grpcSocket,
        bs.quality};
}

auto user_equipment::ControlClient::connect(const std::string& ueid, const std::string& filename)
    -> std::optional<UserConnection>
{
    const auto best = bestChannel();
    if (!best) {
        LOG_ERROR("No connectable channel available.");
        return std::nullopt;
    }

    control::ConnectionRequest request;
    request.set_ueid(ueid);
    request.set_filename(filename);
    control::ConnectionReply reply;
    grpc::ClientContext context;
    auto status = best->service->Connect(&context, request, &reply);

    if (status.ok()) {
        const auto udpPort = reply.dataport();
        return UserConnection{best->grpcSocket, udpPort};
    }
    else {
        LOG_ERROR("Could not connect to {}: {}",
                  best->grpcSocket.toString(),
                  static_cast<int>(reply.error()));
        return std::nullopt;
    }
}

void user_equipment::ControlClient::disconnect(const std::string& ueid, const UserConnection& conn)
{
    if (!channels_.contains(conn.grpcSocket)) {
        LOG_ERROR("No user connection to {}", conn.grpcSocket.toString());
        return;
    }
    control::DisconnectionRequest request;
    request.set_ueid(ueid);
    request.set_dataport(conn.udpPort);
    control::DisconnectionReply reply;
    grpc::ClientContext context;
    const auto& channel = channels_[conn.grpcSocket];
    auto status = channel.service->Disconnect(&context, request, &reply);
}

auto user_equipment::ControlClient::bestChannel() const -> const Channel*
{
    auto is_connectable = [](const Channel& c) { return c.quality.has_value(); };
    auto quality_comparator = [](const Channel& c1, const Channel& c2) {
        return *c1.quality < *c2.quality;
    };

    auto validChannels = std::views::values(channels_) | rv::filter(is_connectable);
    auto best = std::ranges::max_element(validChannels, quality_comparator);

    if (best == validChannels.end()) {
        LOG_ERROR("No available base station found!");
        return nullptr;
    }

    return &(*best);
}
