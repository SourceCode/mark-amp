# Phase 66: Clipboard Duplicate and Template Workflows

## Overview
Improve object reuse fundamentals through better copy/paste, duplicate, and template operations.

## Prerequisites
- `src/canvas/CanvasCommands.*`
- `src/canvas/BoardTemplate.*`

## Tasks

### Task 1: Harden clipboard object graph copy
**Files:** `src/canvas/CanvasCommands.cpp`, `src/canvas/BoardSerializer.cpp`
**Description:** Preserve internal references during copy/paste.
**Acceptance Criteria:**
- Pasted groups/connectors remain internally consistent

### Task 2: Add duplicate-in-place and offset-duplicate
**Files:** `src/canvas/CanvasCommands.cpp`
**Description:** Support both duplicate behaviors with shortcuts.
**Acceptance Criteria:**
- Duplicate mode choice is configurable

### Task 3: Add style-only and content-only paste
**Files:** `src/canvas/CanvasCommands.cpp`, `src/ui/CanvasPanel.cpp`
**Description:** Enable partial paste variants for productivity.
**Acceptance Criteria:**
- Paste variants are available in command palette/context menu

### Task 4: Add reusable board templates
**Files:** `src/canvas/BoardTemplate.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Save selections/boards as reusable templates.
**Acceptance Criteria:**
- Template apply flow is quick and reversible

### Task 5: Add clipboard/template tests
**Files:** `tests/unit/test_canvas_clipboard_templates.cpp`
**Description:** Validate object integrity after duplicate/paste/template apply.
**Acceptance Criteria:**
- Reuse workflows survive complex object mixes

## Testing Requirements
- Cross-board copy/paste scenarios
- Template version compatibility checks

## Phase Completion Criteria
- Reuse workflows are robust and efficient
