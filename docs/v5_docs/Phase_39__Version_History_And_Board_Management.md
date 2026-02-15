# Phase 39 -- Version History and Board Management

## Objective

Implement board version history: automatic snapshots on save, a timeline slider to browse past versions, visual diff between versions, and restore to a previous version. Also includes board archiving, duplication with history, and board management (list, search, archive).

## Prerequisites

- Phase 05 (Board, BoardSerializer)
- Phase 02 (CanvasPanel for rendering snapshots)

## Feature References (PRD)

- PRD #20: Version History
- PRD #85: Board Duplication with History
- PRD #88: Board Archiving

## Data Structures to Implement

### File: `src/canvas/VersionHistory.h`

```cpp
#pragma once

#include "Board.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

struct VersionSnapshot
{
    std::string version_id;
    std::string board_id;
    std::chrono::system_clock::time_point timestamp;
    std::string description;
    std::string serialized_board; // Full board JSON
    int object_count{0};
    size_t byte_size{0};
};

struct VersionDiff
{
    std::vector<ObjectId> added_objects;
    std::vector<ObjectId> removed_objects;
    std::vector<ObjectId> modified_objects;
    int total_changes{0};
};

class VersionHistory
{
public:
    explicit VersionHistory(const std::filesystem::path& history_dir);

    /// Create a snapshot of the current board state.
    auto create_snapshot(const Board& board, const std::string& description = "") -> std::string;

    /// Get all snapshots for a board, most recent first.
    [[nodiscard]] auto get_snapshots(const std::string& board_id) const
        -> std::vector<VersionSnapshot>;

    /// Load a specific snapshot as a Board.
    [[nodiscard]] auto load_snapshot(const std::string& version_id) const
        -> std::unique_ptr<Board>;

    /// Compute the diff between two snapshots.
    [[nodiscard]] auto diff(const std::string& version_a, const std::string& version_b) const
        -> VersionDiff;

    /// Restore a board to a specific snapshot.
    [[nodiscard]] auto restore(const std::string& version_id) const -> std::unique_ptr<Board>;

    /// Delete snapshots older than a given duration.
    auto prune(std::chrono::hours max_age) -> int;

    /// Get total disk usage for a board's history.
    [[nodiscard]] auto disk_usage(const std::string& board_id) const -> size_t;

    /// Set maximum snapshots per board.
    auto set_max_snapshots(int max) -> void;

private:
    std::filesystem::path history_dir_;
    int max_snapshots_{50};
    BoardSerializer serializer_;

    [[nodiscard]] auto snapshot_path(const std::string& version_id) const
        -> std::filesystem::path;
};

} // namespace markamp::canvas
```

### File: `src/canvas/HistoryPanel.h`

```cpp
#pragma once

#include "VersionHistory.h"

#include <functional>

class wxGraphicsContext;

namespace markamp::canvas
{

class HistoryPanel
{
public:
    using OnRestoreCallback = std::function<void(const std::string& version_id)>;
    using OnPreviewCallback = std::function<void(const std::string& version_id)>;

    auto set_snapshots(const std::vector<VersionSnapshot>& snapshots) -> void;
    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;

    /// Timeline slider value (0.0 = oldest, 1.0 = most recent).
    auto set_slider_value(double value) -> void;
    [[nodiscard]] auto slider_value() const -> double;

    auto set_on_restore(OnRestoreCallback cb) -> void;
    auto set_on_preview(OnPreviewCallback cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<VersionSnapshot> snapshots_;
    double slider_value_{1.0};
    bool visible_{false};
    OnRestoreCallback on_restore_;
    OnPreviewCallback on_preview_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/BoardManager.h`

