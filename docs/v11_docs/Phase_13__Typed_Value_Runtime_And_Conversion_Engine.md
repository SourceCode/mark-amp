# Phase 13: Typed Value Runtime And Conversion Engine

## Goal
Create value runtime with typed storage and explicit conversion rules.

## Prerequisites
- Phase 12 complete
- Node editor feature flag available in config and FeatureRegistry

## Blender Reference Files (Full Paths)
- [B1] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/blenkernel/intern/node_runtime.cc
- [B2] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/blenkernel/intern/node_tree_update.cc
- [B3] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/blenkernel/intern/node_tree_interface.cc
- [B4] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/nodes/NOD_node_declaration.hh
- [B5] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/nodes/NOD_socket.hh

## MarkAmp Target Files/Areas
- /Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h
- /Users/ryanrentfro/code/markamp/src/core/WhenClause.h
- /Users/ryanrentfro/code/markamp/src/core/ExtensionSandbox.h
- /Users/ryanrentfro/code/markamp/src/core/AsyncPipeline.h
- /Users/ryanrentfro/code/markamp/src/core/CoalescingTask.h

## Tasks (Agent-Optimized)
### Task 01
**Action:** Specify runtime contracts and lifecycle hooks for this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 02
**Action:** Model deterministic execution ordering and explicit dependency edges.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 03
**Action:** Implement invalidation propagation for changed node outputs.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 04
**Action:** Add lazy evaluation paths to avoid unnecessary computation.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 05
**Action:** Add cancellation tokens and cooperative abort points in long operations.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 06
**Action:** Implement resource accounting (time, memory, work units) for each execution step.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 07
**Action:** Add typed value conversion adapters and explicit failure modes.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 08
**Action:** Add domain capability flags and runtime compatibility checks.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 09
**Action:** Define plugin/runtime registration APIs in core service layer.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 10
**Action:** Wire context-key integration for runtime-driven UI enablement.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 11
**Action:** Add diagnostics emission for runtime errors and degraded paths.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 12
**Action:** Add deterministic snapshot/replay harness for execution debugging.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 13
**Action:** Add cache key strategy and invalidation tests for memoization.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 14
**Action:** Add concurrency guards for background execution and UI updates.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 15
**Action:** Implement fallback behavior when domain runtime is unavailable.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 16
**Action:** Add unit tests for scheduler, dependency graph, and value conversion.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 17
**Action:** Add integration tests for one end-to-end graph execution scenario.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 18
**Action:** Benchmark execution throughput and record budget targets.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 19
**Action:** Document runtime extension points for custom domain implementers.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 20
**Action:** Publish a phase checklist with acceptance criteria and known follow-ups.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

## Completion Gates
- 100% tasks executed or explicitly deferred with rationale
- Unit + integration tests for this phase are green
- No unresolved critical diagnostics
- Follow-up dependencies for the next phase are documented
