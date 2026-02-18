# Phase 41: Plugin API Contribution Points And Node Packs

## Goal
Expose node editor contribution points for extensions.

## Prerequisites
- Phase 40 complete
- Node editor feature flag available in config and FeatureRegistry

## Blender Reference Files (Full Paths)
- [B1] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/include/ED_node.hh
- [B2] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_templates.cc
- [B3] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/clipboard.cc
- [B4] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/makesrna/intern/rna_space.cc
- [B5] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/makesdna/DNA_space_types.h

## MarkAmp Target Files/Areas
- /Users/ryanrentfro/code/markamp/src/core/TreeDataProviderRegistry.h
- /Users/ryanrentfro/code/markamp/src/core/WebviewService.h
- /Users/ryanrentfro/code/markamp/src/core/OutputChannelService.h
- /Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp
- /Users/ryanrentfro/code/markamp/src/ui/WebviewHostPanel.cpp

## Tasks (Agent-Optimized)
### Task 01
**Action:** Define extension API additions required for this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 02
**Action:** Add contribution-point schema changes and validation.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 03
**Action:** Expose secure host services needed by node packs and custom widgets.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 04
**Action:** Implement permission checks for all extension-executed actions.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 05
**Action:** Wire feature toggles and context keys for extension-provided nodes.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 06
**Action:** Implement import/export adapters and compatibility checks.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 07
**Action:** Add starter templates/snippets for extension authors.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 08
**Action:** Integrate output channel logging for extension/runtime failures.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 09
**Action:** Add problems diagnostics mapping for extension-originated graph issues.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 10
**Action:** Add settings UI entries for extension/runtime behavior controls.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 11
**Action:** Implement migration logic for older graph/package formats.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 12
**Action:** Add unit tests for extension manifest parsing and registry wiring.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 13
**Action:** Add integration tests installing and loading a sample node pack.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 14
**Action:** Add security tests for permission-denied and sandbox escape attempts.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 15
**Action:** Add packaging checks for signed/distributed node packs.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 16
**Action:** Document public API contracts and stability levels.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 17
**Action:** Publish AI-agent implementation cookbook for adding a new node domain.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 18
**Action:** Define deprecation policy and compatibility windows.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 19
**Action:** Run release candidate checklist and compatibility matrix verification.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 20
**Action:** Produce final rollout and rollback playbook with observable health signals.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

## Completion Gates
- 100% tasks executed or explicitly deferred with rationale
- Unit + integration tests for this phase are green
- No unresolved critical diagnostics
- Follow-up dependencies for the next phase are documented
