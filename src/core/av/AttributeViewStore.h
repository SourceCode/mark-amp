#pragma once

#include "AttributeView.h"

#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::av
{

/// Persistence layer for Attribute Views.
/// Stores AV definitions as JSON files under `<storage_root>/av/`.
/// Thread-safe with in-memory caching and file-based persistence.
class AttributeViewStore
{
public:
    explicit AttributeViewStore(std::filesystem::path storage_root);

    /// Create a new Attribute View with default Block key and Table view.
    [[nodiscard]] auto create(const std::string& name) -> std::expected<AttributeView, std::string>;

    /// Load an Attribute View by ID (from cache or disk).
    [[nodiscard]] auto load(const std::string& av_id) -> std::expected<AttributeView, std::string>;

    /// Persist an Attribute View to disk and update cache.
    auto save(const AttributeView& attribute_view) -> std::expected<void, std::string>;

    /// Delete an Attribute View file and remove from cache.
    auto delete_av(const std::string& av_id) -> std::expected<void, std::string>;

    /// List all AV IDs in the storage directory.
    [[nodiscard]] auto get_all_ids() const -> std::vector<std::string>;

    /// Check if an AV file exists on disk.
    [[nodiscard]] auto exists(const std::string& av_id) const -> bool;

    /// Remove a single entry from the in-memory cache.
    void evict(const std::string& av_id);

    /// Clear the entire in-memory cache.
    void clear_cache();

private:
    static constexpr int kAVSpecVersion = 1;

    std::filesystem::path storage_root_;
    std::filesystem::path av_directory_;

    mutable std::mutex mutex_;
    std::unordered_map<std::string, AttributeView> cache_;

    /// Generate a UUID v4-style string.
    static auto generate_uuid() -> std::string;

    /// Ensure the AV storage directory exists.
    auto ensure_directory() -> std::expected<void, std::string>;

    /// Get the filesystem path for a given AV ID.
    [[nodiscard]] auto file_path_for(const std::string& av_id) const -> std::filesystem::path;

    /// Serialize an AttributeView to JSON.
    [[nodiscard]] auto serialize(const AttributeView& attribute_view) const -> std::string;

    /// Deserialize JSON to an AttributeView.
    static auto deserialize(const std::string& json_str)
        -> std::expected<AttributeView, std::string>;
};

} // namespace markamp::core::av
