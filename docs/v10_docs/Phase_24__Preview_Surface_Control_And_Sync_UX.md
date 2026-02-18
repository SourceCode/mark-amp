# Phase 24: Preview Surface Control and Sync UX

## Overview
Improve preview-side controls and editor-preview sync visibility.

## Prerequisites
- Existing `src/ui/PreviewPanel.cpp`, `src/ui/SplitView.cpp`
- Phase 22

## Tasks

### Task 1: Add Preview Control Header
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** Add compact controls for sync mode, refresh, pin, and open in side.
**Acceptance Criteria:**
- Preview control header is keyboard and mouse accessible

### Task 2: Clarify Sync Mode States
**Files:** `src/ui/SplitView.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Display exact sync mode and active anchor.
**Acceptance Criteria:**
- Users can see and change sync mode quickly

### Task 3: Add Scroll Anchor and Lock Controls
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** Let users lock preview or follow cursor/heading.
**Acceptance Criteria:**
- Lock and follow states are explicit and persistent

### Task 4: Improve Interactive Block Controls
**Files:** `src/rendering/HtmlRenderer.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** Improve code block and diagram action buttons.
**Acceptance Criteria:**
- Block controls have clear hover/focus states and tooltips

### Task 5: Add Preview UX Tests
**Files:** `tests/unit/test_preview_controls.cpp`
**Description:** Validate sync toggles, lock behavior, and navigation.
**Acceptance Criteria:**
- Preview control behavior covered by automated tests

## Testing Requirements
- Editor-preview sync under rapid edits
- Preview controls at narrow split widths

## Phase Completion Criteria
- Preview controls communicate state and reduce sync confusion
