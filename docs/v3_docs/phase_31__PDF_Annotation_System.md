# Phase 31 -- PDF Viewer & Annotation System

## Objective

Implement PDF viewing and annotation capabilities that allow users to open PDF files within MarkAmp, read them with full navigation and zoom controls, create highlight and rectangle annotations on specific pages, and link those annotations to Markdown blocks as references. SiYuan's PDF integration (implemented in `app/src/asset/anno.ts`, approximately 852 lines) uses PDF.js for rendering with a custom annotation overlay layer. Annotations are stored in `.sya` (SiYuan Annotation) JSON files alongside the PDF assets, enabling annotations to persist independently of the PDF content.

This phase brings three major capabilities: (1) a PDFViewerPanel that renders PDF pages within a wxPanel using a C++ PDF library (poppler-cpp or mupdf), with page navigation (previous/next, jump-to-page, thumbnail strip), zoom controls (fit-width, fit-page, zoom percentage), and text selection; (2) an annotation system that supports two annotation types -- text highlights (selection-based with configurable colors) and rectangle annotations (drawn over areas of interest) -- stored as structured data in `.sya` JSON files; (3) a reference system that links annotations to Markdown blocks via a specialized reference syntax `<<assets/file.pdf/annotation-id "annotation text">>`, enabling bidirectional navigation between a document's text and specific locations in a PDF.

The annotation data model stores each annotation with its page number, bounding rectangle (normalized to page dimensions for resolution independence), color, user-provided note text, and an optional block_ref_id linking it to a Markdown block. The PDFAnnotationStore handles loading and saving `.sya` files, which live alongside their PDF in the `assets/` directory. An annotation list sidebar in the viewer panel shows all annotations for the current PDF, grouped by page, with click-to-navigate functionality.

## Prerequisites

- No strict phase dependencies. Requires a PDF rendering library (poppler-cpp or mupdf) as a build dependency.

## SiYuan Source Reference

