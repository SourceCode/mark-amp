# Phase 26: Graph and Backlink Control UX

## Overview
Improve graph and backlink controls for filtering, navigation, and readability.

## Prerequisites
- Existing `src/ui/GraphPanel.cpp`, `src/ui/GraphViewPanel.cpp`, `src/ui/BacklinksPanel.cpp`

## Tasks

### Task 1: Build Unified Graph Control Bar
**Files:** `src/ui/GraphFilterBar.cpp`, `src/ui/GraphViewPanel.cpp`
**Description:** Centralize filter, depth, layout, and focus controls.
**Acceptance Criteria:**
- Graph controls live in one predictable location

### Task 2: Add Query Presets and Saved Filters
**Files:** `src/ui/GraphFilterBar.cpp`, `src/core/Config.cpp`
**Description:** Persist useful filter presets.
**Acceptance Criteria:**
- Users can save, rename, and apply filter presets

### Task 3: Improve Node Selection Controls
**Files:** `src/ui/GraphViewPanel.cpp`, `src/ui/NavigationService.cpp`
**Description:** Add keyboard node traversal and action menu.
**Acceptance Criteria:**
- Node actions are accessible without pointer-only gestures

### Task 4: Add Backlink Panel Action Improvements
**Files:** `src/ui/BacklinksPanel.cpp`
**Description:** Improve sort, group, and jump controls.
**Acceptance Criteria:**
- Backlink list supports clear grouping and quick open actions

### Task 5: Add Graph UX Tests
**Files:** `tests/unit/test_graph_controls.cpp`
**Description:** Cover filtering, selection, and navigation behavior.
**Acceptance Criteria:**
- Core graph control interactions regression-tested

## Testing Requirements
- Large graph interaction performance pass
- Keyboard navigation verification

## Phase Completion Criteria
- Graph controls are understandable and high-signal
