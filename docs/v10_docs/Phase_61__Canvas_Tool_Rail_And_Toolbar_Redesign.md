# Phase 61: Canvas Tool Rail and Toolbar Redesign

## Overview
Redesign canvas control rails for fast tool access, clearer modes, and reduced mode errors.

## Prerequisites
- Phase 41
- `src/ui/CanvasWorkspacePanel.cpp`

## Tasks

### Task 1: Reorganize tool groups
**Files:** `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Group tools by select/draw/shape/text/media/structure categories.
**Acceptance Criteria:**
- Tool grouping reflects common user workflows

### Task 2: Add quick-switch and recent tools
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/core/Config.cpp`
**Description:** Provide one-key quick switch to last-used tool.
**Acceptance Criteria:**
- Recent tool stack persists per session

### Task 3: Add contextual top toolbar
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Show context-sensitive controls for active tool/selection.
**Acceptance Criteria:**
- Context toolbar updates without noticeable lag

### Task 4: Add custom tool visibility settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/SettingsCatalog.cpp`
**Description:** Let users hide low-usage tools.
**Acceptance Criteria:**
- Hidden tools remain available via command palette

### Task 5: Add tool rail tests
**Files:** `tests/unit/test_canvas_tool_rail.cpp`
**Description:** Validate tool selection, persistence, and context states.
**Acceptance Criteria:**
- Tool rail interactions remain regression-safe

## Testing Requirements
- Keyboard and pointer tool selection checks
- Small-width layout behavior

## Phase Completion Criteria
- Canvas tool controls are efficient and mode-safe
