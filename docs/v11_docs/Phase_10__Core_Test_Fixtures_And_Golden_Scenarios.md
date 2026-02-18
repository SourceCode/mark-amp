# Phase 10: Core Test Fixtures And Golden Scenarios

## Goal
Establish canonical graph fixtures and golden behavior baselines.

## Prerequisites
- Phase 09 complete
- Node editor feature flag available in config and FeatureRegistry

## Blender Reference Files (Full Paths)
- [B1] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/space_node.cc
- [B2] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_edit.cc
- [B3] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_ops.cc
- [B4] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/makesdna/DNA_node_types.h
- [B5] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/makesrna/intern/rna_nodetree.cc

## MarkAmp Target Files/Areas
- /Users/ryanrentfro/code/markamp/src/core/Events.h
- /Users/ryanrentfro/code/markamp/src/core/PluginContext.h
- /Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.h
- /Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.h

## Tasks (Agent-Optimized)
### Task 01
**Action:** Create a phase ADR documenting decisions, assumptions, and rejected alternatives.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 02
**Action:** Read and summarize behaviors from Blender reference file #1 into a parity checklist.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 03
**Action:** Read and summarize behaviors from Blender reference file #2 into a parity checklist.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 04
**Action:** Read and summarize behaviors from Blender reference file #3 into a parity checklist.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 05
**Action:** Define MarkAmp target interfaces and ownership boundaries for this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 06
**Action:** Create new headers/classes with compile-only stubs and explicit TODO contracts.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 07
**Action:** Add strong type aliases and IDs to avoid raw string/int coupling.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 08
**Action:** Add config flags to gate unfinished behavior behind feature toggles.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 09
**Action:** Define event contracts and payloads in /Users/ryanrentfro/code/markamp/src/core/Events.h.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 10
**Action:** Wire minimal UI entry point in /Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 11
**Action:** Add JSON/YAML schema updates needed for persistence and migration.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 12
**Action:** Implement validation guards for user input and loaded graph data.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 13
**Action:** Add structured logging spans for each major operation in this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 14
**Action:** Add unit tests for new data structures and invariants.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 15
**Action:** Add integration tests that exercise the primary happy path.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 16
**Action:** Add negative tests for malformed state and invalid operations.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 17
**Action:** Document API usage examples for future AI coding agents.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 18
**Action:** Create a rollback plan and explicit risk checklist for this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 19
**Action:** Run full build and targeted tests; capture known gaps.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 20
**Action:** Write phase completion report with pass/fail gates and next dependencies.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

## Completion Gates
- 100% tasks executed or explicitly deferred with rationale
- Unit + integration tests for this phase are green
- No unresolved critical diagnostics
- Follow-up dependencies for the next phase are documented
