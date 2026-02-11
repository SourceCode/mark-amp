#include "Config.h"

#include "Logger.h"

#include <cstdlib>
#include <fstream>

namespace markamp::core
{

auto Config::config_directory() -> std::filesystem::path
{
    std::filesystem::path dir;

#ifdef __APPLE__
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        dir = std::filesystem::path(home) / "Library" / "Application Support" / "MarkAmp";
    }
#elif defined(_WIN32)
    const char* appData = std::getenv("APPDATA");
    if (appData != nullptr)
    {
        dir = std::filesystem::path(appData) / "MarkAmp";
    }
#else
    const char* home = std::getenv("HOME");
    if (home != nullptr)
    {
        dir = std::filesystem::path(home) / ".config" / "markamp";
    }
#endif

    if (dir.empty())
    {
        dir = std::filesystem::current_path() / ".markamp";
    }

    return dir;
}

auto Config::config_file_path() -> std::filesystem::path
{
    return config_directory() / "config.json";
}

void Config::apply_defaults()
{
    if (!data_.contains("theme"))
    {
        data_["theme"] = "midnight-neon";
    }
    if (!data_.contains("view_mode"))
    {
        data_["view_mode"] = "split";
    }
    if (!data_.contains("sidebar_visible"))
    {
        data_["sidebar_visible"] = true;
    }
    if (!data_.contains("font_size"))
    {
        data_["font_size"] = 14;
    }
    if (!data_.contains("word_wrap"))
    {
        data_["word_wrap"] = true;
    }
    if (!data_.contains("auto_save"))
    {
        data_["auto_save"] = false;
    }
}

auto Config::load() -> std::expected<void, std::string>
{
    auto path = config_file_path();

    if (!std::filesystem::exists(path))
    {
        MARKAMP_LOG_INFO("Config file not found, creating defaults at: {}", path.string());
        data_ = nlohmann::json::object();
        apply_defaults();
        return save();
    }

    try
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            return std::unexpected("Failed to open config file: " + path.string());
        }
        data_ = nlohmann::json::parse(file, nullptr, true, true);
        apply_defaults();
        MARKAMP_LOG_INFO("Config loaded from: {}", path.string());
        return {};
    }
    catch (const nlohmann::json::parse_error& e)
    {
        MARKAMP_LOG_WARN("Corrupt config file, resetting to defaults: {}", e.what());
        data_ = nlohmann::json::object();
        apply_defaults();
        return save();
    }
}

auto Config::save() const -> std::expected<void, std::string>
{
    auto dir = config_directory();
    try
    {
        std::filesystem::create_directories(dir);
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        return std::unexpected("Failed to create config directory: " + std::string(e.what()));
    }

    auto path = config_file_path();
    std::ofstream file(path);
    if (!file.is_open())
    {
        return std::unexpected("Failed to write config file: " + path.string());
    }

    file << data_.dump(4);
    MARKAMP_LOG_DEBUG("Config saved to: {}", path.string());
    return {};
}

auto Config::get_string(std::string_view key, std::string_view default_val) const -> std::string
{
    auto k = std::string(key);
    if (data_.contains(k) && data_[k].is_string())
    {
        return data_[k].get<std::string>();
    }
    return std::string(default_val);
}

auto Config::get_int(std::string_view key, int default_val) const -> int
{
    auto k = std::string(key);
    if (data_.contains(k) && data_[k].is_number_integer())
    {
        return data_[k].get<int>();
    }
    return default_val;
}

auto Config::get_bool(std::string_view key, bool default_val) const -> bool
{
    auto k = std::string(key);
    if (data_.contains(k) && data_[k].is_boolean())
    {
        return data_[k].get<bool>();
    }
    return default_val;
}

auto Config::get_double(std::string_view key, double default_val) const -> double
{
    auto k = std::string(key);
    if (data_.contains(k) && data_[k].is_number())
    {
        return data_[k].get<double>();
    }
    return default_val;
}

void Config::set(std::string_view key, std::string_view value)
{
    data_[std::string(key)] = std::string(value);
}

void Config::set(std::string_view key, int value)
{
    data_[std::string(key)] = value;
}

void Config::set(std::string_view key, bool value)
{
    data_[std::string(key)] = value;
}

void Config::set(std::string_view key, double value)
{
    data_[std::string(key)] = value;
}

} // namespace markamp::core
