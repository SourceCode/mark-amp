# Phase 12 -- Tag System & Tag Hierarchy

**Priority:** High (core organizational feature)
**Estimated Scope:** ~10 files created/modified
**Dependencies:** Existing EventBus, Config, PluginContext; Phase 09 BlockId (for block-tag association)

---

## Objective

Implement a hierarchical tag system for organizing blocks. Tags in SiYuan use `#tag#` syntax and support hierarchy via `/` separator (e.g., `#project/work#`). Tags are indexed and queryable, providing a cross-cutting organizational dimension orthogonal to the document tree.

Key characteristics:
- Tag syntax: `#tag-name#` delimited by `#` on both sides
- Hierarchical: `#parent/child/grandchild#` builds a tag tree where `parent` has child `child` which has child `grandchild`
- Tags are stored as inline content within blocks and separately indexed for fast lookup
- Rename/remove operations propagate to all blocks containing the affected tag
- Tag tree supports collapse/expand and count display

---

## Prerequisites

- `src/core/EventBus.h` -- publish/subscribe for tag lifecycle events
- `src/core/Config.h` -- tag display and behavior settings
- `src/core/PluginContext.h` -- service injection
- Phase 09 `BlockId` type for block-tag association
- SQLite3 for tag index persistence (blocks table `tag` column)

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/model/tag.go` | `GetTags` | `TagService::get_tags()` |
| `kernel/model/tag.go` | `RenameTag` | `TagService::rename_tag()` |
| `kernel/model/tag.go` | `RemoveTag` | `TagService::remove_tag()` |
| `kernel/api/tag.go` | API endpoint for tag operations | `TagService` public API |
| `kernel/sql/block_query.go` | Tag column queries | `TagIndex` SQL queries |
| `kernel/treenode/node.go` | Tag extraction from inline attribute list | `TagParser::parse_tags()` |

---

## MarkAmp Integration Points

1. **Md4cWrapper / MarkdownParser** -- Post-processing pass to extract `#tag#` syntax from block content. Tags are parsed but left in the rendered output (rendered as styled `<span class="tag">` elements).
2. **HtmlRenderer** -- Detect `#tag#` patterns in text nodes and render as `<span class="tag" data-tag="tag-name">#tag-name#</span>` with styling.
3. **EditorPanel** -- Autocomplete popup when user types `#` showing existing tags.
4. **ActivityBar / Sidebar** -- New `TagPanel` sidebar component showing the tag tree with counts.
5. **PluginContext** -- `TagService*` pointer added for extension access.

---

## Data Structures to Implement

All structures go in `src/core/Tag.h`:

```cpp
#pragma once

#include "BlockRef.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Information about a single tag (leaf or intermediate node in tag tree).
struct TagInfo
{
    /// The tag label (just the leaf component, e.g., "work" in "project/work").
    std::string label;

    /// The full hierarchical path (e.g., "project/work").
    std::string full_path;

    /// Number of blocks directly tagged with this exact tag.
    int direct_count{0};

    /// Total count including all descendant tags.
    int total_count{0};

    /// Child tags (next level in hierarchy).
    std::vector<TagInfo> children;

    /// Whether this node has children.
    [[nodiscard]] auto has_children() const -> bool
    {
        return !children.empty();
    }

    /// Depth in the hierarchy (0 = root level).
    int depth{0};
};

/// A parsed tag occurrence found in block content.
struct ParsedTag
{
    /// The full tag path as written (e.g., "project/work/alpha").
    std::string full_path;

    /// Character offset of the opening `#` in the source content.
    std::size_t source_offset{0};

    /// Character length of the entire tag syntax including delimiters.
    std::size_t source_length{0};

    /// Individual path components (e.g., ["project", "work", "alpha"]).
    [[nodiscard]] auto components() const -> std::vector<std::string>;
};

/// The complete tag tree for the workspace.
struct TagTree
{
    /// Root-level tags (no parent).
    std::vector<TagInfo> roots;

