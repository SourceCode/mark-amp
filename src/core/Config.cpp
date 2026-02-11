#include "Config.h"

#include "Logger.h"

#include <nlohmann/json.hpp> // For migration

#include <cstdlib>
#include <fstream>
#include <sstream>

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
    return config_directory() / "config.md";
}

void Config::apply_defaults()
{
    if (!data_["theme"])
    {
        data_["theme"] = "midnight-neon";
    }
    if (!data_["view_mode"])
    {
        data_["view_mode"] = "split";
    }
    if (!data_["sidebar_visible"])
    {
        data_["sidebar_visible"] = true;
    }
    if (!data_["font_size"])
    {
        data_["font_size"] = 14;
    }
    if (!data_["word_wrap"])
    {
        data_["word_wrap"] = true;
    }
    if (!data_["auto_save"])
    {
        data_["auto_save"] = false;
    }
}

auto Config::load() -> std::expected<void, std::string>
{
    auto path = config_file_path();

    // Migration Check: If config.md doesn't exist but config.json does
    if (!std::filesystem::exists(path))
    {
        auto json_path = config_directory() / "config.json";
        if (std::filesystem::exists(json_path))
        {
            MARKAMP_LOG_INFO("Migrating config.json to config.md...");
            try
            {
                std::ifstream json_file(json_path);
                nlohmann::json j = nlohmann::json::parse(json_file, nullptr, true, true);

                // Transfer values
                if (j.contains("theme") && j["theme"].is_string())
                    data_["theme"] = j["theme"].get<std::string>();
                if (j.contains("view_mode") && j["view_mode"].is_string())
                    data_["view_mode"] = j["view_mode"].get<std::string>();
                if (j.contains("sidebar_visible") && j["sidebar_visible"].is_boolean())
                    data_["sidebar_visible"] = j["sidebar_visible"].get<bool>();
                if (j.contains("font_size") && j["font_size"].is_number())
                    data_["font_size"] = j["font_size"].get<int>();
                if (j.contains("word_wrap") && j["word_wrap"].is_boolean())
                    data_["word_wrap"] = j["word_wrap"].get<bool>();
                if (j.contains("auto_save") && j["auto_save"].is_boolean())
                    data_["auto_save"] = j["auto_save"].get<bool>();

                apply_defaults();
                return save();
            }
            catch (const std::exception& e)
            {
                MARKAMP_LOG_ERROR("Migration failed: {}", e.what());
                // Fallthrough to create new default
            }
        }

        MARKAMP_LOG_INFO("Config file not found, creating defaults at: {}", path.string());
        data_ = YAML::Node();
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

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        // Parse Frontmatter
        if (content.rfind("---", 0) == 0) // Starts with ---
        {
            auto end_pos = content.find("\n---", 3);
            if (end_pos != std::string::npos)
            {
                std::string yaml_content = content.substr(3, end_pos - 3);
                data_ = YAML::Load(yaml_content);
                apply_defaults();
                MARKAMP_LOG_INFO("Config loaded from: {}", path.string());
                return {};
            }
        }

        // Fallback for partial/invalid files
        MARKAMP_LOG_WARN(
            "Invalid config format (missing frontmatter), loading as plain YAML or empty");
        data_ = YAML::Load(content); // Try loading whole file
        if (data_.IsNull())
            data_ = YAML::Node();
        apply_defaults();
        return {};
    }
    catch (const YAML::Exception& e)
    {
        MARKAMP_LOG_WARN("Corrupt config file, resetting to defaults: {}", e.what());
        data_ = YAML::Node();
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

    file << "---\n";
    file << data_;
    file << "\n---\n\n";
    file << "# MarkAmp Configuration\n";
    file << "Edit the values above to customize your experience.\n";
    file << "Supported themes: midnight-neon, synth-wave, retro-terminal\n";

    MARKAMP_LOG_DEBUG("Config saved to: {}", path.string());
    return {};
}

auto Config::get_string(std::string_view key, std::string_view default_val) const -> std::string
{
    auto k = std::string(key);
    if (data_[k] && data_[k].IsScalar())
    {
        return data_[k].as<std::string>();
    }
    return std::string(default_val);
}

auto Config::get_int(std::string_view key, int default_val) const -> int
{
    auto k = std::string(key);
    if (data_[k] && data_[k].IsScalar())
    {
        try
        {
            return data_[k].as<int>();
        }
        catch (...)
        {
        }
    }
    return default_val;
}

auto Config::get_bool(std::string_view key, bool default_val) const -> bool
{
    auto k = std::string(key);
    if (data_[k] && data_[k].IsScalar())
    {
        try
        {
            return data_[k].as<bool>();
        }
        catch (...)
        {
        }
    }
    return default_val;
}

auto Config::get_double(std::string_view key, double default_val) const -> double
{
    auto k = std::string(key);
    if (data_[k] && data_[k].IsScalar())
    {
        try
        {
            return data_[k].as<double>();
        }
        catch (...)
        {
        }
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
