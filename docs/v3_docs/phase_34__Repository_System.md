# Phase 34 — Repository System (Snapshots, Tags, Checkout)

## Objective

Implement a Git-like repository system for workspace-wide versioning of the entire knowledgebase. While Phase 32 provides per-document history, this phase extends versioning to capture the complete state of all notebooks at a point in time as a Snapshot. Users can create snapshots manually or on a scheduled interval, tag them with descriptive names for easy identification, list and browse snapshot history, compare any two snapshots to see what changed, and check out (roll back to) a previous snapshot to restore the entire workspace to that state.

The repository system is modeled after SiYuan's `kernel/model/repository.go`, which provides CreateSnapshot, TagSnapshot, GetRepoSnapshots, CheckoutRepo, and PurgeRepo operations. Each snapshot is a compressed archive of the data directory stored in a dedicated `repo/` directory. The system supports automatic cleanup of old snapshots to prevent unbounded storage growth, and exposes repo size information so users can monitor disk usage.

This phase introduces the RepositoryService class as the central API for all snapshot operations. The service compresses snapshot data using zlib or zstd for space efficiency, maintains a snapshot index file (JSON) for fast listing without decompressing archives, and integrates with the EventBus to notify the UI of snapshot lifecycle events. The SnapshotDiff capability leverages the DiffEngine from Phase 33 to compare block-level changes between two snapshots.

## Prerequisites

- Phase 01 (Block Data Model Core)
- Phase 04 (SQLite Storage Layer)
- Phase 32 (Document History & Versioning)
- Phase 33 (Version Diff & Comparison Engine)

## SiYuan Source Reference

- `kernel/model/repository.go` — CreateSnapshot, TagSnapshot, GetRepoSnapshots, GetRepoTagSnapshots, CheckoutRepo, PurgeRepo, DiffRepoSnapshots, GetRepoSnapshotCheckIndex
- `kernel/conf/repo.go` — RepoConf struct (SyncEnabled, Key, SyncMode, SyncPerception)
- `kernel/model/storage.go` — Underlying storage operations for snapshot data
- `kernel/dejavu/` — Content-addressable storage and snapshot management engine

## MarkAmp Integration Points

- New header: `src/core/RepositoryService.h`
- New source: `src/core/RepositoryService.cpp`
- New header: `src/core/SnapshotTypes.h`
- Extends `Events.h` with snapshot lifecycle events
- Connects to Block model (Phase 01), SQLite storage (Phase 04), DiffEngine (Phase 33)
- Repository data stored in `<workspace>/repo/` directory
- Snapshot index stored in `<workspace>/repo/snapshots.json`

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Represents a single point-in-time snapshot of the entire workspace.
struct Snapshot
{
    std::string id;                              // Unique snapshot ID (SHA-256 of contents)
    std::string memo;                            // User-provided description/memo
    std::string tag;                             // Optional named tag (e.g., "v1.0", "before-refactor")
    int64_t created{0};                          // Unix milliseconds when snapshot was created
    int64_t size_bytes{0};                       // Total uncompressed size of snapshot data
    int64_t compressed_size_bytes{0};            // Compressed archive size on disk
    int32_t block_count{0};                      // Total number of blocks in the snapshot
    int32_t doc_count{0};                        // Number of documents in the snapshot
    int32_t notebook_count{0};                   // Number of notebooks in the snapshot
    std::string archive_path;                    // Path to the compressed archive file

    // Returns true if this snapshot has a named tag
    [[nodiscard]] auto is_tagged() const -> bool
    {
        return !tag.empty();
    }

    // Returns a human-readable size string (e.g., "12.5 MB")
    [[nodiscard]] auto formatted_size() const -> std::string;

    // Returns formatted creation timestamp
    [[nodiscard]] auto formatted_date() const -> std::string;

    // Returns compression ratio as a percentage (compressed / uncompressed * 100)
    [[nodiscard]] auto compression_ratio() const -> double;
};

// Diff result between two snapshots, showing which documents changed.
struct SnapshotDiff
{
    std::string snapshot_id_a;                   // Old snapshot ID
    std::string snapshot_id_b;                   // New snapshot ID
    std::vector<std::string> added_docs;         // Document paths only in snapshot B
    std::vector<std::string> removed_docs;       // Document paths only in snapshot A
    std::vector<std::string> modified_docs;      // Document paths changed between A and B
    int32_t added_blocks{0};                     // Total new blocks across all added/modified docs
    int32_t removed_blocks{0};                   // Total removed blocks across all removed/modified docs
    int32_t modified_blocks{0};                  // Total modified blocks across all modified docs

