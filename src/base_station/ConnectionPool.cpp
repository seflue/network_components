#include "ConnectionPool.h"
#include "Logging.h"
#include <algorithm>
#include <ranges>

using namespace base_station;

auto ConnectionPool::reserveConnection(const std::string &ueid, const std::string &filename)
    -> std::optional<uint32_t>
{
    if (maxClients_ <= reservedClients_.size()) {
        LOG_INFO("Number of max clients already reached!");
        LOG_DEBUG("Max clients: {}", maxClients_);
        LOG_DEBUG("Reserved clients: {}", reservedClients_.size());
        return std::nullopt;
    }
    if (reservedClients_.contains(ueid) && reservedClients_[ueid] == maxConnections_) {
        LOG_INFO("Number of max connections of {} for client {} already reached!");
        LOG_DEBUG("Max connections: {}", maxConnections_);
        LOG_DEBUG("Reserved connetions: {}", reservedClients_[ueid]);
        return std::nullopt;
    }
    auto iter =
        std::ranges::find_if(connections_, [](const std::shared_ptr<Connection> &connection) {
            return connection->isAvailable();
        });
    if (iter == connections_.end()) {
        LOG_INFO("No available connection found in pool.");
        return std::nullopt;
    }

    auto &available = *iter;
    available->connect(ueid, filename);
    reservedClients_[ueid]++;

    return available->udpPort();
}

auto ConnectionPool::releaseConnection(const std::string &ueid, const uint32_t udpPort) -> bool
{
    auto toRelease =
        std::ranges::find_if(connections_, [&](const std::shared_ptr<Connection> &connection) {
            return !connection->isAvailable() && ueid == connection->ueid() &&
                   udpPort == connection->udpPort();
        });
    if (toRelease == connections_.end()) {
        LOG_ERROR("No connection found to release with {}:{}", ueid, udpPort);
        return false;
    }

    LOG_DEBUG("About to disconnect {} ...", (*toRelease)->toString());
    (*toRelease)->disconnect();
    reservedClients_[ueid]--;
    LOG_DEBUG("Disconnected {}.", (*toRelease)->toString());
    return true;
}

ConnectionPool::ConnectionPool(uint16_t maxConnections, uint16_t maxClients
                               //    std::pair<uint32_t , uint32_t> portRange
                               )
    : maxConnections_(maxConnections), maxClients_(maxClients)
{
    // auto [start, end] = portRange;
    auto start = 0;
    auto end = maxConnections * maxClients;
    for (auto &&_ : std::views::iota(start, end)) {
        connections_.push_back(std::make_shared<Connection>());
        LOG_DEBUG("Connection created");
    }
    LOG_DEBUG("Created {} connections", connections_.size());
}
