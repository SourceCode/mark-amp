# Phase 08: Notebook Lifecycle, Runtime, And Output Integrity

## Phase Intent

Close the remaining notebook release-path gaps so notebooks behave like trustworthy first-class artifacts.

## Release-Ready Exit Criteria

- notebook create/open/save/restore flows are shell-owned and dependable,
- kernel lifecycle and execution are not merely simulated on the release path,
- notebook outputs persist, restore, and clear correctly.

## Task Count

3

## Task P08-T01

- Phase ID: P08
- Task ID: P08-T01
- Task Title: Finish notebook shell ownership across create, open, activate, save, and restore
- Priority: P0
- Category: Notebook Hardening
- Objective: make notebook lifecycle operations flow through one shell-authoritative path.
- Why This Matters Now: notebook support cannot remain model-rich but shell-thin if it is on the release path.
- Release Gap Statement: notebook lifecycle code exists, but shell host behavior is still too light and event-driven to guarantee trust.
- User / Product Impact: notebooks will feel secondary and unreliable unless they match document-grade lifecycle behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Prior Plan References: `v20 Phase 03`; `v19 Phase 07`; `v24 Phase 08`
- Scope: notebook create/open/save/save as/restore/close/dirty handling.
- Out of Scope: multi-user notebook collaboration.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: notebook lifecycle; shell host; artifact registry; active surface
- Current Behavior: shell host is still a thin state/notification layer and lifecycle adoption is incomplete.
- Intended Release-Ready Behavior: notebooks behave like real shell artifacts with correct activation, dirty-state, save, and reopen semantics.
- Missing Pieces: shell-host adoption; restore path; lifecycle smoke coverage.
- Technical Approach: make notebook shell host a thin shell adapter over canonical artifact/lifecycle services rather than a separate behavior owner.
- Implementation Steps:
1. Map notebook shell events to artifact lifecycle operations.
2. Ensure dirty/save/restore behavior mirrors text artifact expectations where appropriate.
3. Add notebook lifecycle smoke paths.
- Validation Steps:
1. Create, edit, save, close, reopen, and restore notebooks.
2. Verify shell activation, prompts, and active artifact continuity.
- Acceptance Criteria: notebook lifecycle is consistent with the artifact spine and no longer depends on thin notification-only ownership.
- Dependencies: P02-T03; P03-T03
- Parallelization Notes: runtime and output work can proceed once lifecycle contracts stabilize.
- Risks / Failure Modes: notebook host state drift; wrong active artifact; save prompt mismatch.
- Observability / Diagnostics Notes: log notebook artifact IDs, lifecycle transitions, and host-state changes.
- Rollback / Safety Notes: preserve legacy notebook loading compatibility while changing shell ownership.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`

## Task P08-T02

- Phase ID: P08
- Task ID: P08-T02
- Task Title: Replace simulated kernel lifecycle with release-path execution readiness
- Priority: P0
- Category: Notebook Hardening
- Objective: eliminate purely simulated kernel behavior for supported release-path notebook execution.
- Why This Matters Now: notebook execution cannot be advertised as product-grade if runtime lifecycle is synthetic.
- Release Gap Statement: `KernelManager` still explicitly simulates lifecycle and restart.
- User / Product Impact: execution, restart, and interrupt trust are fundamental notebook expectations.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_kernel_manager.cpp`
- Prior Plan References: `v23 Phase 06`; `v24 Phase 08`
- Scope: kernel discovery, start, stop, restart, interrupt, execution request/result plumbing for supported release-path kernels.
- Out of Scope: broad multi-language execution support beyond the release matrix.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/KernelManager.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`
- Related Features / Systems / Components: kernels; notebook execution pipeline; session state
- Current Behavior: kernel lifecycle is explicitly stubbed and restart is simulated.
- Intended Release-Ready Behavior: supported kernels have real lifecycle transitions, error handling, and observable execution status.
- Missing Pieces: real process/session plumbing; failure handling; capability gating.
- Technical Approach: define a supported release kernel matrix and implement real lifecycle semantics for that subset, gating unsupported kernels clearly.
- Implementation Steps:
1. Choose the release-path kernel support scope.
2. Implement real lifecycle and error paths for supported kernels.
3. Surface unsupported kernels as gated rather than implied.
- Validation Steps:
1. Start, execute, interrupt, restart, and stop supported kernels.
2. Verify notebook UI reflects real state and failures.
- Acceptance Criteria: release-path notebook execution is no longer backed by purely simulated kernel lifecycle.
- Dependencies: P08-T01
- Parallelization Notes: output persistence can proceed after execution result contracts are stable.
- Risks / Failure Modes: process leaks; bad error recovery; unsupported kernel confusion.
- Observability / Diagnostics Notes: record kernel state transitions and execution failures.
- Rollback / Safety Notes: gate unsupported kernels explicitly rather than leaving silent simulation.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp`

## Task P08-T03

- Phase ID: P08
- Task ID: P08-T03
- Task Title: Persist, restore, and validate notebook outputs and execution state
- Priority: P1
- Category: Notebook Hardening
- Objective: make notebook output behavior consistent across save, reopen, and restore flows.
- Why This Matters Now: execution is only useful if results persist and recover correctly.
- Release Gap Statement: notebook shell and output rendering infrastructure still outpace complete end-to-end output trust.
- User / Product Impact: users need confidence that executed work is durable and accurately restored.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.cpp`
- Prior Plan References: `v22 Phase 09`; `v24 Phase 08`
- Scope: persisted outputs, clear output, errored output, restore after restart, output rendering state.
- Out of Scope: rich collaborative notebook output syncing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Features / Systems / Components: notebook serializer; output renderer; session restore
- Current Behavior: notebook model and renderer pieces exist, but durable output and restore trust still need explicit closure.
- Intended Release-Ready Behavior: executed outputs survive save, reopen, and restart where intended, and clear-output behavior is consistent.
- Missing Pieces: serializer coverage; state restoration; output smoke tests.
- Technical Approach: define output persistence rules and bind serializer, renderer, and session restore to them.
- Implementation Steps:
1. Define which output metadata is persisted.
2. Wire serializer and restore path to that contract.
3. Add save/reopen/restart notebook output tests.
- Validation Steps:
1. Execute notebook cells and persist results.
2. Reopen and restart to confirm output integrity.
- Acceptance Criteria: notebook outputs and execution state behave predictably through save and restore cycles.
- Dependencies: P08-T01; P08-T02
- Parallelization Notes: UI polish for notebook output can follow after this trust work.
- Risks / Failure Modes: stale output metadata; mismatched renderer state; file bloat.
- Observability / Diagnostics Notes: log output serialization and restore failures with notebook ID and cell ID.
- Rollback / Safety Notes: prefer explicit output-invalid markers over silent data loss.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