    [[nodiscard]] auto total_doc_changes() const -> int
    {
        return static_cast<int>(added_docs.size() + removed_docs.size() + modified_docs.size());
    }

    [[nodiscard]] auto has_changes() const -> bool
    {
        return !added_docs.empty() || !removed_docs.empty() || !modified_docs.empty();
    }
};

// Statistics returned from a purge operation.
struct PurgeStats
{
    int32_t snapshots_removed{0};                // Number of snapshots deleted
    int64_t bytes_freed{0};                      // Total disk space freed in bytes
    int32_t snapshots_remaining{0};              // Number of snapshots still present
    int64_t remaining_size_bytes{0};             // Total size of remaining snapshots

    [[nodiscard]] auto formatted_freed() const -> std::string;
};

// Compression algorithm for snapshot archives.
enum class CompressionType : uint8_t
{
    None,       // No compression (for testing/debugging)
    Zlib,       // zlib/deflate compression
    Zstd        // Zstandard compression (default, better ratio and speed)
};

// Configuration for the repository system.
struct RepositoryConfig
{
    std::filesystem::path repo_dir;              // Path to repo/ directory
    CompressionType compression{CompressionType::Zstd};
    int compression_level{3};                    // 1-22 for zstd, 1-9 for zlib
    int max_snapshots{50};                       // Maximum number of snapshots to retain
    int auto_snapshot_interval_minutes{0};       // 0 = disabled
    bool auto_tag_date{false};                   // Auto-tag with date on creation
};

// RepositoryService — manages workspace-wide snapshots and versioning.
class RepositoryService
{
public:
    RepositoryService(EventBus& event_bus, Config& config);

    // Initialize the repository directory structure.
    [[nodiscard]] auto initialize(const std::filesystem::path& workspace_path)
        -> std::expected<void, std::string>;

    // Create a new snapshot of the current workspace state.
    [[nodiscard]] auto create_snapshot(const std::string& memo)
        -> std::expected<Snapshot, std::string>;

    // Assign a named tag to an existing snapshot.
    [[nodiscard]] auto tag_snapshot(const std::string& snapshot_id,
                                     const std::string& tag_name)
        -> std::expected<void, std::string>;

    // Remove a tag from a snapshot.
    [[nodiscard]] auto untag_snapshot(const std::string& snapshot_id)
        -> std::expected<void, std::string>;

    // List all snapshots, ordered by creation time descending.
    // Supports pagination via page/page_size.
    [[nodiscard]] auto list_snapshots(int page = 0, int page_size = 20) const
        -> std::vector<Snapshot>;

    // List only tagged snapshots.
    [[nodiscard]] auto list_tagged_snapshots() const
        -> std::vector<Snapshot>;

    // Retrieve a specific snapshot by ID.
    [[nodiscard]] auto get_snapshot(const std::string& snapshot_id) const
        -> std::expected<Snapshot, std::string>;

    // Restore the workspace to the state captured in a snapshot.
    // This replaces all current data with the snapshot's data.
    [[nodiscard]] auto checkout_snapshot(const std::string& snapshot_id)
        -> std::expected<void, std::string>;

    // Compare two snapshots and return the differences.
    [[nodiscard]] auto diff_snapshots(const std::string& snapshot_id_a,
                                       const std::string& snapshot_id_b)
        -> std::expected<SnapshotDiff, std::string>;

    // Remove old snapshots, keeping at most keep_count.
    // Tagged snapshots are never purged unless explicitly requested.
    [[nodiscard]] auto purge_old_snapshots(int keep_count)
        -> std::expected<PurgeStats, std::string>;

    // Delete a specific snapshot by ID.
    [[nodiscard]] auto delete_snapshot(const std::string& snapshot_id)
        -> std::expected<void, std::string>;

    // Returns the total disk size of all snapshot archives.
    [[nodiscard]] auto get_repo_size() const -> int64_t;

    // Returns the total number of snapshots.
    [[nodiscard]] auto get_snapshot_count() const -> int;

    // Check if auto-snapshot timer should fire.
    [[nodiscard]] auto is_auto_snapshot_due() const -> bool;

private:
    EventBus& event_bus_;
    Config& config_;
    RepositoryConfig repo_config_;
    std::vector<Snapshot> snapshot_index_;        // In-memory snapshot index
    std::filesystem::path workspace_path_;

    // Load snapshot index from disk.
    [[nodiscard]] auto load_index() -> std::expected<void, std::string>;

