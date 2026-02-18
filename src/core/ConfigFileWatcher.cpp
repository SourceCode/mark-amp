#include "ConfigFileWatcher.h"

#include "Logger.h"

namespace markamp::core
{

ConfigFileWatcher::ConfigFileWatcher(std::filesystem::path config_path, Callback on_change)
    : config_path_(std::move(config_path))
    , on_change_(std::move(on_change))
{
}

auto ConfigFileWatcher::poll() -> bool
{
    if (!active_)
    {
        return false;
    }

    std::error_code err_code;
    if (!std::filesystem::exists(config_path_, err_code))
    {
        return false;
    }

    auto current_time = std::filesystem::last_write_time(config_path_, err_code);
    if (err_code)
    {
        return false;
    }

    if (last_write_time_ == std::filesystem::file_time_type{})
    {
        // First poll — just record the time
        last_write_time_ = current_time;
        return false;
    }

    if (current_time != last_write_time_)
    {
        last_write_time_ = current_time;
        ++change_count_;
        MARKAMP_LOG_INFO("Config file modified externally: {}", config_path_.string());
        if (on_change_)
        {
            on_change_();
        }
        return true;
    }

    return false;
}

void ConfigFileWatcher::start()
{
    active_ = true;

    // Initialize the last write time
    std::error_code err_code;
    if (std::filesystem::exists(config_path_, err_code))
    {
        last_write_time_ = std::filesystem::last_write_time(config_path_, err_code);
    }
}

void ConfigFileWatcher::stop()
{
    active_ = false;
}

} // namespace markamp::core
