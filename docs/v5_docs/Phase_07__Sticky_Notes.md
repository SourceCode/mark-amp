# Phase 07 -- Sticky Notes

## Objective

Implement colored sticky note canvas objects with inline text editing, configurable background colors, auto-sizing, and drag-to-place creation. Sticky notes are the most fundamental content object on the canvas and serve as the reference implementation for all future content-bearing objects.

## Prerequisites

- Phase 01 (CanvasObject base class, CanvasTypes)
- Phase 02 (CanvasRenderer, IObjectRenderer, CanvasPanel)
- Phase 03 (CanvasInputManager, tool system)
- Phase 04 (SelectionManager for interaction)
- Phase 05 (Board, UndoRedoStack)

## Feature References (PRD)

- PRD #2: Sticky Notes (core feature)
- PRD #39: Sticky Note Grouping (group support -- grouping is Phase 16)
- PRD #92: Smart Sticky Aggregation (duplicate detection -- Phase 31)

## Data Structures to Implement

### File: `src/canvas/StickyNoteObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

/// Predefined sticky note color presets.
struct StickyNoteColors
{
    static constexpr CanvasColor Yellow{255, 255, 204, 255};
    static constexpr CanvasColor Pink{255, 204, 229, 255};
    static constexpr CanvasColor Blue{204, 229, 255, 255};
    static constexpr CanvasColor Green{204, 255, 204, 255};
    static constexpr CanvasColor Orange{255, 229, 204, 255};
    static constexpr CanvasColor Purple{229, 204, 255, 255};
    static constexpr CanvasColor Gray{230, 230, 230, 255};

    [[nodiscard]] static auto all_colors() -> std::vector<CanvasColor>;
    [[nodiscard]] static auto color_name(const CanvasColor& color) -> std::string;
};

/// Sticky note size presets.
enum class StickyNoteSize : uint8_t
{
    Small,   // 140x140 world units
    Medium,  // 200x200
    Large,   // 280x280
    Custom   // User-defined
};

/// A colored sticky note on the canvas with editable text.
class StickyNoteObject : public CanvasObject
{
public:
    StickyNoteObject();
    explicit StickyNoteObject(const CanvasColor& color);

    // --- Content ---
    [[nodiscard]] auto text() const -> const std::string&;
    auto set_text(const std::string& text) -> void;

    // --- Appearance ---
    [[nodiscard]] auto color() const -> const CanvasColor&;
    auto set_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto size_preset() const -> StickyNoteSize;
    auto set_size_preset(StickyNoteSize size) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto font_size() const -> double;
    auto set_font_size(double size) -> void;

    [[nodiscard]] auto text_color() const -> const CanvasColor&;
    auto set_text_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto corner_radius() const -> double;

    // --- Auto-size ---
    /// Whether the note auto-sizes to fit text content.
    [[nodiscard]] auto auto_size() const -> bool;
    auto set_auto_size(bool enabled) -> void;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string text_;
    CanvasColor color_{StickyNoteColors::Yellow};
    CanvasColor text_color_{60, 60, 60, 255};
    StickyNoteSize size_preset_{StickyNoteSize::Medium};
    double width_{200.0};
    double height_{200.0};
    double font_size_{14.0};
    double corner_radius_{8.0};
    bool auto_size_{false};
};

} // namespace markamp::canvas
```

### File: `src/canvas/StickyNoteRenderer.h`

```cpp
#pragma once

#include "CanvasRenderer.h"
#include "StickyNoteObject.h"

namespace markamp::canvas
{

/// Renders StickyNoteObject instances on the canvas using wxGraphicsContext.
class StickyNoteRenderer : public IObjectRenderer
{
public:
    auto render(wxGraphicsContext& gc, const CanvasObject& obj,
                const ViewportTransform& vp) -> void override;
    [[nodiscard]] auto handles_type() const -> CanvasObjectType override;

private:
    /// Draw the sticky note body (rounded rectangle with shadow).
    auto draw_body(wxGraphicsContext& gc, const StickyNoteObject& note,
                    const AABB& screen_bounds) -> void;
    /// Draw the text content with word wrapping.
    auto draw_text(wxGraphicsContext& gc, const StickyNoteObject& note,
                    const AABB& screen_bounds, double zoom) -> void;
    /// Draw the fold/corner decoration.
    auto draw_corner_fold(wxGraphicsContext& gc, const AABB& screen_bounds,
                           const CanvasColor& color) -> void;
};

} // namespace markamp::canvas
```

### File: `src/canvas/StickyNoteTool.h`

```cpp
#pragma once

