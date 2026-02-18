# Phase 12: Settings Control Primitives and Validation UX

## Overview
Improve how settings controls behave and communicate constraints.

## Prerequisites
- Phase 11

## Tasks

### Task 1: Normalize Per-Type Editors
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Align behavior for boolean, integer, double, string, choice, color, keybinding, and list controls.
**Acceptance Criteria:**
- Every control type follows consistent label/help/reset layout

### Task 2: Add Inline Validation and Recovery
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/ValidationUtils.h`
**Description:** Show validation errors inline with corrective hints.
**Acceptance Criteria:**
- Invalid state is explicit and blocks apply where required

### Task 3: Improve Staged Change Visibility
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Make pending changes obvious with per-setting indicators and summary footer.
**Acceptance Criteria:**
- Users can review all pending edits before apply

### Task 4: Add Per-Setting History and Revert
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.cpp`
**Description:** Provide local undo/revert for each setting.
**Acceptance Criteria:**
- Per-setting revert does not require full reset

### Task 5: Add Settings Control Tests
**Files:** `tests/unit/test_settings_control_types.cpp`
**Description:** Validate value parsing, clamping, and staged-apply behavior.
**Acceptance Criteria:**
- Control-specific validation rules covered by tests

## Testing Requirements
- Invalid input scenarios for all supported control types
- Import/export round-trip verification

## Phase Completion Criteria
- Settings controls are clear, resilient, and easy to correct
