# Phase 53: Image Asset Workflows and Rendering

## Overview
Upgrade image object fundamentals for import, placement, sizing, cropping, and rendering quality.

## Prerequisites
- `src/canvas/ImageObject.*`
- `src/canvas/ImageObjectRenderer.*`

## Tasks

### Task 1: Improve image import and placement defaults
**Files:** `src/ui/CanvasPanel.cpp`, `src/canvas/ImageObject.cpp`
**Description:** Add sane initial scale and center placement behavior.
**Acceptance Criteria:**
- Imported images are immediately visible and editable

### Task 2: Add crop and fit modes
**Files:** `src/canvas/ImageObject.cpp`, `src/canvas/ImageObjectRenderer.cpp`
**Description:** Support fit, fill, stretch, and crop controls.
**Acceptance Criteria:**
- Crop state persists and is non-destructive

### Task 3: Add image replacement flow
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/canvas/ImageObject.cpp`
**Description:** Replace source while preserving object transform/style.
**Acceptance Criteria:**
- Replace operation keeps object position and dimensions

### Task 4: Add quality and memory knobs
**Files:** `src/canvas/TileCache.cpp`, `src/canvas/ImageObjectRenderer.cpp`
**Description:** Balance render quality and memory use by zoom level.
**Acceptance Criteria:**
- No visible severe aliasing at common zoom levels

### Task 5: Add image tests
**Files:** `tests/unit/test_canvas_images.cpp`
**Description:** Validate import, crop metadata, and serialization.
**Acceptance Criteria:**
- Image object workflows are regression-protected

## Testing Requirements
- Very large image stress tests
- Multi-image board pan/zoom performance checks

## Phase Completion Criteria
- Image workflows are reliable and visually high quality
