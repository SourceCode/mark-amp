# Phase 39: Control Regression Harness and UX QA Automation

## Overview
Build automated QA around control behavior, visuals, and accessibility to prevent UX regressions.

## Prerequisites
- Phase 35
- Phase 38

## Tasks

### Task 1: Create Control Interaction Test Matrix
**Files:** `docs/v10_docs/control_test_matrix.md`
**Description:** Define expected interactions per control type and state.
**Acceptance Criteria:**
- Matrix covers all primary control surfaces

### Task 2: Add Snapshot Tests for Custom-Drawn Controls
**Files:** `tests/unit/test_control_visual_snapshots.cpp`
**Description:** Capture rendered state snapshots for key controls.
**Acceptance Criteria:**
- Baselines exist for default/hover/focus/pressed/disabled states

### Task 3: Add Keyboard Workflow Integration Tests
**Files:** `tests/unit/test_keyboard_control_workflows.cpp`
**Description:** Test end-to-end keyboard operation across surfaces.
**Acceptance Criteria:**
- Major workflows pass with no mouse interaction

### Task 4: Add Accessibility Automation Checks
**Files:** `tests/unit/test_accessibility_controls_audit.cpp`
**Description:** Validate control role/name/state mappings and focus reachability.
**Acceptance Criteria:**
- Accessibility regressions fail CI

### Task 5: Add UX QA Gate in CI
**Files:** `CMakeLists.txt`, `scripts/`
**Description:** Add a dedicated control UX test target and gate.
**Acceptance Criteria:**
- CI blocks merges on critical control UX regressions

## Testing Requirements
- Snapshot baseline updates with review process
- CI runtime budget validation

## Phase Completion Criteria
- Control UX has automated regression protection
