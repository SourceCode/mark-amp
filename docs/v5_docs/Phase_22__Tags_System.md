# Phase 22 -- Tags System

## Objective

Implement a tagging system for canvas objects: tag badges displayed on objects, a tag management panel, tag-based filtering (show only objects with a specific tag), and tag-based search integration. Leverages the tags vector already present on CanvasObject from Phase 01.

## Prerequisites

- Phase 01 (CanvasObject with tags vector)
- Phase 04 (SelectionManager)
- Phase 05 (UndoRedoStack)
- Phase 21 (MetadataPanel for tag display integration)

## Feature References (PRD)

- PRD #37: Tags

## Data Structures to Implement

### File: `src/canvas/TagManager.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

struct TagInfo
{
    std::string name;
    CanvasColor color{100, 100, 100, 255};
    int usage_count{0};
};

class TagManager
{
public:
    /// Rebuild tag index from all objects in the board.
    auto rebuild_index(const std::unordered_map<ObjectId,
        std::unique_ptr<CanvasObject>>& objects) -> void;

    /// Get all known tags with usage counts.
    [[nodiscard]] auto all_tags() const -> std::vector<TagInfo>;

    /// Get objects with a specific tag.
    [[nodiscard]] auto objects_with_tag(const std::string& tag) const
        -> const std::unordered_set<ObjectId>&;

    /// Set a tag color.
    auto set_tag_color(const std::string& tag, const CanvasColor& color) -> void;

    /// Get tag color.
    [[nodiscard]] auto tag_color(const std::string& tag) const -> CanvasColor;

    /// Autocomplete suggestions for partial tag input.
    [[nodiscard]] auto suggest_tags(const std::string& prefix) const
        -> std::vector<std::string>;

private:
    std::unordered_map<std::string, TagInfo> tags_;
    std::unordered_map<std::string, std::unordered_set<ObjectId>> tag_to_objects_;
    static const std::unordered_set<ObjectId> empty_set_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/TagPanel.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "TagManager.h"

class wxGraphicsContext;

namespace markamp::canvas
{

class TagPanel
{
public:
    auto set_tags(const std::vector<TagInfo>& tags) -> void;
    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;

    using OnFilterByTag = std::function<void(const std::string& tag)>;
    auto set_on_filter_by_tag(OnFilterByTag cb) -> void;

    [[nodiscard]] auto active_filter() const -> const std::string&;
    auto clear_filter() -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    std::vector<TagInfo> tags_;
    std::string active_filter_;
    bool visible_{false};
    OnFilterByTag on_filter_by_tag_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `TagManager::rebuild_index()` -- Iterate all objects, collect tags, build tag-to-objects mapping.

2. Tag badges: After rendering an object, if it has tags, draw small colored badges below the object showing tag names.

3. Tag filter: When a tag filter is active, dim or hide objects without that tag.

4. Tag editing: In the metadata panel or context menu, add/remove tags with autocomplete suggestions.

## Test Cases

File: `tests/unit/test_tag_manager.cpp`

1. **Rebuild index** -- 3 objects with various tags, verify all_tags counts.
2. **Objects with tag** -- Verify correct ObjectIds returned for a tag.
3. **Tag color** -- Set custom color, verify.
4. **Suggest tags** -- Type "pro", verify "project" suggested.
5. **Empty tag set** -- Object with no tags, verify.
6. **Tag filter** -- Filter by "important", verify only matching objects returned.
7. **Add tag to object** -- Add tag, rebuild, verify index updated.
8. **Remove tag from object** -- Remove tag, rebuild, verify.

## Acceptance Criteria

- [ ] TagManager indexes all tags across board objects
- [ ] Tag badges rendered on objects
- [ ] Tag panel shows all tags with usage counts
- [ ] Click tag to filter board to that tag
- [ ] Tag autocomplete during tag entry
- [ ] Tag colors configurable
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/TagManager.h` | Tag index and management |
| CREATE | `src/canvas/TagManager.cpp` | Tag manager implementation |
| CREATE | `src/canvas/TagPanel.h` | Tag panel UI |
| CREATE | `src/canvas/TagPanel.cpp` | Panel implementation |
| MODIFY | `src/canvas/CanvasRenderer.cpp` | Draw tag badges after objects |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_tag_manager.cpp` | 8 Catch2 tests |

## Architecture Notes

- TagManager rebuilds its index on board changes (add/remove object, tag modification). It subscribes to CanvasObjectModifiedEvent.
- Tag badges are rendered as small rounded rectangles with the tag color, displayed in a row below each object.

## Estimated Complexity

**M** -- Tag index, badge rendering, filter system, autocomplete, 8 tests.
