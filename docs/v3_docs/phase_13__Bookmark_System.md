# Phase 13 -- Bookmark Manager

**Priority:** Medium (quality-of-life feature for quick access)
**Estimated Scope:** ~9 files created/modified
**Dependencies:** Existing EventBus, Config, PluginContext; Phase 09 BlockId

---

## Objective

Implement a bookmark system for quick access to important blocks. Bookmarks are stored as IAL (Inline Attribute List) attributes (`bookmark="label"`) on blocks and indexed for fast retrieval. Users can bookmark any block with a label, then browse/search bookmarks grouped by label in a sidebar panel.

SiYuan's bookmark system is lightweight: a single `bookmark` attribute on the block's IAL, with the value being the bookmark label. Blocks with the same label form a logical bookmark group. The bookmark panel shows groups with expandable block lists.

---

## Prerequisites

- `src/core/EventBus.h` -- publish/subscribe for bookmark lifecycle events
- `src/core/Config.h` -- bookmark display settings
- `src/core/PluginContext.h` -- service injection
- Phase 09 `BlockId` type for block identification
- SQLite3 for bookmark persistence (query blocks table IAL column)

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/model/bookmark.go` | `GetBookmarks` | `BookmarkService::get_bookmarks()` |
| `kernel/model/bookmark.go` | `RenameBookmark` | `BookmarkService::rename_bookmark()` |
| `kernel/model/bookmark.go` | `RemoveBookmark` | `BookmarkService::remove_bookmark()` |
| `kernel/sql/block_query.go` | `QueryBookmarkBlocks` | `BookmarkIndex::get_blocks_with_label()` |
| `kernel/sql/block_query.go` | `QueryBookmarkLabels` | `BookmarkIndex::get_all_labels()` |
| `kernel/api/attr.go` | Set/get IAL attributes | `BookmarkService::set_bookmark()` |

---

## MarkAmp Integration Points

1. **Block IAL System** -- Bookmarks are stored as `bookmark="My Label"` in the block's inline attribute list. This requires an IAL storage mechanism (can be a simple `std::unordered_map<std::string, std::string>` per block).
2. **EditorPanel** -- Right-click context menu includes "Add Bookmark" / "Remove Bookmark" options.
3. **HtmlRenderer** -- Bookmarked blocks get a visual indicator (small bookmark icon in the left gutter).
4. **ActivityBar / Sidebar** -- New `BookmarkPanel` sidebar component showing bookmark groups.
5. **PluginContext** -- `BookmarkService*` pointer added for extension access.
6. **PreviewPanel** -- Click on a bookmarked block's indicator in preview navigates to the block in editor.

---

## Data Structures to Implement

All structures go in `src/core/Bookmark.h`:

```cpp
#pragma once

#include "BlockRef.h"

#include <chrono>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// A single bookmark entry associating a block with a label.
struct BookmarkEntry
{
    /// The bookmarked block.
    BlockId block_id;

    /// The bookmark label (group name).
    std::string label;

    /// The root document containing this block.
    std::string root_id;

    /// Document title for display.
    std::string doc_title;

    /// Block type (heading, paragraph, etc.) for icon display.
    std::string block_type;

    /// Content snippet of the bookmarked block (first 100 chars).
    std::string content_snippet;

    /// When the bookmark was created.
    std::chrono::system_clock::time_point created_at;
};

/// A group of bookmarks sharing the same label.
struct BookmarkGroup
{
    /// The bookmark label (group name).
    std::string label;

    /// All blocks bookmarked with this label.
    std::vector<BookmarkEntry> entries;

    /// Number of blocks in this group.
    [[nodiscard]] auto count() const -> std::size_t
    {
        return entries.size();
    }

    /// Whether the group is empty.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return entries.empty();
    }
};

/// In-memory bookmark index for fast lookups.
class BookmarkIndex
{
public:
    /// Add a bookmark.
    void add(const BlockId& block_id, const std::string& label);

    /// Remove a bookmark from a specific block.
    void remove(const BlockId& block_id);

    /// Check if a block is bookmarked.
    [[nodiscard]] auto is_bookmarked(const BlockId& block_id) const -> bool;

    /// Get the bookmark label for a block (empty if not bookmarked).
    [[nodiscard]] auto get_label(const BlockId& block_id) const -> std::string;

    /// Get all blocks with a specific label.
    [[nodiscard]] auto get_blocks_with_label(const std::string& label) const
        -> std::vector<BlockId>;

