# Phase 06: Activity Bar Control Evolution

## Overview
Upgrade the activity bar to a high-fidelity, configurable control surface with improved discoverability and operation.

## Prerequisites
- Phase 02
- Phase 05

## Tasks

### Task 1: Add Full Keyboard and Context Menu Support
**Files:** `src/ui/ActivityBar.cpp`, `src/ui/ActivityBar.h`
**Description:** Add arrow navigation, Enter activation, and context menu actions for item visibility/order.
**Acceptance Criteria:**
- Complete keyboard operation of activity rail
- Right-click menu supports hide/show/reset order

### Task 2: Add Drag-Reorder with Drop Indicators
**Files:** `src/ui/ActivityBar.cpp`
**Description:** Allow users to reorder activity items visually.
**Acceptance Criteria:**
- Drag behavior is smooth and persists order in config

### Task 3: Improve Badge Language
**Files:** `src/ui/ActivityBar.cpp`, `src/core/Events.h`
**Description:** Normalize numeric, dot, and urgency badges.
**Acceptance Criteria:**
- Badge rendering supports count capping and dot-only mode

### Task 4: Add Tooltips with Shortcut Hints
**Files:** `src/ui/ActivityBar.cpp`, `src/ui/MainFrame.cpp`
**Description:** Show surface names and shortcuts in tooltips.
**Acceptance Criteria:**
- Every item tooltip includes action and shortcut if available

### Task 5: Add Accessibility Names
**Files:** `src/ui/ActivityBar.cpp`, `tests/unit/test_activity_bar_accessibility.cpp`
**Description:** Ensure assistive technologies can identify items and badges.
**Acceptance Criteria:**
- Accessible labels and announcements for active item and badge state

## Testing Requirements
- Item reorder persistence test
- Keyboard and screen-reader pass

## Phase Completion Criteria
- Activity bar behaves as a robust, customizable control rail
