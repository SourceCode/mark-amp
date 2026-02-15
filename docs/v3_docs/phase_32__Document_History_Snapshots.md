# Phase 32 -- Document History & Snapshot System

## Objective

Implement document history tracking and a snapshot system that records every significant change to a document, enabling users to browse the history timeline, view diffs between versions, and roll back to any previous state. SiYuan's history system (implemented across `kernel/model/history.go` and `kernel/sql/history.go`) automatically saves timestamped copies of document files whenever content changes, organizing them into date-based directory trees. This phase ports that capability into MarkAmp's architecture, providing a local version control system that operates independently of git.

The history system works at two levels: automatic snapshots and manual snapshots. Automatic snapshots are created on every document save, debounced to at most one snapshot per configurable interval (default: 60 seconds). Each snapshot records the full document content at that point in time, stored as a timestamped copy in a `history/{YYYY-MM-DD-HHmmss}/` directory within the workspace. Manual snapshots can be created on demand via a "Create Snapshot" action, allowing users to mark specific points in a document's evolution. Each history entry records the operation type (update, delete, format change, or manual snapshot), enabling filtered browsing.

The HistoryPanel provides a visual timeline of document changes with: a document selector dropdown, a chronological list of history entries with timestamps and operation types, a side-by-side or inline diff view comparing any two versions, and a rollback button that restores a previous version (with confirmation dialog). The diff computation uses a standard line-based diff algorithm (Myers or patience diff) with colored additions/deletions rendering. History cleanup runs periodically, purging entries older than the configured retention period (default: 30 days) and limiting the total entries per document to prevent unbounded storage growth.

## Prerequisites

- No strict phase dependencies. Uses existing MarkAmp infrastructure: EventBus, Config, FileSystem.

## SiYuan Source Reference

- `kernel/model/history.go` -- `GetDocHistories()`, `RollbackDocHistory()`, `ClearWorkspaceHistory()`, `GetNotebookHistory()`, `HistoryItems`, auto-history on document save, history directory structure
- `kernel/sql/history.go` -- History database operations, history entry schema, query by root_id/box_id/type
- `kernel/model/history.go:autoSaveHistory()` -- Debounced auto-save triggered by document modification events
- History types: `HistoryOpUpdate` (content change), `HistoryOpDelete` (block/doc deletion), `HistoryOpClean` (cleanup), `HistoryOpFormat` (format change)
- SiYuan stores history in `history/{YYYY-MM-DD-HHmmss}/{notebook_id}/{document_path}` directory structure

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/HistoryTypes.h` | `markamp::core` | HistoryEntry, HistoryOpType, HistoryDiff structs |
| `src/core/HistoryService.h` | `markamp::core` | HistoryService class declaration |
| `src/core/HistoryService.cpp` | `markamp::core` | HistoryService implementation |
| `src/core/DiffEngine.h` | `markamp::core` | Line-based diff computation |
| `src/core/DiffEngine.cpp` | `markamp::core` | DiffEngine implementation |
| `src/ui/HistoryPanel.h` | `markamp::ui` | History browser and diff viewer panel |
| `src/ui/HistoryPanel.cpp` | `markamp::ui` | HistoryPanel implementation |
| `tests/unit/test_history_service.cpp` | (test) | Catch2 test suite for history system |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add history lifecycle events |
| `src/core/PluginContext.h` | Add `HistoryService*` pointer |
| `src/ui/MainFrame.h` | Add history panel management and menu entries |
| `src/ui/MainFrame.cpp` | Wire View > Document History menu entry |
| `src/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test_history_service target |
| `resources/config_defaults.json` | Add history config defaults |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/HistoryTypes.h
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// The type of operation that created a history entry.
enum class HistoryOpType : uint8_t
{
    Update,    // Content was modified
    Delete,    // Document or block was deleted
    Clean,     // Cleanup operation (history purge)
    Format,    // Formatting change (no semantic change)
    Snapshot   // Manual user-created snapshot
};

