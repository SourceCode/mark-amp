# Phase 11 -- Document Outline Service

**Priority:** High (core navigation feature)
**Estimated Scope:** ~10 files created/modified
**Dependencies:** Existing Types.h (MdNode, MarkdownDocument), EventBus, Config, BreadcrumbBar

---

## Objective

Implement the document outline (table of contents) service that extracts heading structure from documents and provides navigation. Porting SiYuan's outline functionality which shows a hierarchical heading tree for any document.

The outline is the primary structural navigation tool. It extracts heading blocks from the document AST, builds a hierarchical tree (h1 > h2 > h3 etc.), and synchronizes with editor scroll position. The UI component (OutlinePanel) renders the tree in the sidebar and highlights the currently visible heading.

---

## Prerequisites

- `src/core/Types.h` -- `MdNode`, `MdNodeType::Heading`, `MarkdownDocument`
- `src/core/Md4cWrapper.h` / `MarkdownParser` -- parse markdown into AST
- `src/core/EventBus.h` -- event pub/sub
- `src/core/Config.h` -- outline display settings
- `src/ui/BreadcrumbBar.h` -- existing component that will consume outline data
- `src/core/PluginContext.h` -- service injection
- Phase 09 `BlockId` type (optional dependency -- outline works without block IDs but integrates better with them)

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/model/outline.go` | `GetDocOutline` | `OutlineService::get_outline()` |
| `kernel/api/outline.go` | API endpoints for outline | `OutlineService` public API |
| `kernel/treenode/node.go` | Heading node extraction from block tree | `OutlineBuilder::build_from_ast()` |
| `kernel/model/heading.go` | `GetHeadingChildrenIDs`, `GetHeadingDeleteTransaction` | `OutlineService::get_heading_children()` |

---

## MarkAmp Integration Points

1. **MarkdownParser / Md4cWrapper** -- After parsing, the AST root is passed to `OutlineBuilder::build_from_ast()` which walks the tree and extracts heading nodes.
2. **BreadcrumbBar** -- Currently shows file path breadcrumbs. Will be enhanced to also show the current heading position via outline data (e.g., `File.md > ## Introduction > ### Background`).
3. **EditorPanel** -- `CursorPositionChangedEvent` triggers outline scroll sync (highlights the heading closest to cursor position).
4. **PreviewPanel** -- `EditorScrollChangedEvent` triggers outline scroll sync for preview mode.
5. **LayoutManager / ActivityBar** -- New `OutlinePanel` registered as a sidebar panel accessible via activity bar.
6. **PluginContext** -- `OutlineService*` pointer added for extension access.

---

## Data Structures to Implement

All structures go in `src/core/Outline.h`:

