# Phase 04: Unified Input and Gesture Model

## Overview
Standardize mouse, keyboard, wheel, and trackpad gesture behavior across controls to eliminate inconsistent interactions.

## Prerequisites
- Phase 01
- Phase 03

## Tasks

### Task 1: Build Input Mapping Table
**Files:** `docs/v10_docs/input_mapping_table.md`
**Description:** Define canonical mappings for click, double-click, right-click, wheel, and modifiers.
**Acceptance Criteria:**
- All major control surfaces mapped

### Task 2: Standardize Wheel and Trackpad Behavior
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/TabBar.cpp`, `src/ui/SplitView.cpp`
**Description:** Normalize scroll speed, inertia handling, and horizontal scroll behavior.
**Acceptance Criteria:**
- Predictable wheel behavior across panels

### Task 3: Normalize Double-Click Semantics
**Files:** `src/ui/ActivityBar.cpp`, `src/ui/TabBar.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** Align double-click meanings and prevent accidental destructive actions.
**Acceptance Criteria:**
- Double-click action documented and consistent for each surface

### Task 4: Standardize Context Invocation
**Files:** `src/ui/*.cpp`
**Description:** Make right-click and keyboard menu key produce equivalent context menus.
**Acceptance Criteria:**
- Keyboard context invocation works in tree, tabs, and lists

### Task 5: Add Input Regression Tests
**Files:** `tests/unit/test_input_model_contracts.cpp`
**Description:** Verify key gesture mappings for critical controls.
**Acceptance Criteria:**
- Contract tests enforce mapping consistency

## Testing Requirements
- Manual pass with mouse + trackpad
- Keyboard-only context menu verification

## Phase Completion Criteria
- Input behaviors are consistent and documented across key controls
