# Phase 68: Board Serialization Recovery and Compatibility

## Overview
Harden board persistence fundamentals for reliability, forward compatibility, and corruption recovery.

## Prerequisites
- `src/canvas/BoardSerializer.*`
- `src/canvas/Board.*`

## Tasks

### Task 1: Add schema versioning for board JSON
**Files:** `src/canvas/BoardSerializer.cpp`, `src/canvas/BoardSerializer.h`
**Description:** Introduce explicit format version and migration hooks.
**Acceptance Criteria:**
- Older saved boards load through migration path

### Task 2: Add partial-load recovery mode
**Files:** `src/canvas/BoardSerializer.cpp`
**Description:** Load recoverable objects even when some entries are invalid.
**Acceptance Criteria:**
- Corrupt segments are reported without losing full board load

### Task 3: Add atomic save and backup snapshots
**Files:** `src/canvas/BoardSerializer.cpp`, `src/core/FileSystem.*`
**Description:** Prevent data loss on interrupted writes.
**Acceptance Criteria:**
- Save path is atomic with rollback on failure

### Task 4: Add validation diagnostics panel hooks
**Files:** `src/ui/OutputPanel.cpp`, `src/canvas/BoardSerializer.cpp`
**Description:** Surface serialization warnings in a user-visible channel.
**Acceptance Criteria:**
- Load/save warnings include actionable context

### Task 5: Add serializer tests
**Files:** `tests/unit/test_canvas_board_serializer.cpp`
**Description:** Validate migrations, recovery, and round-trip correctness.
**Acceptance Criteria:**
- Board integrity remains stable across versions

## Testing Requirements
- Fuzzed/corrupted board input tests
- Large board save/load timing checks

## Phase Completion Criteria
- Board persistence is resilient and version-safe
