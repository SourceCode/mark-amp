# Phase 03: Layout Density and Spacing System

## Overview
Introduce a density system so controls can scale between comfortable and compact layouts while preserving alignment and readability.

## Prerequisites
- Phase 02

## Tasks

### Task 1: Define Density Profiles
**Files:** `src/core/SettingsCatalog.cpp`, `src/ui/LayoutMetrics.h`
**Description:** Add `comfortable`, `default`, and `compact` density profiles.
**Acceptance Criteria:**
- Profile drives row heights, paddings, icon sizes, and spacing
- Setting persisted and applied at runtime

### Task 2: Centralize Spacing Constants
**Files:** `src/ui/LayoutMetrics.h`, `src/ui/*.h`
**Description:** Replace scattered fixed spacing constants with shared metrics.
**Acceptance Criteria:**
- ActivityBar, TabBar, Toolbar, and StatusBarPanel consume shared metrics

### Task 3: Add Dynamic Row Height Support
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/SettingsPanel.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** Ensure list/tree rows adapt to active density profile.
**Acceptance Criteria:**
- No clipped text or icon overlap in any profile

### Task 4: Add Density Toggle Commands
**Files:** `src/ui/MainFrame.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Add commands to switch density quickly.
**Acceptance Criteria:**
- Commands available in menu and command palette
- Changes apply instantly

### Task 5: Validate Hit Target Minimums
**Files:** `tests/unit/test_density_hit_targets.cpp`
**Description:** Verify minimum hit sizes are preserved for accessibility.
**Acceptance Criteria:**
- Interactive targets remain usable in compact mode

## Testing Requirements
- Density switch smoke tests
- Manual verification at 100%, 150%, 200% scale

## Phase Completion Criteria
- Core control surfaces honor a shared density model
