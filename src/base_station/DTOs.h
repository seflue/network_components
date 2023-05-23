#ifndef NETWORK_COMPONENTS_DTOS_H
#define NETWORK_COMPONENTS_DTOS_H

namespace base_station {

struct UdpConnection final {
    std::string ueid;
    std::string filename;
    uint32_t dataPort;
};

struct State final {
    const uint32_t grpcPort;

    explicit State(int grpcPort) : grpcPort(grpcPort) {}
    std::string quality;
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_DTOS_H