#include "CanvasTool.h"
#include "CanvasTypes.h"

namespace markamp::canvas
{

class Board;
class UndoRedoStack;

/// Tool for placing new sticky notes on the canvas.
/// Click to place at default size, or drag to define custom size.
class StickyNoteTool : public ICanvasTool
{
public:
    StickyNoteTool(Board& board, UndoRedoStack& undo_stack);

    [[nodiscard]] auto tool_mode() const -> ToolMode override { return ToolMode::StickyNote; }
    [[nodiscard]] auto tool_name() const -> std::string override { return "Sticky Note"; }
    [[nodiscard]] auto cursor_name() const -> std::string override { return "crosshair"; }

    auto on_activate() -> void override;
    auto on_deactivate() -> void override;
    auto on_mouse_down(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_move(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_up(const CanvasInputEvent& event) -> bool override;
    auto on_mouse_double_click(const CanvasInputEvent& event) -> bool override;
    auto on_scroll(const CanvasInputEvent& event) -> bool override;
    auto on_key_down(int key_code, int modifiers) -> bool override;
    auto on_key_up(int key_code, int modifiers) -> bool override;
    auto render_overlay(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void override;

    /// Set the color for new sticky notes.
    auto set_creation_color(const CanvasColor& color) -> void;

private:
    Board& board_;
    UndoRedoStack& undo_stack_;
    CanvasColor creation_color_{StickyNoteColors::Yellow};
    bool dragging_{false};
    Point2D drag_start_;
    Point2D drag_current_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/InlineTextEditor.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "ViewportTransform.h"

#include <functional>
#include <string>

class wxGraphicsContext;

namespace markamp::canvas
{

/// Inline text editor overlay for editing text directly on canvas objects.
/// Activated by double-clicking a text-bearing object. Renders a text cursor,
/// handles keyboard input, and commits changes on Enter or click-away.
class InlineTextEditor
{
public:
    using OnCommitCallback = std::function<void(const std::string& new_text)>;
    using OnCancelCallback = std::function<void()>;

    /// Begin editing at the given world-space bounds with initial text.
    auto begin_editing(ObjectId target_id, const AABB& world_bounds,
                        const std::string& initial_text,
                        double font_size, const CanvasColor& text_color) -> void;

    /// End editing, committing the current text.
    auto commit() -> void;

    /// Cancel editing, discarding changes.
    auto cancel() -> void;

    [[nodiscard]] auto is_editing() const -> bool;
    [[nodiscard]] auto target_id() const -> ObjectId;
    [[nodiscard]] auto current_text() const -> const std::string&;

    /// Handle key input. Returns true if consumed.
    auto handle_key(int key_code, int modifiers, bool shift) -> bool;
    /// Handle character input.
    auto handle_char(wchar_t ch) -> bool;

    /// Render the editor overlay (text, cursor, selection highlight).
    auto render(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void;

    auto set_on_commit(OnCommitCallback cb) -> void;
    auto set_on_cancel(OnCancelCallback cb) -> void;

private:
    bool editing_{false};
    ObjectId target_id_{kInvalidObjectId};
    AABB world_bounds_;
    std::string text_;
    std::string original_text_;
    double font_size_{14.0};
    CanvasColor text_color_{60, 60, 60, 255};
    int cursor_pos_{0};
    int selection_start_{-1};
    int selection_end_{-1};

    OnCommitCallback on_commit_;
    OnCancelCallback on_cancel_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `StickyNoteRenderer::render()` -- Transform world bounds to screen. Draw rounded rectangle with fill color. Draw shadow (offset darker rectangle). Draw corner fold. Draw text with word wrapping. At low zoom levels (<0.3), skip text and show color-only rectangles for performance.

2. `StickyNoteRenderer::draw_text()` -- Use wxGraphicsContext::SetFont with scaled font size (font_size * zoom). Implement word wrapping: split text into words, accumulate lines until width exceeded. Render each line.

3. `StickyNoteTool::on_mouse_down()` -- Record click position as drag start. Start dragging.

4. `StickyNoteTool::on_mouse_up()` -- If drag distance < 5px: place sticky note at click position with default size. Else: create sticky note with drag-defined bounds. Execute AddObjectCommand through UndoRedoStack.

5. `InlineTextEditor::begin_editing()` -- Store target, bounds, and text. Position cursor at end. Capture keyboard focus.

6. `InlineTextEditor::handle_key()` -- Arrow keys move cursor. Backspace/Delete remove characters. Enter commits (or inserts newline with Shift+Enter). Escape cancels. Cmd+A selects all.

7. `InlineTextEditor::render()` -- Draw white background over the edit area. Draw text with cursor (blinking line). Draw selection highlight if active.

8. Double-click on sticky note: SelectTool detects double-click on StickyNoteObject, activates InlineTextEditor for that note.

## Events to Add

```cpp
// In Events.h

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasInlineEditStartedEvent)
uint64_t object_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasInlineEditCommittedEvent)
uint64_t object_id{0};
std::string new_text;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_sticky_note.cpp`

1. **Default construction** -- Verify yellow color, 200x200 size, empty text.
2. **Set text** -- Set text, verify getter returns it.
3. **Set color** -- Set color, verify getter.
4. **Size presets** -- Set Small, verify 140x140. Set Large, verify 280x280.
5. **Local bounds** -- Verify local_bounds returns (0,0)-(width,height).
6. **World bounds** -- Set transform tx=100, ty=200. Verify world_bounds offset.
7. **JSON round-trip** -- Set all fields, serialize, deserialize, verify equality.
8. **Clone** -- Clone a sticky note, verify separate ID, same text/color/size.
9. **Color presets** -- Verify all_colors() returns 7 colors.
10. **Auto-size** -- Set auto_size=true, verify getter.

File: `tests/unit/test_inline_text_editor.cpp`

11. **Begin editing** -- begin_editing, verify is_editing=true.
12. **Commit** -- begin_editing, handle_char to add text, commit, verify on_commit callback called.
13. **Cancel** -- begin_editing, cancel, verify on_cancel called, original text preserved.
14. **Cursor movement** -- Arrow left/right, verify cursor position.
15. **Backspace** -- Type text, backspace, verify character removed.
16. **Select all** -- Cmd+A, verify selection covers full text.

## Acceptance Criteria

- [ ] StickyNoteObject with text, color, size presets, font size, auto-size
- [ ] StickyNoteRenderer draws rounded rect, shadow, corner fold, word-wrapped text
- [ ] StickyNoteTool places notes via click (default size) or drag (custom size)
- [ ] InlineTextEditor for double-click text editing with cursor, selection, commit/cancel
- [ ] JSON serialization round-trips all sticky note fields
- [ ] Clone produces independent copy with new ID
- [ ] Low-zoom optimization: skip text rendering at zoom < 0.3
- [ ] All 16 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/StickyNoteObject.h` | Sticky note data model |
| CREATE | `src/canvas/StickyNoteObject.cpp` | StickyNote implementation |
| CREATE | `src/canvas/StickyNoteRenderer.h` | Sticky note rendering |
| CREATE | `src/canvas/StickyNoteRenderer.cpp` | wxGraphicsContext drawing |
| CREATE | `src/canvas/StickyNoteTool.h` | Sticky note creation tool |
| CREATE | `src/canvas/StickyNoteTool.cpp` | Tool implementation |
| CREATE | `src/canvas/InlineTextEditor.h` | Inline text editing overlay |
| CREATE | `src/canvas/InlineTextEditor.cpp` | Text editing logic |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register StickyNoteObject factory |
| MODIFY | `src/canvas/CanvasInputManager.cpp` | Register StickyNoteTool |
| MODIFY | `src/core/Events.h` | Add inline edit events |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_sticky_note.cpp` | 10 tests |
| CREATE | `tests/unit/test_inline_text_editor.cpp` | 6 tests |

## Architecture Notes

- StickyNoteObject is the first concrete CanvasObject subclass. It establishes the pattern for all future object types: data model, renderer, tool, serialization factory registration.
- InlineTextEditor is shared across all text-bearing objects (sticky notes, text boxes, shapes with labels). It is not specific to sticky notes.
- The StickyNoteRenderer handles level-of-detail: at very low zoom, it renders only the color rectangle without text for performance.
- Word wrapping is implemented manually rather than using wxDC::DrawText word wrap, because wxGraphicsContext provides more control over anti-aliased text rendering.
- The corner fold is a small triangle in the top-right corner, rendered as a path with slightly darker fill -- a visual signature of sticky notes.

## Estimated Complexity

**L** -- New CanvasObject subclass, renderer with text wrapping and shadows, creation tool, inline text editor with cursor/selection, 16 tests.
