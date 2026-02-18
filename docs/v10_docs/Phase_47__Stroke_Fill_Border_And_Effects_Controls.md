# Phase 47: Stroke Fill Border and Effects Controls

## Overview
Complete foundational style controls for object appearance: stroke, fill, border styles, opacity, and shadow basics.

## Prerequisites
- Phase 46
- `src/canvas/CanvasObject.h`

## Tasks

### Task 1: Expand stroke and border options
**Files:** `src/canvas/CanvasObject.h`, `src/canvas/ShapeRenderer.cpp`
**Description:** Add dash patterns, cap/join options, and border style consistency.
**Acceptance Criteria:**
- Style options render consistently across shape families

### Task 2: Add fill variants
**Files:** `src/canvas/ShapeData.cpp`, `src/canvas/TextBoxRenderer.cpp`
**Description:** Support solid and basic gradient fills.
**Acceptance Criteria:**
- Fill rendering is stable under zoom and export

### Task 3: Add object shadow/elevation controls
**Files:** `src/canvas/CanvasObject.h`, `src/canvas/ShapeRenderer.cpp`
**Description:** Implement subtle shadow controls with performance-safe defaults.
**Acceptance Criteria:**
- Shadow settings remain performant on large boards

### Task 4: Add style panel grouping
**Files:** `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Organize appearance controls into coherent sections.
**Acceptance Criteria:**
- Inspector style controls are discoverable and keyboard-accessible

### Task 5: Add style rendering tests
**Files:** `tests/unit/test_canvas_style_rendering.cpp`
**Description:** Validate style output and serialization.
**Acceptance Criteria:**
- Visual style properties survive save/load round-trips

## Testing Requirements
- Multi-object style apply checks
- Render stress with shadows and dashed strokes

## Phase Completion Criteria
- Appearance controls are complete for core object styling
