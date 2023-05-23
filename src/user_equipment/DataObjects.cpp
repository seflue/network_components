#include "DataObjects.h"

using namespace user_equipment;

auto to_string(const Socket &socket) -> std::string
{
    return socket.ip + ":" + std::to_string(socket.port);
}

std::string Socket::toString() const { return ip + ":" + std::to_string(port); }