/// A single history entry representing a saved version of a document.
struct HistoryEntry
{
    std::string id;            // Unique entry ID (UUID)
    std::string root_id;       // Document root ID (file ID)
    std::string notebook_id;   // Notebook/folder containing the document
    std::string file_path;     // Relative path to the original document
    std::string history_path;  // Path to the history copy (within history/ dir)
    std::string title;         // Document title at the time of snapshot
    HistoryOpType op_type{HistoryOpType::Update};
    std::string content_hash;  // SHA-256 hash of the content (for deduplication)
    std::size_t content_size{0}; // Size in bytes of the saved content
    std::chrono::system_clock::time_point created;

    [[nodiscard]] auto is_update() const -> bool
    {
        return op_type == HistoryOpType::Update;
    }

    [[nodiscard]] auto is_manual_snapshot() const -> bool
    {
        return op_type == HistoryOpType::Snapshot;
    }

    [[nodiscard]] auto created_display() const -> std::string;

    [[nodiscard]] auto op_type_display() const -> std::string_view
    {
        switch (op_type)
        {
        case HistoryOpType::Update:
            return "Modified";
        case HistoryOpType::Delete:
            return "Deleted";
        case HistoryOpType::Clean:
            return "Cleaned";
        case HistoryOpType::Format:
            return "Formatted";
        case HistoryOpType::Snapshot:
            return "Snapshot";
        }
        return "Unknown";
    }
};

/// A diff hunk representing a contiguous change between two versions.
enum class DiffLineType : uint8_t
{
    Context,   // Unchanged line (shown for context)
    Addition,  // Line added in the new version
    Deletion   // Line removed from the old version
};

struct DiffLine
{
    DiffLineType type{DiffLineType::Context};
    int old_line_number{-1}; // Line number in old version (-1 if addition)
    int new_line_number{-1}; // Line number in new version (-1 if deletion)
    std::string content;
};

struct DiffHunk
{
    int old_start{0};
    int old_count{0};
    int new_start{0};
    int new_count{0};
    std::vector<DiffLine> lines;

    [[nodiscard]] auto header() const -> std::string
    {
        return "@@ -" + std::to_string(old_start) + "," + std::to_string(old_count)
             + " +" + std::to_string(new_start) + "," + std::to_string(new_count) + " @@";
    }
};

/// Result of comparing two document versions.
struct DiffResult
{
    std::string old_title;
    std::string new_title;
    std::string old_timestamp;
    std::string new_timestamp;
    std::vector<DiffHunk> hunks;
    int additions{0};
    int deletions{0};
    int context_lines{0};

    [[nodiscard]] auto is_identical() const -> bool
    {
        return additions == 0 && deletions == 0;
    }

    [[nodiscard]] auto total_changes() const -> int
    {
        return additions + deletions;
    }

    [[nodiscard]] auto change_summary() const -> std::string
    {
        return "+" + std::to_string(additions) + " -" + std::to_string(deletions);
    }
};

/// Summary of history for a document.
struct HistorySummary
{
    std::string root_id;
    std::string title;
    int entry_count{0};
    std::chrono::system_clock::time_point oldest_entry;
    std::chrono::system_clock::time_point newest_entry;
    std::size_t total_storage_bytes{0};
};

/// Configuration for the history system.
struct HistoryConfig
{
    int retention_days{30};               // Delete entries older than this
    int max_entries_per_doc{100};          // Maximum entries per document
    int auto_save_interval_seconds{60};   // Minimum seconds between auto-saves
    int diff_context_lines{3};            // Context lines in diff output
    bool auto_save_enabled{true};         // Enable automatic history on save
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/HistoryService.h
// ============================================================================
#pragma once

#include "HistoryTypes.h"

#include <chrono>
#include <expected>
#include <filesystem>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

/// Service for managing document history and snapshots.
/// Creates timestamped copies of documents, supports browsing history,
/// computing diffs, and rolling back to previous versions.
class HistoryService
{
public:
    HistoryService(EventBus& event_bus, Config& config,
                   std::filesystem::path workspace_root);
    ~HistoryService() = default;

