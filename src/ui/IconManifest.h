/// @file IconManifest.h
/// @brief V16 Phase 01-04 — Manifest-driven icon lookup system.
///
/// Replaces scattered emoji/hardcoded icon resolution with a single
/// versioned JSON manifest that maps canonical icon IDs to SVG assets,
/// file extensions to icon IDs, and folder names to icon IDs.
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::ui
{

/// Describes a single icon entry in the manifest.
struct IconEntry
{
    std::string canonical_id; ///< Stable key, e.g. "file_cpp", "folder_git"
    std::string asset_path;   ///< Relative path to SVG, e.g. "icons/file_cpp.svg"
    std::string display_name; ///< Human-readable name, e.g. "C++ Source File"

    /// Optional theme color override tokens.
    /// Maps theme variant ("dark", "light", "highContrast") → color hex.
    std::unordered_map<std::string, std::string> color_overrides;
};

/// Describes a folder icon variant (open / closed states).
struct FolderIconEntry
{
    std::string canonical_id; ///< e.g. "folder_git"
    std::string open_asset;   ///< SVG path for open state
    std::string closed_asset; ///< SVG path for closed state
    std::string display_name;
};

/// Manifest schema version for forward compatibility.
struct ManifestVersion
{
    int major{1};
    int minor{0};
    int patch{0};

    [[nodiscard]] auto to_string() const -> std::string;
};

/// The central icon manifest: a versioned, JSON-loadable registry mapping
/// file extensions, folder names, special filenames, and command IDs to
/// their canonical icon IDs and SVG asset paths.
///
/// Usage:
/// ```cpp
/// IconManifest manifest;
/// manifest.load_from_file("resources/icons/icon_manifest.json");
/// auto id = manifest.resolve_file_icon("main.cpp");  // → "file_cpp"
/// auto entry = manifest.get_entry("file_cpp");
/// ```
class IconManifest
{
public:
    IconManifest() = default;

    /// Load manifest from a JSON file on disk.
    /// Returns true on success, false on parse/validation failure.
    [[nodiscard]] auto load_from_file(const std::filesystem::path& path) -> bool;

    /// Load manifest from a JSON string.
    [[nodiscard]] auto load_from_string(const std::string& json_content) -> bool;

    // ── File icon resolution ──

    /// Resolve a filename to its canonical icon ID.
    /// Checks special filenames first, then extension mapping, then fallback.
    [[nodiscard]] auto resolve_file_icon(const std::string& filename) const -> std::string;

    /// Get the icon entry for a canonical ID.
    [[nodiscard]] auto get_entry(const std::string& canonical_id) const -> std::optional<IconEntry>;

    // ── Folder icon resolution ──

    /// Resolve a folder name to its folder icon entry.
    [[nodiscard]] auto resolve_folder_icon(const std::string& folder_name) const
        -> std::optional<FolderIconEntry>;

    /// Get the default folder icon entry.
    [[nodiscard]] auto default_folder_icon() const -> const FolderIconEntry&;

    // ── Command icon resolution ──

    /// Resolve a command ID to its canonical icon ID.
    [[nodiscard]] auto resolve_command_icon(const std::string& command_id) const -> std::string;

    // ── Introspection ──

    /// Get the manifest schema version.
    [[nodiscard]] auto version() const -> const ManifestVersion&;

    /// Total number of icon entries.
    [[nodiscard]] auto icon_count() const -> size_t;

    /// Total number of file extension mappings.
    [[nodiscard]] auto extension_count() const -> size_t;

    /// Total number of folder name mappings.
    [[nodiscard]] auto folder_mapping_count() const -> size_t;

    /// Total number of command icon mappings.
    [[nodiscard]] auto command_mapping_count() const -> size_t;

    /// All registered canonical icon IDs.
    [[nodiscard]] auto all_icon_ids() const -> std::vector<std::string>;

    /// Check for validation errors after loading.
    [[nodiscard]] auto validation_errors() const -> const std::vector<std::string>&;

    /// Canonical ID for "unknown file type" fallback.
    static constexpr const char* kFallbackFileIcon = "file_default";

    /// Canonical ID for "unknown folder" fallback.
    static constexpr const char* kFallbackFolderIcon = "folder_default";

    /// Canonical ID for "unknown command" fallback.
    static constexpr const char* kFallbackCommandIcon = "command_default";

private:
    ManifestVersion version_;

    /// canonical_id → IconEntry
    std::unordered_map<std::string, IconEntry> icons_;

    /// file extension (lowercase, without dot) → canonical_id
    std::unordered_map<std::string, std::string> extension_map_;

    /// special filename (exact match, lowercase) → canonical_id
    std::unordered_map<std::string, std::string> filename_map_;

    /// folder name (lowercase) → FolderIconEntry
    std::unordered_map<std::string, FolderIconEntry> folder_map_;

    /// command id → canonical_id
    std::unordered_map<std::string, std::string> command_map_;

    /// Default folder icon (normal/generic folder).
    FolderIconEntry default_folder_{"folder_default",
                                    "icons/folder_default_open.svg",
                                    "icons/folder_default_closed.svg",
                                    "Folder"};

    /// Validation errors accumulated during load.
    std::vector<std::string> errors_;

    /// Normalize a file extension to lowercase without leading dot.
    [[nodiscard]] static auto normalize_extension(const std::string& filename) -> std::string;

    /// Normalize a filename to lowercase for special filename matching.
    [[nodiscard]] static auto normalize_filename(const std::string& filename) -> std::string;

    /// Internal: parse the icons array from JSON.
    void parse_icons(const std::string& json_content);

    /// Internal: parse the extension mappings from JSON.
    void parse_extensions(const std::string& json_content);

    /// Internal: parse the folder mappings from JSON.
    void parse_folders(const std::string& json_content);

    /// Internal: parse the command mappings from JSON.
    void parse_commands(const std::string& json_content);

    /// Validate internal consistency (no dangling references, etc.).
    void validate();
};

} // namespace markamp::ui
