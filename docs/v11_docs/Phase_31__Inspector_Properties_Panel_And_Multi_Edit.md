# Phase 31: Inspector Properties Panel And Multi Edit

## Goal
Implement contextual inspector with mixed-state multi-selection editing.

## Prerequisites
- Phase 30 complete
- Node editor feature flag available in config and FeatureRegistry

## Blender Reference Files (Full Paths)
- [B1] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_draw.cc
- [B2] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/drawnode.cc
- [B3] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_select.cc
- [B4] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_add.cc
- [B5] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_group.cc

## MarkAmp Target Files/Areas
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp
- /Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp

## Tasks (Agent-Optimized)
### Task 01
**Action:** Define UI interaction contract (mouse, keyboard, focus, context menu) for this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 02
**Action:** Implement panel/view model state separate from rendering primitives.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 03
**Action:** Mirror key Blender UX patterns from reference file #1 with MarkAmp styling.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 04
**Action:** Mirror key Blender UX patterns from reference file #2 with MarkAmp styling.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 05
**Action:** Mirror key Blender UX patterns from reference file #3 with MarkAmp styling.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 06
**Action:** Implement precise hit-testing and hover state transitions.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 07
**Action:** Implement drag interactions with cancel and rollback behavior.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 08
**Action:** Add keyboard navigation and command IDs for all critical actions.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 09
**Action:** Add context menu entries and route them through command handlers.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 10
**Action:** Integrate with command palette entries and fuzzy search keywords.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 11
**Action:** Apply theme tokens for all control states (default/hover/active/focus/disabled).
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 12
**Action:** Add status bar signals to expose current mode and operation state.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 13
**Action:** Add inspector/property bindings with staged edits and apply/cancel.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 14
**Action:** Add clipboard operations and duplicate semantics for this UI surface.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 15
**Action:** Add visual affordances for warnings, invalid links, and blocked actions.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 16
**Action:** Add unit tests for selection, hit-test, and command routing behavior.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 17
**Action:** Add UI integration tests for high-value workflows and regressions.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 18
**Action:** Add accessibility checks: tab order, focus ring, labels, and narration hooks.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 19
**Action:** Profile paint/update paths and remove avoidable redraw hotspots.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 20
**Action:** Document interaction playbook so AI agents can implement follow-up features consistently.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

## Completion Gates
- 100% tasks executed or explicitly deferred with rationale
- Unit + integration tests for this phase are green
- No unresolved critical diagnostics
- Follow-up dependencies for the next phase are documented