```cpp
#pragma once

#include "BlockRef.h"
#include "Types.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A node in the hierarchical document outline tree.
struct OutlineNode
{
    /// Block ID of the heading (if block IDs are assigned).
    std::optional<BlockId> block_id;

    /// Plain text content of the heading (stripped of formatting).
    std::string text;

    /// Heading level (1-6).
    int level{1};

    /// Line number in the source document (0-based).
    int source_line{0};

    /// Character offset in the source document.
    std::size_t source_offset{0};

    /// Nesting depth in the outline tree (0 = top-level).
    int depth{0};

    /// Number of sub-headings directly under this heading.
    [[nodiscard]] auto sub_heading_count() const -> std::size_t
    {
        return children.size();
    }

    /// Total number of sub-headings recursively.
    [[nodiscard]] auto total_sub_heading_count() const -> std::size_t;

    /// Child headings in document order.
    std::vector<OutlineNode> children;
};

/// A flat (non-hierarchical) outline entry for list-style display.
struct OutlineEntry
{
    /// Block ID of the heading (if assigned).
    std::optional<BlockId> block_id;

    /// Plain text content of the heading.
    std::string text;

    /// Heading level (1-6).
    int level{1};

    /// Nesting depth for indentation (derived from hierarchy).
    int depth{0};

    /// Indentation string for display (e.g., "  " per depth level).
    std::string indent;

    /// Line number in the source document (0-based).
    int source_line{0};

    /// Character offset in the source document.
    std::size_t source_offset{0};

    /// Sequential index in the flat list.
    int index{0};
};

/// Represents the complete outline state for a document.
struct DocumentOutline
{
    /// Root document identifier.
    std::string root_id;

    /// Hierarchical outline tree.
    std::vector<OutlineNode> roots;

    /// Total number of headings across all levels.
    int total_heading_count{0};

    /// Heading counts by level (index 0 = h1, index 5 = h6).
    std::array<int, 6> level_counts{};

    /// Whether the outline is empty (no headings found).
    [[nodiscard]] auto is_empty() const -> bool
    {
        return total_heading_count == 0;
    }
};

/// Builds an outline tree from a MarkdownDocument AST.
class OutlineBuilder
{
public:
    /// Build a hierarchical outline from the document AST.
    [[nodiscard]] auto build_from_ast(const MarkdownDocument& doc) -> DocumentOutline;

    /// Build a hierarchical outline from a flat list of heading entries.
    /// Useful when headings are extracted from a block database rather than AST.
    [[nodiscard]] auto build_from_headings(
        const std::vector<std::pair<std::string, int>>& headings) -> DocumentOutline;

private:
    /// Insert a heading into the tree at the correct hierarchy position.
    void insert_heading(std::vector<OutlineNode>& roots,
                        const OutlineNode& heading,
                        int current_depth);

    /// Extract plain text from an MdNode heading (strips inline formatting).
    [[nodiscard]] auto extract_heading_text(const MdNode& heading_node) const -> std::string;
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### OutlineService class (`src/core/OutlineService.h` / `src/core/OutlineService.cpp`)

Constructor: `OutlineService(EventBus& event_bus, Config& config)`

1. **`auto get_outline(const std::string& root_id) -> DocumentOutline`** -- Return the hierarchical outline tree for the given document. If the outline is cached and the document has not changed since last build, returns the cached version. Otherwise, re-parses and rebuilds.

2. **`auto get_flat_outline(const std::string& root_id) -> std::vector<OutlineEntry>`** -- Flatten the hierarchical outline into a linear list with depth/indent info. Useful for list-style UI rendering and accessibility. Each entry includes its depth for indentation.

3. **`void scroll_to_heading(const std::string& block_id)`** -- Publish a `OutlineScrollToEvent` that the EditorPanel and PreviewPanel subscribe to. The editor scrolls to the line containing the heading.

4. **`auto get_heading_at_line(const std::string& root_id, int line) -> std::optional<OutlineEntry>`** -- Find the heading that contains (or is closest above) the given line number. Used for scroll sync -- when the cursor moves, the outline panel highlights the active heading.

5. **`auto get_breadcrumb_path(const std::string& root_id, int line) -> std::vector<OutlineEntry>`** -- Return the heading ancestry path for the given line (e.g., for line 50 inside "### Background" under "## Introduction": returns `[{h2, "Introduction"}, {h3, "Background"}]`). Used by BreadcrumbBar.

6. **`void invalidate(const std::string& root_id)`** -- Mark the outline cache for the given document as stale. Called when `EditorContentChangedEvent` fires. The next call to `get_outline()` will rebuild.

7. **`auto search_headings(const std::string& root_id, const std::string& query) -> std::vector<OutlineEntry>`** -- Filter outline entries by query string (case-insensitive substring match). Used for quick-jump within outline panel.

8. **`void on_content_changed(const std::string& content, const std::string& root_id)`** -- Internal handler subscribed to `EditorContentChangedEvent`. Parses the new content, rebuilds the outline, diffs against the previous outline, and publishes `OutlineChangedEvent` if the structure changed.

9. **`void on_cursor_moved(int line, const std::string& root_id)`** -- Internal handler subscribed to `CursorPositionChangedEvent`. Determines the active heading and publishes `OutlineActiveHeadingChangedEvent` if it changed.

10. **`auto get_statistics(const std::string& root_id) -> DocumentOutline`** -- Return heading statistics (count by level, total count, max depth). Lightweight metadata without full tree structure.

### OutlineBuilder methods (in `src/core/Outline.cpp`)

These are implemented as part of `OutlineBuilder` and called by `OutlineService`:

- `build_from_ast()` walks `MarkdownDocument.root.children`, extracts `MdNodeType::Heading` nodes, and builds the hierarchy by comparing heading levels.
- `build_from_headings()` takes pre-extracted heading data and builds the tree (for database-backed scenarios).
- `insert_heading()` uses a stack-based approach: maintain a stack of `(level, node_ptr)` pairs. When a new heading arrives, pop the stack until the top has a lower level, then insert as a child of the top.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Document outline events (Phase 11)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineChangedEvent)
std::string root_id;
int heading_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineScrollToEvent)
std::string block_id;
int source_line{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineActiveHeadingChangedEvent)
std::string root_id;
std::string heading_text;
int heading_level{0};
int source_line{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(OutlinePanelOpenRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BreadcrumbNavigateEvent)
std::string root_id;
int heading_index{0};
int source_line{0};
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.outline.show_in_sidebar` | bool | `true` | Show outline panel in sidebar |
| `knowledgebase.outline.follow_cursor` | bool | `true` | Auto-highlight active heading as cursor moves |
| `knowledgebase.outline.sort_by_position` | bool | `true` | Sort headings by document position (vs alphabetical) |
| `knowledgebase.outline.show_level_icons` | bool | `true` | Show H1-H6 level indicators in outline |
| `knowledgebase.outline.max_depth` | int | `6` | Maximum heading depth to display (1-6) |
| `knowledgebase.outline.breadcrumb_enabled` | bool | `true` | Show heading breadcrumbs in BreadcrumbBar |

---

## Test Cases

File: `tests/unit/test_outline.cpp`

