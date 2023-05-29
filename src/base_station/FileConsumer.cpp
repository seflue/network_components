#include "FileConsumer.h"
#include "Logging.h"

#include <filesystem>
#include <format>

namespace bs = base_station;
namespace fs = std::filesystem;

auto bs::FileConsumer::open(const std::string& identifier) -> bool
{
    auto filePath = fs::path(_filename);
    auto directoryPath = filePath.parent_path();
    auto fullDirPath = std::format("{}/{}/{}", _handlerId, identifier, directoryPath.string());
    if (fullDirPath.starts_with('/')) {
        LOG_ERROR("FileConsumer {}, {}: Handler id must not be empty!", __FILE__, __LINE__);
        return false;
    }

    if (!fs::exists(fullDirPath) && !fs::create_directories(fs::path(fullDirPath))) {
        LOG_ERROR("FileConsumer {}, {}: Failed to create directory: {}",
                  __FILE__,
                  __LINE__,
                  fullDirPath);
        return false;
    }

    _fullFilePath = fullDirPath + "/" + filePath.filename().string();
    _file = std::ofstream(_fullFilePath, std::ios::binary);
    if (!_file.is_open()) {
        LOG_ERROR("Failed to open file {}", _fullFilePath);
        return false;
    }
    LOG_DEBUG("File {} successfully opened.", _fullFilePath);
    return true;
}

void bs::FileConsumer::close()
{
    LOG_DEBUG("Close file.");
    _file.flush();
    _file.close();
}

void base_station::FileConsumer::consume(const char* data, size_t dataSize)
{
    if (dataSize > 0) {
        if (!_file.is_open())
            LOG_ERROR("File not open, opening at {}", _fullFilePath);
        else if (_file.write(data, dataSize)) {
            LOG_DEBUG("{} bytes written successfully", dataSize);
            if (_file.fail())
                LOG_DEBUG("File status: fail");
            if (_file.bad())
                LOG_DEBUG("File status: bad");
        }
        else
            LOG_ERROR("Error wile writing to file");
    }
}
