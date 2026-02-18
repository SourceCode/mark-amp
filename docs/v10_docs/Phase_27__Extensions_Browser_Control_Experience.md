# Phase 27: Extensions Browser Control Experience

## Overview
Refine extension browsing and lifecycle controls for install, update, enablement, and troubleshooting.

## Prerequisites
- Existing `src/ui/ExtensionsBrowserPanel.cpp`, `src/ui/ExtensionDetailPanel.cpp`, `src/ui/ExtensionCard.cpp`

## Tasks

### Task 1: Redesign Extension List Controls
**Files:** `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** Improve sort/filter/scope controls and empty states.
**Acceptance Criteria:**
- Users can quickly filter installed, enabled, disabled, updates available

### Task 2: Improve Card Actions and Progress Feedback
**Files:** `src/ui/ExtensionCard.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** Clarify install/update/uninstall states and pending progress.
**Acceptance Criteria:**
- Action buttons reflect lifecycle state without ambiguity

### Task 3: Enhance Detail Panel Controls
**Files:** `src/ui/ExtensionDetailPanel.cpp`
**Description:** Add dependency visibility, permissions summary, and changelog controls.
**Acceptance Criteria:**
- High-risk actions show explicit confirmation context

### Task 4: Add Bulk Extension Controls
**Files:** `src/ui/ExtensionsBrowserPanel.cpp`
**Description:** Support select-multiple enable/disable/update where safe.
**Acceptance Criteria:**
- Bulk operations provide clear progress and partial-failure handling

### Task 5: Add Extensions UI Tests
**Files:** `tests/unit/test_extensions_browser_controls.cpp`
**Description:** Test filtering, action transitions, and error display.
**Acceptance Criteria:**
- Extension control workflows covered in tests

## Testing Requirements
- Offline/failed-gallery scenarios
- Enablement scope behavior checks

## Phase Completion Criteria
- Extension management controls are clear and trustworthy
