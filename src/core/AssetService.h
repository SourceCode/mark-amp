// ============================================================================
// File: src/core/AssetService.h
// Phase 36: Asset Management — Asset lifecycle service
// ============================================================================
#pragma once

#include "AssetTypes.h"

#include <expected>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

// AssetService — manages asset lifecycle: import, dedup, search, cleanup.
class AssetService
{
public:
    AssetService(EventBus& event_bus, Config& config);

    // Register a new asset file (copies to assets dir, computes hash).
    [[nodiscard]] auto register_asset(const std::filesystem::path& source_path,
                                      const std::string& notebook_id)
        -> std::expected<AssetInfo, std::string>;

    // Register from raw bytes (e.g., pasted image).
    [[nodiscard]] auto register_asset_data(const std::string& filename,
                                           const std::vector<uint8_t>& data,
                                           const std::string& notebook_id)
        -> std::expected<AssetInfo, std::string>;

    // Get asset info by ID.
    [[nodiscard]] auto get_asset(const std::string& asset_id) const -> std::optional<AssetInfo>;

    // Find asset by hash (for dedup).
    [[nodiscard]] auto find_by_hash(const std::string& sha256_hash) const
        -> std::optional<AssetInfo>;

    // Search assets by name, type, or content.
    [[nodiscard]] auto search(const std::string& query,
                              AssetCategory category = AssetCategory::Other,
                              int limit = 50) const -> AssetSearchResult;

    // Update reference count for an asset.
    auto update_reference_count(const std::string& asset_id, int32_t delta) -> void;

    // Rename an asset and update all block references.
    [[nodiscard]] auto rename_asset(const std::string& asset_id, const std::string& new_name)
        -> std::expected<void, std::string>;

    // Delete an asset by ID.
    [[nodiscard]] auto delete_asset(const std::string& asset_id)
        -> std::expected<void, std::string>;

    // Find and remove duplicate assets (keep one, update references).
    [[nodiscard]] auto deduplicate() -> int32_t;

    // Find orphaned assets (zero references).
    [[nodiscard]] auto find_orphans() const -> std::vector<AssetInfo>;

    // Clean up orphaned assets older than grace period.
    [[nodiscard]] auto cleanup_orphans(int32_t grace_days = 30) -> int32_t;

    // Get storage statistics.
    [[nodiscard]] auto stats() const -> AssetStats;

    // Compute SHA-256 hash of a file.
    [[nodiscard]] static auto compute_hash(const std::filesystem::path& path) -> std::string;

    // Detect MIME type from file extension.
    [[nodiscard]] static auto detect_mime_type(const std::filesystem::path& path) -> std::string;

    // Detect asset category from MIME type.
    [[nodiscard]] static auto categorize(const std::string& mime_type) -> AssetCategory;

private:
    EventBus& event_bus_;
    Config& config_;
    AssetServiceConfig service_config_;
    mutable std::mutex mutex_;

    // In-memory asset index (asset_id -> AssetInfo).
    std::unordered_map<std::string, AssetInfo> assets_;
    // Hash index for dedup (hash -> asset_id).
    std::unordered_map<std::string, std::string> hash_index_;

    // Generate a unique asset filename.
    [[nodiscard]] auto generate_asset_path(const std::string& original_name,
                                           const std::string& notebook_id) const
        -> std::filesystem::path;

    // Load asset index from disk.
    auto load_index() -> void;
    // Save asset index to disk.
    auto save_index() const -> void;
};

} // namespace markamp::core
