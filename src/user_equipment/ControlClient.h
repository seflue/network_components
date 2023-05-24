#ifndef NETWORK_COMPONENTS_CONTROLCLIENT_H
#define NETWORK_COMPONENTS_CONTROLCLIENT_H

#include "DataObjects.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace user_equipment {

class ControlClient {
  public:
    void addChannel(const BaseStation& bs);

    void scan(const std::string& ueid);
    auto connect(const std::string& ueid, const std::string& filename)
        -> std::optional<UserConnection>;
    void disconnect(const std::string& ueid, const UserConnection& userConnection);

  private:
    [[nodiscard]] auto bestChannel() const -> const Channel*;
    std::string _ueid;
    std::map<Socket, Channel> channels_;
};

} // namespace user_equipment

#endif // NETWORK_COMPONENTS_CONTROLCLIENT_H
