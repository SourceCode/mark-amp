# Phase 78: Canvas Onboarding Help and Fundamentals Tutorials

## Overview
Improve learning controls so users can quickly master core Canvas operations.

## Prerequisites
- `src/ui/WalkthroughPanel.cpp`
- Phase 61

## Tasks

### Task 1: Add canvas-first walkthrough flow
**Files:** `src/ui/WalkthroughPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Create guided fundamentals: draw, shape, text, style, align, export.
**Acceptance Criteria:**
- Walkthrough is completable in under 5 minutes

### Task 2: Add contextual help tooltips
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Provide progressive hints on first use of major tools.
**Acceptance Criteria:**
- Hints are dismissible and do not block workflow

### Task 3: Add starter board templates
**Files:** `src/canvas/BoardTemplate.cpp`, `src/ui/StartupPanel.cpp`
**Description:** Include flowchart, brainstorm, roadmap, and diagram starters.
**Acceptance Criteria:**
- Templates launch with clear editable placeholders

### Task 4: Add quick reference sheet
**Files:** `src/ui/ShortcutOverlay.cpp`, `docs/user_guide.md`
**Description:** Publish canvas-specific shortcuts and gestures.
**Acceptance Criteria:**
- Reference matches actual shortcut bindings

### Task 5: Add onboarding tests
**Files:** `tests/unit/test_canvas_onboarding.cpp`
**Description:** Validate walkthrough steps and template availability.
**Acceptance Criteria:**
- Onboarding flows remain stable across updates

## Testing Requirements
- First-run and returning-user onboarding scenarios
- No-regression check for dismissible hint preferences

## Phase Completion Criteria
- Users can learn core canvas workflows rapidly
