# Phase 21: Search Panel Control Surface Completion

## Overview
Complete the Search panel as a first-class control surface with clear query controls, result navigation, and replace workflows.

## Prerequisites
- Phase 10
- Existing `src/ui/SearchPanel.h`

## Tasks

### Task 1: Implement Search Panel UI
**Files:** `src/ui/SearchPanel.cpp`, `src/ui/SearchPanel.h`
**Description:** Build query input, toggles, scope selector, and results list.
**Acceptance Criteria:**
- Search panel supports regex, case, whole-word, and include/exclude paths

### Task 2: Add Result Grouping and Collapse Controls
**Files:** `src/ui/SearchPanel.cpp`
**Description:** Group matches by file with expand/collapse rows.
**Acceptance Criteria:**
- Group controls are keyboard and mouse accessible

### Task 3: Add Replace Preview Controls
**Files:** `src/ui/SearchPanel.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Provide staged replace preview and per-result include/exclude toggles.
**Acceptance Criteria:**
- Replace operation shows exact planned changes before commit

### Task 4: Add Search Session History Controls
**Files:** `src/ui/SearchPanel.cpp`, `src/core/Config.cpp`
**Description:** Persist recent queries and filters.
**Acceptance Criteria:**
- Search history is recallable and clearable

### Task 5: Add Search Panel Tests
**Files:** `tests/unit/test_search_panel.cpp`
**Description:** Validate query parsing, grouping, navigation, and replace-preview logic.
**Acceptance Criteria:**
- Search and replace control behavior covered by tests

## Testing Requirements
- Large workspace search with filters
- Keyboard-only result navigation

## Phase Completion Criteria
- Search panel is complete, reliable, and efficient to operate
