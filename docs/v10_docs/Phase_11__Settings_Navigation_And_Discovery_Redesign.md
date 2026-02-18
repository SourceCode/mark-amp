# Phase 11: Settings Navigation and Discovery Redesign

## Overview
Make settings faster to navigate through improved hierarchy, deep links, and filter tools.

## Prerequisites
- Phase 05
- Existing `src/ui/SettingsPanel.cpp`

## Tasks

### Task 1: Rebuild Category Navigation as Hierarchical Tree
**Files:** `src/ui/SettingsPanel.cpp`, `src/ui/SettingsPanel.h`
**Description:** Move from flat category list to grouped tree navigation with collapse/expand state.
**Acceptance Criteria:**
- Hierarchical categories reflect `SettingsCatalog` groups and subgroups

### Task 2: Improve Search with Match Highlight and Scope
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/SettingsCatalog.cpp`
**Description:** Add highlighted matches and scope-aware filtering.
**Acceptance Criteria:**
- Search results show match snippets and highlighted terms

### Task 3: Add Deep-Link Breadcrumb in Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/ui/SettingsDialog.cpp`
**Description:** Show current path and allow quick backtracking.
**Acceptance Criteria:**
- Breadcrumb updates with category and selected setting

### Task 4: Add Restart-Required and Experimental Badges
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Render metadata badges consistently for special settings.
**Acceptance Criteria:**
- Badges are visible, theme-aware, and tooltip-documented

### Task 5: Add Settings Navigation Tests
**Files:** `tests/unit/test_settings_navigation.cpp`
**Description:** Cover category tree, search filtering, and deep-link behavior.
**Acceptance Criteria:**
- Navigation and search regressions are test-detected

## Testing Requirements
- Keyboard traversal in settings tree and form
- Deep-link open from command palette and events

## Phase Completion Criteria
- Settings are discoverable and fast to navigate
