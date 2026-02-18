# Phase 46: Canvas Color System and Palette Controls

## Overview
Introduce a cohesive color system for Canvas objects with fast palette workflows and reusable style consistency.

## Prerequisites
- Phase 45
- `src/canvas/CanvasObject.h`

## Tasks

### Task 1: Add canvas color token model
**Files:** `src/canvas/CanvasTypes.h`, `src/canvas/CanvasTypes.cpp`
**Description:** Define semantic colors for fills, strokes, text, accents, and backgrounds.
**Acceptance Criteria:**
- Object styles can reference semantic or literal colors

### Task 2: Implement palette UI and recent colors
**Files:** `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Add quick palette, recent swatches, and custom color input.
**Acceptance Criteria:**
- Color selection is reachable in <=2 interactions

### Task 3: Add color style copy/paste
**Files:** `src/canvas/CanvasCommands.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Support copying style from one object to others.
**Acceptance Criteria:**
- Style transfer includes fill/stroke/text color options

### Task 4: Add contrast warnings for text+fill
**Files:** `src/canvas/TextBoxRenderer.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Warn when text becomes unreadable on selected fill.
**Acceptance Criteria:**
- Warning appears non-blocking with fix suggestions

### Task 5: Add color system tests
**Files:** `tests/unit/test_canvas_color_system.cpp`
**Description:** Validate palette persistence and style application.
**Acceptance Criteria:**
- Color application and serialization are deterministic

## Testing Requirements
- Theme changes vs custom object colors
- Contrast warning edge cases

## Phase Completion Criteria
- Color workflows are fast, consistent, and safe
