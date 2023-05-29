#ifndef NETWORK_COMPONENTS_DATACONSUMER_H
#define NETWORK_COMPONENTS_DATACONSUMER_H

#include <cstddef>
#include <string>

namespace base_station {
class DataConsumer {
  public:
    virtual bool open(const std::string& identifier) = 0;
    virtual void consume(const char* data, size_t dataSize) = 0;
    virtual void close() = 0;
    virtual ~DataConsumer() {}
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_DATACONSUMER_H
