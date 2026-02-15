# Phase 15 -- Board Sections

## Objective

Implement board sections -- semantic grouping areas with background tinting, header labels, and collapsible regions. Sections are lighter-weight than frames: they provide visual organization without the slide/export semantics.

## Prerequisites

- Phase 01 (CanvasObject, CanvasTypes)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 14 (FrameObject as reference container, FrameContainmentTracker pattern)

## Feature References (PRD)

- PRD #59: Board Sections

## Data Structures to Implement

### File: `src/canvas/SectionObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

namespace markamp::canvas
{

class SectionObject : public CanvasObject
{
public:
    SectionObject();
    explicit SectionObject(const std::string& label, double width, double height);

    [[nodiscard]] auto label() const -> const std::string&;
    auto set_label(const std::string& label) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto tint_color() const -> const CanvasColor&;
    auto set_tint_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto tint_opacity() const -> double;
    auto set_tint_opacity(double opacity) -> void;

    [[nodiscard]] auto label_font_size() const -> double;
    auto set_label_font_size(double size) -> void;

    [[nodiscard]] auto is_collapsed() const -> bool;
    auto set_collapsed(bool collapsed) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string label_{"Section"};
    double width_{600.0};
    double height_{400.0};
    CanvasColor tint_color_{100, 150, 255, 255};
    double tint_opacity_{0.08};
    double label_font_size_{18.0};
    bool collapsed_{false};
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `SectionRenderer::render()` -- Draw a filled rectangle with the tint color at low opacity. Draw the label at the top-left corner with larger font. Draw a subtle border. If collapsed, draw only the label header bar.

2. Collapse/expand: Double-clicking a section's header toggles collapsed state. When collapsed, contained objects are hidden (their visible flag is temporarily set to false).

3. Section creation tool: Drag to define section bounds. Label is editable inline.

## Test Cases

File: `tests/unit/test_section_object.cpp`

1. **Default construction** -- Verify label, dimensions, tint color.
2. **Tint color and opacity** -- Set tint, verify.
3. **Collapsed toggle** -- Set collapsed, verify.
4. **JSON round-trip** -- Serialize/deserialize, verify all fields.
5. **Clone** -- Clone, verify independent.
6. **Local bounds** -- Verify bounds match dimensions.

## Acceptance Criteria

- [ ] SectionObject with label, tint color, opacity, collapsible state
- [ ] SectionRenderer draws tinted background, label header, subtle border
- [ ] Collapse hides contained objects
- [ ] Section creation tool
- [ ] JSON serialization
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/SectionObject.h` | Section data model |
| CREATE | `src/canvas/SectionObject.cpp` | Section implementation |
| CREATE | `src/canvas/SectionRenderer.h` | Section rendering |
| CREATE | `src/canvas/SectionRenderer.cpp` | Tinted background + label drawing |
| MODIFY | `src/canvas/FrameContainmentTracker.h` | Extend to track section containment too |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register SectionObject factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_section_object.cpp` | 6 Catch2 tests |

## Architecture Notes

- Sections are rendered at the lowest z-index layer, below frames, which are below regular objects.
- The FrameContainmentTracker is extended to also track section containment (rename to ContainmentTracker if appropriate).
- Sections differ from frames: no slide index, no clipping, just visual organization.

## Estimated Complexity

**S** -- Simple container object with tinted rendering, collapse toggle, reuses containment tracker pattern from Phase 14, 6 tests.
