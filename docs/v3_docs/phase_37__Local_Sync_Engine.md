# Phase 37 — Local Sync Engine

## Objective

Implement a local synchronization engine that synchronizes knowledgebase data between the MarkAmp workspace data directory and an external local directory (e.g., a network drive, USB drive, Dropbox folder, or another filesystem path). This is the foundational sync layer upon which cloud sync (Phase 38) is built. The engine uses a snapshot-based approach: it computes file-level change sets by comparing SHA-256 hashes between the local and remote directories, merges changes bidirectionally, detects and handles conflicts when the same file has been modified on both sides, and applies the merged changes atomically with file-level locking to prevent corruption.

The sync model is inspired by SiYuan's `kernel/model/sync.go`, which synchronizes data snapshots between local storage and a remote repository. The key insight is that sync operates on immutable snapshots rather than live data — the engine first creates a local snapshot, then compares it with the remote snapshot, computes the merge plan, and applies changes. This ensures consistency even if the sync process is interrupted.

Conflict resolution follows a configurable strategy: by default, conflicting files (modified on both sides since last sync) create a conflict copy with a timestamped suffix (e.g., `document-conflict-20260214.sy`), preserving both versions for manual resolution. Users can also configure automatic resolution to always prefer the local or remote version. The SyncEngine emits progress events throughout the process so the UI can display a progress bar with the current file being processed.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 04 (SQLite Storage Layer)
- Phase 34 (Repository System — for snapshot creation)

## SiYuan Source Reference

- `kernel/model/sync.go` — SyncData (main entry point), syncData (core logic), CreateCloudSyncDir, SetCloudSyncDir, ListCloudSyncDir, PerformSync
- `kernel/conf/sync.go` — SyncConf struct (Enabled, Mode, Perception, GenerateConflictDoc, Provider, S3/WebDAV configs)
- `kernel/model/file.go` — File-level operations used during sync (read, write, hash)
- `kernel/dejavu/` — Content-addressable storage layer for snapshot diffs

## MarkAmp Integration Points

- New header: `src/core/SyncEngine.h`
- New source: `src/core/SyncEngine.cpp`
- New header: `src/core/SyncTypes.h`
- Extends `Events.h` with sync lifecycle events
- Connects to RepositoryService (Phase 34) for snapshot creation before sync
- File locking via platform-specific advisory locks (flock on POSIX, LockFile on Windows)
- SyncEngine added to PluginContext for extension access

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Resolution strategy when the same file is modified on both sides.
enum class ConflictResolution : uint8_t
{
    KeepBoth,    // Create conflict copy, keep both versions (default)
    UseLocal,    // Local version always wins
    UseRemote,   // Remote version always wins
    Manual       // Pause sync and prompt user for each conflict
};

// Direction of a sync operation.
enum class SyncDirection : uint8_t
{
    Upload,      // Local -> Remote
    Download,    // Remote -> Local
    Bidirectional // Both directions (default)
};

// State of a sync operation.
enum class SyncState : uint8_t
{
    Idle,
    DetectingChanges,
    MergingChanges,
    ApplyingChanges,
    ResolvingConflicts,
    Completed,
    Failed,
    Cancelled
};

// Information about a single file in a directory.
struct FileInfo
{
    std::string relative_path;                   // Path relative to sync root
    std::string hash;                            // SHA-256 hash of file content
    int64_t size_bytes{0};                       // File size
    int64_t modified_time{0};                    // Last modification time (Unix ms)
    bool is_directory{false};

    [[nodiscard]] auto filename() const -> std::string;
    [[nodiscard]] auto parent_path() const -> std::string;
};

// Set of changes detected in a directory relative to last sync state.
struct ChangeSet
{
    std::vector<std::string> added;              // Files added since last sync
    std::vector<std::string> modified;           // Files modified since last sync
    std::vector<std::string> deleted;            // Files deleted since last sync
    std::vector<std::string> unchanged;          // Files unchanged since last sync

    [[nodiscard]] auto total_changes() const -> int
    {
        return static_cast<int>(added.size() + modified.size() + deleted.size());
    }

    [[nodiscard]] auto has_changes() const -> bool
    {
        return !added.empty() || !modified.empty() || !deleted.empty();
    }

    [[nodiscard]] auto is_empty() const -> bool
    {
        return added.empty() && modified.empty() && deleted.empty() && unchanged.empty();
    }
};

// Information about a conflict between local and remote versions.
struct ConflictInfo
{
    std::string path;                            // Relative path of the conflicting file
    std::string local_hash;                      // Hash of the local version
    std::string remote_hash;                     // Hash of the remote version
    int64_t local_modified{0};                   // Local modification time (Unix ms)
    int64_t remote_modified{0};                  // Remote modification time (Unix ms)
    int64_t local_size{0};                       // Local file size
    int64_t remote_size{0};                      // Remote file size
    ConflictResolution resolution{ConflictResolution::KeepBoth};
    std::string conflict_copy_path;              // Path of the conflict copy (if KeepBoth)

