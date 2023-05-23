#ifndef NETWORK_COMPONENTS_CONNECTIONPOOL_H
#define NETWORK_COMPONENTS_CONNECTIONPOOL_H

#include "Connection.h"
#include <map>
#include <optional>
#include <set>
#include <vector>

namespace base_station {

class ConnectionPool {
  public:
    // explicit ConnectionPool(uint16_t maxConnections, uint16_t maxClients,
    //                std::pair<uint32_t , uint32_t> portRange);
    explicit ConnectionPool(uint16_t maxConnections, uint16_t maxClients);

    auto reserveConnection(const std::string &ueid, const std::string &filename)
        -> std::optional<uint32_t>;

    bool releaseConnection(const std::string &ueid, const uint32_t udpPort);

  private:
    std::vector<std::shared_ptr<Connection>> connections_;
    std::map<std::string, size_t> reservedClients_;
    size_t maxConnections_;
    size_t maxClients_;
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_CONNECTIONPOOL_H
