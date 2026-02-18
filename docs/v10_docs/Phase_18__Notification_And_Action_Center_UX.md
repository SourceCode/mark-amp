# Phase 18: Notification and Action Center UX

## Overview
Upgrade notifications from transient toasts into a controllable, reviewable feedback system.

## Prerequisites
- Existing `src/ui/NotificationManager.cpp`
- Phase 13

## Tasks

### Task 1: Improve Toast Interaction Model
**Files:** `src/ui/NotificationManager.cpp`
**Description:** Add pause-on-hover, keyboard dismiss, and stacked focus traversal.
**Acceptance Criteria:**
- Toast controls are fully interactive and dismissible by keyboard

### Task 2: Add Notification Center Panel
**Files:** `src/ui/NotificationCenterPanel.h`, `src/ui/NotificationCenterPanel.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Persist recent notifications with filtering by level/source.
**Acceptance Criteria:**
- Users can review and clear historical notifications

### Task 3: Standardize Action Buttons and Severity Styling
**Files:** `src/ui/NotificationManager.cpp`
**Description:** Align action layout and severity color semantics.
**Acceptance Criteria:**
- Action buttons are clear and visually consistent

### Task 4: Add Quiet Hours and Noise Controls
**Files:** `src/core/SettingsCatalog.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Provide settings to suppress non-critical toasts during focused work.
**Acceptance Criteria:**
- Notification routing respects user noise preferences

### Task 5: Add Notification Tests
**Files:** `tests/unit/test_notification_manager.cpp`
**Description:** Cover timing, dismissal, actions, and persistence logic.
**Acceptance Criteria:**
- Toast and center behavior validated by tests

## Testing Requirements
- High-volume notification stress scenario
- Keyboard navigation for notification center

## Phase Completion Criteria
- Notifications are actionable, manageable, and non-intrusive
