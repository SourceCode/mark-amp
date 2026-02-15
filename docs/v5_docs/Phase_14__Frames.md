# Phase 14 -- Frames

## Objective

Implement frame container objects that define named rectangular regions on the canvas. Frames act as slide boundaries for presentation mode, as grouping containers for export, and as semantic regions for organization. Objects placed inside a frame's bounds are automatically parented to it and move with the frame.

## Prerequisites

- Phase 01 (CanvasObject, AABB, Transform2D)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 03 (CanvasInputManager)
- Phase 04 (SelectionManager)
- Phase 05 (Board, UndoRedoStack)

## Feature References (PRD)

- PRD #11: Frames (container/slide regions)
- PRD #12: Presentation Mode (frames as slides -- presentation in Phase 36)

## Data Structures to Implement

### File: `src/canvas/FrameObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

/// Preset frame sizes.
struct FramePresets
{
    static constexpr Size2D Slide16_9{1920, 1080};
    static constexpr Size2D Slide4_3{1600, 1200};
    static constexpr Size2D A4{842, 1191};       // Landscape
    static constexpr Size2D Letter{1056, 816};    // Landscape
    static constexpr Size2D Custom{800, 600};

    [[nodiscard]] static auto all_presets()
        -> std::vector<std::pair<std::string, Size2D>>;
};

/// A named rectangular container region on the canvas.
/// Frames serve as slide boundaries, grouping containers, and export regions.
class FrameObject : public CanvasObject
{
public:
    FrameObject();
    explicit FrameObject(const std::string& title, double width, double height);

    // --- Title ---
    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;

    // --- Dimensions ---
    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    // --- Appearance ---
    [[nodiscard]] auto background_color() const -> const CanvasColor&;
    auto set_background_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto has_background() const -> bool;
    auto set_has_background(bool enabled) -> void;
    [[nodiscard]] auto border_color() const -> const CanvasColor&;
    auto set_border_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto border_width() const -> double;
    auto set_border_width(double w) -> void;

    // --- Slide ordering ---
    [[nodiscard]] auto slide_index() const -> int;
    auto set_slide_index(int index) -> void;

    // --- Contained objects ---
    /// IDs of objects contained within this frame's bounds.
    /// These are tracked by the board, not stored on the frame itself.
    /// The frame determines containment dynamically from object bounds.

    // --- Clipping ---
    [[nodiscard]] auto clips_content() const -> bool;
    auto set_clips_content(bool clip) -> void;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string title_{"Untitled Frame"};
    double width_{1920.0};
    double height_{1080.0};
    CanvasColor background_color_{255, 255, 255, 255};
    bool has_background_{true};
    CanvasColor border_color_{180, 180, 180, 255};
    double border_width_{2.0};
    int slide_index_{-1}; // -1 = not in slideshow
    bool clips_content_{false};
};

} // namespace markamp::canvas
```

### File: `src/canvas/FrameRenderer.h`

```cpp
#pragma once

#include "CanvasRenderer.h"

