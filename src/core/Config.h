#pragma once

#include <nlohmann/json.hpp>

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>

namespace markamp::core
{

/// Persistent configuration manager backed by a JSON file.
/// Uses platform-appropriate config directory (wxStandardPaths-compatible).
class Config
{
public:
    /// Load configuration from disk. Creates defaults if file doesn't exist.
    auto load() -> std::expected<void, std::string>;

    /// Persist current configuration to disk.
    auto save() const -> std::expected<void, std::string>;

    // Typed getters with defaults
    [[nodiscard]] auto get_string(std::string_view key, std::string_view default_val = "") const
        -> std::string;
    [[nodiscard]] auto get_int(std::string_view key, int default_val = 0) const -> int;
    [[nodiscard]] auto get_bool(std::string_view key, bool default_val = false) const -> bool;
    [[nodiscard]] auto get_double(std::string_view key, double default_val = 0.0) const -> double;

    // Setters
    void set(std::string_view key, std::string_view value);
    void set(std::string_view key, int value);
    void set(std::string_view key, bool value);
    void set(std::string_view key, double value);

    /// Platform-appropriate config directory
    [[nodiscard]] static auto config_directory() -> std::filesystem::path;

    /// Full path to config file
    [[nodiscard]] static auto config_file_path() -> std::filesystem::path;

private:
    nlohmann::json data_;

    void apply_defaults();
};

} // namespace markamp::core
