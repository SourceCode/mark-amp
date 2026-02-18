# Phase 64: Minimap and Board Navigator Improvements

## Overview
Upgrade minimap and navigator controls for orientation, faster travel, and clearer viewport awareness.

## Prerequisites
- `src/canvas/MinimapPanel.*`
- `src/canvas/BoardNavigator.*`

## Tasks

### Task 1: Improve viewport frame interaction
**Files:** `src/canvas/MinimapPanel.cpp`, `src/ui/CanvasWorkspacePanel.cpp`
**Description:** Dragging minimap viewport should pan main canvas precisely.
**Acceptance Criteria:**
- Minimap drag remains stable at all zoom levels

### Task 2: Add minimap object density rendering modes
**Files:** `src/canvas/MinimapPanel.cpp`
**Description:** Support simplified and detailed minimap modes.
**Acceptance Criteria:**
- Mode switch preserves performance on large boards

### Task 3: Add frame/section markers in minimap
**Files:** `src/canvas/MinimapPanel.cpp`, `src/canvas/FrameObject.cpp`
**Description:** Surface named regions for quick jumps.
**Acceptance Criteria:**
- Clicking marker centers corresponding region

### Task 4: Add navigator hotkeys
**Files:** `src/ui/CanvasPanel.cpp`, `src/ui/MainFrame.cpp`
**Description:** Keyboard commands for jump to fit, center selection, and previous location.
**Acceptance Criteria:**
- Navigation commands are exposed in command palette

### Task 5: Add minimap tests
**Files:** `tests/unit/test_canvas_minimap.cpp`
**Description:** Validate viewport transform synchronization.
**Acceptance Criteria:**
- Minimap-main viewport mapping remains correct

## Testing Requirements
- Extreme board sizes and zoom levels
- Navigator command latency checks

## Phase Completion Criteria
- Users can orient and traverse large boards quickly