    /// Get all unique bookmark labels.
    [[nodiscard]] auto get_all_labels() const -> std::vector<std::string>;

    /// Rename a label across all bookmarks.
    /// Returns the number of bookmarks affected.
    [[nodiscard]] auto rename_label(const std::string& old_label, const std::string& new_label)
        -> int;

    /// Remove all bookmarks with a specific label.
    /// Returns the number of bookmarks removed.
    [[nodiscard]] auto remove_label(const std::string& label) -> int;

    /// Get total number of bookmarks.
    [[nodiscard]] auto total_count() const -> std::size_t;

    /// Clear the entire index.
    void clear();

private:
    /// block_id -> label
    std::unordered_map<BlockId, std::string, BlockIdHash> block_to_label_;

    /// label -> set of block IDs
    std::unordered_map<std::string, std::unordered_set<BlockId, BlockIdHash>> label_to_blocks_;
};

/// Inline Attribute List for a block.
/// Stores key-value pairs (bookmark, name, alias, custom-attributes, etc.)
struct BlockAttributes
{
    /// The block this attribute set belongs to.
    BlockId block_id;

    /// Key-value attribute pairs.
    std::unordered_map<std::string, std::string> attrs;

    /// Get an attribute value, or empty string if not present.
    [[nodiscard]] auto get(const std::string& key) const -> std::string;

    /// Set an attribute value.
    void set(const std::string& key, const std::string& value);

    /// Remove an attribute.
    void remove(const std::string& key);

    /// Check if an attribute exists.
    [[nodiscard]] auto has(const std::string& key) const -> bool;

    /// Serialize attributes to IAL string format: `{: key="value" key2="value2" }`.
    [[nodiscard]] auto to_ial_string() const -> std::string;

