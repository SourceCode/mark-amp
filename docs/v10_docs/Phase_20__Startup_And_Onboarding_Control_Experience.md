# Phase 20: Startup and Onboarding Control Experience

## Overview
Improve first-use and return-use controls so users can quickly start meaningful work.

## Prerequisites
- Existing `src/ui/StartupPanel.cpp`, `src/ui/WalkthroughPanel.cpp`
- Phase 11

## Tasks

### Task 1: Redesign Startup Quick Actions
**Files:** `src/ui/StartupPanel.cpp`
**Description:** Prioritize high-value actions (new file, open folder, reopen workspace, templates).
**Acceptance Criteria:**
- Startup actions are clear, keyboard-focusable, and low-latency

### Task 2: Add Workspace Intent Presets
**Files:** `src/ui/StartupPanel.cpp`, `src/core/Config.cpp`
**Description:** Offer preset launch modes (writing, research, canvas, review).
**Acceptance Criteria:**
- Presets apply relevant layout/settings defaults

### Task 3: Improve Recent Workspaces Controls
**Files:** `src/ui/StartupPanel.cpp`, `src/core/RecentWorkspaces.cpp`
**Description:** Add pin, remove, sort, and search for recents.
**Acceptance Criteria:**
- Recent list supports direct management actions

### Task 4: Add Guided First-Run Controls
**Files:** `src/ui/WalkthroughPanel.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Provide an optional setup flow for theme, density, shortcuts, and layout.
**Acceptance Criteria:**
- First-run setup can be skipped and revisited later

### Task 5: Add Startup/Onboarding Tests
**Files:** `tests/unit/test_startup_controls.cpp`
**Description:** Validate quick actions, recents behavior, and preset application.
**Acceptance Criteria:**
- Startup control workflows covered by tests

## Testing Requirements
- Fresh profile first-run verification
- Startup keyboard traversal and shortcut checks

## Phase Completion Criteria
- Startup controls guide users to productive state quickly
