# Phase 35: Accessibility and Assistive Control Semantics

## Overview
Ensure all controls provide strong keyboard and assistive technology support.

## Prerequisites
- Phase 05
- Phase 31

## Tasks

### Task 1: Add Accessible Names and Roles to Custom Controls
**Files:** `src/ui/*.cpp`
**Description:** Assign explicit semantics for custom-rendered widgets.
**Acceptance Criteria:**
- Custom controls expose role/name/state semantics

### Task 2: Add Screen Reader State Announcements
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/ActivityBar.cpp`, `src/ui/NotificationManager.cpp`
**Description:** Announce meaningful state changes and action outcomes.
**Acceptance Criteria:**
- Important state transitions are announced succinctly

### Task 3: Improve Contrast and Focus Visibility
**Files:** `src/core/Theme.cpp`, `src/ui/*.cpp`
**Description:** Validate focus and control contrast in all supported themes.
**Acceptance Criteria:**
- Focus visibility remains high across theme variants

### Task 4: Add High-Contrast and Reduced-Complexity Modes
**Files:** `src/core/SettingsCatalog.cpp`, `src/ui/*.cpp`
**Description:** Provide modes for stronger legibility and simpler visuals.
**Acceptance Criteria:**
- Modes are configurable and immediately applied

### Task 5: Add Accessibility Tests
**Files:** `tests/unit/test_control_accessibility_semantics.cpp`
**Description:** Automate role/name/state checks where possible.
**Acceptance Criteria:**
- Accessibility regressions produce test failures

## Testing Requirements
- Keyboard-only operation across all primary surfaces
- Screen-reader smoke tests

## Phase Completion Criteria
- Control system is accessible by default
