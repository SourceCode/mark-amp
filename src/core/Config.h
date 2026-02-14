#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <variant>
#include <yaml-cpp/yaml.h>

namespace markamp::core
{

/// Persistent configuration manager backed by a Markdown file (YAML frontmatter).
/// Uses platform-appropriate config directory (wxStandardPaths-compatible).
class Config
{
public:
    /// Load configuration from disk. Creates defaults if file doesn't exist.
    [[nodiscard]] auto load() -> std::expected<void, std::string>;

    /// Persist current configuration to disk.
    [[nodiscard]] auto save() const -> std::expected<void, std::string>;

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

    /// Path to the bundled config_defaults.json file.
    /// Searches macOS bundle Resources, then source tree resources/ directory.
    [[nodiscard]] static auto defaults_file_path() -> std::filesystem::path;

    /// Load default values from a JSON file. Only sets keys that are not
    /// already present in the config. Returns an error string on failure.
    [[nodiscard]] auto load_defaults_from_json(const std::filesystem::path& path)
        -> std::expected<void, std::string>;

    /// Cached frequently-accessed config values for O(1) access.
    /// Rebuilt automatically on load() and set() calls.
    struct CachedValues
    {
        std::string theme = "midnight-neon";
        std::string view_mode = "split";
        std::string font_family = "Menlo";
        std::string last_workspace;
        std::string cursor_blinking = "blink";

        int font_size = 14;
        int tab_size = 4;
        int edge_column = 80;
        int auto_save_interval_seconds = 60;
        int cursor_width = 2;
        int word_wrap_column = 80;
        int line_height = 0;
        int padding_top = 0;
        int padding_bottom = 0;

        double letter_spacing = 0.0;

        bool sidebar_visible = true;
        bool word_wrap = true;
        bool auto_save = false;
        bool show_line_numbers = true;
        bool highlight_current_line = true;
        bool show_whitespace = false;
        bool show_minimap = false;
        bool auto_indent = true;
        bool indent_guides = true;
        bool bracket_matching = true;
        bool code_folding = true;
        bool show_status_bar = true;
        bool show_tab_bar = true;
        bool mouse_wheel_zoom = false;
        bool bracket_pair_colorization = false;
        bool dim_whitespace = false;
    };

    /// Access the cached values struct for O(1) lookups.
    [[nodiscard]] auto cached() const -> const CachedValues&;

private:
    YAML::Node data_;
    CachedValues cached_;

    void apply_defaults();
    void rebuild_cache();

    /// Extract and load YAML from frontmatter-delimited content.
    /// Returns true if frontmatter was found and parsed.
    auto parse_frontmatter(const std::string& content) -> bool;

    /// Attempt to migrate from legacy config.json file.
    /// Returns the result of the migration attempt.
    auto migrate_from_json(const std::filesystem::path& json_path)
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
