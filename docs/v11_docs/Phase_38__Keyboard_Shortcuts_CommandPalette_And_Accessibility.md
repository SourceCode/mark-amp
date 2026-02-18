# Phase 38: Keyboard Shortcuts CommandPalette And Accessibility

## Goal
Implement keyboard-first operation and accessibility semantics.

## Prerequisites
- Phase 37 complete
- Node editor feature flag available in config and FeatureRegistry

## Blender Reference Files (Full Paths)
- [B1] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_relationships.cc
- [B2] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_view.cc
- [B3] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/editors/space_node/node_shader_preview.cc
- [B4] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/gpu/intern/gpu_node_graph.cc
- [B5] /Users/ryanrentfro/code/markamp/tmp/blender/source/blender/nodes/NOD_socket_usage_inference.hh

## MarkAmp Target Files/Areas
- /Users/ryanrentfro/code/markamp/src/rendering/ViewportCache.h
- /Users/ryanrentfro/code/markamp/src/rendering/DirtyRegion.h
- /Users/ryanrentfro/code/markamp/src/rendering/ScrollBlit.h
- /Users/ryanrentfro/code/markamp/src/core/Profiler.h
- /Users/ryanrentfro/code/markamp/tests/unit

## Tasks (Agent-Optimized)
### Task 01
**Action:** Define measurable performance and reliability budgets for this phase.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 02
**Action:** Implement instrumentation counters and trace spans for hot paths.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 03
**Action:** Identify and optimize allocation-heavy code paths.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 04
**Action:** Add partial recompute paths to minimize full graph refreshes.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 05
**Action:** Add viewport culling or virtualization for large graph scenes.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 06
**Action:** Optimize linking/selection algorithms for dense graphs.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 07
**Action:** Add async job boundaries and main-thread handoff safety.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 08
**Action:** Implement bounded queues and backpressure strategy for burst workloads.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 09
**Action:** Add debouncing/throttling for high-frequency UI events.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 10
**Action:** Add error classification and recovery UX for common failures.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 11
**Action:** Integrate diagnostics with output/problems surfaces.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 12
**Action:** Add crash-safe state checkpoints for in-progress graph edits.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 13
**Action:** Add stress fixtures with large node/link counts.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 14
**Action:** Add benchmark tests with strict thresholds in CI.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 15
**Action:** Add flaky-test detection and stabilization for this subsystem.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 16
**Action:** Add fuzz tests for malformed serialized graph inputs.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 17
**Action:** Document known bottlenecks and mitigation strategy.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 18
**Action:** Create runbooks for profiling and triaging regressions.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 19
**Action:** Validate cross-platform behavior and performance parity.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

### Task 20
**Action:** Ship final pass/fail gate report with evidence from tests and benchmarks.
**Deliverable:** Commit-ready code/docs/tests for this task only.
**Validation:** Add or run focused tests and record pass/fail in phase notes.

## Completion Gates
- 100% tasks executed or explicitly deferred with rationale
- Unit + integration tests for this phase are green
- No unresolved critical diagnostics
- Follow-up dependencies for the next phase are documented