    // Save snapshot index to disk.
    [[nodiscard]] auto save_index() const -> std::expected<void, std::string>;

    // Compress a directory into an archive file.
    [[nodiscard]] auto compress_directory(const std::filesystem::path& source_dir,
                                           const std::filesystem::path& archive_path)
        -> std::expected<int64_t, std::string>;

    // Extract an archive into a target directory.
    [[nodiscard]] auto extract_archive(const std::filesystem::path& archive_path,
                                        const std::filesystem::path& target_dir)
        -> std::expected<void, std::string>;

    // Compute SHA-256 hash of a directory's contents for snapshot ID.
    [[nodiscard]] auto compute_directory_hash(const std::filesystem::path& dir) const
        -> std::string;

    // Count blocks and documents in a directory.
    [[nodiscard]] auto count_contents(const std::filesystem::path& dir) const
        -> std::pair<int32_t, int32_t>; // {block_count, doc_count}
};

} // namespace markamp::core
```

## Key Functions to Implement

1. `RepositoryService::initialize()` — Create `<workspace>/repo/` directory if it does not exist. Create `snapshots.json` index file if missing. Load the index into memory. Set up auto-snapshot timer if configured.
2. `RepositoryService::create_snapshot()` — Copy the current `data/` directory to a temporary staging area, compute the directory hash for the snapshot ID, compress the staging directory into `repo/<id>.zst`, count blocks and docs, create Snapshot record, append to index, save index, publish SnapshotCreatedEvent. Clean up staging directory.
3. `RepositoryService::tag_snapshot()` — Find snapshot by ID in the index, set the tag field, save index. Return error if ID not found or tag name already used by another snapshot.
4. `RepositoryService::list_snapshots()` — Return a paginated slice of the in-memory index sorted by created descending. Validate page/page_size bounds.
5. `RepositoryService::checkout_snapshot()` — Find snapshot by ID. Create a backup of current data (as an auto-snapshot with memo "Pre-checkout backup"). Extract the snapshot archive to a temporary directory. Replace the current data directory with the extracted contents. Publish SnapshotCheckedOutEvent. Reload all open documents.
6. `RepositoryService::diff_snapshots()` — Extract both snapshots to temporary directories. Parse the block trees from each. Use DiffEngine::diff_documents() to compare. Aggregate results into SnapshotDiff. Clean up temporary directories.
7. `RepositoryService::purge_old_snapshots()` — Sort snapshots by creation time. Keep the most recent `keep_count` plus all tagged snapshots. Delete archive files for purged snapshots. Update index. Return PurgeStats. Publish SnapshotPurgedEvent.
8. `RepositoryService::compress_directory()` — Walk the source directory recursively. For zstd: use ZSTD_compress() on concatenated file data with a manifest header. For zlib: use deflate. Write to archive_path. Return compressed size.
9. `RepositoryService::extract_archive()` — Read archive file, decompress, recreate directory structure and files in target_dir. Verify integrity with checksums.
10. `RepositoryService::compute_directory_hash()` — Walk directory, hash each file with SHA-256, combine hashes into a Merkle-style root hash. Use `std::error_code` overloads for all filesystem operations.
11. `Snapshot::formatted_size()` — Convert size_bytes to human-readable string: bytes, KB, MB, GB as appropriate.
12. `Snapshot::compression_ratio()` — Return `(static_cast<double>(compressed_size_bytes) / size_bytes) * 100.0`, guarding against division by zero.
13. `PurgeStats::formatted_freed()` — Convert bytes_freed to human-readable string.

## Events to Add (in Events.h)

```cpp
// ============================================================================
// Repository / Snapshot events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotCreatedEvent)
std::string snapshot_id;
std::string memo;
int64_t size_bytes{0};
int32_t doc_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotCheckedOutEvent)
std::string snapshot_id;
std::string memo;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotTaggedEvent)
std::string snapshot_id;
std::string tag_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotPurgedEvent)
int32_t snapshots_removed{0};
int64_t bytes_freed{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotDiffComputedEvent)
std::string snapshot_id_a;
std::string snapshot_id_b;
int32_t total_doc_changes{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutoSnapshotTriggeredEvent)
std::string snapshot_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