    /// Total number of unique tag paths.
    int unique_tag_count{0};

    /// Total number of tag usages across all blocks.
    int total_usage_count{0};

    /// Whether the tree is empty.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return roots.empty();
    }
};

/// Association between a block and its tags.
struct BlockTagEntry
{
    BlockId block_id;
    std::vector<std::string> tags; // Full paths of all tags on this block
};

/// Parses tag syntax from block content.
class TagParser
{
public:
    /// Extract all `#tag#` occurrences from content.
    /// Pattern: `#([a-zA-Z0-9_/\-]+)#` with boundary checks to avoid
    /// matching markdown headings (`# Heading`).
    [[nodiscard]] auto parse_tags(std::string_view content) const -> std::vector<ParsedTag>;

    /// Validate a tag path (no empty components, valid characters).
    [[nodiscard]] auto validate_tag(std::string_view tag_path) const -> bool;

    /// Split a hierarchical tag into components.
    [[nodiscard]] static auto split_path(std::string_view tag_path) -> std::vector<std::string>;

    /// Join components into a hierarchical tag path.
    [[nodiscard]] static auto join_path(const std::vector<std::string>& components) -> std::string;
};

/// In-memory tag index for fast lookups.
class TagIndex
{
public:
    /// Add a tag association for a block.
    void add_tag(const BlockId& block_id, const std::string& tag_path);

    /// Remove a specific tag from a block.
    void remove_tag(const BlockId& block_id, const std::string& tag_path);

    /// Remove all tags for a block.
    void remove_block(const BlockId& block_id);

    /// Get all blocks with a specific tag (exact match).
    [[nodiscard]] auto get_blocks_with_tag(const std::string& tag_path) const
        -> std::vector<BlockId>;

    /// Get all blocks with a tag or any of its descendants.
    [[nodiscard]] auto get_blocks_with_tag_recursive(const std::string& tag_path) const
        -> std::vector<BlockId>;

    /// Get all tags for a specific block.
    [[nodiscard]] auto get_tags_for_block(const BlockId& block_id) const
        -> std::vector<std::string>;

    /// Get the count of blocks with a specific tag.
    [[nodiscard]] auto tag_count(const std::string& tag_path) const -> int;

    /// Build the complete tag tree from the index.
    [[nodiscard]] auto build_tree() const -> TagTree;

    /// Search for tags matching a query (prefix match).
    [[nodiscard]] auto search_tags(const std::string& query) const -> std::vector<TagInfo>;

    /// Get all unique tag paths.
    [[nodiscard]] auto all_tag_paths() const -> std::vector<std::string>;

    /// Clear the entire index.
    void clear();

private:
    /// tag_path -> set of block IDs
    std::unordered_map<std::string, std::unordered_set<BlockId, BlockIdHash>> tag_to_blocks_;

    /// block_id -> set of tag paths
    std::unordered_map<BlockId, std::unordered_set<std::string>, BlockIdHash> block_to_tags_;

