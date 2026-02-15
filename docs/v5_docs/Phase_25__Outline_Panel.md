# Phase 25 -- Outline Panel

## Objective

Implement an outline panel that shows the hierarchical structure of the board: frames, sections, and their contained objects in a navigable tree view. Click an entry to navigate to that object. Drag entries to reorder frames/sections.

## Prerequisites

- Phase 14 (FrameObject)
- Phase 15 (SectionObject)
- Phase 14 (FrameContainmentTracker)

## Feature References (PRD)

- PRD #35: Outline Panel

## Data Structures to Implement

### File: `src/canvas/OutlinePanel.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>
#include <string>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

struct OutlineEntry
{
    ObjectId id{kInvalidObjectId};
    std::string label;
    CanvasObjectType type;
    int indent_level{0};
    bool is_expanded{true};
    bool is_container{false}; // Frame or Section
    int child_count{0};
};

class OutlinePanel
{
public:
    using OnNavigateCallback = std::function<void(ObjectId)>;
    using OnReorderCallback = std::function<void(ObjectId, int new_index)>;

    auto set_entries(const std::vector<OutlineEntry>& entries) -> void;
    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;
    auto handle_double_click(double x, double y) -> bool;

    auto set_on_navigate(OnNavigateCallback cb) -> void;
    auto set_on_reorder(OnReorderCallback cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    auto toggle_expand(ObjectId id) -> void;

private:
    std::vector<OutlineEntry> entries_;
    bool visible_{false};
    double scroll_offset_{0.0};
    OnNavigateCallback on_navigate_;
    OnReorderCallback on_reorder_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. Build outline entries: Iterate board objects. First add frames (sorted by slide index or position). Under each frame, add contained objects. Then add sections. Then add top-level (uncontained) objects.

2. `OutlinePanel::render()` -- Draw indented tree with type icons (frame icon, section icon, sticky icon, etc.). Expand/collapse toggles for containers.

3. Click to navigate: Center the canvas on the clicked object.

4. Drag to reorder: Drag frames to change slide order.

## Test Cases

File: `tests/unit/test_outline_panel.cpp`

1. **Build entries** -- Board with 2 frames, 5 objects, verify hierarchical entries.
2. **Frame children** -- Objects inside frame appear indented under frame.
3. **Collapse** -- Collapse frame, verify children hidden.
4. **Navigate** -- Click entry, verify callback with correct ID.
5. **Slide ordering** -- Frames ordered by slide_index.
6. **Uncontained objects** -- Objects outside frames appear at root level.

## Acceptance Criteria

- [ ] Outline shows hierarchical frame/section/object structure
- [ ] Click navigates to object
- [ ] Expand/collapse for containers
- [ ] Frames ordered by slide index
- [ ] Type icons for each object type
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/OutlinePanel.h` | Outline panel |
| CREATE | `src/canvas/OutlinePanel.cpp` | Outline implementation |
| MODIFY | `src/ui/CanvasPanel.cpp` | Build and render outline panel |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_outline_panel.cpp` | 6 Catch2 tests |

## Architecture Notes

- The outline is rebuilt on board structure changes (object add/remove, frame move, containment change).
- The outline panel is rendered as a left-side overlay on the canvas.

## Estimated Complexity

**S** -- Tree view rendering, containment hierarchy, navigation callbacks, 6 tests.
