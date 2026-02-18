# Phase 36: Responsive DPI and Platform Adaptive Controls

## Overview
Make controls robust across DPI scales, window sizes, and OS conventions.

## Prerequisites
- Phase 03
- Phase 35

## Tasks

### Task 1: Audit Control Scaling Rules
**Files:** `src/ui/*.cpp`, `src/ui/*.h`
**Description:** Ensure sizes, padding, and icon metrics scale correctly.
**Acceptance Criteria:**
- Controls remain legible and usable at 100%-300% scale

### Task 2: Improve Compact Width Behavior
**Files:** `src/ui/Toolbar.cpp`, `src/ui/TabBar.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Add truncation, collapse, and overflow behaviors for narrow layouts.
**Acceptance Criteria:**
- No unusable control overlap at small window widths

### Task 3: Align Platform-Specific Control Conventions
**Files:** `src/ui/MainFrame.cpp`, `src/ui/CustomChrome.cpp`, `src/platform/*`
**Description:** Respect macOS, Windows, and Linux interaction expectations.
**Acceptance Criteria:**
- Control order and shortcut hints follow platform conventions

### Task 4: Add DPI Change Live Handling
**Files:** `src/ui/MainFrame.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Recompute control metrics when DPI changes at runtime.
**Acceptance Criteria:**
- Control layout refreshes cleanly on DPI transitions

### Task 5: Add Responsive Control Tests
**Files:** `tests/unit/test_control_responsiveness.cpp`
**Description:** Validate adaptive layout and metric calculations.
**Acceptance Criteria:**
- DPI and responsive regressions are test-covered

## Testing Requirements
- Multi-monitor mixed-DPI transitions
- Window width sweep with visual checks

## Phase Completion Criteria
- Controls adapt cleanly across platforms and display scales
