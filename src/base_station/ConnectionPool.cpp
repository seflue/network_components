#include "ConnectionPool.h"
#include "FileConsumer.h"
#include "Logging.h"
#include <algorithm>
#include <ranges>

namespace bs = base_station;

auto bs::ConnectionPool::reserveConnection(const std::string& ueid,
                                           std::unique_ptr<DataConsumer> dataConsumer)
    -> std::optional<uint32_t>
{
    if (_maxClients <= _reservedClients.size()) {
        LOG_INFO("Number of max clients already reached!");
        LOG_DEBUG("Max clients: {}", _maxClients);
        LOG_DEBUG("Reserved clients: {}", _reservedClients.size());
        return std::nullopt;
    }
    if (_reservedClients.contains(ueid) && _reservedClients[ueid] == _maxConnections) {
        LOG_INFO("Number of max connections of {} for client {} already reached!");
        LOG_DEBUG("Max connections: {}", _maxConnections);
        LOG_DEBUG("Reserved connections: {}", _reservedClients[ueid]);
        return std::nullopt;
    }
    auto iter =
        std::ranges::find_if(_connections, [](const std::shared_ptr<Connection>& connection) {
            return connection->isAvailable();
        });
    if (iter == _connections.end()) {
        LOG_INFO("No available connection found in pool.");
        return std::nullopt;
    }

    auto& available = *iter;
    available->connect(ueid, std::move(dataConsumer));
    if (!_reservedClients.contains(ueid))
        _reservedClients[ueid] = 0;
    _reservedClients[ueid]++;

    return available->udpPort();
}

auto bs::ConnectionPool::releaseConnection(const std::string& ueid, const uint32_t udpPort) -> bool
{
    auto toRelease =
        std::ranges::find_if(_connections, [&](const std::shared_ptr<Connection>& connection) {
            return !connection->isAvailable() && ueid == connection->ueid() &&
                   udpPort == connection->udpPort();
        });
    if (toRelease == _connections.end()) {
        LOG_ERROR("No connection found to release with {}:{}", ueid, udpPort);
        return false;
    }

    LOG_DEBUG("About to disconnect {} ...", (*toRelease)->toString());
    (*toRelease)->disconnect();
    _reservedClients[ueid]--;
    if (_reservedClients[ueid] == 0)
        _reservedClients.erase(ueid);
    LOG_DEBUG("Disconnected {}.", (*toRelease)->toString());
    return true;
}

bs::ConnectionPool::ConnectionPool(uint32_t maxConnections, uint32_t maxClients) :
    _maxConnections(maxConnections), _maxClients(maxClients)
{
    auto totalConnections = maxConnections * maxClients;
    for (auto i = 0u; i < totalConnections; ++i) {
        _connections.push_back(std::make_shared<Connection>());
        LOG_DEBUG("Connection created");
    }
    LOG_DEBUG("Created {} connections", _connections.size());
}
