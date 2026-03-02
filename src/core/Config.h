#pragma once

#include <expected>
#include <filesystem>
#include <string>
#include <string_view>
#include <variant>
#include <yaml-cpp/yaml.h>

namespace markamp::core
{

class SettingsCatalog;

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

    // ── Phase 02 Task 1: SettingsCatalog wiring ──

    /// Attach a SettingsCatalog for default resolution and schema validation.
    void set_catalog(SettingsCatalog* catalog);

    /// Apply defaults from the attached SettingsCatalog (replaces hardcoded kDefaults).
    void apply_catalog_defaults();

    // ── Phase 02 Task 3: Schema validation ──

    /// Validate a value against the SettingsCatalog schema.
    /// Returns true if the value is valid, false with a reason if not.
    [[nodiscard]] auto validate_value(std::string_view key, int value) const
        -> std::expected<void, std::string>;
    [[nodiscard]] auto validate_value(std::string_view key, const std::string& value) const
        -> std::expected<void, std::string>;

    // ── Phase 02 Task 4: Change batching ──

    /// Begin a batch — defers `rebuild_cache()` and `save()` until `commit_batch()`.
    void begin_batch();

    /// Commit the current batch — rebuilds cache and saves once.
    void commit_batch();

    /// Discard all changes made since `begin_batch()`.
    void discard_batch();

    /// Whether the config is currently in a batch.
    [[nodiscard]] auto is_batching() const -> bool;

    /// Cached frequently-accessed config values for O(1) access.
    /// Rebuilt automatically on load() and set() calls.
    struct CachedValues
    {
        std::string theme = "midnight-neon";
        std::string density_profile = "default";
        std::string view_mode = "split";
        std::string font_family = "Menlo";
        std::string font_family_sans = "Inter";
        std::string font_family_mono = "JetBrains Mono";
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
        bool reduced_motion = false;
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
        bool format_on_save = false;
        bool format_on_paste = false;
        bool linked_editing = false;
    };

    /// Access the cached values struct for O(1) lookups.
    [[nodiscard]] auto cached() const -> const CachedValues&;

    // ── Batch 9: Additional Config methods ──

    /// Check whether a key exists in the config data.
    [[nodiscard]] auto has_key(std::string_view key) const -> bool;

    /// Remove a key from the config data (resets to default on next load).
    void remove(std::string_view key);

    /// Return all stored config keys.
    [[nodiscard]] auto all_keys() const -> std::vector<std::string>;

    /// Return the total number of stored config keys.
    [[nodiscard]] auto key_count() const -> std::size_t;

    /// Return the list of git commit templates
    [[nodiscard]] auto get_commit_templates() const -> std::vector<std::string>;

    // ── New Batch 9: Config profile support (#55-58) ──

    /// Export current config to a JSON file.
    void export_to_json(const std::filesystem::path& path) const;

    /// Import config from a JSON file, merging with current values.
    void import_from_json(const std::filesystem::path& path);

    /// Return keys with differing values between this config and another.
    [[nodiscard]] auto diff(const Config& other) const -> std::vector<std::string>;

    /// Create an in-memory copy for undo/revert support.
    [[nodiscard]] auto snapshot() const -> Config;

    /// Return keys whose current values differ from the catalog defaults.
    [[nodiscard]] auto modified_settings() const -> std::vector<std::string>;

    /// Restore all settings from a snapshot (for undo).
    void restore_from_snapshot(const Config& snap);

private:
    YAML::Node data_;
    CachedValues cached_;
    SettingsCatalog* catalog_{nullptr};
    bool batching_{false};
    YAML::Node batch_snapshot_; // snapshot of data_ before batch

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
