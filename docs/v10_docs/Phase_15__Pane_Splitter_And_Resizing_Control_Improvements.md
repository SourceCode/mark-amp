# Phase 15: Pane Splitter and Resizing Control Improvements

## Overview
Make pane resizing controls more precise and less frustrating across editor and workbench layouts.

## Prerequisites
- Phase 03
- Existing `src/ui/SplitterBar.cpp`

## Tasks

### Task 1: Improve Splitter Hit Zones and Visual Cues
**Files:** `src/ui/SplitterBar.cpp`, `src/ui/SplitterBar.h`
**Description:** Increase discoverability with clearer hover affordances and optional grip marker.
**Acceptance Criteria:**
- Splitter hover and drag state are easy to identify

### Task 2: Add Snap Points and Double-Click Reset
**Files:** `src/ui/SplitterBar.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Support snap widths and reset-to-default interaction.
**Acceptance Criteria:**
- Users can quickly return to default layout widths

### Task 3: Add Keyboard Resize Controls
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/MainFrame.cpp`
**Description:** Provide keyboard commands for incremental pane resize.
**Acceptance Criteria:**
- Keyboard resizing works with visible feedback

### Task 4: Add Resize Constraints and Persistence
**Files:** `src/ui/LayoutManager.cpp`, `src/core/Config.cpp`
**Description:** Enforce min/max constraints and persist per layout mode.
**Acceptance Criteria:**
- Pane sizes restore correctly per mode/session

### Task 5: Add Splitter Tests
**Files:** `tests/unit/test_splitter_behavior.cpp`
**Description:** Test drag, clamp, snap, and persistence logic.
**Acceptance Criteria:**
- Resize invariants protected by tests

## Testing Requirements
- Mouse and keyboard resizing paths
- Multi-monitor DPI checks

## Phase Completion Criteria
- Pane sizing controls feel stable and predictable