    /// Build a TagInfo subtree rooted at the given path prefix.
    [[nodiscard]] auto build_subtree(const std::string& prefix,
                                      const std::vector<std::string>& all_paths) const
        -> std::vector<TagInfo>;
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### TagService class (`src/core/TagService.h` / `src/core/TagService.cpp`)

Constructor: `TagService(EventBus& event_bus, Config& config)`

1. **`auto get_tags() -> TagTree`** -- Return the complete hierarchical tag tree for the workspace. Builds from `TagIndex::build_tree()`. Sorts children alphabetically by label. Computes `total_count` recursively (direct + all descendants).

2. **`auto get_tag_blocks(const std::string& tag_path) -> std::vector<BlockId>`** -- Return all blocks tagged with the exact tag path. If `include_descendants` config is true, also includes blocks tagged with descendant tags (e.g., `get_tag_blocks("project")` includes blocks tagged `project/work`).

3. **`void rename_tag(const std::string& old_path, const std::string& new_path)`** -- Rename a tag across all blocks. Finds every block containing `#old_path#` and replaces with `#new_path#`. Also renames all descendant tags (e.g., renaming "project" to "proj" also renames "project/work" to "proj/work"). Updates TagIndex. Publishes `TagRenamedEvent`.

4. **`void remove_tag(const std::string& tag_path)`** -- Remove a tag from all blocks. Finds every block containing `#tag_path#` and removes the tag syntax. Also removes all descendant tags. Updates TagIndex. Publishes `TagRemovedEvent`.

5. **`void merge_tags(const std::string& source_path, const std::string& target_path)`** -- Merge source tag into target tag. All blocks tagged with `#source_path#` get `#target_path#` added (if not already present) and `#source_path#` removed. Descendant tags under source are re-parented under target.

6. **`auto get_tag_count(const std::string& tag_path) -> int`** -- Return the number of blocks with the given tag. O(1) from TagIndex.

7. **`auto search_tags(const std::string& query) -> std::vector<TagInfo>`** -- Search for tags matching the query (case-insensitive prefix/substring match). Returns TagInfo entries with counts. Used for tag autocomplete.

8. **`void add_tag_to_block(const BlockId& block_id, const std::string& tag_path)`** -- Programmatically add a tag to a block. Appends `#tag_path#` to the block's content. Updates TagIndex. Publishes `TagsChangedEvent`.

9. **`void remove_tag_from_block(const BlockId& block_id, const std::string& tag_path)`** -- Remove a specific tag from a specific block. Updates content and index.

10. **`void rebuild_index()`** -- Full re-index: scan all blocks, extract tags, populate TagIndex. Called on workspace open.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Tag events (Phase 12)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagRenamedEvent)
std::string old_path;
std::string new_path;
int affected_block_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagRemovedEvent)
std::string tag_path;
int affected_block_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagsChangedEvent)
std::string block_id;
std::vector<std::string> old_tags;
std::vector<std::string> new_tags;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(TagPanelOpenRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagSelectedEvent)
std::string tag_path;
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.tags.enable_autocomplete` | bool | `true` | Show tag suggestions when typing `#` |
| `knowledgebase.tags.include_descendants` | bool | `true` | Include descendant tags in tag block queries |
| `knowledgebase.tags.sort_by` | string | `"name"` | Tag tree sort order: `"name"` or `"count"` |
| `knowledgebase.tags.show_count_badge` | bool | `true` | Show usage count next to each tag in panel |
| `knowledgebase.tags.min_autocomplete_chars` | int | `1` | Characters typed before autocomplete triggers |
| `knowledgebase.tags.render_as_pills` | bool | `true` | Render tags as styled pill badges in preview |

---

## Test Cases

File: `tests/unit/test_tags.cpp`

```cpp
#include "core/Tag.h"
#include "core/TagService.h"
#include "core/EventBus.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Parse tags from content** -- Input: `"Working on #project/alpha# and #status/active# today"`. Verify: 2 `ParsedTag` entries returned. First: `full_path == "project/alpha"`, correct offset pointing to the first `#`. Second: `full_path == "status/active"`.

2. **Hierarchical tag tree** -- Add tags: `#a/b#`, `#a/c#`, `#a/b/d#`, `#e#`. Build tree. Verify: 2 root nodes (`a` and `e`). `a` has children `b` and `c`. `b` has child `d`. Counts propagate correctly: `a.total_count >= a.direct_count + b.total_count + c.total_count`.

3. **Rename tag updates all blocks** -- Block X has `#project/old#`, Block Y has `#project/old/sub#`. Rename `"project/old"` to `"project/new"`. Verify: Block X now has `#project/new#`. Block Y now has `#project/new/sub#`. `TagRenamedEvent` published with `affected_block_count == 2`.

4. **Remove tag from all blocks** -- Blocks X and Y both have `#deprecated#`. Remove tag `"deprecated"`. Verify: both blocks no longer contain `#deprecated#`. `TagRemovedEvent` published with `affected_block_count == 2`. TagIndex no longer contains "deprecated".