- `app/src/asset/anno.ts` (~852 lines) -- PDF.js integration, annotation creation/editing/deletion, highlight rendering overlay, rectangle annotation tool, annotation list panel, color picker
- `kernel/model/asset.go` -- Asset management, `.sya` file path resolution
- `kernel/model/pdf.go` -- Annotation storage and retrieval, annotation-to-block reference resolution
- SiYuan `.sya` format: JSON file with array of annotations, each containing page, type, color, rect, text, notes, block_ref_id
- Annotation reference syntax: `<<assets/file.pdf/annotation-id "page 5">>` in Markdown

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/PDFAnnotationTypes.h` | `markamp::core` | PDFAnnotation, AnnotationType, AnnotationColor structs |
| `src/core/PDFAnnotationStore.h` | `markamp::core` | Annotation persistence (load/save .sya files) |
| `src/core/PDFAnnotationStore.cpp` | `markamp::core` | PDFAnnotationStore implementation |
| `src/ui/PDFViewerPanel.h` | `markamp::ui` | PDF viewer with annotation overlay |
| `src/ui/PDFViewerPanel.cpp` | `markamp::ui` | PDFViewerPanel implementation |
| `src/ui/PDFAnnotationSidebar.h` | `markamp::ui` | Annotation list sidebar |
| `src/ui/PDFAnnotationSidebar.cpp` | `markamp::ui` | Annotation sidebar implementation |
| `src/ui/PDFThumbnailStrip.h` | `markamp::ui` | Page thumbnail strip |
| `src/ui/PDFThumbnailStrip.cpp` | `markamp::ui` | Thumbnail strip implementation |
| `tests/unit/test_pdf_annotations.cpp` | (test) | Catch2 test suite for annotation system |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add PDF annotation and navigation events |
| `src/ui/MainFrame.h` | Add PDF viewer panel management |
| `src/ui/MainFrame.cpp` | Wire PDF file opening to PDFViewerPanel |
| `src/ui/LayoutManager.h` | Register PDF viewer as a content panel |
| `src/CMakeLists.txt` | Add new source files, link PDF library |
| `tests/CMakeLists.txt` | Add test_pdf_annotations target |
| `resources/config_defaults.json` | Add PDF viewer config defaults |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/PDFAnnotationTypes.h
// ============================================================================
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of PDF annotation.
enum class AnnotationType : uint8_t
{
    Highlight, // Text selection highlight
    Rectangle  // Area selection rectangle
};

/// Predefined annotation colors.
enum class AnnotationColor : uint8_t
{
    Yellow,
    Green,
    Blue,
    Pink,
    Orange
};

/// Normalized rectangle (0.0 to 1.0 relative to page dimensions).
/// Resolution-independent: stored as fractions of page width/height.
struct NormalizedRect
{
    double x{0.0};     // Left edge (0.0 = left side of page)
    double y{0.0};     // Top edge (0.0 = top of page)
    double width{0.0};
    double height{0.0};

    [[nodiscard]] auto right() const -> double
    {
        return x + width;
    }

    [[nodiscard]] auto bottom() const -> double
    {
        return y + height;
    }

    [[nodiscard]] auto contains_point(double px, double py) const -> bool
    {
        return px >= x && px <= right() && py >= y && py <= bottom();
    }

    [[nodiscard]] auto is_valid() const -> bool
    {
        return width > 0.0 && height > 0.0
            && x >= 0.0 && y >= 0.0
            && right() <= 1.0 && bottom() <= 1.0;
    }

    /// Convert to pixel coordinates given page dimensions.
    [[nodiscard]] auto to_pixels(int page_width, int page_height) const
        -> std::tuple<int, int, int, int>
    {
        return {
            static_cast<int>(x * page_width),
            static_cast<int>(y * page_height),
            static_cast<int>(width * page_width),
            static_cast<int>(height * page_height)
        };
    }
};

/// A single annotation on a PDF page.
struct PDFAnnotation
{
    std::string id;              // Unique annotation ID (UUID)
    std::string pdf_path;        // Relative path to the PDF file (e.g., "assets/paper.pdf")
    int page{0};                 // 0-indexed page number
    AnnotationType type{AnnotationType::Highlight};
    AnnotationColor color{AnnotationColor::Yellow};
    NormalizedRect rect;         // Position on the page (normalized)
    std::string selected_text;   // Selected text (for highlights)
    std::string content;         // User-provided note/comment
    std::string block_ref_id;    // Linked Markdown block ID (empty if unlinked)
    std::chrono::system_clock::time_point created;
    std::chrono::system_clock::time_point updated;

    [[nodiscard]] auto is_highlight() const -> bool
    {
        return type == AnnotationType::Highlight;
    }

    [[nodiscard]] auto is_rectangle() const -> bool
    {
        return type == AnnotationType::Rectangle;
    }

    [[nodiscard]] auto has_block_ref() const -> bool
    {
        return !block_ref_id.empty();
    }

    [[nodiscard]] auto has_note() const -> bool
    {
        return !content.empty();
    }

    /// Generate the Markdown reference syntax for this annotation.
    [[nodiscard]] auto to_ref_syntax() const -> std::string
    {
        return "<<" + pdf_path + "/" + id + " \"page " + std::to_string(page + 1) + "\">>";
    }
};

/// Color values for annotation rendering.
struct AnnotationColorValues
{
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};
    uint8_t a{80}; // Semi-transparent by default

    [[nodiscard]] static auto for_color(AnnotationColor color) -> AnnotationColorValues
    {
        switch (color)
        {
        case AnnotationColor::Yellow:
            return {255, 235, 59, 80};
        case AnnotationColor::Green:
            return {76, 175, 80, 80};
        case AnnotationColor::Blue:
            return {33, 150, 243, 80};
        case AnnotationColor::Pink:
            return {233, 30, 99, 80};
        case AnnotationColor::Orange:
            return {255, 152, 0, 80};
        }
        return {255, 235, 59, 80}; // Default yellow
    }
};

/// A collection of annotations for a single PDF file.
struct PDFAnnotationSet
{
    std::string pdf_path;
    std::vector<PDFAnnotation> annotations;

    [[nodiscard]] auto count() const -> int
    {
        return static_cast<int>(annotations.size());
    }

    [[nodiscard]] auto annotations_on_page(int page) const -> std::vector<const PDFAnnotation*>
    {
        std::vector<const PDFAnnotation*> result;
        for (const auto& ann : annotations)
        {
            if (ann.page == page)
            {
                result.push_back(&ann);
            }
        }
        return result;
    }

    [[nodiscard]] auto find_annotation(const std::string& annotation_id) const
        -> const PDFAnnotation*
    {
        for (const auto& ann : annotations)
        {
            if (ann.id == annotation_id)
            {
                return &ann;
            }
        }
        return nullptr;
    }

    [[nodiscard]] auto find_annotation_mut(const std::string& annotation_id)
        -> PDFAnnotation*
    {
        for (auto& ann : annotations)
        {
            if (ann.id == annotation_id)
            {
                return &ann;
            }
        }
        return nullptr;
    }
};

/// PDF document metadata.
struct PDFDocumentInfo
{
    std::string title;
    std::string author;
    std::string subject;
    int page_count{0};
    double page_width{0.0};  // Points (1/72 inch)
    double page_height{0.0};

    [[nodiscard]] auto aspect_ratio() const -> double
    {
        if (page_height == 0.0)
        {
            return 1.0;
        }
        return page_width / page_height;
    }
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/PDFAnnotationStore.h
// ============================================================================
#pragma once

#include "PDFAnnotationTypes.h"

#include <expected>
#include <filesystem>
#include <string>

namespace markamp::core
{

/// Persists PDF annotations in .sya (SiYuan Annotation) JSON files.
/// Each PDF has a corresponding .sya file in the same directory:
///   assets/paper.pdf -> assets/paper.pdf.sya
class PDFAnnotationStore
{
public:
    explicit PDFAnnotationStore(std::filesystem::path workspace_root);

    /// Load annotations for a PDF file. Returns empty set if no .sya file exists.
    [[nodiscard]] auto load_annotations(const std::string& pdf_path)
        -> std::expected<PDFAnnotationSet, std::string>;

    /// Save annotations for a PDF file. Creates/overwrites the .sya file.
    [[nodiscard]] auto save_annotations(const PDFAnnotationSet& annotations)
        -> std::expected<void, std::string>;

    /// Add a single annotation and persist.
    [[nodiscard]] auto add_annotation(const std::string& pdf_path,
                                      PDFAnnotation annotation)
        -> std::expected<std::string, std::string>;

    /// Remove an annotation by ID and persist.
    [[nodiscard]] auto remove_annotation(const std::string& pdf_path,
                                          const std::string& annotation_id)
        -> std::expected<void, std::string>;

    /// Update an existing annotation and persist.
    [[nodiscard]] auto update_annotation(const std::string& pdf_path,
                                          const PDFAnnotation& annotation)
        -> std::expected<void, std::string>;

    /// Get annotations for a specific page.
    [[nodiscard]] auto get_annotations_by_page(const std::string& pdf_path, int page)
        -> std::expected<std::vector<PDFAnnotation>, std::string>;

    /// Get the .sya file path for a PDF.
    [[nodiscard]] auto sya_path(const std::string& pdf_path) const -> std::filesystem::path;

    /// Export all annotations as Markdown text.
    [[nodiscard]] auto export_as_markdown(const PDFAnnotationSet& annotations) const
        -> std::string;

private:
    std::filesystem::path workspace_root_;

    [[nodiscard]] auto serialize(const PDFAnnotationSet& annotations) const -> std::string;
    [[nodiscard]] auto deserialize(const std::string& json, const std::string& pdf_path) const
        -> std::expected<PDFAnnotationSet, std::string>;

    [[nodiscard]] static auto generate_id() -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`PDFAnnotationStore::load_annotations(pdf_path)`** -- Compute the `.sya` file path via `sya_path()`. Check if the file exists using `std::error_code` overloads. If it does not exist, return an empty `PDFAnnotationSet`. If it exists, read the file content, pass to `deserialize()`, and return the result.

2. **`PDFAnnotationStore::save_annotations(annotations)`** -- Serialize the annotation set to JSON via `serialize()`. Write atomically: write to a temp file in the same directory, then rename to the `.sya` path. Use `std::error_code` overloads for all filesystem operations.

3. **`PDFAnnotationStore::add_annotation(pdf_path, annotation)`** -- Load existing annotations. Generate a unique ID. Set `annotation.id` and `annotation.created` to now. Append to the list. Save. Return the new annotation ID.

4. **`PDFAnnotationStore::serialize(annotations)`** -- Produce JSON with: `pdf_path`, `annotations` array where each entry has: id, page, type (string), color (string), rect (object with x/y/w/h), selected_text, content, block_ref_id, created (ISO 8601), updated (ISO 8601).

5. **`PDFAnnotationStore::deserialize(json, pdf_path)`** -- Parse the JSON string. Validate required fields. Convert type and color strings back to enums. Parse ISO 8601 timestamps. Return `std::unexpected` with descriptive error for malformed JSON.

6. **`PDFAnnotationStore::export_as_markdown(annotations)`** -- Generate Markdown text with: a heading for the PDF filename, annotations grouped by page, each annotation as a blockquote with the selected text (or rectangle description), the user note, and the annotation reference syntax.

7. **`PDFViewerPanel::OnPaint(event)`** -- Render the current page using the PDF library. Apply zoom transform. Overlay annotations for the current page: draw semi-transparent colored rectangles for highlights and bordered rectangles for rectangle annotations. Highlight the selected annotation with a thicker border.

8. **`PDFViewerPanel::OnMouseDown(event)`** -- Determine the tool mode: if in highlight mode, start text selection. If in rectangle mode, start drawing a rectangle. If in navigate mode, check for annotation hit-testing (click on existing annotation to select it).

9. **`PDFViewerPanel::OnHighlightComplete(selection)`** -- Get the selected text from the PDF library. Compute the normalized rectangle from the selection bounds. Create a new PDFAnnotation with the selected text and color. Add via `annotation_store_.add_annotation()`. Publish `PDFAnnotationCreatedEvent`. Refresh.

10. **`PDFViewerPanel::navigate_to_page(page_number)`** -- Validate the page number (0 to page_count-1). Render the new page. Update the thumbnail strip selection. Update the page indicator. Publish `PDFPageChangedEvent`.

11. **`PDFViewerPanel::set_zoom(zoom_percent)`** -- Clamp zoom to [25, 400] percent. Re-render the current page at the new zoom level. Update scroll position to keep the current center point. Publish `PDFZoomChangedEvent`.

12. **`PDFAnnotationSidebar::populate(annotations)`** -- Clear the list. Group annotations by page number. For each page group, add a page header. For each annotation, add a list item showing: color indicator, type icon, selected text preview (truncated to 50 chars), and the note text if present. Bind click handler to navigate to the annotation's page and scroll to its position.

13. **`PDFViewerPanel::open_pdf(file_path)`** -- Open the PDF using the PDF library. Extract document info (title, author, page count). Load annotations from the `.sya` file. Render the first page. Populate the thumbnail strip. Populate the annotation sidebar.

14. **`PDFThumbnailStrip::render_thumbnails()`** -- For each page in the PDF, render a small thumbnail (100px wide). Display in a vertical scrolling strip. Highlight the current page. On click, navigate to the clicked page.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// PDF viewer events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PDFAnnotationCreatedEvent)
std::string pdf_path;
std::string annotation_id;
int page{0};
AnnotationType type{AnnotationType::Highlight};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PDFAnnotationDeletedEvent)
std::string pdf_path;
std::string annotation_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PDFAnnotationUpdatedEvent)
std::string pdf_path;
std::string annotation_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PDFPageChangedEvent)
std::string pdf_path;
int page{0};
int total_pages{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PDFZoomChangedEvent)
std::string pdf_path;
int zoom_percent{100};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OpenPDFRequestEvent)
std::string file_path;
int page{0}; // Optional: open at specific page
std::string annotation_id; // Optional: scroll to specific annotation
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.pdf.default_zoom` | int | 100 | Default zoom percentage for PDF viewer |
| `knowledgebase.pdf.highlight_color` | string | "yellow" | Default highlight annotation color |
| `knowledgebase.pdf.show_thumbnails` | bool | true | Show page thumbnail strip |
| `knowledgebase.pdf.show_annotations_sidebar` | bool | true | Show annotation list sidebar |
| `knowledgebase.pdf.annotation_export_format` | string | "markdown" | Export format: "markdown" or "json" |
| `knowledgebase.pdf.continuous_scroll` | bool | true | Continuous page scrolling vs. single page |

