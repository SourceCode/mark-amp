# Phase 38 -- Export System

## Objective

Implement export of canvas content to PNG, PDF, SVG, and CSV formats. Includes full-board export, selection export, frame export, and area-selection export. Bulk export exports multiple boards or frames at once. The export dialog allows configuring resolution, background inclusion, and margin.

## Prerequisites

- Phase 02 (CanvasRenderer for rendering to off-screen contexts)
- Phase 05 (Board)
- Phase 14 (FrameObject for frame-based export)
- Phase 19 (TableObject for CSV export)

## Feature References (PRD)

- PRD #23: Export (PNG/PDF/SVG)
- PRD #44: Export to CSV
- PRD #79: Bulk Export

## Data Structures to Implement

### File: `src/canvas/CanvasExporter.h`

```cpp
#pragma once

#include "Board.h"
#include "CanvasRenderer.h"
#include "CanvasTypes.h"

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

enum class ExportFormat : uint8_t { PNG, PDF, SVG, CSV };

struct ExportOptions
{
    ExportFormat format{ExportFormat::PNG};
    double scale{2.0};         // Resolution multiplier (2.0 = retina)
    bool include_background{true};
    bool include_grid{false};
    double margin{20.0};       // World units
    int jpeg_quality{90};      // For JPEG (if supported)
    bool transparent_background{false}; // For PNG
};

enum class ExportScope : uint8_t
{
    FullBoard,      // Entire board content bounds
    Selection,      // Selected objects only
    Frame,          // Specific frame
    Area,           // User-defined rectangle
    AllFrames       // Each frame as a separate file (bulk)
};

struct ExportRequest
{
    ExportScope scope{ExportScope::FullBoard};
    ExportOptions options;
    std::filesystem::path output_path;
    ObjectId frame_id{kInvalidObjectId};      // For Frame scope
    AABB area;                                 // For Area scope
    std::vector<ObjectId> selected_ids;        // For Selection scope
};

class CanvasExporter
{
public:
    CanvasExporter(CanvasRenderer& renderer);

    /// Export according to the request.
    [[nodiscard]] auto export_board(const Board& board,
                                     const ExportRequest& request) -> bool;

    /// Export a specific area to PNG.
    [[nodiscard]] auto export_area_to_png(const Board& board, const AABB& area,
                                           const ExportOptions& options,
                                           const std::filesystem::path& output) -> bool;

    /// Export a specific area to SVG.
    [[nodiscard]] auto export_area_to_svg(const Board& board, const AABB& area,
                                           const ExportOptions& options,
                                           const std::filesystem::path& output) -> bool;

    /// Export a specific area to PDF.
    [[nodiscard]] auto export_area_to_pdf(const Board& board, const AABB& area,
                                           const ExportOptions& options,
                                           const std::filesystem::path& output) -> bool;

    /// Export tables to CSV.
    [[nodiscard]] auto export_tables_to_csv(const Board& board,
                                             const std::filesystem::path& output) -> bool;

    /// Bulk export: each frame as a separate file.
    [[nodiscard]] auto bulk_export_frames(const Board& board,
                                           const ExportOptions& options,
                                           const std::filesystem::path& output_dir) -> int;

    using OnProgressCallback = std::function<void(int current, int total)>;
    auto set_on_progress(OnProgressCallback cb) -> void;

private:
    CanvasRenderer& renderer_;
    OnProgressCallback on_progress_;

    auto render_to_bitmap(const Board& board, const AABB& area,
                           const ExportOptions& options) -> std::unique_ptr<wxBitmap>;
    auto render_to_svg_string(const Board& board, const AABB& area,
                               const ExportOptions& options) -> std::string;
};

} // namespace markamp::canvas
```

### File: `src/canvas/ExportDialog.h`

```cpp
#pragma once

#include "CanvasExporter.h"

#include <functional>

class wxGraphicsContext;

namespace markamp::canvas
{

class ExportDialog
{
public:
    using OnExportCallback = std::function<void(const ExportRequest& request)>;

    auto show(ExportScope default_scope = ExportScope::FullBoard) -> void;
    auto hide() -> void;
    auto render(wxGraphicsContext& gc, double cx, double cy, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;

    auto set_on_export(OnExportCallback cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;

private:
    bool visible_{false};
    ExportRequest request_;
    OnExportCallback on_export_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `CanvasExporter::export_area_to_png()` -- Create an off-screen wxBitmap at the specified scale. Create wxMemoryDC + wxGraphicsContext. Set viewport to fit the area. Render using CanvasRenderer. Save as PNG.

2. `CanvasExporter::export_area_to_svg()` -- Build SVG XML string. For each visible object, emit SVG elements (rect, circle, path, text). This is a parallel rendering path to the wxGraphicsContext path.

3. `CanvasExporter::export_area_to_pdf()` -- Use wxPrintDC or a PDF library to render the content.

4. `CanvasExporter::export_tables_to_csv()` -- Find all TableObject instances. For each, write headers and rows to CSV.

5. `CanvasExporter::bulk_export_frames()` -- Iterate frames sorted by slide_index. Export each to a separate file (frame_01.png, frame_02.png, etc.).

6. `ExportDialog::render()` -- Modal dialog with format picker, scope picker, scale slider, background toggle, output path selector.

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+E | Open export dialog | canvas |

## Test Cases

File: `tests/unit/test_canvas_exporter.cpp`

1. **Export PNG** -- Export small board, verify file created and non-empty.
2. **Export with scale** -- Scale 2x, verify bitmap dimensions are 2x.
3. **Export SVG** -- Export, verify valid SVG XML.
4. **Export CSV** -- Board with table, export, verify CSV content.
5. **Transparent background** -- Export PNG with transparent, verify alpha channel.
6. **Frame export** -- Export specific frame, verify only frame content.
7. **Selection export** -- Export selected objects only.
8. **Bulk export** -- 3 frames, bulk export, verify 3 files.
9. **Empty board** -- Export empty board, verify empty image/file.

## Acceptance Criteria

- [ ] Export to PNG, PDF, SVG, CSV formats
- [ ] Export scopes: full board, selection, frame, area, all frames
- [ ] Configurable scale, background, margin
- [ ] Bulk export creates one file per frame
- [ ] CSV export for tables
- [ ] Export dialog with format/scope/options
- [ ] All 9 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CanvasExporter.h` | Export engine |
| CREATE | `src/canvas/CanvasExporter.cpp` | Export implementations |
| CREATE | `src/canvas/ExportDialog.h` | Export dialog UI |
| CREATE | `src/canvas/ExportDialog.cpp` | Dialog implementation |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_canvas_exporter.cpp` | 9 Catch2 tests |

## Architecture Notes

- The PNG export path reuses the same CanvasRenderer used for on-screen rendering, just targeting an off-screen wxBitmap.
- SVG export is a separate code path because it needs to emit SVG primitives rather than rasterize. This is the most complex export format.
- PDF export can leverage wxPrintDC or an external library. The simplest approach is rendering to a high-resolution bitmap and embedding in a PDF page.
- CSV export only targets TableObjects; other object types are ignored.

## Estimated Complexity

**XL** -- 4 export format implementations, 5 scope modes, bulk export, SVG generation, export dialog, 9 tests.
