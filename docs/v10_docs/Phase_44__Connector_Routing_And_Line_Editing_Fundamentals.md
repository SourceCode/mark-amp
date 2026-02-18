# Phase 44: Connector Routing and Line Editing Fundamentals

## Overview
Upgrade connector and line fundamentals for reliable node linking, routing, and label placement.

## Prerequisites
- Phase 43
- `src/canvas/ConnectorData.*`
- `src/canvas/ConnectorRenderer.*`

## Tasks

### Task 1: Add robust anchor attachment model
**Files:** `src/canvas/ConnectorData.cpp`, `src/canvas/SelectionManager.cpp`
**Description:** Keep connectors attached through object movement/resize.
**Acceptance Criteria:**
- Anchors remain stable under transform operations

### Task 2: Implement routing modes
**Files:** `src/canvas/ConnectorRenderer.cpp`, `src/canvas/ConnectorData.h`
**Description:** Support straight, orthogonal, and curved routes.
**Acceptance Criteria:**
- Users can switch routing mode per connector

### Task 3: Add mid-point and bend editing
**Files:** `src/canvas/SelectionRenderer.cpp`, `src/canvas/ConnectorData.cpp`
**Description:** Enable manual route tuning with handle points.
**Acceptance Criteria:**
- Bend handle edits persist and remain undoable

### Task 4: Add connector label controls
**Files:** `src/canvas/ConnectorRenderer.cpp`, `src/canvas/TextBoxRenderer.cpp`
**Description:** Add inline labels with offset and alignment options.
**Acceptance Criteria:**
- Labels remain readable and linked during route changes

### Task 5: Add connector tests
**Files:** `tests/unit/test_canvas_connectors.cpp`
**Description:** Validate anchor persistence and route recomputation.
**Acceptance Criteria:**
- Connector stability verified across move/resize/delete

## Testing Requirements
- Dense diagrams with many crossing connectors
- Route recompute performance checks

## Phase Completion Criteria
- Connector fundamentals are dependable and editable