## Test Cases

All tests in `tests/unit/test_pdf_annotations.cpp` using Catch2.

1. **"PDFAnnotationStore returns empty set for nonexistent .sya file"** -- Call `load_annotations("nonexistent.pdf")`. Verify the result is a success containing an empty PDFAnnotationSet with `count() == 0`.

2. **"PDFAnnotationStore round-trips annotations through save/load"** -- Create a PDFAnnotationSet with 3 annotations (2 highlights, 1 rectangle). Save it. Load it back. Verify all 3 annotations are present with identical fields.

3. **"add_annotation generates unique ID and sets timestamps"** -- Call `add_annotation()` twice. Verify both returned IDs are different. Verify `created` timestamps are non-zero and within the last second.

4. **"remove_annotation deletes by ID"** -- Add 3 annotations. Remove the middle one by ID. Load annotations. Verify 2 remain and the removed ID is not present.

5. **"NormalizedRect correctly identifies containment"** -- Create a rect at (0.1, 0.2, 0.3, 0.4). Verify `contains_point(0.2, 0.3)` returns true. Verify `contains_point(0.5, 0.3)` returns false (outside right edge). Verify `contains_point(0.2, 0.7)` returns false (outside bottom edge).

6. **"NormalizedRect converts to pixel coordinates"** -- Create a rect at (0.25, 0.5, 0.5, 0.25) and convert with page dimensions (800, 1000). Verify pixel result is (200, 500, 400, 250).