    [[nodiscard]] auto formatted_local_date() const -> std::string;
    [[nodiscard]] auto formatted_remote_date() const -> std::string;
};

// Result of merging local and remote change sets.
struct MergeResult
{
    std::vector<std::string> to_upload;          // Files to copy local -> remote
    std::vector<std::string> to_download;        // Files to copy remote -> local
    std::vector<std::string> to_delete_local;    // Files to delete locally
    std::vector<std::string> to_delete_remote;   // Files to delete remotely
    std::vector<ConflictInfo> conflicts;         // Files modified on both sides

    [[nodiscard]] auto total_operations() const -> int
    {
        return static_cast<int>(to_upload.size() + to_download.size() +
                                to_delete_local.size() + to_delete_remote.size());
    }

    [[nodiscard]] auto has_conflicts() const -> bool
    {
        return !conflicts.empty();
    }
};

// Overall result of a sync operation.
struct SyncResult
{
    int32_t uploaded{0};                         // Files uploaded (local -> remote)
    int32_t downloaded{0};                       // Files downloaded (remote -> local)
    int32_t deleted_local{0};                    // Files deleted locally
    int32_t deleted_remote{0};                   // Files deleted remotely
    int32_t conflicts{0};                        // Conflicts encountered
    int32_t errors{0};                           // Errors encountered
    int64_t bytes_uploaded{0};                   // Total bytes uploaded
    int64_t bytes_downloaded{0};                 // Total bytes downloaded
    int64_t elapsed_ms{0};                       // Total sync time
    std::vector<ConflictInfo> conflict_details;  // Details of each conflict
    std::vector<std::string> error_messages;     // Error details
    SyncState final_state{SyncState::Completed};

    [[nodiscard]] auto succeeded() const -> bool
    {
        return final_state == SyncState::Completed && errors == 0;
    }

    [[nodiscard]] auto total_files_processed() const -> int
    {
        return uploaded + downloaded + deleted_local + deleted_remote;
    }

    [[nodiscard]] auto formatted_elapsed() const -> std::string;
    [[nodiscard]] auto formatted_bytes_transferred() const -> std::string;
};

// Sync state snapshot stored for change detection between sync runs.
struct SyncStateSnapshot
{
    std::unordered_map<std::string, std::string> file_hashes;  // path -> SHA-256 hash
    int64_t timestamp{0};                        // When this snapshot was taken (Unix ms)

    // Serialize to JSON for persistence.
    [[nodiscard]] auto to_json() const -> std::string;

    // Deserialize from JSON.
    [[nodiscard]] static auto from_json(const std::string& json)
        -> std::expected<SyncStateSnapshot, std::string>;
};

// Configuration for sync behavior.
struct SyncConfig
{
    std::filesystem::path local_path;            // Local data directory
    std::filesystem::path remote_path;           // Remote/external directory
    SyncDirection direction{SyncDirection::Bidirectional};
    ConflictResolution conflict_resolution{ConflictResolution::KeepBoth};
    int interval_minutes{0};                     // 0 = manual sync only
    bool enabled{false};
    std::vector<std::string> exclude_patterns;   // Glob patterns to exclude (e.g., "*.tmp")
};

// SyncEngine — orchestrates local directory synchronization.
class SyncEngine
{
public:
    SyncEngine(EventBus& event_bus, Config& config);

    // Perform a full sync between local and remote directories.
    [[nodiscard]] auto sync(const std::filesystem::path& local_dir,
                             const std::filesystem::path& remote_dir)
        -> std::expected<SyncResult, std::string>;

    // Detect changes in a directory since the last sync state snapshot.
    [[nodiscard]] auto detect_changes(const std::filesystem::path& dir,
                                       const SyncStateSnapshot& previous_state)
        -> std::expected<ChangeSet, std::string>;

    // Merge local and remote change sets into a sync plan.
    [[nodiscard]] auto merge_changes(const ChangeSet& local_changes,
                                      const ChangeSet& remote_changes,
                                      ConflictResolution resolution)
        -> MergeResult;

    // Apply the merged changes to the target directories.
    [[nodiscard]] auto apply_changes(const MergeResult& merge_result,
                                      const std::filesystem::path& local_dir,
                                      const std::filesystem::path& remote_dir)
        -> std::expected<SyncResult, std::string>;

    // Cancel a running sync operation.
    auto cancel() -> void;

    // Get the current sync state.
    [[nodiscard]] auto state() const -> SyncState;

