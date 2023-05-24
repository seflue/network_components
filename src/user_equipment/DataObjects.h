#ifndef NETWORK_COMPONENTS_DATAOBJECTS_H
#define NETWORK_COMPONENTS_DATAOBJECTS_H

#include "controlplane.grpc.pb.h"
#include <string>

namespace user_equipment {

struct Socket {
    std::string ip;
    uint32_t port;
    [[nodiscard]] std::string toString() const;
    bool operator<(const Socket& rhs) const { return this->toString() < rhs.toString(); }
};

struct BaseStation {
    Socket grpcSocket;
    int quality{};
};

struct Channel {
    std::unique_ptr<control::ControlService::Stub> service;
    Socket grpcSocket;
    std::optional<uint32_t> quality;
};

struct UserConnection {
    Socket grpcSocket;
    uint32_t udpPort{};
};
} // namespace user_equipment

#endif // NETWORK_COMPONENTS_DATAOBJECTS_H
