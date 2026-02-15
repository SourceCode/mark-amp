# Phase 23 -- Minimap / Board Map

## Objective

Implement a floating minimap panel that shows a scaled-down overview of the entire board with a viewport indicator rectangle. Click or drag on the minimap to navigate the main canvas. The minimap renders a simplified version of all objects for performance.

## Prerequisites

- Phase 01 (CanvasTypes, AABB, ViewportTransform)
- Phase 02 (CanvasPanel, object storage)
- Phase 05 (Board with content_bounds)

## Feature References (PRD)

- PRD #60: Board Map / Mini-Map

## Data Structures to Implement

### File: `src/canvas/MinimapPanel.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "ViewportTransform.h"

#include <functional>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

struct MinimapEntry
{
    AABB bounds;
    CanvasColor color;
    CanvasObjectType type;
};

class MinimapPanel
{
public:
    using OnNavigateCallback = std::function<void(const Point2D& world_center)>;

    auto set_content_bounds(const AABB& bounds) -> void;
    auto set_viewport(const AABB& visible_region) -> void;
    auto set_entries(const std::vector<MinimapEntry>& entries) -> void;

    auto render(wxGraphicsContext& gc, double panel_x, double panel_y,
                 double panel_width, double panel_height) -> void;

    auto handle_click(double x, double y) -> bool;
    auto handle_drag(double x, double y) -> bool;

    auto set_on_navigate(OnNavigateCallback cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    auto set_panel_size(double width, double height) -> void;

private:
    AABB content_bounds_;
    AABB viewport_region_;
    std::vector<MinimapEntry> entries_;
    bool visible_{true};
    double panel_width_{200.0};
    double panel_height_{150.0};
    OnNavigateCallback on_navigate_;
    bool dragging_{false};

    [[nodiscard]] auto world_to_minimap(const Point2D& world) const -> Point2D;
    [[nodiscard]] auto minimap_to_world(const Point2D& minimap) const -> Point2D;
    [[nodiscard]] auto compute_scale() const -> double;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `MinimapPanel::render()` -- Draw a dark background. Draw each entry as a small colored rectangle at the scaled position. Draw the viewport indicator as a semi-transparent rectangle with a blue border.

2. `MinimapPanel::handle_click()` -- Convert click position to world coordinates. Call on_navigate with the world center point. The main canvas then centers on that point.

3. `MinimapPanel::handle_drag()` -- During drag, continuously update the navigate position for smooth panning.

4. `compute_scale()` -- Scale factor = min(panel_width / content_width, panel_height / content_height) with a margin.

5. Integration: CanvasPanel renders the MinimapPanel in the bottom-right corner as an overlay. Updates entries on every object change.

## Test Cases

File: `tests/unit/test_minimap.cpp`

1. **World to minimap** -- Known content bounds and panel size, verify conversion.
2. **Minimap to world** -- Verify inverse conversion.
3. **Click navigation** -- Click minimap center, verify on_navigate called with correct world point.
4. **Viewport indicator** -- Set viewport, verify indicator rectangle position.
5. **Scale computation** -- Wide content, tall panel, verify correct scale.
6. **Entry rendering** -- 5 entries, verify all within panel bounds.

## Acceptance Criteria

- [ ] Minimap shows simplified overview of all board objects
- [ ] Viewport indicator shows current visible area
- [ ] Click/drag on minimap navigates the main canvas
- [ ] Minimap positioned at bottom-right corner
- [ ] Toggleable visibility
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/MinimapPanel.h` | Minimap panel |
| CREATE | `src/canvas/MinimapPanel.cpp` | Minimap implementation |
| MODIFY | `src/ui/CanvasPanel.cpp` | Render minimap overlay |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_minimap.cpp` | 6 Catch2 tests |

## Architecture Notes

- The minimap renders simplified rectangles, not full object details. This keeps rendering fast.
- The minimap updates its entries on CanvasObjectAddedEvent/RemovedEvent/MovedEvent.
- Click/drag on the minimap publishes viewport change through the existing ViewportTransform.

## Estimated Complexity

**M** -- Coordinate mapping, simplified rendering, viewport indicator, click navigation, 6 tests.
