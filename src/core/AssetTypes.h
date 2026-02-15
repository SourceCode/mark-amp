// ============================================================================
// File: src/core/AssetTypes.h
// Phase 36: Asset Management — Types and configuration
// ============================================================================
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

namespace markamp::core
{

// MIME type categories for assets.
enum class AssetCategory : uint8_t
{
    Image,
    Document,
    Audio,
    Video,
    Font,
    Archive,
    Other
};

// Metadata about a managed asset.
struct AssetInfo
{
    std::string asset_id;            // Unique asset identifier
    std::filesystem::path file_path; // Absolute path to the asset file
    std::string original_name;       // Original filename
    std::string mime_type;           // MIME type (e.g., "image/png")
    AssetCategory category{AssetCategory::Other};
    std::string sha256_hash;    // SHA-256 content hash
    int64_t file_size{0};       // File size in bytes
    int32_t width{0};           // Image width (0 if not image)
    int32_t height{0};          // Image height (0 if not image)
    int32_t reference_count{0}; // Number of blocks referencing this
    std::string thumbnail_path; // Path to thumbnail (if generated)
    int64_t created_at{0};      // Unix timestamp
    int64_t modified_at{0};     // Unix timestamp

    [[nodiscard]] auto is_image() const -> bool
    {
        return category == AssetCategory::Image;
    }

    [[nodiscard]] auto is_orphan() const -> bool
    {
        return reference_count <= 0;
    }

    [[nodiscard]] auto formatted_size() const -> std::string;
};

// Result of an asset search query.
struct AssetSearchResult
{
    std::vector<AssetInfo> assets;
    int32_t total_count{0};
    int64_t elapsed_ms{0};
};

// Entry in the content index (text extracted from binary assets).
struct ContentIndexEntry
{
    std::string asset_id;
    std::string extracted_text; // Full text extracted from asset
    std::string language;       // Detected language
    int64_t indexed_at{0};      // Unix timestamp
};

// Configuration for the asset service.
struct AssetServiceConfig
{
    std::filesystem::path assets_root; // Root directory for assets
    int64_t max_file_size_mb{100};     // Max single file size
    int64_t storage_quota_mb{10000};   // Total storage quota
    bool generate_thumbnails{true};    // Auto-generate thumbnails
    int32_t thumbnail_size{256};       // Thumbnail max dimension
    bool enable_dedup{true};           // Hash-based deduplication
    bool auto_cleanup_orphans{false};  // Auto-delete unreferenced assets
    int32_t orphan_grace_days{30};     // Days before orphan cleanup
};

// Statistics about asset storage.
struct AssetStats
{
    int32_t total_assets{0};
    int32_t images{0};
    int32_t documents{0};
    int32_t other{0};
    int32_t orphans{0};
    int64_t total_size_bytes{0};
    int32_t duplicates{0};
    int64_t duplicate_size_bytes{0};
};

} // namespace markamp::core