- `knowledgebase.repo.auto_snapshot_interval_minutes` — Interval for automatic snapshots in minutes; 0 disables (default: `0`)
- `knowledgebase.repo.max_snapshots` — Maximum number of snapshots to retain before auto-purge (default: `50`)
- `knowledgebase.repo.compression` — Compression algorithm: `"none"`, `"zlib"`, `"zstd"` (default: `"zstd"`)
- `knowledgebase.repo.compression_level` — Compression level; 1-9 for zlib, 1-22 for zstd (default: `3`)
- `knowledgebase.repo.auto_tag_date` — Automatically tag snapshots with creation date (default: `false`)
- `knowledgebase.repo.purge_keep_tagged` — Never purge tagged snapshots during auto-purge (default: `true`)

## Test Cases (Catch2)

File: `tests/unit/test_repository_service.cpp`

1. **Initialize creates repo directory** — Call `initialize()` on an empty workspace path. Verify `repo/` directory exists, `snapshots.json` is created and contains an empty array, `get_snapshot_count()` returns 0.
2. **Create snapshot captures workspace state** — Create a workspace with 3 mock documents. Call `create_snapshot("test memo")`. Verify returned Snapshot has non-empty id, correct memo, size_bytes > 0, doc_count == 3, archive file exists on disk.
3. **Tag and untag snapshot** — Create a snapshot, tag it with "release-1.0". Verify `is_tagged()` returns true. List tagged snapshots, verify it appears. Untag it, verify `is_tagged()` returns false.
4. **List snapshots with pagination** — Create 5 snapshots. Call `list_snapshots(0, 2)`, verify 2 returned. Call `list_snapshots(1, 2)`, verify 2 returned. Call `list_snapshots(2, 2)`, verify 1 returned. Verify ordering is newest first.
5. **Checkout restores workspace state** — Create snapshot S1. Modify workspace data. Create snapshot S2. Checkout S1. Verify workspace data matches S1's state. Verify a pre-checkout backup snapshot was auto-created.
6. **Diff snapshots detects changes** — Create S1 with docs [A, B, C]. Modify doc B, add doc D, remove doc C. Create S2. Call `diff_snapshots(S1.id, S2.id)`. Verify: added_docs contains D, removed_docs contains C, modified_docs contains B.
7. **Purge removes old snapshots** — Create 10 snapshots, tag snapshot #5. Call `purge_old_snapshots(3)`. Verify 6 removed (10 - 3 kept - 1 tagged = 6 purged). Verify tagged snapshot still exists. Verify `bytes_freed > 0`.
8. **Delete specific snapshot** — Create 3 snapshots. Delete the middle one. Verify `get_snapshot_count()` == 2. Verify deleted snapshot's archive file no longer exists. Verify `get_snapshot()` returns error for deleted ID.
9. **Repo size calculation** — Create 3 snapshots. Verify `get_repo_size()` equals sum of all archive file sizes. Delete one. Verify size decreases.
10. **Snapshot formatted output** — Create a snapshot with known sizes. Verify `formatted_size()` produces correct human-readable string (e.g., "1.5 MB"). Verify `compression_ratio()` returns expected percentage. Verify `formatted_date()` produces valid date string.

## Acceptance Criteria

- [ ] RepositoryService creates and manages snapshot archives in `repo/` directory
- [ ] Snapshots compress workspace data using configurable compression (zstd default)
- [ ] Tag system allows naming snapshots for easy identification
- [ ] Checkout restores complete workspace state from a snapshot
- [ ] Auto-creates pre-checkout backup before restoring
- [ ] Diff between snapshots correctly identifies added, removed, and modified documents
- [ ] Purge respects tagged snapshots and keep_count
- [ ] Snapshot index persists to `snapshots.json` and survives app restart
- [ ] All 10 test cases pass
- [ ] Uses `std::error_code` overloads for all filesystem operations

## Files to Create/Modify

- CREATE: `src/core/SnapshotTypes.h`
- CREATE: `src/core/RepositoryService.h`
- CREATE: `src/core/RepositoryService.cpp`
- MODIFY: `src/core/Events.h` (add SnapshotCreatedEvent, SnapshotCheckedOutEvent, SnapshotTaggedEvent, SnapshotPurgedEvent, SnapshotDiffComputedEvent, AutoSnapshotTriggeredEvent)
- MODIFY: `src/core/Config.h` (add repo config cached values if needed)
- MODIFY: `src/core/PluginContext.h` (add `RepositoryService*` pointer)
- MODIFY: `src/app/MarkAmpApp.cpp` (instantiate and wire RepositoryService)
- MODIFY: `src/CMakeLists.txt` (add RepositoryService.cpp to source list)
- CREATE: `tests/unit/test_repository_service.cpp`
- MODIFY: `tests/CMakeLists.txt` (add test_repository_service target)
