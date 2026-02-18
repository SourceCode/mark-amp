// ============================================================================
// File: src/core/SyncEngine.h
// Phase 37: Local Sync Engine — Bidirectional snapshot-based sync
// ============================================================================
#pragma once

#include "SyncTypes.h"

#include <expected>
#include <functional>
#include <mutex>
#include <string>

namespace markamp::core
{

class EventBus;

// Progress callback: (current_file, percent_complete).
using SyncProgressCallback = std::function<void(const std::string&, int)>;

// SyncEngine — bidirectional snapshot-based sync between directories.
class SyncEngine
{
public:
    SyncEngine(EventBus& event_bus);

    // Perform a full sync between local and remote directories.
    [[nodiscard]] auto full_sync(const SyncConfig& config, SyncProgressCallback progress = nullptr)
        -> SyncResult;

    // Generate a snapshot of a directory.
    [[nodiscard]] auto generate_snapshot(const std::filesystem::path& root)
        -> std::expected<SyncSnapshot, std::string>;

    // Compute the diff between two snapshots.
    [[nodiscard]] auto diff_snapshots(const SyncSnapshot& old_snap,
                                      const SyncSnapshot& new_snap) const -> SnapshotDiff;

    // Apply changes from a diff to a target directory.
    [[nodiscard]] auto apply_changes(const SnapshotDiff& diff,
                                     const std::filesystem::path& source_root,
                                     const std::filesystem::path& target_root,
                                     SyncProgressCallback progress = nullptr)
        -> std::expected<int32_t, std::string>;

    // Detect conflicts between local and remote diffs.
    [[nodiscard]] auto detect_conflicts(const SnapshotDiff& local_diff,
                                        const SnapshotDiff& remote_diff) const
        -> std::vector<SyncConflict>;

    // Resolve conflicts using the specified strategy.
    auto resolve_conflicts(std::vector<SyncConflict>& conflicts, SyncConflictResolution strategy)
        -> void;

    // Get/set the last sync snapshot (for incremental sync).
    [[nodiscard]] auto last_snapshot() const -> const SyncSnapshot&;
    auto set_last_snapshot(SyncSnapshot snapshot) -> void;

private:
    [[maybe_unused]] EventBus& event_bus_;
    SyncSnapshot last_snapshot_;
    mutable std::mutex mutex_;

    // Compute hash of a file (FNV-1a for speed).
    [[nodiscard]] static auto hash_file(const std::filesystem::path& path) -> std::string;

    // Check if path matches any exclude pattern.
    [[nodiscard]] static auto is_excluded(const std::filesystem::path& relative_path,
                                          const std::vector<std::string>& patterns) -> bool;

    // Copy a file from source to target, creating directories as needed.
    [[nodiscard]] static auto copy_file_safe(const std::filesystem::path& source,
                                             const std::filesystem::path& target)
        -> std::expected<void, std::string>;

    // Delete a file safely.
    [[nodiscard]] static auto delete_file_safe(const std::filesystem::path& path)
        -> std::expected<void, std::string>;
};

} // namespace markamp::core
