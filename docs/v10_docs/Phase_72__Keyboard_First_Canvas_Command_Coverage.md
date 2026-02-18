# Phase 72: Keyboard-First Canvas Command Coverage

## Overview
Make Canvas fully operable via keyboard with broad command coverage and consistent shortcuts.

## Prerequisites
- Phase 61
- Phase 67

## Tasks

### Task 1: Map essential canvas commands
**Files:** `src/ui/MainFrame.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Ensure core actions have keyboard command routes.
**Acceptance Criteria:**
- Select/move/style/arrange/navigation actions are command-addressable

### Task 2: Add tool switching shortcuts
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/MainFrame.cpp`
**Description:** Provide direct keys for primary tools.
**Acceptance Criteria:**
- Tool shortcut map is consistent and conflict-free

### Task 3: Add object nudge and transform shortcuts
**Files:** `src/canvas/SelectionManager.cpp`, `src/canvas/CanvasCommands.cpp`
**Description:** Support pixel/grid nudging and constrained transforms.
**Acceptance Criteria:**
- Nudge behaviors are precise and undoable

### Task 4: Add shortcut discoverability in UI
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/ShortcutOverlay.cpp`
**Description:** Show shortcut hints in tooltips and menus.
**Acceptance Criteria:**
- Shortcut hints are visible and platform-correct

### Task 5: Add keyboard command tests
**Files:** `tests/unit/test_canvas_keyboard_commands.cpp`
**Description:** Validate shortcut execution and context rules.
**Acceptance Criteria:**
- Keyboard workflows pass regression tests

## Testing Requirements
- Keyboard-only canvas editing session
- Shortcut conflict audit

## Phase Completion Criteria
- Canvas can be efficiently used without pointer-only dependence