```cpp
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace markamp::canvas
{

struct BoardSummary
{
    std::string id;
    std::string name;
    std::filesystem::path path;
    std::chrono::system_clock::time_point modified_at;
    int object_count{0};
    bool archived{false};
};

class BoardManager
{
public:
    explicit BoardManager(const std::filesystem::path& workspace_dir);

    /// Scan workspace for boards.
    auto scan() -> void;

    [[nodiscard]] auto all_boards() const -> const std::vector<BoardSummary>&;
    [[nodiscard]] auto active_boards() const -> std::vector<const BoardSummary*>;
    [[nodiscard]] auto archived_boards() const -> std::vector<const BoardSummary*>;

    /// Archive a board (sets flag, moves to archive subdirectory).
    auto archive_board(const std::string& board_id) -> bool;
    /// Unarchive a board.
    auto unarchive_board(const std::string& board_id) -> bool;

    /// Duplicate a board (deep copy with new ID, optionally copy history).
    auto duplicate_board(const std::string& board_id, bool copy_history = false) -> std::string;

    /// Delete a board permanently.
    auto delete_board(const std::string& board_id) -> bool;

    /// Search boards by name.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<const BoardSummary*>;

private:
    std::filesystem::path workspace_dir_;
    std::vector<BoardSummary> boards_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `VersionHistory::create_snapshot()` -- Serialize board to JSON, generate version ID (UUID or timestamp-based), write to history directory.

2. `VersionHistory::diff()` -- Load both snapshots, compare object IDs. Objects in B but not A are added. Objects in A but not B are removed. Objects in both with different JSON are modified.

3. `VersionHistory::restore()` -- Load snapshot, return as new Board. The caller replaces the current board and creates a "Restored from version X" snapshot.

4. `HistoryPanel` -- Timeline slider with snapshot markers. Hovering a marker shows timestamp and description. Clicking shows a preview. Double-click restores.

5. `BoardManager::duplicate_board()` -- Load board, clone, assign new ID, save. If copy_history, copy all snapshot files.

6. `BoardManager::archive_board()` -- Set archived flag in board metadata. Move file to workspace/archive/ subdirectory.

## Test Cases

File: `tests/unit/test_version_history.cpp`

1. **Create snapshot** -- Create, verify file exists.
2. **Get snapshots** -- Create 3, verify all returned in order.
3. **Load snapshot** -- Load, verify board state matches.
4. **Diff** -- Modify board, create second snapshot, diff, verify changes.
5. **Restore** -- Restore to old snapshot, verify board state.
6. **Prune** -- Create old snapshots, prune, verify removed.
7. **Max snapshots** -- Set max=3, create 5, verify only 3 remain.

File: `tests/unit/test_board_manager.cpp`

8. **Scan boards** -- Create temp .markboard files, scan, verify found.
9. **Archive** -- Archive board, verify in archived list.
10. **Unarchive** -- Unarchive, verify back in active list.
11. **Duplicate** -- Duplicate, verify new ID, same content.
12. **Delete** -- Delete, verify file removed.
13. **Search** -- Search by name, verify matches.

## Acceptance Criteria

- [ ] Automatic snapshots on board save
- [ ] Timeline slider to browse version history
- [ ] Visual diff between versions (added/removed/modified counts)
- [ ] Restore to any previous version
- [ ] Snapshot pruning by age and count
- [ ] Board archiving with archived badge
- [ ] Board duplication with optional history copy
- [ ] Board manager with search, list, archive, delete
- [ ] All 13 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/VersionHistory.h` | Version history management |
| CREATE | `src/canvas/VersionHistory.cpp` | Snapshot CRUD, diff, restore |
| CREATE | `src/canvas/HistoryPanel.h` | History timeline UI |
| CREATE | `src/canvas/HistoryPanel.cpp` | Panel implementation |
| CREATE | `src/canvas/BoardManager.h` | Board listing, archive, duplicate |
| CREATE | `src/canvas/BoardManager.cpp` | Board manager implementation |
| MODIFY | `src/ui/CanvasPanel.cpp` | Auto-snapshot on save |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_version_history.cpp` | 7 tests |
| CREATE | `tests/unit/test_board_manager.cpp` | 6 tests |

## Architecture Notes

- Snapshots are stored as individual JSON files in a `.markboard_history/` directory alongside each board file.
- The diff algorithm compares object IDs and their serialized JSON. It does not do deep field-level diffing.
- Pruning runs on application startup or when snapshot count exceeds max.
- Board archiving is a soft delete: the file is moved to an archive subdirectory and the metadata flag is set.

## Estimated Complexity

**XL** -- Version history with snapshots, diff, restore, pruning; board manager with archive, duplicate, delete; timeline panel; 13 tests.