    // Non-copyable
    HistoryService(const HistoryService&) = delete;
    auto operator=(const HistoryService&) -> HistoryService& = delete;

    /// Initialize the history system: create directories, subscribe to events.
    [[nodiscard]] auto initialize() -> std::expected<void, std::string>;

    // ── History Retrieval ──

    /// Get history entries for a specific document.
    [[nodiscard]] auto get_doc_histories(const std::string& root_id,
                                         int page, int page_size) const
        -> std::expected<std::vector<HistoryEntry>, std::string>;

    /// Get history entries for a specific notebook.
    [[nodiscard]] auto get_notebook_history(const std::string& notebook_id,
                                            HistoryOpType type_filter,
                                            const std::string& query) const
        -> std::expected<std::vector<HistoryEntry>, std::string>;

    /// Get all documents that have history entries.
    [[nodiscard]] auto get_documents_with_history() const
        -> std::vector<HistorySummary>;

    /// Get the content of a specific history entry.
    [[nodiscard]] auto get_history_content(const std::string& history_entry_id) const
        -> std::expected<std::string, std::string>;

    // ── Snapshot Creation ──

    /// Create an automatic history snapshot for a document.
    /// Debounced: skips if the last snapshot for this document was too recent.
    [[nodiscard]] auto create_auto_snapshot(const std::string& root_id,
                                            const std::string& file_path,
                                            const std::string& content,
                                            const std::string& title)
        -> std::expected<std::string, std::string>;

    /// Create a manual snapshot (always saved, ignores debounce).
    [[nodiscard]] auto create_manual_snapshot(const std::string& root_id,
                                              const std::string& file_path,
                                              const std::string& content,
                                              const std::string& title)
        -> std::expected<std::string, std::string>;

    // ── Rollback ──

    /// Roll back a document to a specific history entry.
    /// Replaces the current document content with the history version.
    /// Creates a new history entry of the current version before rolling back.
    [[nodiscard]] auto rollback_doc(const std::string& root_id,
                                    const std::string& history_entry_id)
        -> std::expected<std::string, std::string>;

    // ── Diff ──

    /// Compute the diff between two history entries.
    [[nodiscard]] auto diff_entries(const std::string& old_entry_id,
                                    const std::string& new_entry_id) const
        -> std::expected<DiffResult, std::string>;

    /// Compute the diff between a history entry and the current document content.
    [[nodiscard]] auto diff_with_current(const std::string& history_entry_id,
                                          const std::string& current_content) const
        -> std::expected<DiffResult, std::string>;

    // ── Cleanup ──

    /// Clear all history for a specific document.
    [[nodiscard]] auto clear_doc_history(const std::string& root_id)
        -> std::expected<void, std::string>;

    /// Clear all history in the workspace.
    [[nodiscard]] auto clear_all_history()
        -> std::expected<void, std::string>;

    /// Run retention cleanup: delete entries older than retention_days
    /// and enforce max_entries_per_doc.
    [[nodiscard]] auto run_cleanup()
        -> std::expected<int, std::string>;

    // ── Configuration ──

    [[nodiscard]] auto history_config() const -> HistoryConfig;

private:
    EventBus& event_bus_;
    Config& config_;
    std::filesystem::path workspace_root_;
    std::filesystem::path history_root_; // workspace_root / "history"

    mutable std::mutex mutex_;

    /// Index of all history entries, keyed by entry ID.
    std::unordered_map<std::string, HistoryEntry> entry_index_;

    /// Per-document: last auto-snapshot timestamp (for debouncing).
    std::unordered_map<std::string, std::chrono::steady_clock::time_point>
        last_auto_snapshot_;

    /// Create a history entry with the given parameters.
    [[nodiscard]] auto create_entry(const std::string& root_id,
                                    const std::string& file_path,
                                    const std::string& content,
                                    const std::string& title,
                                    HistoryOpType op_type)
        -> std::expected<std::string, std::string>;

