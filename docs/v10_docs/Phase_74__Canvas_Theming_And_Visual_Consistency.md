# Phase 74: Canvas Theming and Visual Consistency

## Overview
Ensure Canvas controls and objects align with app-wide theming while preserving board legibility.

## Prerequisites
- Phase 46
- `src/core/Theme.*`

## Tasks

### Task 1: Map canvas semantic tokens
**Files:** `src/core/Theme.h`, `src/canvas/CanvasRenderer.cpp`
**Description:** Define tokens for grid, guides, handles, selection, and canvas UI chrome.
**Acceptance Criteria:**
- Canvas visuals avoid hardcoded colors in upgraded paths

### Task 2: Add theme-aware control surfaces
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Ensure tool rail, inspector, and overlays update on theme switch.
**Acceptance Criteria:**
- Theme switching updates canvas UI instantly

### Task 3: Add legibility guardrails
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Prevent invisible guides/handles under extreme themes.
**Acceptance Criteria:**
- Critical interaction visuals maintain minimum contrast

### Task 4: Add board background style presets
**Files:** `src/canvas/CanvasRenderer.h`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Provide neutral board background presets with grid variants.
**Acceptance Criteria:**
- Background presets can be switched without object style corruption

### Task 5: Add theming tests
**Files:** `tests/unit/test_canvas_theming.cpp`
**Description:** Validate token coverage and runtime updates.
**Acceptance Criteria:**
- Missing token paths fallback safely and log warnings

## Testing Requirements
- Representative built-in themes visual checks
- Dark/light and high-contrast scenarios

## Phase Completion Criteria
- Canvas visuals are consistent, theme-aware, and legible
