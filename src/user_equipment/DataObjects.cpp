#include "DataObjects.h"

using namespace user_equipment;

std::string Socket::toString() const { return std::format("{}:{}", ip, port); }
