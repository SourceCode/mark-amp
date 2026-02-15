# Phase 19 -- Table Creation and Editing

## Objective

Implement table objects on the canvas with a grid of editable cells, row/column management (add, remove, resize), cell merging, and basic styling (background colors, borders). Tables are first-class canvas objects with full selection, transform, and serialization support.

## Prerequisites

- Phase 01 (CanvasObject, CanvasTypes)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 07 (InlineTextEditor for cell editing)
- Phase 05 (Board, UndoRedoStack)

## Feature References (PRD)

- PRD #18: Table Creation

## Data Structures to Implement

### File: `src/canvas/TableObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

struct TableCell
{
    std::string text;
    CanvasColor background_color{255, 255, 255, 0}; // Transparent
    CanvasColor text_color{0, 0, 0, 255};
    bool bold{false};
    bool italic{false};
    int col_span{1};
    int row_span{1};
    enum class Alignment : uint8_t { Left, Center, Right };
    Alignment alignment{Alignment::Left};
};

struct TableColumn
{
    double width{120.0};
    std::string header;
};

struct TableRow
{
    double height{36.0};
    std::vector<TableCell> cells;
};

class TableObject : public CanvasObject
{
public:
    TableObject();
    TableObject(int rows, int cols);

    [[nodiscard]] auto row_count() const -> int;
    [[nodiscard]] auto col_count() const -> int;

    auto add_row(int at_index = -1) -> void;
    auto remove_row(int index) -> void;
    auto add_column(int at_index = -1) -> void;
    auto remove_column(int index) -> void;

    [[nodiscard]] auto cell(int row, int col) -> TableCell&;
    [[nodiscard]] auto cell(int row, int col) const -> const TableCell&;
    auto set_cell(int row, int col, const TableCell& cell) -> void;

    [[nodiscard]] auto column(int col) const -> const TableColumn&;
    auto set_column_width(int col, double width) -> void;
    auto set_column_header(int col, const std::string& header) -> void;

    [[nodiscard]] auto row(int row) const -> const TableRow&;
    auto set_row_height(int row, double height) -> void;

    [[nodiscard]] auto has_header_row() const -> bool;
    auto set_has_header_row(bool enabled) -> void;

    [[nodiscard]] auto total_width() const -> double;
    [[nodiscard]] auto total_height() const -> double;

    /// Get the cell at a local-space point (for click handling).
    [[nodiscard]] auto cell_at_point(const Point2D& local_point) const
        -> std::optional<std::pair<int, int>>;

    [[nodiscard]] auto border_color() const -> const CanvasColor&;
    auto set_border_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto header_background() const -> const CanvasColor&;
    auto set_header_background(const CanvasColor& color) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::vector<TableColumn> columns_;
    std::vector<TableRow> rows_;
    bool has_header_row_{true};
    CanvasColor border_color_{200, 200, 200, 255};
    CanvasColor header_background_{240, 240, 240, 255};
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `TableRenderer::render()` -- Draw grid lines. Draw header row with background. Draw each cell text with alignment. Highlight selected cell.

2. `TableObject::cell_at_point()` -- Iterate columns/rows, accumulate widths/heights, find which cell contains the point.

3. Column resize: When hovering a column border in the table, show a resize cursor. Drag to resize.

4. Cell editing: Double-click a cell to activate InlineTextEditor bounded to the cell rectangle.

5. Tab key navigation: While editing a cell, Tab moves to next cell, Shift+Tab to previous.

## Test Cases

File: `tests/unit/test_table_object.cpp`

1. **Construction** -- 3x4 table, verify row/col counts.
2. **Add row** -- Add row, verify count increments.
3. **Remove row** -- Remove row, verify count decrements.
4. **Add column** -- Add column, verify.
5. **Cell access** -- Set cell text, retrieve, verify.
6. **Column width** -- Set width, verify total_width changes.
7. **Cell at point** -- Known table layout, query point in cell (1,2), verify.
8. **Header row** -- has_header_row=true, verify.
9. **JSON round-trip** -- Full table, serialize/deserialize, verify cells.
10. **Clone** -- Clone table, verify independent.
11. **Total dimensions** -- 3 cols at 120px, 4 rows at 36px, verify totals.
12. **Cell alignment** -- Set center alignment, verify.

## Acceptance Criteria

- [ ] TableObject with rows, columns, cells, headers
- [ ] Cell editing via double-click with InlineTextEditor
- [ ] Column resize by dragging borders
- [ ] Add/remove rows and columns
- [ ] Cell styling: background color, text color, bold, italic, alignment
- [ ] Tab navigation between cells
- [ ] Table rendered with grid lines, header background, cell text
- [ ] JSON serialization preserves all cell data
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/TableObject.h` | Table data model |
| CREATE | `src/canvas/TableObject.cpp` | Table implementation |
| CREATE | `src/canvas/TableRenderer.h` | Table rendering |
| CREATE | `src/canvas/TableRenderer.cpp` | Grid + cell drawing |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register TableObject factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_table_object.cpp` | 12 Catch2 tests |

## Architecture Notes

- Tables are single CanvasObjects, not collections of separate cell objects. This simplifies selection and serialization.
- Cell editing reuses InlineTextEditor from Phase 07, scoped to the cell rectangle.
- Column resize and row resize are handled by special hit zones on the table borders, distinct from the normal resize handles.

## Estimated Complexity

**L** -- Table data model with row/column operations, cell editing, column resize, renderer with grid lines, 12 tests.
