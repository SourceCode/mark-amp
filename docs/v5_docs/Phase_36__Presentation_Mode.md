# Phase 36 -- Presentation Mode

## Objective

Implement a fullscreen presentation mode that navigates through frames as slides. Includes smooth animated transitions between slides, presenter view with notes, keyboard/click navigation, and a slide overview strip. Frames with slide_index >= 0 define the slide order.

## Prerequisites

- Phase 14 (FrameObject with slide_index)
- Phase 02 (CanvasRenderer for frame content rendering)
- Phase 25 (OutlinePanel for slide ordering)

## Feature References (PRD)

- PRD #12: Presentation Mode

## Data Structures to Implement

### File: `src/canvas/PresentationMode.h`

```cpp
#pragma once

#include "Board.h"
#include "CanvasRenderer.h"
#include "CanvasTypes.h"
#include "ViewportTransform.h"

#include <functional>
#include <string>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

struct SlideInfo
{
    ObjectId frame_id{kInvalidObjectId};
    std::string title;
    int slide_index{0};
    AABB world_bounds;
    std::string notes; // Presenter notes (stored in frame metadata)
};

struct PresentationConfig
{
    double transition_duration{0.5};  // Seconds
    bool show_slide_numbers{true};
    bool loop{false};
    enum class TransitionType : uint8_t { None, Fade, Slide, Zoom };
    TransitionType transition{TransitionType::Zoom};
};

class PresentationMode
{
public:
    PresentationMode(Board& board, CanvasRenderer& renderer);

    /// Enter presentation mode. Returns false if no slides found.
    [[nodiscard]] auto enter() -> bool;
    /// Exit presentation mode.
    auto exit() -> void;
    [[nodiscard]] auto is_active() const -> bool;

    /// Navigation.
    auto next_slide() -> void;
    auto previous_slide() -> void;
    auto go_to_slide(int index) -> void;

    [[nodiscard]] auto current_slide_index() const -> int;
    [[nodiscard]] auto total_slides() const -> int;
    [[nodiscard]] auto current_slide() const -> const SlideInfo*;

    /// Render the current slide to the given graphics context.
    auto render(wxGraphicsContext& gc, double width, double height) -> void;

    /// Render the slide strip overview (bottom bar).
    auto render_slide_strip(wxGraphicsContext& gc, double y, double width, double height) -> void;

    /// Handle key events in presentation mode.
    auto handle_key(int key_code) -> bool;
    /// Handle click (advance slide).
    auto handle_click() -> bool;

    auto set_config(const PresentationConfig& config) -> void;
    [[nodiscard]] auto config() const -> const PresentationConfig&;

    [[nodiscard]] auto slides() const -> const std::vector<SlideInfo>&;

private:
    Board& board_;
    CanvasRenderer& renderer_;
    bool active_{false};
    int current_index_{0};
    std::vector<SlideInfo> slides_;
    PresentationConfig config_;

    // Transition animation state
    double transition_progress_{1.0}; // 0.0 = start, 1.0 = complete
    int transition_from_{-1};
    bool animating_{false};

    auto build_slides() -> void;
    auto start_transition(int from, int to) -> void;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `PresentationMode::build_slides()` -- Iterate all objects, find FrameObjects with slide_index >= 0, sort by slide_index, build SlideInfo list.

2. `PresentationMode::render()` -- Set up a ViewportTransform that fits the current frame exactly into the screen. Render all objects within the frame's bounds. During transitions, interpolate between two viewports.

3. `PresentationMode::start_transition()` -- Begin animating from one slide's viewport to the next. Transition type determines the interpolation: Zoom smoothly zooms between frame bounds, Slide pans horizontally, Fade cross-fades.

4. `PresentationMode::handle_key()` -- Right arrow/Space/Enter/PageDown = next slide. Left arrow/PageUp = previous. Escape = exit. Number keys = jump to slide.

5. Slide strip: A row of slide thumbnails at the bottom. Click to jump.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| F5 | Enter presentation mode | canvas |
| Escape | Exit presentation mode | presentation |
| Right/Space | Next slide | presentation |
| Left | Previous slide | presentation |

## Test Cases

File: `tests/unit/test_presentation_mode.cpp`

1. **Build slides** -- Board with 3 frames (slide_index 0,1,2), verify 3 slides built.
2. **Slide ordering** -- Frames with indices 2,0,1 -> verify ordered 0,1,2.
3. **No slides** -- Board with no frames, enter returns false.
4. **Next/previous** -- Navigate forward and back, verify index changes.
5. **Go to slide** -- Jump to slide 2, verify.
6. **Loop mode** -- At last slide, next wraps to first (when loop=true).
7. **Keyboard handling** -- Right key -> next, Left -> previous, Escape -> exit.
8. **Enter/exit** -- Enter, verify active. Exit, verify inactive.

## Acceptance Criteria

- [ ] Frames with slide_index used as presentation slides
- [ ] Fullscreen rendering of frame content
- [ ] Smooth transitions (zoom, slide, fade, none)
- [ ] Keyboard and click navigation
- [ ] Slide strip overview at bottom
- [ ] Presenter notes from frame metadata
- [ ] Loop mode option
- [ ] F5 to enter, Escape to exit
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/PresentationMode.h` | Presentation mode |
| CREATE | `src/canvas/PresentationMode.cpp` | Implementation |
| MODIFY | `src/ui/CanvasPanel.cpp` | Integrate presentation mode |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_presentation_mode.cpp` | 8 Catch2 tests |

## Architecture Notes

- PresentationMode takes over the entire CanvasPanel during presentation. Normal input handling is suspended.
- Transitions use a wxTimer for animation, interpolating ViewportTransform between frame bounds.
- The slide strip renders miniature thumbnails by scaling down the frame rendering.

## Estimated Complexity

**L** -- Fullscreen viewport management, animated transitions, slide strip, keyboard navigation, 8 tests.
