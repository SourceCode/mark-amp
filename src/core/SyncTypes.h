// ============================================================================
// File: src/core/SyncTypes.h
// Phase 37: Local Sync Engine — Types and configuration
// ============================================================================
#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// Direction of synchronisation.
enum class SyncDirection : uint8_t
{
    Bidirectional, // Merge changes in both directions
    PushOnly,      // Local → remote only
    PullOnly       // Remote → local only
};

// How to resolve conflicts.
enum class SyncConflictResolution : uint8_t
{
    AskUser,    // Prompt user for each conflict
    KeepLocal,  // Always keep local version
    KeepRemote, // Always keep remote version
    KeepNewer,  // Keep the version with the latest timestamp
    KeepBoth    // Create both as separate files
};

// Current sync status.
enum class SyncStatus : uint8_t
{
    Idle,
    Scanning,
    Comparing,
    Syncing,
    Resolving,
    Completed,
    Failed
};

// File change type.
enum class FileChangeType : uint8_t
{
    Added,
    Modified,
    Deleted,
    Renamed
};

// Configuration for the sync engine.
struct SyncConfig
{
    std::filesystem::path local_root;  // Local data directory
    std::filesystem::path remote_root; // Remote/target data directory
    SyncDirection direction{SyncDirection::Bidirectional};
    SyncConflictResolution conflict_resolution{SyncConflictResolution::KeepNewer};
    std::vector<std::string> exclude_patterns; // Glob patterns to exclude
    bool sync_assets{true};                    // Sync the assets directory
    bool sync_metadata{true};                  // Sync metadata/settings
    int64_t debounce_ms{2000};                 // Debounce interval for watch
};

// Entry in a sync snapshot (one file).
struct SyncFileEntry
{
    std::filesystem::path relative_path; // Path relative to root
    std::string hash;                    // Content hash
    int64_t size_bytes{0};               // File size
    int64_t modified_at{0};              // Last modified timestamp
    bool is_directory{false};
};

// A conflict between local and remote versions.
struct SyncConflict
{
    std::filesystem::path relative_path;
    SyncFileEntry local_entry;
    SyncFileEntry remote_entry;
    FileChangeType local_change{FileChangeType::Modified};
    FileChangeType remote_change{FileChangeType::Modified};
    bool resolved{false};
    SyncConflictResolution resolution{SyncConflictResolution::AskUser};
};

// Result of a sync operation.
struct SyncResult
{
    SyncStatus status{SyncStatus::Completed};
    int32_t files_added{0};
    int32_t files_modified{0};
    int32_t files_deleted{0};
    int32_t conflicts_found{0};
    int32_t conflicts_resolved{0};
    int64_t bytes_transferred{0};
    int64_t elapsed_ms{0};
    std::vector<SyncConflict> unresolved_conflicts;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;

    [[nodiscard]] auto succeeded() const -> bool
    {
        return status == SyncStatus::Completed && errors.empty();
    }

    [[nodiscard]] auto total_changes() const -> int32_t
    {
        return files_added + files_modified + files_deleted;
    }
};

// A directory snapshot — all files at a point in time.
struct SyncSnapshot
{
    std::string snapshot_id;
    int64_t created_at{0};
    std::filesystem::path root_path;
    std::unordered_map<std::string, SyncFileEntry> files; // path -> entry

    [[nodiscard]] auto file_count() const -> int32_t
    {
        return static_cast<int32_t>(files.size());
    }
};

// Diff between two snapshots.
struct SnapshotDiffEntry
{
    std::filesystem::path relative_path;
    FileChangeType change_type{FileChangeType::Modified};
    SyncFileEntry old_entry;
    SyncFileEntry new_entry;
};

struct SnapshotDiff
{
    std::vector<SnapshotDiffEntry> changes;

    [[nodiscard]] auto change_count() const -> int32_t
    {
        return static_cast<int32_t>(changes.size());
    }

    [[nodiscard]] auto has_changes() const -> bool
    {
        return !changes.empty();
    }
};

} // namespace markamp::core