```cpp
#include "core/Outline.h"
#include "core/OutlineService.h"
#include "core/EventBus.h"
#include "core/Config.h"
#include "core/Md4cWrapper.h"
#include "core/MarkdownParser.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Single document outline** -- Parse a markdown document with headings: `# Title`, `## Section A`, `### Subsection`, `## Section B`. Build outline. Verify: 2 root nodes (Title at h1... actually h1 is the single root). Verify tree: h1("Title") has children h2("Section A") and h2("Section B"). h2("Section A") has child h3("Subsection"). `total_heading_count == 4`.

2. **Nested headings correct hierarchy** -- Document: `## A`, `### B`, `#### C`, `### D`. Verify: h2("A") is root, with children h3("B") and h3("D"). h3("B") has child h4("C").

3. **Skipped levels (h1 -> h3)** -- Document: `# Top`, `### Jumped`. Verify: h1("Top") is root with h3("Jumped") as a direct child (no phantom h2 inserted). Depth of "Jumped" is 1.

4. **Empty document outline** -- Parse empty string. Verify: `DocumentOutline.is_empty() == true`, `total_heading_count == 0`, `roots` is empty.

5. **Heading-only document** -- Document with only headings (no body text): `# A`, `## B`, `## C`, `# D`. Verify: 2 root nodes (h1 "A" and h1 "D"). h1 "A" has children h2 "B" and h2 "C". `total_heading_count == 4`.

6. **Headings with children blocks** -- Document: `## Heading`, `Some paragraph text`, `- list item`, `## Next`. Verify: outline only contains heading nodes, not paragraph or list nodes. `total_heading_count == 2`.

7. **Flat outline generation** -- From test 1 hierarchy, call `get_flat_outline()`. Verify: 4 entries in order, with depths `[0, 1, 2, 1]` and correct `indent` strings (`""`, `"  "`, `"    "`, `"  "`).

8. **Scroll sync -- heading at line** -- Document with headings at lines 0, 5, 12, 20. Call `get_heading_at_line(root_id, 8)`. Verify: returns heading at line 5 (the closest heading above line 8).

9. **Outline update on edit** -- Initial document has 3 headings. Modify content to have 4 headings. Call `on_content_changed()`. Verify: `OutlineChangedEvent` published with `heading_count == 4`.

10. **Large document (100+ headings)** -- Generate a document with 120 headings at various levels. Build outline. Verify: `total_heading_count == 120`. Verify: `get_flat_outline()` returns 120 entries. Verify: performance completes in under 50ms (measured with `std::chrono`).

---

## Acceptance Criteria

- [ ] `OutlineBuilder::build_from_ast()` correctly extracts headings from AST and builds hierarchy
- [ ] Skipped heading levels (h1->h3) produce correct tree without phantom nodes
- [ ] `OutlineService::get_outline()` caches results and invalidates on content change
- [ ] `OutlineService::get_flat_outline()` produces correct depth/indent for all entries
- [ ] `OutlineService::get_heading_at_line()` finds the nearest heading above the given line
- [ ] `OutlineService::get_breadcrumb_path()` returns correct ancestor chain
- [ ] `OutlineService::scroll_to_heading()` publishes `OutlineScrollToEvent`
- [ ] `OutlineChangedEvent` fires only when heading structure actually changes
- [ ] `OutlineActiveHeadingChangedEvent` fires when cursor crosses heading boundaries
- [ ] Outline search (`search_headings()`) filters case-insensitively
- [ ] All 10 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/Outline.h` | `OutlineNode`, `OutlineEntry`, `DocumentOutline`, `OutlineBuilder` |
| `src/core/Outline.cpp` | `OutlineBuilder` implementations, `OutlineNode::total_sub_heading_count()` |
| `src/core/OutlineService.h` | `OutlineService` class declaration |
| `src/core/OutlineService.cpp` | Service methods, event subscriptions, caching, scroll sync |
| `src/ui/OutlinePanel.h` | `OutlinePanel` wxPanel sidebar component declaration |
| `src/ui/OutlinePanel.cpp` | Tree-style outline rendering with click-to-navigate |
| `tests/unit/test_outline.cpp` | All 10 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `OutlineChangedEvent`, `OutlineScrollToEvent`, `OutlineActiveHeadingChangedEvent`, `OutlinePanelOpenRequestEvent`, `BreadcrumbNavigateEvent` |
| `src/core/PluginContext.h` | Add `OutlineService* outline_service{nullptr};` pointer |
| `src/ui/BreadcrumbBar.h` / `.cpp` | Subscribe to `OutlineActiveHeadingChangedEvent`, display heading ancestry |
| `src/ui/EditorPanel.cpp` | Publish cursor line to OutlineService for scroll sync |
| `src/CMakeLists.txt` | Add `core/Outline.cpp`, `core/OutlineService.cpp`, `ui/OutlinePanel.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_outline` test target |
| `resources/config_defaults.json` | Add 6 `knowledgebase.outline.*` config keys |