    /// Parse attributes from IAL string format.
    [[nodiscard]] static auto from_ial_string(std::string_view ial) -> BlockAttributes;
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### BookmarkService class (`src/core/BookmarkService.h` / `src/core/BookmarkService.cpp`)

Constructor: `BookmarkService(EventBus& event_bus, Config& config)`

1. **`auto get_bookmarks() -> std::vector<BookmarkGroup>`** -- Return all bookmarks grouped by label. Each `BookmarkGroup` contains its label and all `BookmarkEntry` items (with content snippets and document titles populated). Groups are sorted alphabetically by label. Entries within each group are sorted by document title then by content.

2. **`auto get_bookmark_blocks(const std::string& label) -> std::vector<BookmarkEntry>`** -- Return all bookmark entries for a specific label. Populates content snippets and document metadata from the block database.

3. **`void set_bookmark(const BlockId& block_id, const std::string& label)`** -- Set a bookmark on a block. If the block already has a different bookmark label, replaces it. Updates the block's IAL attribute `bookmark="label"`. Updates BookmarkIndex. Publishes `BookmarkAddedEvent`.

4. **`void remove_bookmark(const BlockId& block_id)`** -- Remove the bookmark from a block. Removes the `bookmark` IAL attribute. Updates BookmarkIndex. Publishes `BookmarkRemovedEvent`.

5. **`void rename_bookmark(const std::string& old_label, const std::string& new_label)`** -- Rename a bookmark label across all blocks. Updates IAL attributes on all affected blocks. Updates BookmarkIndex. Publishes `BookmarkRenamedEvent`.

6. **`auto search_bookmarks(const std::string& query) -> std::vector<BookmarkGroup>`** -- Search bookmark labels and block content snippets for the query string (case-insensitive substring match). Returns matching groups with only matching entries.

7. **`auto get_bookmark_labels() -> std::vector<std::pair<std::string, int>>`** -- Return all unique bookmark labels with their block counts. Lightweight alternative to `get_bookmarks()` when full entry data is not needed.

8. **`void rebuild_index()`** -- Full rebuild of BookmarkIndex from block database. Scans all blocks for `bookmark` IAL attribute and populates the index. Called on workspace open.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Bookmark events (Phase 13)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkAddedEvent)
std::string block_id;
std::string label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkRemovedEvent)
std::string block_id;
std::string previous_label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkRenamedEvent)
std::string old_label;
std::string new_label;
int affected_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(BookmarkPanelOpenRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkNavigateEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.bookmarks.show_in_sidebar` | bool | `true` | Show bookmark panel in sidebar |
| `knowledgebase.bookmarks.show_gutter_icon` | bool | `true` | Show bookmark icon in editor gutter |
| `knowledgebase.bookmarks.default_label` | string | `"Favorites"` | Default label when no label is specified |
| `knowledgebase.bookmarks.sort_by` | string | `"label"` | Sort order: `"label"`, `"created"`, `"document"` |
| `knowledgebase.bookmarks.snippet_length` | int | `100` | Max characters for content snippet in bookmark list |

---

## Test Cases

File: `tests/unit/test_bookmarks.cpp`

```cpp
#include "core/Bookmark.h"
#include "core/BookmarkService.h"
#include "core/EventBus.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Add bookmark** -- Set bookmark on block A with label "Important". Verify: `BookmarkIndex::is_bookmarked(A) == true`. `BookmarkIndex::get_label(A) == "Important"`. `BookmarkAddedEvent` published with correct block_id and label.

2. **Remove bookmark** -- Add bookmark on block A, then remove it. Verify: `BookmarkIndex::is_bookmarked(A) == false`. `BookmarkRemovedEvent` published with `previous_label == "Important"`.

3. **Rename bookmark label** -- Blocks A and B both bookmarked with "OldName". Rename to "NewName". Verify: both blocks now have label "NewName". `BookmarkRenamedEvent` published with `affected_count == 2`. Label "OldName" no longer exists in index.

4. **List bookmarks grouped by label** -- Block A bookmarked "Alpha", Block B bookmarked "Alpha", Block C bookmarked "Beta". Call `get_bookmarks()`. Verify: 2 groups returned. "Alpha" group has 2 entries. "Beta" group has 1 entry. Groups sorted alphabetically.

5. **Search bookmarks** -- Bookmarks: A("Project Notes"), B("Personal"), C("Project Tasks"). Search for "project". Verify: returns groups/entries matching "project" (A and C).

6. **Bookmark on different block types** -- Bookmark a heading block, a paragraph block, and a code block. Verify: all appear in `get_bookmarks()` with correct `block_type` strings.

7. **Multiple blocks same label** -- 5 blocks all bookmarked with "ReadLater". Verify: `get_bookmark_blocks("ReadLater")` returns 5 entries. `get_bookmark_labels()` shows `("ReadLater", 5)`.

8. **Edge cases** -- Set bookmark with empty label (should use default label from config). Remove bookmark from non-bookmarked block (no-op, no event). Rename to same name (no-op). Set bookmark overwrites existing bookmark on same block.

---

## Acceptance Criteria

- [ ] `BookmarkService::set_bookmark()` correctly sets IAL attribute and updates index
- [ ] `BookmarkService::remove_bookmark()` removes IAL attribute and updates index
- [ ] `BookmarkService::rename_bookmark()` propagates rename to all affected blocks
- [ ] `BookmarkService::get_bookmarks()` returns correctly grouped and sorted results
- [ ] `BookmarkService::search_bookmarks()` performs case-insensitive matching
- [ ] `BookmarkIndex` provides O(1) lookups for `is_bookmarked()` and `get_label()`
- [ ] `BlockAttributes::to_ial_string()` / `from_ial_string()` round-trip correctly
- [ ] All 5 events fire at correct lifecycle points with correct payloads
- [ ] All 5 config keys load with correct defaults
- [ ] Empty label defaults to `knowledgebase.bookmarks.default_label` config value
- [ ] All 8 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/Bookmark.h` | `BookmarkEntry`, `BookmarkGroup`, `BookmarkIndex`, `BlockAttributes` |
| `src/core/Bookmark.cpp` | `BookmarkIndex`, `BlockAttributes` implementations |
| `src/core/BookmarkService.h` | `BookmarkService` class declaration |
| `src/core/BookmarkService.cpp` | All 8 service methods, event handling, index management |
| `src/ui/BookmarkPanel.h` | `BookmarkPanel` wxPanel sidebar component declaration |
| `src/ui/BookmarkPanel.cpp` | Grouped bookmark display with expand/collapse, context menu |
| `tests/unit/test_bookmarks.cpp` | All 8 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `BookmarkAddedEvent`, `BookmarkRemovedEvent`, `BookmarkRenamedEvent`, `BookmarkPanelOpenRequestEvent`, `BookmarkNavigateEvent` |
| `src/core/PluginContext.h` | Add `BookmarkService* bookmark_service{nullptr};` pointer |
| `src/CMakeLists.txt` | Add `core/Bookmark.cpp`, `core/BookmarkService.cpp`, `ui/BookmarkPanel.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_bookmarks` test target |
| `resources/config_defaults.json` | Add 5 `knowledgebase.bookmarks.*` config keys |