5. **Merge tags** -- Block A has `#old-label#`. Block B has `#new-label#`. Block C has both. Merge `"old-label"` into `"new-label"`. Verify: Block A now has `#new-label#` (added). Block B unchanged. Block C has `#new-label#` only (old removed, new already existed). TagIndex shows `"old-label"` has 0 blocks.

6. **Tag count accuracy** -- Add `#status/active#` to 5 blocks. Verify: `get_tag_count("status/active") == 5`. Remove from 2 blocks. Verify: `get_tag_count("status/active") == 3`.

7. **Search tags** -- Index contains: `"project/alpha"`, `"project/beta"`, `"personal"`, `"status"`. Search for `"pro"`. Verify: returns `"project/alpha"` and `"project/beta"` (prefix match on `"project"`). Search for `"xyz"`. Verify: empty result.

8. **Special characters in tags** -- Tags with hyphens (`#my-tag#`) and underscores (`#my_tag#`) are valid. Tags with spaces (`#my tag#`) are NOT matched (parser stops at space). Tags with only numbers (`#123#`) are valid. Verify parser handles each case correctly.

9. **Duplicate tag handling** -- Add `#project#` to block A twice (content has `#project# ... #project#`). Verify: TagIndex counts block A once for tag `"project"` (deduplicated). ParsedTag list has 2 entries (both occurrences reported for display).

10. **Tag tree sorting** -- Add tags: `#zebra#`, `#alpha#`, `#middle#`. Build tree with sort_by="name". Verify: roots are `[alpha, middle, zebra]`. Build tree with sort_by="count" and zebra has 5 blocks, alpha has 3, middle has 1. Verify: roots are `[zebra, alpha, middle]`.

---

## Acceptance Criteria

- [ ] `TagParser::parse_tags()` correctly extracts `#tag#` syntax, avoiding false matches on `# Heading` syntax
- [ ] `TagParser::validate_tag()` rejects empty components, special characters, and too-long paths
- [ ] `TagIndex::build_tree()` produces correct hierarchical `TagTree` with accurate counts
- [ ] `TagService::rename_tag()` propagates rename to all blocks and descendant tags
- [ ] `TagService::remove_tag()` removes tag from all blocks and cleans up index
- [ ] `TagService::merge_tags()` correctly merges source into target without duplicates
- [ ] `TagService::search_tags()` performs case-insensitive prefix matching
- [ ] All 5 events (`TagRenamedEvent`, `TagRemovedEvent`, `TagsChangedEvent`, `TagPanelOpenRequestEvent`, `TagSelectedEvent`) fire correctly
- [ ] All 6 config keys load with correct defaults
- [ ] All 10 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/Tag.h` | `TagInfo`, `ParsedTag`, `TagTree`, `BlockTagEntry`, `TagParser`, `TagIndex` |
| `src/core/Tag.cpp` | `TagParser`, `TagIndex`, `ParsedTag::components()` implementations |
| `src/core/TagService.h` | `TagService` class declaration |
| `src/core/TagService.cpp` | All 10 service methods, event handling, index management |
| `src/ui/TagPanel.h` | `TagPanel` wxPanel sidebar component declaration |
| `src/ui/TagPanel.cpp` | Tree-style tag display with click-to-filter, rename/remove context menu |
| `tests/unit/test_tags.cpp` | All 10 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `TagRenamedEvent`, `TagRemovedEvent`, `TagsChangedEvent`, `TagPanelOpenRequestEvent`, `TagSelectedEvent` |
| `src/core/PluginContext.h` | Add `TagService* tag_service{nullptr};` pointer |
| `src/rendering/HtmlRenderer.cpp` | Detect `#tag#` in text nodes, render as `<span class="tag">` |
| `src/CMakeLists.txt` | Add `core/Tag.cpp`, `core/TagService.cpp`, `ui/TagPanel.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_tags` test target |
| `resources/config_defaults.json` | Add 6 `knowledgebase.tags.*` config keys |