    // Get the current sync configuration.
    [[nodiscard]] auto sync_config() const -> const SyncConfig&;

    // Check if a sync is currently in progress.
    [[nodiscard]] auto is_syncing() const -> bool
    {
        return state_ != SyncState::Idle &&
               state_ != SyncState::Completed &&
               state_ != SyncState::Failed &&
               state_ != SyncState::Cancelled;
    }

private:
    EventBus& event_bus_;
    Config& config_;
    SyncConfig sync_config_;
    std::atomic<SyncState> state_{SyncState::Idle};
    std::atomic<bool> cancel_requested_{false};

    // Scan a directory and compute file hashes for all files.
    [[nodiscard]] auto scan_directory(const std::filesystem::path& dir) const
        -> std::expected<SyncStateSnapshot, std::string>;

    // Compute SHA-256 hash of a file.
    [[nodiscard]] auto hash_file(const std::filesystem::path& path) const
        -> std::expected<std::string, std::string>;

    // Copy a file with integrity verification (hash check after copy).
    [[nodiscard]] auto copy_file_verified(const std::filesystem::path& source,
                                           const std::filesystem::path& dest) const
        -> std::expected<int64_t, std::string>;

    // Create a conflict copy with timestamped suffix.
    [[nodiscard]] auto create_conflict_copy(const std::filesystem::path& file_path) const
        -> std::expected<std::string, std::string>;

    // Acquire a file lock for sync operations.
    [[nodiscard]] auto acquire_lock(const std::filesystem::path& lock_file) const
        -> std::expected<void, std::string>;

    // Release a file lock.
    auto release_lock(const std::filesystem::path& lock_file) const -> void;

    // Load the previous sync state from disk.
    [[nodiscard]] auto load_sync_state(const std::filesystem::path& state_file) const
        -> std::expected<SyncStateSnapshot, std::string>;

    // Save the current sync state to disk.
    [[nodiscard]] auto save_sync_state(const SyncStateSnapshot& state,
                                        const std::filesystem::path& state_file) const
        -> std::expected<void, std::string>;

    // Check if a file matches any exclude pattern.
    [[nodiscard]] auto is_excluded(const std::string& relative_path) const -> bool;

