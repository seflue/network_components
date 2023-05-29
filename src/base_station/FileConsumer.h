#ifndef NETWORK_COMPONENTS_FILECONSUMER_H
#define NETWORK_COMPONENTS_FILECONSUMER_H

#include "DataConsumer.h"
#include <fstream>
#include <string>

namespace base_station {
class FileConsumer : public DataConsumer {
  public:
    explicit FileConsumer(std::string handlerId, std::string fileName) :
        _handlerId(std::move(handlerId)), _filename(std::move(fileName))
    {
    }
    auto open(const std::string& identifier) -> bool override;
    void consume(const char* data, size_t dataSize) override;
    void close() override;

  private:
    std::string _handlerId;
    std::string _filename;
    std::string _fullFilePath;
    std::ofstream _file;
};
} // namespace base_station

#endif // NETWORK_COMPONENTS_FILECONSUMER_H