    /// Generate the history directory path for the current timestamp.
    [[nodiscard]] auto history_dir_for_now() const -> std::filesystem::path;

    /// Compute SHA-256 hash of content for deduplication.
    [[nodiscard]] static auto compute_hash(const std::string& content) -> std::string;

    /// Load the history index from disk.
    [[nodiscard]] auto load_index()
        -> std::expected<void, std::string>;

    /// Save the history index to disk.
    [[nodiscard]] auto save_index() const
        -> std::expected<void, std::string>;

    /// Subscribe to document save events for auto-history.
    void subscribe_to_events();

    Subscription file_saved_sub_;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/DiffEngine.h
// ============================================================================
#pragma once

#include "HistoryTypes.h"

#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Line-based diff engine using the Myers diff algorithm.
/// Computes minimal edit scripts between two text versions.
class DiffEngine
{
public:
    DiffEngine() = default;

    /// Compute the diff between two text strings.
    [[nodiscard]] auto compute_diff(std::string_view old_text,
                                    std::string_view new_text,
                                    int context_lines = 3) const -> DiffResult;

    /// Compute the diff between two sets of lines.
    [[nodiscard]] auto compute_diff_lines(const std::vector<std::string>& old_lines,
                                           const std::vector<std::string>& new_lines,
                                           int context_lines = 3) const -> DiffResult;

    /// Split text into lines for diff computation.
    [[nodiscard]] static auto split_lines(std::string_view text)
        -> std::vector<std::string>;

    /// Apply a diff to an old text to produce the new text.
    [[nodiscard]] static auto apply_diff(std::string_view old_text,
                                          const DiffResult& diff) -> std::string;

    /// Render a diff as unified diff format text.
    [[nodiscard]] static auto render_unified(const DiffResult& diff) -> std::string;

    /// Render a diff as side-by-side HTML for display.
    [[nodiscard]] static auto render_side_by_side_html(const DiffResult& diff) -> std::string;

    /// Render a diff as inline HTML (additions highlighted, deletions struck).
    [[nodiscard]] static auto render_inline_html(const DiffResult& diff) -> std::string;

private:
    /// Core Myers diff algorithm implementation.
    /// Returns a list of edit operations (insert/delete/equal).
    struct EditOp
    {
        enum class Type : uint8_t
        {
            Equal,
            Insert,
            Delete
        };
        Type type{Type::Equal};
        int old_idx{0};
        int new_idx{0};
        int count{1};
    };

    [[nodiscard]] auto myers_diff(const std::vector<std::string>& old_lines,
                                   const std::vector<std::string>& new_lines) const
        -> std::vector<EditOp>;

