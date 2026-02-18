# Phase 32: Drag Drop and Manipulation Affordance Improvements

## Overview
Improve drag/drop and direct manipulation controls across tabs, tree, panes, and canvas.

## Prerequisites
- Phase 07
- Phase 08
- Phase 15

## Tasks

### Task 1: Standardize Drag Initiation Thresholds
**Files:** `src/ui/TabBar.cpp`, `src/ui/FileTreeCtrl.cpp`, `src/ui/SplitterBar.cpp`
**Description:** Use consistent drag thresholds to reduce accidental drags.
**Acceptance Criteria:**
- Drag begins only after clear intent threshold

### Task 2: Add Drop Target Highlight System
**Files:** `src/ui/*.cpp`
**Description:** Show explicit drop zones and insertion markers.
**Acceptance Criteria:**
- Users can predict drop outcome before release

### Task 3: Add Drag Cancel and Escape Behavior
**Files:** `src/ui/*.cpp`
**Description:** Allow canceling drag operations safely.
**Acceptance Criteria:**
- Escape reliably cancels active drag without side effects

### Task 4: Improve Drag-and-Drop Error Messaging
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/NotificationManager.cpp`
**Description:** Provide actionable feedback for invalid drops.
**Acceptance Criteria:**
- Invalid drops produce clear reason and next action

### Task 5: Add Drag/Drop Tests
**Files:** `tests/unit/test_drag_drop_controls.cpp`
**Description:** Cover drag lifecycle and drop validation paths.
**Acceptance Criteria:**
- Drag-drop regressions are automatically detected

## Testing Requirements
- Cross-surface drag scenarios
- Drag behavior with high DPI and scaled windows

## Phase Completion Criteria
- Manipulation controls are accurate and confidence-inspiring
