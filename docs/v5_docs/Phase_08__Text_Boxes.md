# Phase 08 -- Text Boxes

## Objective

Implement freeform text box objects with rich text formatting support (bold, italic, underline, bullet lists, headings), a contextual formatting toolbar, and auto-expanding height. Text boxes complement sticky notes by providing longer-form, formatted text on the canvas.

## Prerequisites

- Phase 01 (CanvasObject, CanvasTypes)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 07 (InlineTextEditor, StickyNoteObject as reference implementation)

## Feature References (PRD)

- PRD #3: Text Boxes

## Data Structures to Implement

### File: `src/canvas/TextBoxObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

/// A text span with formatting attributes.
struct TextSpan
{
    std::string text;
    bool bold{false};
    bool italic{false};
    bool underline{false};
    bool strikethrough{false};
    double font_size{14.0};
    CanvasColor color{0, 0, 0, 255};
    std::string font_family; // Empty = use default
};

/// A line/paragraph of formatted text.
struct TextLine
{
    std::vector<TextSpan> spans;
    enum class ListType : uint8_t { None, Bullet, Numbered, Checkbox };
    ListType list_type{ListType::None};
    int list_level{0};    // Indentation level for nested lists
    bool checked{false};  // For Checkbox list type
    int heading_level{0}; // 0 = body, 1-3 = heading levels
};

/// A freeform text box on the canvas with rich text formatting.
class TextBoxObject : public CanvasObject
{
public:
    TextBoxObject();

    // --- Content ---
    [[nodiscard]] auto lines() const -> const std::vector<TextLine>&;
    auto set_lines(const std::vector<TextLine>& lines) -> void;

    /// Get plain text (all spans concatenated, lines joined by newline).
    [[nodiscard]] auto plain_text() const -> std::string;
    /// Set from plain text (single span per line, no formatting).
    auto set_plain_text(const std::string& text) -> void;

    // --- Dimensions ---
    [[nodiscard]] auto width() const -> double;
    auto set_width(double w) -> void;
    [[nodiscard]] auto min_height() const -> double;
    [[nodiscard]] auto computed_height() const -> double; // Based on text content
    [[nodiscard]] auto auto_height() const -> bool;
    auto set_auto_height(bool enabled) -> void;

    // --- Styling ---
    [[nodiscard]] auto background_color() const -> const CanvasColor&;
    auto set_background_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto border_color() const -> const CanvasColor&;
    auto set_border_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto border_width() const -> double;
    auto set_border_width(double w) -> void;
    [[nodiscard]] auto has_background() const -> bool;
    auto set_has_background(bool enabled) -> void;
    [[nodiscard]] auto padding() const -> double;
    auto set_padding(double p) -> void;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::vector<TextLine> lines_;
    double width_{300.0};
    double min_height_{40.0};
    bool auto_height_{true};
    CanvasColor background_color_{255, 255, 255, 0}; // Transparent default
    CanvasColor border_color_{200, 200, 200, 255};
    double border_width_{0.0}; // 0 = no border
    bool has_background_{false};
    double padding_{12.0};
};

} // namespace markamp::canvas
```

### File: `src/canvas/TextBoxRenderer.h`

```cpp
#pragma once

#include "CanvasRenderer.h"

namespace markamp::canvas
{

class TextBoxRenderer : public IObjectRenderer
{
public:
    auto render(wxGraphicsContext& gc, const CanvasObject& obj,
                const ViewportTransform& vp) -> void override;
    [[nodiscard]] auto handles_type() const -> CanvasObjectType override;

private:
    auto draw_background(wxGraphicsContext& gc, const AABB& screen_bounds,
                          const CanvasColor& bg, const CanvasColor& border,
                          double border_width) -> void;
    auto draw_formatted_text(wxGraphicsContext& gc, const std::vector<TextLine>& lines,
                              const AABB& content_bounds, double zoom) -> void;
    auto draw_list_marker(wxGraphicsContext& gc, const TextLine& line,
                           double x, double y, double zoom) -> void;
};

} // namespace markamp::canvas
```

### File: `src/canvas/FormatToolbar.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>

class wxGraphicsContext;

namespace markamp::canvas
{

/// Floating toolbar that appears above a text-bearing object during editing.
/// Provides bold, italic, underline, list, heading formatting buttons.
class FormatToolbar
{
public:
    struct FormatState
    {
        bool bold{false};
        bool italic{false};
        bool underline{false};
        bool strikethrough{false};
        int heading_level{0};
        TextLine::ListType list_type{TextLine::ListType::None};
    };

