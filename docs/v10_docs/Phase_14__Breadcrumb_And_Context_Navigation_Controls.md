# Phase 14: Breadcrumb and Context Navigation Controls

## Overview
Improve breadcrumb controls to support faster orientation and navigation in large documents/workspaces.

## Prerequisites
- Phase 05
- Existing `src/ui/BreadcrumbBar.cpp`

## Tasks

### Task 1: Add Segment Menus and Jump Navigation
**Files:** `src/ui/BreadcrumbBar.cpp`
**Description:** Clicking a breadcrumb segment opens sibling navigation options.
**Acceptance Criteria:**
- Segment menus are keyboard accessible and searchable

### Task 2: Add Overflow Handling
**Files:** `src/ui/BreadcrumbBar.cpp`
**Description:** Add start/middle truncation and overflow menu for long paths.
**Acceptance Criteria:**
- Breadcrumb remains usable at narrow widths

### Task 3: Improve Visual State Language
**Files:** `src/ui/BreadcrumbBar.cpp`
**Description:** Distinguish current segment, hover segment, and menu-open segment.
**Acceptance Criteria:**
- Segment states are visually clear in all themes

### Task 4: Integrate with Navigation History
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/NavigationService.cpp`
**Description:** Breadcrumb jumps add proper history entries.
**Acceptance Criteria:**
- Back/forward behaves correctly after breadcrumb navigation

### Task 5: Add Breadcrumb Tests
**Files:** `tests/unit/test_breadcrumb_navigation.cpp`
**Description:** Cover truncation, segment hits, and jump behavior.
**Acceptance Criteria:**
- Path rendering and segment action correctness verified

## Testing Requirements
- Deeply nested path scenarios
- Keyboard-only breadcrumb navigation

## Phase Completion Criteria
- Breadcrumb controls improve context awareness and movement
