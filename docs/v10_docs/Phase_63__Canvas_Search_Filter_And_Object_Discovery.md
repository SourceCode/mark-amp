# Phase 63: Canvas Search Filter and Object Discovery

## Overview
Add robust object discovery controls for large boards through search, filter, and quick-jump features.

## Prerequisites
- `src/canvas/CanvasSearch.*`
- `src/canvas/SearchBar.*`

## Tasks

### Task 1: Implement board search bar UX
**Files:** `src/canvas/SearchBar.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Add persistent search input with keyboard toggle.
**Acceptance Criteria:**
- Search bar opens quickly and keeps query history

### Task 2: Add filter chips by object type/tag
**Files:** `src/canvas/CanvasSearch.cpp`, `src/canvas/TagManager.cpp`
**Description:** Filter results by object category and tags.
**Acceptance Criteria:**
- Filters combine predictably and can be cleared in one action

### Task 3: Add result list with jump and highlight
**Files:** `src/canvas/SearchBar.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Selecting a result centers viewport and highlights object.
**Acceptance Criteria:**
- Jump behavior is smooth and reversible via navigation back

### Task 4: Add saved search views
**Files:** `src/core/Config.cpp`, `src/canvas/CanvasSearch.cpp`
**Description:** Save reusable filter/search combinations.
**Acceptance Criteria:**
- Saved searches persist per board/workspace

### Task 5: Add canvas search tests
**Files:** `tests/unit/test_canvas_search.cpp`
**Description:** Validate indexing, filtering, and jump operations.
**Acceptance Criteria:**
- Search returns deterministic results for stable board data

## Testing Requirements
- Large board with thousands of objects
- Jump-to-result latency checks

## Phase Completion Criteria
- Canvas discovery controls scale with board size
