# Phase 24 -- Canvas Search

## Objective

Implement text search across all canvas objects. A search bar queries text content in sticky notes, text boxes, shape labels, table cells, and frame titles. Results are highlighted on the canvas and navigable with next/previous buttons.

## Prerequisites

- Phase 07 (StickyNoteObject with text)
- Phase 08 (TextBoxObject with text)
- Phase 09 (ShapeObject with label)
- Phase 14 (FrameObject with title)
- Phase 19 (TableObject with cell text)

## Feature References (PRD)

- PRD #36: Search

## Data Structures to Implement

### File: `src/canvas/CanvasSearch.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

struct SearchResult
{
    ObjectId object_id{kInvalidObjectId};
    std::string matched_text;
    std::string context;      // Surrounding text for preview
    int match_offset{0};      // Character offset within the object's text
    int match_length{0};
};

class CanvasSearch
{
public:
    /// Search all objects for the query string.
    [[nodiscard]] auto search(
        const std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>>& objects,
        const std::string& query,
        bool case_sensitive = false) const -> std::vector<SearchResult>;

    /// Get the text content of any canvas object (for search indexing).
    [[nodiscard]] static auto extract_text(const CanvasObject& obj) -> std::string;
};

} // namespace markamp::canvas
```

### File: `src/canvas/SearchBar.h`

```cpp
#pragma once

#include "CanvasSearch.h"

#include <functional>
#include <string>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

class SearchBar
{
public:
    using OnNavigateToResult = std::function<void(ObjectId, const AABB& bounds)>;

    auto set_results(const std::vector<SearchResult>& results) -> void;
    auto render(wxGraphicsContext& gc, double canvas_width) -> void;
    auto handle_key(int key_code, int modifiers) -> bool;
    auto handle_char(wchar_t ch) -> bool;

    [[nodiscard]] auto query() const -> const std::string&;
    [[nodiscard]] auto result_count() const -> int;
    [[nodiscard]] auto current_index() const -> int;
    [[nodiscard]] auto current_result() const -> const SearchResult*;

    auto next_result() -> void;
    auto previous_result() -> void;
    auto show() -> void;
    auto hide() -> void;
    [[nodiscard]] auto is_visible() const -> bool;

    auto set_on_navigate(OnNavigateToResult cb) -> void;

    /// Get all result object IDs for highlighting.
    [[nodiscard]] auto highlighted_objects() const -> std::vector<ObjectId>;

private:
    std::string query_;
    std::vector<SearchResult> results_;
    int current_index_{-1};
    bool visible_{false};
    OnNavigateToResult on_navigate_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `CanvasSearch::extract_text()` -- Switch on object type: StickyNoteObject -> text(), TextBoxObject -> plain_text(), ShapeObject -> label(), FrameObject -> title(), TableObject -> concatenate all cell text, ConnectorObject -> label(). Default -> name().

2. `CanvasSearch::search()` -- For each object, extract text, search for query (case-insensitive by default). Return SearchResults with match offset and context snippet.

3. `SearchBar` -- Render at the top of the canvas. Input field + result count + prev/next arrows. Cmd+F to show, Escape to hide.

4. Navigation: When navigating to a result, zoom/pan to center the matched object.

5. Highlight: Objects with search matches are rendered with a yellow highlight outline.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+F | Show search bar | canvas |
| Enter | Next result | canvas_search |
| Shift+Enter | Previous result | canvas_search |
| Escape | Close search bar | canvas_search |

## Test Cases

File: `tests/unit/test_canvas_search.cpp`

1. **Search sticky note** -- Sticky with "Hello World", search "world", verify found.
2. **Case insensitive** -- Search "hello" matches "Hello".
3. **Search text box** -- TextBox with formatted text, search finds plain text.
4. **Search table** -- Table with cells, search finds cell text.
5. **Search shape label** -- Shape with label, search finds.
6. **No results** -- Search for non-existent text, verify empty results.
7. **Multiple results** -- 3 sticky notes with "test", verify 3 results.
8. **Extract text** -- Verify extract_text for each object type.
9. **Context snippet** -- Verify context includes surrounding text.
10. **Navigate results** -- next_result/previous_result cycle through results.

## Acceptance Criteria

- [ ] Search across all text-bearing canvas object types
- [ ] Case-insensitive search by default
- [ ] Search bar with input, result count, next/previous navigation
- [ ] Matched objects highlighted on canvas
- [ ] Navigate zooms/pans to center the current result
- [ ] Cmd+F to open, Escape to close
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CanvasSearch.h` | Search engine |
| CREATE | `src/canvas/CanvasSearch.cpp` | Search implementation |
| CREATE | `src/canvas/SearchBar.h` | Search bar UI |
| CREATE | `src/canvas/SearchBar.cpp` | Search bar implementation |
| MODIFY | `src/canvas/CanvasRenderer.cpp` | Highlight search results |
| MODIFY | `src/ui/CanvasPanel.cpp` | Render search bar, handle Cmd+F |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_canvas_search.cpp` | 10 Catch2 tests |

## Architecture Notes

- Search runs synchronously for simplicity. For very large boards (>10,000 objects), consider background indexing.
- The extract_text function uses dynamic_cast to determine object type. This is acceptable because search is not on the hot path.

## Estimated Complexity

**M** -- Text extraction from multiple types, search bar UI, result navigation, highlight rendering, 10 tests.
