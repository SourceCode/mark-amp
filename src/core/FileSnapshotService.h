/// @file FileSnapshotService.h
/// @brief V4 Phase 39 – File Snapshot Service.

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data structures
// ============================================================================

/// Information about a single snapshot.
struct SnapshotInfo
{
    std::string snapshot_id;
    std::string file_path;
    std::string timestamp; ///< ISO 8601 timestamp.
    size_t size_bytes{0};
    std::string content_hash;
};

/// Diff between two snapshots.
struct SnapshotDiff
{
    int added_lines{0};
    int removed_lines{0};
    int changed_lines{0};
    std::string unified_diff; ///< Unified diff text.
};

/// Configuration for the snapshot service.
struct SnapshotConfig
{
    int auto_interval_seconds{300}; ///< Auto-snapshot interval (default 5 min).
    int max_snapshots_per_file{50}; ///< Maximum snapshots kept per file.
};

// ============================================================================
// FileSnapshotService
// ============================================================================

class FileSnapshotService
{
public:
    explicit FileSnapshotService(EventBus& event_bus);

    /// Create a new snapshot for a file.
    [[nodiscard]] auto create_snapshot(const std::string& path, const std::string& content)
        -> std::string;

    /// List all snapshots for a file, sorted by time (newest first).
    [[nodiscard]] auto list_snapshots(const std::string& path) const -> std::vector<SnapshotInfo>;

    /// Retrieve content of a specific snapshot.
    [[nodiscard]] auto get_snapshot(const std::string& snapshot_id) const
        -> std::optional<std::string>;

    /// Get the snapshot info for a specific snapshot.
    [[nodiscard]] auto get_snapshot_info(const std::string& snapshot_id) const
        -> std::optional<SnapshotInfo>;

    /// Restore a snapshot (returns its content).
    [[nodiscard]] auto restore_snapshot(const std::string& snapshot_id)
        -> std::optional<std::string>;

    /// Compute diff between two snapshots.
    [[nodiscard]] auto diff_snapshots(const std::string& id_a, const std::string& id_b) const
        -> SnapshotDiff;

    /// Remove oldest snapshots beyond the limit.
    auto prune_old(const std::string& path, int max_count = -1) -> int;

    /// Configure the service.
    auto set_config(const SnapshotConfig& config) -> void;

    /// Get snapshot count for a file.
    [[nodiscard]] auto snapshot_count(const std::string& path) const -> int;

    /// Estimate total storage used by all snapshots.
    [[nodiscard]] auto total_storage_bytes() const -> size_t;

private:
    EventBus& event_bus_;
    SnapshotConfig config_;

    /// In-memory snapshot storage (production would use disk).
    std::unordered_map<std::string, std::string> snapshot_contents_;           ///< id -> content
    std::unordered_map<std::string, SnapshotInfo> snapshot_infos_;             ///< id -> info
    std::unordered_map<std::string, std::vector<std::string>> file_snapshots_; ///< path -> [ids]

    int next_id_{1};

    /// Compute a simple hash of content.
    [[nodiscard]] static auto compute_hash(const std::string& content) -> std::string;

    /// Get current timestamp as ISO 8601 string.
    [[nodiscard]] static auto current_timestamp() -> std::string;
};

} // namespace markamp::core
