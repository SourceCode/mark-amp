# Phase 37: Personalization and Layout Preset Controls

## Overview
Give users stronger control over workspace layout, control visibility, and saved presets.

## Prerequisites
- Phase 15
- Phase 36

## Tasks

### Task 1: Add Layout Preset Manager
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/WorkspaceLayout.cpp`, `src/core/Config.cpp`
**Description:** Save/load named layout presets for panes, sidebars, and tool windows.
**Acceptance Criteria:**
- Users can create, rename, apply, and delete presets

### Task 2: Add Control Visibility Preferences
**Files:** `src/ui/Toolbar.cpp`, `src/ui/StatusBarPanel.cpp`, `src/ui/ActivityBar.cpp`
**Description:** Allow toggling optional control clusters.
**Acceptance Criteria:**
- Hidden controls remain discoverable via settings and reset options

### Task 3: Add Profile-Aware Control Sets
**Files:** `src/core/SettingsCatalog.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Tie control presets to user profiles (writing, coding, canvas).
**Acceptance Criteria:**
- Profile switch updates control visibility/layout predictably

### Task 4: Add Quick Reset and Recovery Controls
**Files:** `src/ui/MainFrame.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Provide one-click restore-default-layout action.
**Acceptance Criteria:**
- Users can recover from broken/cluttered layouts immediately

### Task 5: Add Personalization Tests
**Files:** `tests/unit/test_layout_personalization_controls.cpp`
**Description:** Test preset persistence and profile switching behavior.
**Acceptance Criteria:**
- Preset load/save behavior regression-protected

## Testing Requirements
- Preset portability between sessions
- Conflict resolution when controls are unavailable

## Phase Completion Criteria
- Users can tailor control surfaces without instability
