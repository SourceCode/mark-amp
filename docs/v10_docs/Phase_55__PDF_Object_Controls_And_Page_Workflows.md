# Phase 55: PDF Object Controls and Page Workflows

## Overview
Improve PDF-on-canvas fundamentals for page import, paging controls, and placement workflows.

## Prerequisites
- `src/canvas/PDFImporter.*`
- `src/canvas/PDFPageObject.*`

## Tasks

### Task 1: Add multi-page import modes
**Files:** `src/canvas/PDFImporter.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Support single-page, range, and all-pages import modes.
**Acceptance Criteria:**
- Import options are exposed with predictable defaults

### Task 2: Add per-object page controls
**Files:** `src/canvas/PDFPageObject.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Navigate page number for an imported PDF object.
**Acceptance Criteria:**
- Page changes are quick and undoable

### Task 3: Add render quality settings
**Files:** `src/canvas/PDFImporter.cpp`, `src/canvas/ImageObjectRenderer.cpp`
**Description:** Tune raster quality by zoom/export requirements.
**Acceptance Criteria:**
- Quality setting balances clarity and performance

### Task 4: Add source-link and refresh controls
**Files:** `src/canvas/PDFPageObject.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Let users relink missing PDFs and refresh render.
**Acceptance Criteria:**
- Broken link states are recoverable in UI

### Task 5: Add PDF object tests
**Files:** `tests/unit/test_canvas_pdf_objects.cpp`
**Description:** Validate import mappings and page controls.
**Acceptance Criteria:**
- PDF object behavior and serialization are stable

## Testing Requirements
- Large document import stress tests
- Missing source file recovery paths

## Phase Completion Criteria
- PDF canvas workflows are practical and robust
