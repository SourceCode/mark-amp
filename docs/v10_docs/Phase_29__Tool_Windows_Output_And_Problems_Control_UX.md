# Phase 29: Tool Windows, Output, and Problems Control UX

## Overview
Improve bottom and side tool-window controls for output, diagnostics, and extension views.

## Prerequisites
- Existing `src/ui/OutputPanel.cpp`, `src/ui/ProblemsPanel.h`, `src/ui/ToolWindowHost.cpp`

## Tasks

### Task 1: Standardize Tool Window Tab Controls
**Files:** `src/ui/ToolWindowHost.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Align tab behaviors for close, pin, reorder, and split.
**Acceptance Criteria:**
- Tool-window tab controls mirror main tab conventions where applicable

### Task 2: Upgrade Output Panel Controls
**Files:** `src/ui/OutputPanel.cpp`
**Description:** Add channel filter, wrap toggle, copy, clear, and follow-tail controls.
**Acceptance Criteria:**
- Output controls support rapid log triage

### Task 3: Improve Problems Panel Controls
**Files:** `src/ui/ProblemsPanel.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Add severity filters, grouping, and quick-fix entry actions.
**Acceptance Criteria:**
- Problem navigation is keyboard-first and context-preserving

### Task 4: Add Panel Lifecycle Controls
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** Add show/hide/restore defaults and panel layout reset.
**Acceptance Criteria:**
- Users can recover from cluttered panel layouts quickly

### Task 5: Add Tool Window Tests
**Files:** `tests/unit/test_tool_window_controls.cpp`
**Description:** Cover panel routing, tab behavior, and filtering controls.
**Acceptance Criteria:**
- Tool window control flows have regression coverage

## Testing Requirements
- High-volume log rendering scenarios
- Diagnostic navigation from problems to editor

## Phase Completion Criteria
- Tool windows are manageable and efficient under load