    using OnFormatAction = std::function<void(const std::string& action)>;

    auto show(const Point2D& anchor_screen, const FormatState& state) -> void;
    auto hide() -> void;
    [[nodiscard]] auto is_visible() const -> bool;

    /// Hit-test the toolbar. Returns action name or empty string.
    [[nodiscard]] auto hit_test(const Point2D& screen_pos) const -> std::string;

    auto render(wxGraphicsContext& gc) -> void;

    auto set_on_format_action(OnFormatAction cb) -> void;

private:
    bool visible_{false};
    Point2D position_;
    FormatState state_;
    OnFormatAction on_format_action_;

    struct Button
    {
        std::string action;
        std::string label;
        AABB bounds;
        bool active{false};
    };
    std::vector<Button> buttons_;

    auto rebuild_buttons() -> void;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `TextBoxRenderer::draw_formatted_text()` -- Iterate lines. For each line, iterate spans. Set font attributes (bold/italic/underline) per span. Compute text extent for positioning. Handle heading sizes (H1=24pt, H2=20pt, H3=16pt).

2. `TextBoxRenderer::draw_list_marker()` -- For bullet lists, draw a filled circle. For numbered lists, draw the number. For checkboxes, draw a square (checked = filled). Indent based on list_level.

3. `TextBoxObject::computed_height()` -- Estimate height by counting lines, accounting for font sizes and line spacing. This approximation is used for layout; exact measurement happens during rendering.

4. `FormatToolbar::rebuild_buttons()` -- Create buttons: Bold(B), Italic(I), Underline(U), Strikethrough(S), H1, H2, H3, Bullet, Numbered, Checkbox. Position in a horizontal row.

5. `FormatToolbar::render()` -- Draw a rounded rectangle background. Draw each button with text/icon. Highlight active buttons.

6. Integration: When InlineTextEditor is active on a TextBoxObject, show FormatToolbar above the object. Format actions modify the TextSpan attributes of the current selection.

## Test Cases

File: `tests/unit/test_text_box.cpp`

1. **Default construction** -- Verify 300px width, auto-height, transparent background.
2. **Set plain text** -- Set "Hello\nWorld", verify 2 lines.
3. **Plain text extraction** -- Set formatted lines, verify plain_text() concatenates correctly.
4. **Auto-height** -- Set text content, verify computed_height > min_height.
5. **Background toggle** -- set_has_background(true), verify background renders.
6. **JSON round-trip** -- Full text box with formatted spans, serialize/deserialize, verify.
7. **Clone** -- Clone text box, verify independent copy.
8. **TextSpan formatting** -- Create span with bold+italic, verify attributes.
9. **List types** -- Create lines with bullet, numbered, checkbox types, verify.
10. **Heading levels** -- Set heading_level=1, verify in serialization.

## Acceptance Criteria

- [ ] TextBoxObject with rich text (TextLine/TextSpan model)
- [ ] TextBoxRenderer draws formatted text with bold, italic, underline, headings
- [ ] Bullet, numbered, and checkbox list rendering with indentation
- [ ] FormatToolbar appears during text editing with format toggle buttons
- [ ] Auto-height expands text box to fit content
- [ ] Optional background and border
- [ ] JSON serialization preserves all formatting
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/TextBoxObject.h` | Rich text box data model |
| CREATE | `src/canvas/TextBoxObject.cpp` | TextBox implementation |
| CREATE | `src/canvas/TextBoxRenderer.h` | Text box rendering |
| CREATE | `src/canvas/TextBoxRenderer.cpp` | Formatted text drawing |
| CREATE | `src/canvas/FormatToolbar.h` | Floating format toolbar |
| CREATE | `src/canvas/FormatToolbar.cpp` | Toolbar rendering and interaction |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register TextBoxObject factory |
| MODIFY | `src/canvas/CanvasInputManager.cpp` | Register Text creation tool |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_text_box.cpp` | 10 Catch2 tests |

## Architecture Notes

- TextBoxObject uses a line+span model rather than raw HTML or Markdown. This keeps the data model simple and renderable without a full markup parser.
- The FormatToolbar is drawn by the canvas renderer as an overlay, not as a wxWidgets child window. This avoids coordinate system issues.
- Rich text editing extends InlineTextEditor from Phase 07 with span-aware cursor movement and formatting commands.
- TextBoxes default to transparent background (just text on the canvas). Users can add a background for contrast.

## Estimated Complexity

**L** -- Rich text data model, formatted text rendering with multiple font styles, list rendering, floating toolbar, 10 tests.