    /// Group edit operations into hunks with context lines.
    [[nodiscard]] auto create_hunks(const std::vector<EditOp>& ops,
                                     const std::vector<std::string>& old_lines,
                                     const std::vector<std::string>& new_lines,
                                     int context_lines) const
        -> std::vector<DiffHunk>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`HistoryService::initialize()`** -- Create the `history/` directory under workspace_root if it does not exist (using `std::error_code`). Call `load_index()` to read the history index from disk. Subscribe to `FileSavedEvent` for auto-history. Log the number of loaded history entries.

2. **`HistoryService::create_auto_snapshot(root_id, file_path, content, title)`** -- Check the debounce map: if the last auto-snapshot for this `root_id` was less than `auto_save_interval_seconds` ago, return early (no-op, return empty string). Compute the content hash. Check if the most recent entry for this document has the same hash (content unchanged) -- skip if identical. Otherwise, call `create_entry()` with `HistoryOpType::Update`. Update the debounce map. Publish `HistoryCreatedEvent`.

3. **`HistoryService::create_entry(root_id, file_path, content, title, op_type)`** -- Generate a UUID for the entry. Compute the history directory path using the current timestamp (`history/2026-02-14-153045/`). Create the directory. Write the content to a file in that directory, using atomic write (temp + rename). Build a HistoryEntry struct. Add to `entry_index_`. Save the updated index. Return the entry ID.

4. **`HistoryService::rollback_doc(root_id, history_entry_id)`** -- Look up the history entry. Read its content via `get_history_content()`. Before overwriting, create a new auto-snapshot of the current document (so the pre-rollback state is preserved). Write the history content to the original file path. Publish `HistoryRolledBackEvent`. Return the content.

5. **`HistoryService::get_doc_histories(root_id, page, page_size)`** -- Filter `entry_index_` for entries with matching `root_id`. Sort by `created` descending (newest first). Apply pagination: skip `page * page_size` entries, return up to `page_size` entries.

6. **`HistoryService::run_cleanup()`** -- Iterate all entries. Remove entries where `created` is older than `retention_days` from now. For each document, if there are more than `max_entries_per_doc` entries, remove the oldest ones beyond the limit. Delete the corresponding history files. Update the index. Return the number of entries removed.

7. **`HistoryService::diff_entries(old_entry_id, new_entry_id)`** -- Load content for both entries via `get_history_content()`. Pass both to `DiffEngine::compute_diff()`. Set titles and timestamps from the entry metadata. Return the DiffResult.

8. **`DiffEngine::compute_diff(old_text, new_text, context_lines)`** -- Split both texts into lines via `split_lines()`. Run `myers_diff()` to compute the edit script. Group operations into hunks via `create_hunks()` with the specified context lines. Count additions and deletions. Return the assembled DiffResult.

9. **`DiffEngine::myers_diff(old_lines, new_lines)`** -- Implement the Myers O(ND) diff algorithm. Compute the shortest edit script (SES) that transforms old_lines into new_lines. Return a sequence of EditOp entries (Equal, Insert, Delete) with line indices.

10. **`DiffEngine::create_hunks(ops, old_lines, new_lines, context_lines)`** -- Group consecutive changes into DiffHunks. For each group, include `context_lines` unchanged lines before and after. Merge adjacent hunks if their context regions overlap. Build DiffLine entries with line numbers and content.

11. **`DiffEngine::render_side_by_side_html(diff)`** -- Generate an HTML table with two columns: old version (left) and new version (right). Context lines appear in both columns. Deletions appear in the left column with red background. Additions appear in the right column with green background. Line numbers in margin.

12. **`HistoryPanel::populate_timeline(root_id)`** -- Fetch history entries via `history_service_.get_doc_histories(root_id, 0, 50)`. Display as a vertical timeline in the panel. Each entry shows: timestamp, operation type badge, title, content size. Click an entry to load its content. Shift-click two entries to show a diff between them.

13. **`HistoryPanel::show_diff(old_entry_id, new_entry_id)`** -- Fetch the diff via `history_service_.diff_entries()`. Display using the selected diff mode (side-by-side or inline). Show the change summary ("+N -N") in the header. Highlight additions in green, deletions in red.

14. **`HistoryPanel::OnRollbackClicked(entry_id)`** -- Show a confirmation dialog: "Roll back to version from {timestamp}? Current changes will be saved as a snapshot first." On confirm, call `history_service_.rollback_doc()`. Refresh the file in the editor via `FileReloadRequestEvent`.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// Document history events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(HistoryCreatedEvent)
std::string root_id;
std::string entry_id;
HistoryOpType op_type{HistoryOpType::Update};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(HistoryRolledBackEvent)
std::string root_id;
std::string entry_id;
std::string restored_content_hash;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(HistoryClearedEvent)
std::string root_id; // Empty string means all history cleared
int entries_removed{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShowDocHistoryRequestEvent)
std::string root_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.history.retention_days` | int | 30 | Delete history entries older than this |
| `knowledgebase.history.max_entries_per_doc` | int | 100 | Maximum history entries per document |
| `knowledgebase.history.auto_save_interval_seconds` | int | 60 | Minimum seconds between auto-snapshots |
| `knowledgebase.history.auto_save_enabled` | bool | true | Enable automatic history on save |
| `knowledgebase.history.diff_context_lines` | int | 3 | Context lines shown in diff views |
| `knowledgebase.history.diff_mode` | string | "side-by-side" | Default diff display mode |

## Test Cases

All tests in `tests/unit/test_history_service.cpp` using Catch2.

1. **"create_auto_snapshot saves content to history directory"** -- Create an auto-snapshot. Verify the entry appears in `get_doc_histories()`. Verify the content is retrievable via `get_history_content()` and matches the original.

2. **"auto-snapshot debounces within interval"** -- Create an auto-snapshot. Immediately create another for the same document. Verify only 1 entry exists (the second was debounced). Wait beyond the interval and create a third. Verify 2 entries exist.

3. **"auto-snapshot skips identical content"** -- Create a snapshot with content "hello". Create another with the same content (after debounce interval). Verify only 1 entry exists (content hash matched, deduplication).

4. **"manual snapshot ignores debounce"** -- Create an auto-snapshot. Immediately create a manual snapshot. Verify 2 entries exist (manual snapshot always saves).

5. **"rollback restores content and preserves pre-rollback state"** -- Create 3 snapshots with different content. Roll back to snapshot 1. Verify the document content matches snapshot 1. Verify a new snapshot was created for the pre-rollback content (total is now 4 entries).

6. **"get_doc_histories paginates correctly"** -- Create 15 snapshots. Request page 0 with page_size 10. Verify 10 entries returned, sorted newest first. Request page 1. Verify 5 entries returned.

7. **"run_cleanup removes old entries"** -- Create entries with artificial timestamps: 5 from 40 days ago, 3 from 10 days ago. Run cleanup with retention_days=30. Verify 5 entries were removed. Verify 3 remain.

8. **"DiffEngine computes correct additions and deletions"** -- Old: "line1\nline2\nline3". New: "line1\nline2_modified\nline3\nline4". Verify diff shows: 1 deletion (line2), 1 addition (line2_modified), 1 addition (line4). Total: additions=2, deletions=1.

9. **"DiffEngine handles empty old text (new document)"** -- Old: "" (empty). New: "line1\nline2". Verify diff shows 2 additions and 0 deletions.

10. **"DiffEngine identical texts produce zero changes"** -- Old and new are the same text. Verify `is_identical()` returns true. Verify hunks list is empty.

## Acceptance Criteria

- [ ] Auto-snapshots are created on document save with proper debouncing
- [ ] Content hash deduplication prevents saving identical consecutive versions
- [ ] Manual snapshots bypass debounce and always save
- [ ] Rollback restores content and creates a pre-rollback snapshot for safety
- [ ] History entries are paginated and sorted newest-first
- [ ] Cleanup removes entries exceeding retention_days and max_entries_per_doc
- [ ] DiffEngine correctly computes line-based diffs with context
- [ ] Side-by-side and inline diff rendering produce valid HTML
- [ ] All filesystem operations use std::error_code overloads
- [ ] HistoryPanel displays timeline, supports entry selection and diff viewing

## Files to Create/Modify

```
CREATE  src/core/HistoryTypes.h
CREATE  src/core/HistoryService.h
CREATE  src/core/HistoryService.cpp
CREATE  src/core/DiffEngine.h
CREATE  src/core/DiffEngine.cpp
CREATE  src/ui/HistoryPanel.h
CREATE  src/ui/HistoryPanel.cpp
CREATE  tests/unit/test_history_service.cpp
MODIFY  src/core/Events.h              -- add history lifecycle events
MODIFY  src/core/PluginContext.h        -- add HistoryService* history_service{nullptr};
MODIFY  src/ui/MainFrame.h             -- add history panel management
MODIFY  src/ui/MainFrame.cpp           -- wire View > Document History menu
MODIFY  src/CMakeLists.txt              -- add new source files
MODIFY  tests/CMakeLists.txt            -- add test_history_service target
MODIFY  resources/config_defaults.json  -- add knowledgebase.history.* defaults
```
