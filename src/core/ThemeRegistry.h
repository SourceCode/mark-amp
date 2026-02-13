#pragma once

#include "Theme.h"

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

/// Manages the collection of available themes (built-in + user-imported).
class ThemeRegistry
{
public:
    ThemeRegistry();

    /// Load all themes (built-in + user directory).
    [[nodiscard]] auto initialize() -> std::expected<void, std::string>;

    // Query
    [[nodiscard]] auto get_theme(const std::string& id) const -> std::optional<Theme>;
    [[nodiscard]] auto list_themes() const -> std::vector<ThemeInfo>;
    [[nodiscard]] auto theme_count() const -> size_t;
    [[nodiscard]] auto has_theme(const std::string& id) const -> bool;
    [[nodiscard]] auto is_builtin(const std::string& id) const -> bool;

    // Import/Export
    [[nodiscard]] auto import_theme(const std::filesystem::path& path)
        -> std::expected<Theme, std::string>;
    [[nodiscard]] auto export_theme(const std::string& id, const std::filesystem::path& path)
        -> std::expected<void, std::string>;

    // Delete (custom themes only)
    [[nodiscard]] auto delete_theme(const std::string& id) -> std::expected<void, std::string>;

    /// Platform-aware user themes directory.
    [[nodiscard]] static auto user_themes_directory() -> std::filesystem::path;

    /// Sanitize a theme name for use as a filename (lowercase, underscores, no special chars).
    [[nodiscard]] static auto sanitize_filename(const std::string& name) -> std::string;

private:
    std::vector<Theme> themes_;
    void load_builtin_themes();
    auto load_user_themes() -> std::expected<void, std::string>;
    auto persist_theme(const Theme& theme) -> std::expected<void, std::string>;
    auto generate_unique_id(const std::string& base_id) const -> std::string;
};

} // namespace markamp::core
