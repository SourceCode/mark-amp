# Phase 34 -- Diagram Libraries: UML and BPMN

## Objective

Implement predefined shape libraries for UML (class diagrams, sequence diagrams, use case diagrams) and BPMN (business process) modeling. Each library provides specialized shape types, connectors, and templates. Shapes are rendered using the existing ShapeRenderer infrastructure.

## Prerequisites

- Phase 09 (ShapeObject, ShapeRenderer::build_shape_path)
- Phase 10 (ConnectorObject for specialized connectors)
- Phase 08 (TextBoxObject for compartmented text)

## Feature References (PRD)

- PRD #68: Diagram Libraries (UML, BPMN)

## Data Structures to Implement

### File: `src/canvas/DiagramShapeObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

enum class DiagramLibrary : uint8_t { UML, BPMN };

enum class UMLShapeType : uint8_t
{
    Class,          // Compartmented rectangle (name/attributes/methods)
    Interface,      // Dashed outline rectangle
    AbstractClass,  // Italicized name
    Actor,          // Stick figure
    UseCase,        // Ellipse
    Package,        // Folder shape
    Component,      // Rectangle with tabs
    Node,           // 3D box
    Note,           // Folded corner rectangle
    Lifeline,       // Dashed vertical line (sequence)
    ActivationBar,  // Thin rectangle on lifeline
    Object          // Underlined name rectangle
};

enum class BPMNShapeType : uint8_t
{
    StartEvent,     // Thin circle
    EndEvent,       // Thick circle
    IntermediateEvent, // Double circle
    Task,           // Rounded rectangle
    SubProcess,     // Rounded rectangle with + marker
    Gateway,        // Diamond (XOR/AND/OR)
    DataObject,     // Rectangle with folded corner
    DataStore,      // Cylinder
    Pool,           // Large container rectangle
    Lane,           // Horizontal partition of pool
    Annotation,     // Open bracket with text
    MessageFlow     // Dashed line with envelope
};

class DiagramShapeObject : public CanvasObject
{
public:
    DiagramShapeObject();

    [[nodiscard]] auto library() const -> DiagramLibrary;
    auto set_library(DiagramLibrary lib) -> void;

    [[nodiscard]] auto uml_type() const -> UMLShapeType;
    auto set_uml_type(UMLShapeType type) -> void;

    [[nodiscard]] auto bpmn_type() const -> BPMNShapeType;
    auto set_bpmn_type(BPMNShapeType type) -> void;

    /// Compartments (e.g., class name / attributes / methods for UML Class).
    [[nodiscard]] auto compartments() const -> const std::vector<std::string>&;
    auto set_compartments(const std::vector<std::string>& compartments) -> void;

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto fill_color() const -> const CanvasColor&;
    auto set_fill_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto stroke_color() const -> const CanvasColor&;
    auto set_stroke_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    DiagramLibrary library_{DiagramLibrary::UML};
    UMLShapeType uml_type_{UMLShapeType::Class};
    BPMNShapeType bpmn_type_{BPMNShapeType::Task};
    std::vector<std::string> compartments_;
    std::string title_{"Class"};
    double width_{160.0};
    double height_{120.0};
    CanvasColor fill_color_{255, 255, 200, 255};
    CanvasColor stroke_color_{0, 0, 0, 255};
};

} // namespace markamp::canvas
```

### File: `src/canvas/DiagramLibraryPanel.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>
#include <string>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

struct DiagramShapeTemplate
{
    std::string name;
    DiagramLibrary library;
    UMLShapeType uml_type;
    BPMNShapeType bpmn_type;
    double default_width;
    double default_height;
};

class DiagramLibraryPanel
{
public:
    using OnShapeSelected = std::function<void(const DiagramShapeTemplate& tmpl)>;

    auto set_library(DiagramLibrary library) -> void;
    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;
    auto set_on_shape_selected(OnShapeSelected cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    DiagramLibrary active_library_{DiagramLibrary::UML};
    bool visible_{false};
    OnShapeSelected on_shape_selected_;

    [[nodiscard]] auto get_templates() const -> std::vector<DiagramShapeTemplate>;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `DiagramShapeRenderer::render()` -- Switch on library + type. UML Class: draw 3 compartments with divider lines. UML Actor: draw stick figure. BPMN Gateway: draw diamond. Reuse ShapeRenderer::build_shape_path for basic shapes.

2. `DiagramLibraryPanel::get_templates()` -- Return all shape templates for the active library (12 UML types, 12 BPMN types).

3. Specialized connectors: UML inheritance (triangle arrow), UML composition (filled diamond), BPMN message flow (dashed envelope). These extend the ArrowHead enum.

## Test Cases

File: `tests/unit/test_diagram_shapes.cpp`

1. **UML Class shape** -- Create with 3 compartments, verify.
2. **UML Actor** -- Set type, verify dimensions.
3. **BPMN Task** -- Create BPMN task, verify.
4. **BPMN Gateway** -- Diamond shape, verify.
5. **Library templates** -- Verify UML has 12 templates, BPMN has 12.
6. **JSON round-trip** -- Serialize/deserialize diagram shape.
7. **Clone** -- Clone diagram shape, verify.
8. **Compartment editing** -- Set compartments, verify.

## Acceptance Criteria

- [ ] DiagramShapeObject with UML and BPMN type enums
- [ ] 12 UML shape types rendered correctly
- [ ] 12 BPMN shape types rendered correctly
- [ ] Compartmented shapes (UML Class with name/attributes/methods sections)
- [ ] DiagramLibraryPanel with shape templates
- [ ] Specialized arrow heads for UML/BPMN connectors
- [ ] All 8 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/DiagramShapeObject.h` | Diagram shape model |
| CREATE | `src/canvas/DiagramShapeObject.cpp` | Implementation |
| CREATE | `src/canvas/DiagramShapeRenderer.h` | Specialized rendering |
| CREATE | `src/canvas/DiagramShapeRenderer.cpp` | UML/BPMN drawing |
| CREATE | `src/canvas/DiagramLibraryPanel.h` | Shape picker panel |
| CREATE | `src/canvas/DiagramLibraryPanel.cpp` | Panel implementation |
| MODIFY | `src/canvas/ConnectorObject.h` | Add UML/BPMN arrow head types |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_diagram_shapes.cpp` | 8 Catch2 tests |

## Architecture Notes

- DiagramShapeObject is a single CanvasObject subclass that wraps both UML and BPMN types. The library field determines which type enum is used.
- The rendering delegates to ShapeRenderer::build_shape_path for basic shapes (rectangles, circles, diamonds) and adds compartment lines and specialized decorations.
- The existing ConnectorObject is extended with new ArrowHead types rather than creating a separate connector class.

## Estimated Complexity

**L** -- 24 shape types with specialized rendering, compartmented class shapes, library panel, connector extensions, 8 tests.
