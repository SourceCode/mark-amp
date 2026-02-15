# Phase 36 -- Version History and Snapshots

## Objective

Implement local version history: automatic periodic snapshots of documents, a history panel showing all versions with timestamps, diff between versions, and one-click restore to a previous version. Works independently of Git (local-only, lightweight). Snapshots are stored in `.markamp/history/` as compressed diffs.

## Prerequisites

- Phase 02 (VaultService -- document access)
- Phase 35 (GitService -- diff algorithm reuse, optional)

## Feature References (PRD)

- PRD #19: Version History / Revisions
- PRD #41: Note History and Snapshots
- PRD Notebook #17: Auto-Save + Checkpoints

## Data Structures to Implement

### File: `src/core/VersionHistoryService.h`

```cpp
#pragma once

#include <chrono>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;

struct Snapshot
{
    std::string snapshot_id;
    std::string document_id;
    std::chrono::system_clock::time_point timestamp;
    int64_t size_bytes{0};
    std::string summary;        // First 100 chars of content
    std::string change_type;    // "auto", "manual", "save"
};

struct VersionDiff
{
    std::string old_snapshot_id;
    std::string new_snapshot_id;
    std::vector<std::string> added_lines;
    std::vector<std::string> removed_lines;
    int additions{0};
    int deletions{0};
};

class VersionHistoryService
{
public:
    VersionHistoryService(EventBus& event_bus, Config& config, VaultService& vault_service);

    /// Create a snapshot of the current document state.
    [[nodiscard]] auto create_snapshot(const std::string& document_id,
                                        const std::string& change_type = "manual")
        -> std::expected<std::string, std::string>;

    /// List all snapshots for a document.
    [[nodiscard]] auto list_snapshots(const std::string& document_id) const
        -> std::vector<Snapshot>;

    /// Get the content of a specific snapshot.
    [[nodiscard]] auto get_snapshot_content(const std::string& snapshot_id) const
        -> std::expected<std::string, std::string>;

    /// Diff between two snapshots.
    [[nodiscard]] auto diff_snapshots(const std::string& old_id,
                                       const std::string& new_id) const
        -> std::expected<VersionDiff, std::string>;

    /// Diff between a snapshot and the current document state.
    [[nodiscard]] auto diff_with_current(const std::string& snapshot_id,
                                          const std::string& document_id) const
        -> std::expected<VersionDiff, std::string>;

    /// Restore a document to a snapshot version.
    [[nodiscard]] auto restore_snapshot(const std::string& snapshot_id)
        -> std::expected<void, std::string>;

    /// Delete a specific snapshot.
    auto delete_snapshot(const std::string& snapshot_id) -> void;

    /// Prune old snapshots (keep last N per document).
    auto prune(int keep_per_document = 50) -> int;

    /// Enable/disable auto-snapshots on save.
    auto set_auto_snapshot(bool enabled) -> void;

    /// Get auto-snapshot interval.
    [[nodiscard]] auto auto_snapshot_interval_minutes() const -> int;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;

    auto history_dir() const -> std::filesystem::path;
    auto snapshot_path(const std::string& snapshot_id) const -> std::filesystem::path;
    auto compute_diff(const std::string& old_content, const std::string& new_content) const
        -> VersionDiff;

    Subscription doc_saved_sub_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`create_snapshot(document_id, change_type)`** -- Read current document content. Compute content hash. If identical to last snapshot, skip. Otherwise, compress and write to history dir. Return snapshot ID.

2. **`list_snapshots(document_id)`** -- Read snapshot metadata files from `.markamp/history/doc_id/`. Sort by timestamp descending.

3. **`get_snapshot_content(snapshot_id)`** -- Read compressed snapshot file. Decompress and return content.

4. **`diff_snapshots(old_id, new_id)`** -- Load both snapshot contents. Compute line-level diff using LCS algorithm.

5. **`restore_snapshot(snapshot_id)`** -- Load snapshot content. Create a "before restore" snapshot of current state. Write snapshot content to document file. Publish SnapshotRestoredEvent.

6. **`prune(keep_per_document)`** -- For each document, sort snapshots by timestamp. Delete all but the most recent `keep_per_document` snapshots.

7. **Auto-snapshot** -- Subscribe to FileSavedEvent. On each save (or at configured interval), create an auto-snapshot.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotCreatedEvent)
std::string snapshot_id;
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotRestoredEvent)
std::string snapshot_id;
std::string document_id;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_version_history.cpp`

1. **Create snapshot** -- Save document. Create snapshot. Verify snapshot exists.
2. **List snapshots** -- Create 3 snapshots. list_snapshots() returns 3, newest first.
3. **Get content** -- Create snapshot. Retrieve content. Verify matches original.
4. **Diff snapshots** -- Create 2 snapshots with changes. Diff shows additions/deletions.
5. **Diff with current** -- Snapshot from earlier. Modify document. diff_with_current() shows changes.
6. **Restore snapshot** -- Modify document. Restore earlier snapshot. Verify content reverted.
7. **Prune** -- Create 60 snapshots. prune(50). Verify 50 remain.
8. **Skip identical** -- Save without changes. create_snapshot() skips (returns existing ID).
9. **Auto-snapshot on save** -- Enable auto. Save document. Verify snapshot created.
10. **Delete snapshot** -- Delete specific snapshot. Verify removed.

## Acceptance Criteria

- [ ] Snapshots created and stored as compressed files
- [ ] Snapshot content retrievable and decompressible
- [ ] Diff between snapshots shows line-level changes
- [ ] Restore reverts document to snapshot state (with backup)
- [ ] Pruning keeps only recent snapshots per document
- [ ] Auto-snapshot on save works when enabled
- [ ] Identical content detection skips redundant snapshots
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/VersionHistoryService.h` | VersionHistoryService, Snapshot, VersionDiff |
| CREATE | `src/core/VersionHistoryService.cpp` | Full implementation |
| CREATE | `src/ui/HistoryPanel.h` | History sidebar panel |
| CREATE | `src/ui/HistoryPanel.cpp` | Snapshot list and diff UI |
| MODIFY | `src/core/Events.h` | Add 2 snapshot events |
| MODIFY | `src/core/PluginContext.h` | Add `VersionHistoryService* version_history{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add new .cpp files |
| CREATE | `tests/unit/test_version_history.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_version_history target |

## Estimated Complexity

**L** -- Snapshot storage, compression, LCS diff, auto-snapshot, prune logic, 10 tests.