7. **"annotations_on_page filters correctly"** -- Create a set with annotations on pages 0, 1, 1, 2. Call `annotations_on_page(1)`. Verify 2 annotations returned.

8. **"export_as_markdown produces valid Markdown"** -- Create annotations with selected_text and content. Export. Verify the output contains the PDF filename as a heading, page numbers, blockquoted text, and annotation reference syntax.

9. **"PDFAnnotation::to_ref_syntax produces correct format"** -- Create an annotation with `pdf_path="assets/paper.pdf"`, `id="ann-123"`, `page=4`. Verify `to_ref_syntax()` returns `"<<assets/paper.pdf/ann-123 \"page 5\">>"` (1-indexed page).

10. **"AnnotationColorValues returns correct RGBA for each color"** -- Verify all five colors return the expected RGB values. Verify alpha is 80 (semi-transparent).

## Acceptance Criteria

- [ ] PDFAnnotationStore persists annotations in .sya JSON files alongside PDFs
- [ ] Annotations survive save/load round-trips with all fields intact
- [ ] NormalizedRect provides resolution-independent positioning
- [ ] Highlight annotations capture selected text and bounding rectangle
- [ ] Rectangle annotations store arbitrary area selections
- [ ] Annotations link to Markdown blocks via block_ref_id
- [ ] to_ref_syntax() generates valid annotation reference syntax
- [ ] Export produces Markdown with annotations grouped by page
- [ ] PDFViewerPanel renders pages with annotation overlays
- [ ] Page navigation, zoom, and thumbnail strip work correctly

## Files to Create/Modify

```
CREATE  src/core/PDFAnnotationTypes.h
CREATE  src/core/PDFAnnotationStore.h
CREATE  src/core/PDFAnnotationStore.cpp
CREATE  src/ui/PDFViewerPanel.h
CREATE  src/ui/PDFViewerPanel.cpp
CREATE  src/ui/PDFAnnotationSidebar.h
CREATE  src/ui/PDFAnnotationSidebar.cpp
CREATE  src/ui/PDFThumbnailStrip.h
CREATE  src/ui/PDFThumbnailStrip.cpp
CREATE  tests/unit/test_pdf_annotations.cpp
MODIFY  src/core/Events.h              -- add PDF annotation and navigation events
MODIFY  src/ui/MainFrame.h             -- add PDF viewer panel management
MODIFY  src/ui/MainFrame.cpp           -- wire PDF file opening to viewer panel
MODIFY  src/ui/LayoutManager.h         -- register PDF viewer as content panel
MODIFY  src/CMakeLists.txt              -- add new source files, link PDF library
MODIFY  tests/CMakeLists.txt            -- add test_pdf_annotations target
MODIFY  resources/config_defaults.json  -- add knowledgebase.pdf.* defaults
```
