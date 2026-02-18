# Phase 34: Error Prevention and Recovery Control UX

## Overview
Improve controls so users make fewer mistakes and recover quickly when mistakes happen.

## Prerequisites
- Phase 19
- Phase 33

## Tasks

### Task 1: Add Preflight Checks for Risky Actions
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/FileTreeCtrl.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** Validate constraints before executing risky operations.
**Acceptance Criteria:**
- Preflight checks block invalid operations with clear rationale

### Task 2: Add Inline Recovery Actions
**Files:** `src/ui/NotificationManager.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Expose retry/open-log/open-settings actions where applicable.
**Acceptance Criteria:**
- Errors include relevant recovery controls

### Task 3: Add Undo Coverage Expansion
**Files:** `src/ui/*.cpp`, `src/core/*`
**Description:** Expand reversible operations and unify undo messaging.
**Acceptance Criteria:**
- More user-facing actions are undoable with consistent UX

### Task 4: Improve Validation Copy and Severity Language
**Files:** `src/ui/*.cpp`
**Description:** Replace vague error text with actionable copy.
**Acceptance Criteria:**
- Messages include what happened, why, and what to do next

### Task 5: Add Recovery UX Tests
**Files:** `tests/unit/test_control_error_recovery.cpp`
**Description:** Validate preflight, failure, and recovery paths.
**Acceptance Criteria:**
- Critical recovery flows regression-tested

## Testing Requirements
- Injected failure path simulations
- Undo/redo consistency checks

## Phase Completion Criteria
- Control UX reduces error rate and improves recovery confidence
