# Phase 19: Dialogs and Confirmation Control Patterns

## Overview
Create consistent dialogs and confirmations for destructive and high-impact actions.

## Prerequisites
- Phase 16
- Phase 18

## Tasks

### Task 1: Build Dialog Pattern Library
**Files:** `src/ui/DialogStyles.h`, `src/ui/DialogStyles.cpp`
**Description:** Define shared dialog layout, button order, and iconography.
**Acceptance Criteria:**
- Dialogs follow one standard across all surfaces

### Task 2: Standardize Confirmation Severity Levels
**Files:** `src/ui/*.cpp`
**Description:** Introduce info/warn/danger confirmation variants.
**Acceptance Criteria:**
- Destructive actions use explicit danger styling and wording

### Task 3: Add "Undo Instead of Confirm" Policy
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/NotificationManager.cpp`
**Description:** For low-risk actions, prefer immediate action with undo toast.
**Acceptance Criteria:**
- Reduced confirmation fatigue for reversible operations

### Task 4: Add Remembered Choice Controls
**Files:** `src/core/Config.cpp`, `src/ui/*.cpp`
**Description:** Allow optional "Do not ask again" for repeatable confirmations.
**Acceptance Criteria:**
- Remembered choices are transparent and resettable in settings

### Task 5: Add Dialog UX Tests
**Files:** `tests/unit/test_dialog_patterns.cpp`
**Description:** Test button ordering, default action, and escape behavior.
**Acceptance Criteria:**
- Dialog interaction conventions enforced by tests

## Testing Requirements
- Destructive-action flow review
- Platform-consistent button order validation

## Phase Completion Criteria
- Confirmation UX is safe and consistent without unnecessary friction
