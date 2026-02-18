# Phase 76: Canvas Validation and Data Integrity Guards

## Overview
Add defensive integrity checks for Canvas operations to prevent silent corruption and invalid object states.

## Prerequisites
- Phase 68
- `src/canvas/CanvasObject.*`

## Tasks

### Task 1: Add object invariant validators
**Files:** `src/canvas/CanvasObject.cpp`, `src/canvas/Board.cpp`
**Description:** Validate bounds, transforms, and required fields.
**Acceptance Criteria:**
- Invalid object states are detected early with structured errors

### Task 2: Add command precondition checks
**Files:** `src/canvas/CanvasCommands.cpp`
**Description:** Verify inputs before mutate operations.
**Acceptance Criteria:**
- Invalid commands fail safely without partial mutation

### Task 3: Add board consistency scan
**Files:** `src/canvas/Board.cpp`, `src/canvas/BoardSerializer.cpp`
**Description:** Run lightweight consistency scan on load/save.
**Acceptance Criteria:**
- Scan reports orphan references and invalid links

### Task 4: Add integrity report channel
**Files:** `src/ui/OutputPanel.cpp`, `src/core/DiagnosticsService.*`
**Description:** Surface integrity issues in diagnostics output.
**Acceptance Criteria:**
- Reports include location and remediation hints

### Task 5: Add integrity tests
**Files:** `tests/unit/test_canvas_integrity.cpp`
**Description:** Validate guard behavior under malformed inputs.
**Acceptance Criteria:**
- Guard rails prevent known corruption classes

## Testing Requirements
- Malformed board fixture coverage
- Mutation sequence stress tests

## Phase Completion Criteria
- Canvas data integrity is actively enforced
