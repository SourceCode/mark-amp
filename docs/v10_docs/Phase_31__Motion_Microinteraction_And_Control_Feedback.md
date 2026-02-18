# Phase 31: Motion, Microinteraction, and Control Feedback

## Overview
Refine control motion so interactions feel responsive and intentional without distracting users.

## Prerequisites
- Phase 02
- Existing FX infrastructure in `src/rendering`

## Tasks

### Task 1: Define Motion Tokens for Controls
**Files:** `src/core/Theme.h`, `src/rendering/FxMotionPreset.h`
**Description:** Standardize durations/easing for hover, press, reveal, and dismiss.
**Acceptance Criteria:**
- Motion tokens are reusable across control surfaces

### Task 2: Apply Microinteractions to Key Controls
**Files:** `src/ui/ActivityBar.cpp`, `src/ui/TabBar.cpp`, `src/ui/Toolbar.cpp`, `src/ui/NotificationManager.cpp`
**Description:** Add subtle and consistent transitions.
**Acceptance Criteria:**
- Animations improve feedback without reducing clarity

### Task 3: Add Reduced Motion Compliance
**Files:** `src/core/SettingsCatalog.cpp`, `src/ui/*.cpp`
**Description:** Respect reduced-motion setting globally.
**Acceptance Criteria:**
- Reduced motion disables non-essential animations

### Task 4: Add Interaction Latency Budget Checks
**Files:** `src/rendering/FxSafetyController.cpp`, `tests/unit/test_control_motion_budget.cpp`
**Description:** Ensure motion does not degrade interaction speed.
**Acceptance Criteria:**
- Motion effects remain within frame and latency budgets

### Task 5: Document Motion Usage Rules
**Files:** `docs/v10_docs/motion_control_guidelines.md`
**Description:** Publish do/don't rules for control animation.
**Acceptance Criteria:**
- Guidelines adopted by subsequent control work

## Testing Requirements
- Reduced-motion behavior verification
- Frame timing checks on lower-end hardware profile

## Phase Completion Criteria
- Control motion is consistent, performant, and optional