    // Publish progress event with current state.
    auto publish_progress(int progress_percent,
                           const std::string& current_file) -> void;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `SyncEngine::sync()` — Main entry point. Acquire lock on both directories. Load previous sync state. Scan both directories for current state. Detect changes on each side. Merge changes. Apply changes. Save new sync state. Release locks. Publish SyncCompletedEvent. Return SyncResult. Handle cancellation at each major step.
2. `SyncEngine::detect_changes()` — Compare current directory scan with the previous SyncStateSnapshot. Files with new paths = added. Files with same path but different hash = modified. Files in previous state but not on disk = deleted. Files with same path and same hash = unchanged.
3. `SyncEngine::merge_changes()` — Cross-reference local and remote change sets. Added locally + not in remote = upload. Added remotely + not in local = download. Modified locally + unchanged remotely = upload. Modified remotely + unchanged locally = download. Modified on both sides = conflict. Deleted locally + unchanged remotely = delete remote. Deleted remotely + unchanged locally = delete local. Deleted on one side + modified on other = conflict.
4. `SyncEngine::apply_changes()` — Execute the MergeResult plan. Copy files for uploads and downloads using `copy_file_verified()`. Delete files as planned. Resolve conflicts according to the configured ConflictResolution strategy. Publish SyncProgressEvent after each file. Track bytes transferred. Handle errors gracefully (continue on error, collect error messages).
5. `SyncEngine::scan_directory()` — Recursively walk the directory using `std::filesystem::recursive_directory_iterator` with `std::error_code`. Skip excluded patterns. For each regular file, compute hash and record FileInfo. Return SyncStateSnapshot with the complete file hash map.
6. `SyncEngine::copy_file_verified()` — Copy file using `std::filesystem::copy_file` with `std::error_code`. After copy, hash the destination file and verify it matches the source hash. If mismatch, delete the corrupted copy and return error. Return bytes copied on success.
7. `SyncEngine::create_conflict_copy()` — Given a file path, construct a conflict copy path by inserting `-conflict-YYYYMMDD-HHmmss` before the file extension. Copy the file to the conflict path. Return the conflict copy path.
8. `SyncEngine::merge_changes()` conflict detection — For each file path, check if it appears in both local and remote change sets as modified. If so, create a ConflictInfo record with both hashes, modification times, and sizes.
9. `SyncStateSnapshot::to_json()` / `from_json()` — Serialize the file hash map as JSON: `{ "timestamp": 123456, "files": { "path1": "hash1", "path2": "hash2" } }`. Parse back from JSON string using the JSON library already in the project.
10. `SyncEngine::is_excluded()` — Match the relative file path against the list of exclude patterns. Support glob-style patterns with `*` and `**` wildcards. Common defaults: `"*.tmp"`, `"*.lock"`, `".sync_state"`.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Sync engine events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncStartedEvent)
std::string local_path;
std::string remote_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncCompletedEvent)
int32_t uploaded{0};
int32_t downloaded{0};
int32_t conflicts{0};
int32_t errors{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncConflictEvent)
std::string path;
std::string local_hash;
std::string remote_hash;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncProgressEvent)
int progress_percent{0};
std::string current_file;
SyncState state{SyncState::Idle};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncErrorEvent)
std::string error_message;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(SyncCancelledEvent);
```

## Config Keys to Add

- `knowledgebase.sync.enabled` — Enable sync functionality (default: `false`)
- `knowledgebase.sync.local_path` — Local data directory to sync (default: `""` — uses workspace data dir)
- `knowledgebase.sync.remote_path` — Remote/external directory path (default: `""`)
- `knowledgebase.sync.interval_minutes` — Auto-sync interval in minutes; 0 = manual only (default: `0`)
- `knowledgebase.sync.conflict_resolution` — Default conflict strategy: `"keep_both"`, `"use_local"`, `"use_remote"`, `"manual"` (default: `"keep_both"`)
- `knowledgebase.sync.direction` — Sync direction: `"upload"`, `"download"`, `"bidirectional"` (default: `"bidirectional"`)
- `knowledgebase.sync.exclude_patterns` — Comma-separated glob patterns to exclude (default: `"*.tmp,*.lock,.sync_state"`)

## Test Cases (Catch2)

File: `tests/unit/test_sync_engine.cpp`

1. **Detect no changes in unchanged directory** — Create a directory, scan it, save state, scan again. Verify ChangeSet: added/modified/deleted all empty, unchanged matches file count.
2. **Detect added files** — Create state snapshot, add 2 new files, detect changes. Verify ChangeSet.added contains both files, modified and deleted are empty.
3. **Detect modified files** — Create state snapshot, modify content of 1 file (hash changes), detect changes. Verify ChangeSet.modified contains that file.
4. **Detect deleted files** — Create state snapshot with 3 files, delete 1 file, detect changes. Verify ChangeSet.deleted contains the deleted file.
5. **Merge bidirectional changes without conflicts** — Local adds file A, remote adds file B. Merge. Verify to_upload = [A], to_download = [B], conflicts is empty.
6. **Merge detects conflict on same-file modification** — Local modifies file X, remote also modifies file X. Merge. Verify conflicts contains X with both hashes and timestamps.
7. **Apply sync copies files correctly** — Set up local dir with file A, empty remote dir. Apply merge result that uploads A. Verify A exists in remote dir with identical hash.
8. **Conflict creates timestamped copy** — Configure KeepBoth resolution. Trigger conflict on file X. Verify X-conflict-YYYYMMDD file created. Verify both versions preserved.
9. **Sync state snapshot serialization roundtrip** — Create SyncStateSnapshot with 5 file hashes. Serialize to JSON. Deserialize back. Verify all hashes match, timestamp matches.
10. **Exclude patterns filter files** — Configure exclude pattern `"*.tmp"`. Add files `data.sy` and `scratch.tmp`. Verify sync ignores `scratch.tmp`, processes `data.sy`.

## Acceptance Criteria

- [ ] SyncEngine detects added, modified, and deleted files via hash comparison
- [ ] Bidirectional merge correctly classifies files as upload, download, or conflict
- [ ] File copies are verified with post-copy hash check
- [ ] Conflict resolution creates timestamped conflict copies (KeepBoth mode)
- [ ] SyncStateSnapshot persists to JSON and survives restart
- [ ] File-level locking prevents concurrent sync corruption
- [ ] Progress events published with percentage and current file name
- [ ] Cancellation stops the sync process gracefully at the next checkpoint
- [ ] Exclude patterns filter out specified file types
- [ ] All 10 test cases pass

## Files to Create/Modify

- CREATE: `src/core/SyncTypes.h`
- CREATE: `src/core/SyncEngine.h`
- CREATE: `src/core/SyncEngine.cpp`
- MODIFY: `src/core/Events.h` (add SyncStartedEvent, SyncCompletedEvent, SyncConflictEvent, SyncProgressEvent, SyncErrorEvent, SyncCancelledEvent)
- MODIFY: `src/core/PluginContext.h` (add `SyncEngine*` pointer)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate SyncEngine, wire to PluginContext)
- MODIFY: `src/CMakeLists.txt` (add SyncEngine.cpp to source list)
- CREATE: `tests/unit/test_sync_engine.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_sync_engine target)
