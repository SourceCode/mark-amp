#pragma once

#include <chrono>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Source from which an extension was installed.
enum class ExtensionSource
{
    kGallery, // Installed from marketplace
    kVsix,    // Installed from a .vsix file
    kBuiltin  // Bundled with the application
};

/// Metadata about an installed extension, persisted to extensions.json.
struct ExtensionMetadata
{
    std::string extension_id; // "publisher.name" format
    std::string version;
    ExtensionSource source{ExtensionSource::kBuiltin};
    std::string installed_at; // ISO-8601 timestamp
    bool enabled{true};
    std::string location; // Filesystem path to extension directory
};

/// Service to persist and retrieve extension metadata.
/// Stores data in a JSON file (e.g. `~/.markamp/extensions.json`).
class ExtensionStorageService
{
public:
    /// Construct with a specific storage file path.
    explicit ExtensionStorageService(std::filesystem::path storage_path);

    /// Load metadata from disk.
    void load();

    /// Save metadata to disk.
    void save() const;

    /// Add or update metadata for an extension.
    void upsert(const ExtensionMetadata& metadata);

    /// Remove metadata for an extension by ID.
    void remove(const std::string& extension_id);

    /// Get metadata for a specific extension.
    [[nodiscard]] auto get(const std::string& extension_id) const -> const ExtensionMetadata*;

    /// Get all stored metadata.
    [[nodiscard]] auto get_all() const -> std::vector<ExtensionMetadata>;

    /// Check if an extension is tracked.
    [[nodiscard]] auto contains(const std::string& extension_id) const -> bool;

    /// Get total count of tracked extensions.
    [[nodiscard]] auto count() const -> size_t;

private:
    std::filesystem::path storage_path_;
    std::unordered_map<std::string, ExtensionMetadata> entries_;
};

} // namespace markamp::core
