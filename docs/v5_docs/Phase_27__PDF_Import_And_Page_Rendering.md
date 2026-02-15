# Phase 27 -- PDF Import and Page Rendering

## Objective

Implement PDF file import that parses a multi-page PDF and creates one FrameObject per page with the rendered page as a background image. Includes a page navigator for quickly jumping between pages and integration with the existing PDFViewerPanel infrastructure.

## Prerequisites

- Phase 13 (ImageObject, ImageCache for page images)
- Phase 14 (FrameObject for page frames)
- Phase 05 (Board, UndoRedoStack)
- Existing PDFViewerPanel infrastructure (`src/ui/PDFViewerPanel.h`)

## Feature References (PRD)

- PRD #16: PDF Import/Pages

## Data Structures to Implement

### File: `src/canvas/PDFPageObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <filesystem>

namespace markamp::canvas
{

class PDFPageObject : public CanvasObject
{
public:
    PDFPageObject();

    [[nodiscard]] auto source_pdf() const -> const std::filesystem::path&;
    auto set_source_pdf(const std::filesystem::path& path) -> void;

    [[nodiscard]] auto page_number() const -> int;
    auto set_page_number(int page) -> void;

    [[nodiscard]] auto total_pages() const -> int;
    auto set_total_pages(int total) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto rendered_image_path() const -> const std::filesystem::path&;
    auto set_rendered_image_path(const std::filesystem::path& path) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::filesystem::path source_pdf_;
    int page_number_{0};
    int total_pages_{0};
    double width_{842.0};  // A4 width at 72 DPI
    double height_{1191.0};
    std::filesystem::path rendered_image_path_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/PDFImporter.h`

```cpp
#pragma once

#include "Board.h"

#include <filesystem>
#include <functional>

namespace markamp::canvas
{

struct PDFImportOptions
{
    bool create_frames{true};
    double page_gap{50.0};
    double scale{1.0};
    int dpi{150};
    bool horizontal_layout{false}; // false = vertical stack
};

class PDFImporter
{
public:
    using OnProgressCallback = std::function<void(int current_page, int total_pages)>;

    /// Import a PDF into the board. Creates one PDFPageObject per page.
    auto import_pdf(const std::filesystem::path& pdf_path,
                     Board& board,
                     const PDFImportOptions& options) -> bool;

    auto set_on_progress(OnProgressCallback cb) -> void;

private:
    OnProgressCallback on_progress_;

    auto render_page_to_image(const std::filesystem::path& pdf_path,
                               int page_number, int dpi,
                               const std::filesystem::path& output_path) -> bool;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `PDFImporter::import_pdf()` -- Open PDF (using existing PDF infrastructure or libpoppler). For each page: render to PNG at specified DPI, save to assets directory, create PDFPageObject with position (stacked vertically or horizontally with gap), add to board.

2. `PDFPageRenderer::render()` -- Load the rendered page image from ImageCache. Draw with the page number overlay in the corner.

3. Page navigator: A floating bar showing current page / total pages with prev/next buttons.

## Test Cases

File: `tests/unit/test_pdf_import.cpp`

1. **PDFPageObject construction** -- Verify defaults.
2. **Set page number** -- Set page, verify.
3. **JSON round-trip** -- Serialize/deserialize.
4. **Import options defaults** -- Verify default DPI, layout.
5. **Vertical layout positions** -- Import 3 pages, verify Y positions stacked.
6. **Horizontal layout** -- Set horizontal, verify X positions.

## Acceptance Criteria

- [ ] PDF import creates one PDFPageObject per page
- [ ] Pages rendered to PNG at configurable DPI
- [ ] Pages laid out vertically or horizontally with configurable gap
- [ ] Page navigator for quick jumping
- [ ] JSON serialization with source PDF reference
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/PDFPageObject.h` | PDF page data model |
| CREATE | `src/canvas/PDFPageObject.cpp` | Implementation |
| CREATE | `src/canvas/PDFPageRenderer.h` | PDF page rendering |
| CREATE | `src/canvas/PDFPageRenderer.cpp` | Render page image |
| CREATE | `src/canvas/PDFImporter.h` | PDF import logic |
| CREATE | `src/canvas/PDFImporter.cpp` | Import implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_pdf_import.cpp` | 6 Catch2 tests |

## Architecture Notes

- Page rendering uses the existing PDF infrastructure (PDFViewerPanel uses a PDF rendering backend). The importer renders pages to PNG files for canvas display.
- PNGs are stored in the board's assets directory for portability.
- For large PDFs, import runs with a progress callback.

## Estimated Complexity

**L** -- PDF parsing/rendering integration, multi-page layout, image asset management, 6 tests.
