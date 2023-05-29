#ifndef NETWORK_COMPONENTS_CONNECTIONPOOL_H
#define NETWORK_COMPONENTS_CONNECTIONPOOL_H

#include "Connection.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include <memory>

namespace base_station {
class Connection;
class DataConsumer;

class ConnectionPool {
  public:
    explicit ConnectionPool(uint32_t maxConnections, uint32_t maxClients);

    auto reserveConnection(const std::string& ueid, std::unique_ptr<DataConsumer> dataConsumer)
        -> std::optional<uint32_t>;

    bool releaseConnection(const std::string& ueid, uint32_t udpPort);

  private:
    std::vector<std::shared_ptr<Connection>> _connections;
    std::map<std::string, size_t> _reservedClients;
    size_t _maxConnections;
    size_t _maxClients;
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_CONNECTIONPOOL_H