namespace markamp::canvas
{

class FrameRenderer : public IObjectRenderer
{
public:
    auto render(wxGraphicsContext& gc, const CanvasObject& obj,
                const ViewportTransform& vp) -> void override;
    [[nodiscard]] auto handles_type() const -> CanvasObjectType override;

private:
    auto draw_title_bar(wxGraphicsContext& gc, const std::string& title,
                         const AABB& screen_bounds, double zoom) -> void;
    auto draw_slide_badge(wxGraphicsContext& gc, int slide_index,
                           const AABB& screen_bounds) -> void;
};

} // namespace markamp::canvas
```

### File: `src/canvas/FrameContainmentTracker.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::canvas
{

class Board;

/// Tracks which objects are contained within which frames.
/// Containment is determined by whether an object's center is within
/// a frame's bounds. This runs after object moves to update parenting.
class FrameContainmentTracker
{
public:
    /// Recompute containment for all objects in the board.
    auto recompute(const Board& board) -> void;

    /// Get the frame that contains a given object (if any).
    [[nodiscard]] auto containing_frame(ObjectId object_id) const -> ObjectId;

    /// Get all objects contained in a given frame.
    [[nodiscard]] auto objects_in_frame(ObjectId frame_id) const
        -> const std::unordered_set<ObjectId>&;

    /// Get all frame IDs.
    [[nodiscard]] auto all_frame_ids() const -> std::vector<ObjectId>;

private:
    std::unordered_map<ObjectId, ObjectId> object_to_frame_;
    std::unordered_map<ObjectId, std::unordered_set<ObjectId>> frame_to_objects_;
    static const std::unordered_set<ObjectId> empty_set_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `FrameRenderer::render()` -- Draw frame border. Draw background if enabled. Draw title bar above the frame with the title text. Draw slide index badge in corner if slide_index >= 0.

2. `FrameRenderer::draw_title_bar()` -- Draw a small rectangle above the frame's top edge with the title. This is always visible regardless of zoom level (capped minimum font size).

3. `FrameContainmentTracker::recompute()` -- Iterate all objects. For each non-frame object, check if its center is within any frame's bounds. Update the bidirectional map.

4. Frame move behavior: When a frame is moved, all objects contained in that frame move with it. This is implemented by: (1) SelectionManager detects that a frame is being moved. (2) Query FrameContainmentTracker for contained objects. (3) Add contained objects to the move operation.

5. Frame creation: A FrameTool lets users drag to define frame bounds, or select a preset size.

## Test Cases

File: `tests/unit/test_frame_object.cpp`

1. **Default construction** -- Verify 1920x1080, white background, titled "Untitled Frame".
2. **Set dimensions** -- Set 800x600, verify.
3. **Slide index** -- Set slide_index=3, verify.
4. **Background toggle** -- set_has_background(false), verify.
5. **Clips content** -- set_clips_content(true), verify.
6. **JSON round-trip** -- Full frame, serialize/deserialize, verify.
7. **Clone** -- Clone frame, verify.
8. **Presets** -- Verify FramePresets::all_presets() returns expected sizes.

File: `tests/unit/test_frame_containment.cpp`

9. **Containment detection** -- Frame at (0,0) 500x500. Object at (250,250). Verify contained.
10. **Non-containment** -- Object at (600,600). Verify not contained.
11. **Multiple frames** -- Two frames, objects in each, verify correct mapping.
12. **Frame move carries objects** -- Move frame, verify contained objects also moved.

## Acceptance Criteria

- [ ] FrameObject with title, dimensions, background, border, slide index, clipping
- [ ] FrameRenderer draws border, title bar, background, slide badge
- [ ] FrameContainmentTracker determines object-to-frame parenting
- [ ] Moving a frame moves all contained objects
- [ ] Frame presets for common sizes (16:9, 4:3, A4, Letter)
- [ ] Frame creation tool with drag-to-size and preset picker
- [ ] JSON serialization round-trips all fields
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/FrameObject.h` | Frame data model |
| CREATE | `src/canvas/FrameObject.cpp` | Frame implementation |
| CREATE | `src/canvas/FrameRenderer.h` | Frame rendering |
| CREATE | `src/canvas/FrameRenderer.cpp` | Border, title, slide badge drawing |
| CREATE | `src/canvas/FrameContainmentTracker.h` | Containment tracking |
| CREATE | `src/canvas/FrameContainmentTracker.cpp` | Containment recomputation |
| CREATE | `src/canvas/FrameTool.h` | Frame creation tool |
| CREATE | `src/canvas/FrameTool.cpp` | Tool implementation |
| MODIFY | `src/canvas/SelectionManager.cpp` | Frame move includes contained objects |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register FrameObject factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_frame_object.cpp` | 8 tests |
| CREATE | `tests/unit/test_frame_containment.cpp` | 4 tests |

## Architecture Notes

- Frames are rendered below other objects (low z-index) but their title bars render above all objects as an overlay.
- Containment is computed dynamically based on object centers, not stored as a persistent parent-child relationship. This avoids synchronization issues.
- The FrameContainmentTracker runs after every move/add/remove operation. For performance, it only recomputes affected objects.
- Frames form the basis of presentation mode (Phase 36): frames with slide_index >= 0 are ordered slides.

## Estimated Complexity

**M** -- Frame model, renderer with title bar, containment tracking, frame-move behavior, 12 tests.
