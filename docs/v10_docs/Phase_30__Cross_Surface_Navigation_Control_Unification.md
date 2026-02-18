# Phase 30: Cross-Surface Navigation Control Unification

## Overview
Unify navigation controls across editor, canvas, graph, and panels so movement patterns are consistent.

## Prerequisites
- Phase 14
- Phase 26

## Tasks

### Task 1: Create Unified Navigation Command Set
**Files:** `src/ui/MainFrame.cpp`, `src/ui/NavigationService.cpp`
**Description:** Standardize back, forward, recent locations, and quick switch controls.
**Acceptance Criteria:**
- Same navigation commands work across all surfaces

### Task 2: Add Global "Jump To" Control
**Files:** `src/ui/CommandPalette.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Add a single jump control for files, headings, symbols, and nodes.
**Acceptance Criteria:**
- Jump UI supports source type filters and MRU ranking

### Task 3: Add Navigation Breadcrumb Sync
**Files:** `src/ui/BreadcrumbBar.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Keep breadcrumb, status location, and history stack synchronized.
**Acceptance Criteria:**
- Navigation context never disagrees between controls

### Task 4: Improve Surface Transition Feedback
**Files:** `src/ui/SurfaceTransitionCoordinator.cpp`
**Description:** Show lightweight transition cues when jumping between surfaces.
**Acceptance Criteria:**
- Users retain context during cross-surface jumps

### Task 5: Add Navigation Integration Tests
**Files:** `tests/unit/test_cross_surface_navigation_controls.cpp`
**Description:** Cover history integrity and cross-surface jump behavior.
**Acceptance Criteria:**
- Navigation regressions are detected in CI

## Testing Requirements
- Rapid cross-surface jump sessions
- Back/forward stack correctness checks

## Phase Completion Criteria
- Navigation controls feel unified across the product
