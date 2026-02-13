#include "Logger.h"

#include <cstdlib>
#include <filesystem>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace markamp::core
{

auto getLogFilePath() -> std::string
{
    std::filesystem::path logDir;

#ifdef __APPLE__
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        logDir = std::filesystem::path(home) / "Library" / "Logs" / "MarkAmp";
    }
#elif defined(_WIN32)
    const char* appData = std::getenv("LOCALAPPDATA");
    if (appData != nullptr)
    {
        logDir = std::filesystem::path(appData) / "MarkAmp" / "Logs";
    }
#else
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        logDir = std::filesystem::path(home) / ".local" / "share" / "markamp" / "logs";
    }
#endif

    if (logDir.empty())
    {
        // R20 Fix 6: current_path() can throw if CWD is deleted
        try
        {
            logDir = std::filesystem::current_path() / "logs";
        }
        catch (const std::filesystem::filesystem_error&)
        {
            logDir = std::filesystem::path("/tmp") / "markamp" / "logs";
        }
    }

    // R20 Fix 5: use error_code overload — don't throw on permission failures
    std::error_code log_ec;
    std::filesystem::create_directories(logDir, log_ec);
    // If directory creation fails, we still try to write — the log call itself
    // will fail gracefully via spdlog
    return (logDir / "markamp.log").string();
}

void initLogger()
{
    // Console sink
    auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    consoleSink->set_level(spdlog::level::info);

    // Rotating file sink (5 MB max, 3 files)
    auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        getLogFilePath(), 5 * 1024 * 1024, 3);
    fileSink->set_level(spdlog::level::trace);

    // Multi-sink logger
    auto logger =
        std::make_shared<spdlog::logger>("markamp", spdlog::sinks_init_list{consoleSink, fileSink});

    // Set log level from environment variable
    const char* logLevel = std::getenv("MARKAMP_LOG_LEVEL");
    if (logLevel != nullptr)
    {
        auto level = spdlog::level::from_str(logLevel);
        logger->set_level(level);
    }
    else
    {
        logger->set_level(spdlog::level::debug);
    }

    logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");
    spdlog::set_default_logger(logger);
    spdlog::flush_every(std::chrono::seconds(3));
}

} // namespace markamp::core
