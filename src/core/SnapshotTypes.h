// ============================================================================
// File: src/core/SnapshotTypes.h
// Phase 34: Repository System — Snapshot types and configuration
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Compression type for snapshot storage.
enum class CompressionType : uint8_t
{
    None,
    Gzip,
    Zstd
};

/// A single workspace snapshot.
struct Snapshot
{
    std::string id;           // Unique snapshot ID (UUID)
    std::string tag;          // Optional user-friendly tag/name
    std::string memo;         // User description of the snapshot
    std::string root_path;    // Workspace root at time of snapshot
    std::string storage_path; // Path to the compressed snapshot file
    CompressionType compression{CompressionType::Gzip};
    std::size_t file_count{0};
    std::size_t total_bytes{0}; // Uncompressed size
    std::size_t compressed_bytes{0};
    std::chrono::system_clock::time_point created;

    [[nodiscard]] auto compression_ratio() const -> double
    {
        if (total_bytes == 0)
        {
            return 1.0;
        }
        return static_cast<double>(compressed_bytes) / static_cast<double>(total_bytes);
    }

    [[nodiscard]] auto has_tag() const -> bool
    {
        return !tag.empty();
    }

    [[nodiscard]] auto display_name() const -> std::string
    {
        if (!tag.empty())
        {
            return tag;
        }
        return id.substr(0, 8); // Short ID
    }

    [[nodiscard]] auto compression_name() const -> std::string_view
    {
        switch (compression)
        {
            case CompressionType::None:
                return "None";
            case CompressionType::Gzip:
                return "Gzip";
            case CompressionType::Zstd:
                return "Zstd";
        }
        return "Unknown";
    }
};

/// Result of diffing two snapshots.
struct SnapshotDiff
{
    std::string old_snapshot_id;
    std::string new_snapshot_id;
    std::vector<std::string> added_files;
    std::vector<std::string> deleted_files;
    std::vector<std::string> modified_files;
    std::vector<std::string> unchanged_files;
    std::size_t old_total_bytes{0};
    std::size_t new_total_bytes{0};

    [[nodiscard]] auto total_changes() const -> int
    {
        return static_cast<int>(added_files.size() + deleted_files.size() + modified_files.size());
    }

    [[nodiscard]] auto summary() const -> std::string
    {
        return "+" + std::to_string(added_files.size()) + " -" +
               std::to_string(deleted_files.size()) + " ~" + std::to_string(modified_files.size()) +
               " files";
    }
};

/// Statistics from a purge operation.
struct PurgeStats
{
    int snapshots_removed{0};
    std::size_t bytes_freed{0};
    int snapshots_remaining{0};
};

/// Configuration for the repository system.
struct RepositoryConfig
{
    int retention_days{90};                  // Purge snapshots older than this
    int max_snapshots{200};                  // Maximum total snapshots
    int auto_snapshot_interval_minutes{120}; // Minutes between auto-snapshots
    bool auto_snapshot_enabled{true};
    CompressionType default_compression{CompressionType::Gzip};
    std::vector<std::string> exclude_patterns{".git", "node_modules", ".DS_Store"};
};

} // namespace markamp::core
