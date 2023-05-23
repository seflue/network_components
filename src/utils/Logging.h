#ifndef NETWORK_COMPONENTS_LOGGING_H
#define NETWORK_COMPONENTS_LOGGING_H

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace utils {
namespace logging {
inline void initializeLogger()
{
    auto console = spdlog::stdout_color_mt("console");
    auto err_logger = spdlog::stderr_color_mt("stderr");
    spdlog::set_level(spdlog::level::debug);
}
} // namespace logging
} // namespace utils

#define LOG_ERROR(...) spdlog::get("stderr")->error(__VA_ARGS__)
#define LOG_WARN(...) spdlog::get("console")->warn(__VA_ARGS__)
#define LOG_INFO(...) spdlog::get("console")->info(__VA_ARGS__)
#define LOG_DEBUG(...) spdlog::get("console")->debug(__VA_ARGS__)

#endif // NETWORK_COMPONENTS_LOGGING_H
