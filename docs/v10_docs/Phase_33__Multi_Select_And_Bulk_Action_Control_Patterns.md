# Phase 33: Multi-Select and Bulk Action Control Patterns

## Overview
Define consistent multi-select and bulk action controls across file lists, tabs, extensions, and canvas items.

## Prerequisites
- Phase 07
- Phase 27
- Phase 32

## Tasks

### Task 1: Standardize Selection Model
**Files:** `src/ui/*.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Align Ctrl/Cmd, Shift, and range-select behavior.
**Acceptance Criteria:**
- Selection behavior matches platform expectations

### Task 2: Add Bulk Action Bars
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Show contextual bulk action strip when multiple items selected.
**Acceptance Criteria:**
- Bulk strip shows valid actions and selection count

### Task 3: Add Safe Bulk Destructive Flows
**Files:** `src/ui/*.cpp`, `src/ui/NotificationManager.cpp`
**Description:** Provide preview and undo where feasible.
**Acceptance Criteria:**
- Bulk destructive actions are reviewable and recoverable

### Task 4: Improve Selection State Visibility
**Files:** `src/ui/*.cpp`
**Description:** Ensure selected and focused rows/items remain visually distinct.
**Acceptance Criteria:**
- Focus and selection are never visually conflated

### Task 5: Add Multi-Select Tests
**Files:** `tests/unit/test_multi_select_controls.cpp`
**Description:** Cover range selection, toggle selection, and bulk action routing.
**Acceptance Criteria:**
- Multi-select behavior has reliable regression coverage

## Testing Requirements
- Mixed keyboard/mouse selection workflows
- Large selection performance checks

## Phase Completion Criteria
- Bulk actions are discoverable, safe, and consistent
