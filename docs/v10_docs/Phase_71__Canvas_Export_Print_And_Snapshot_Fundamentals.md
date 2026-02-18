# Phase 71: Canvas Export Print and Snapshot Fundamentals

## Overview
Complete export and snapshot fundamentals so boards can be reliably shared as artifacts without collaboration features.

## Prerequisites
- `src/canvas/BoardSerializer.*`
- `src/core/ExportService.*`

## Tasks

### Task 1: Add board export formats
**Files:** `src/core/ExportService.cpp`, `src/canvas/CanvasRenderer.cpp`
**Description:** Support PNG, SVG, and PDF export modes with selectable bounds.
**Acceptance Criteria:**
- Export supports full board, frame, and selection scopes

### Task 2: Add resolution and quality controls
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/core/ExportService.cpp`
**Description:** Expose DPI, scale, background transparency options.
**Acceptance Criteria:**
- Output quality controls are clear and validated

### Task 3: Add print-friendly pagination mode
**Files:** `src/ui/MainFrame.cpp`, `src/core/ExportService.cpp`
**Description:** Split large boards into printable pages.
**Acceptance Criteria:**
- Print preview reflects final page tiling accurately

### Task 4: Add quick snapshot command
**Files:** `src/ui/CanvasPanel.cpp`, `src/core/ExportService.cpp`
**Description:** Provide one-click snapshot to clipboard/file.
**Acceptance Criteria:**
- Snapshot action is fast and available via shortcut

### Task 5: Add export tests
**Files:** `tests/unit/test_canvas_export.cpp`
**Description:** Validate export bounds and format correctness.
**Acceptance Criteria:**
- Export round-trip checks pass for core object types

## Testing Requirements
- Large board export timing tests
- Visual diff checks on rendered outputs

## Phase Completion Criteria
- Canvas export/print fundamentals are reliable and configurable
