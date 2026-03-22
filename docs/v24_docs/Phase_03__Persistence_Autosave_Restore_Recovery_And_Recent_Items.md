# Phase 03 - Persistence, Autosave, Restore, Recovery, And Recent Items

## Phase Goal

Make persistence trustworthy across artifact families so users can create, edit, save, crash, restart, and continue work without losing state or coherence.

## Measurable Outcome

- Save, autosave, restore, and recovery flows share one persistence contract.
- Recent-items continuity is artifact-aware and stable.
- Restart and crash scenarios are testable and deterministic.

### Task P03-T01

- Phase ID: `P03`
- Task ID: `P03-T01`
- Task Title: Replace `LayoutManager`-local save ownership with persistence orchestrators
- Priority: `P0`
- Category: `Persistence / Restore`
- Objective: Move save responsibility out of direct editor-facing UI code.
- Why This Matters Now: Direct `LayoutManager` file I/O blocks consistent artifact-family persistence and recovery.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still saves active content directly to disk.
- User / Product Impact: Save semantics differ by surface and are fragile around unsaved artifacts and save-as flows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`
- Prior Plan References: `v19` Phase 03, `v20` Phase 05, `v23` Phase 04
- Scope: Save, save-as, save-all ownership for text, notebook, canvas artifacts
- Out of Scope: Export/share flows
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Features / Systems / Components: Persistence orchestration, shell save commands
- Current Behavior: UI surfaces still decide too much of persistence behavior.
- Intended Completed Behavior: Save requests delegate to artifact-aware persistence orchestrators.
- Missing Pieces: Shared persistence dispatch, result propagation, error/reporting contract
- Technical Approach: Introduce artifact-aware save command handling outside widget surfaces.
- Implementation Steps: Refactor save entry points; route to orchestrators; return structured results; update UI on completion.
- Validation Steps: Save text, notebook, and canvas artifacts from all shell entry points.
- Acceptance Criteria: No primary save path performs raw save logic solely in `LayoutManager`.
- Dependencies: `P02-T01`, `P02-T02`
- Parallelization Notes: Blocks robust autosave and recovery work.
- Risks / Failure Modes: Save result propagation can regress if UI assumes synchronous local writes.
- Cleanup / Migration Notes: Remove duplicated save logic from shell widgets after migration.
- Observability / Diagnostics Notes: Emit save request source, artifact type, duration, and outcome.
- Rollback / Safety Notes: Keep old behavior as fallback only during staged migration.
- References / Context: Persistence orchestrator and artifact lifecycle services
- Example scenarios where useful: `Save All` should persist files, notebooks, and canvases through one orchestrated pipeline with per-artifact results.

### Task P03-T02

- Phase ID: `P03`
- Task ID: `P03-T02`
- Task Title: Unify autosave semantics across artifact families
- Priority: `P0`
- Category: `Persistence / Restore`
- Objective: Ensure autosave obeys one timing, dirty-state, and failure policy.
- Why This Matters Now: Autosave cannot be trusted while save ownership and artifact identity are fragmented.
- Execution Gap Statement: Different surfaces maintain different modified-state assumptions, and canvas already has its own autosave model.
- User / Product Impact: Users risk silent data loss or inconsistent autosave behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/AutosaveModel.h`
- Prior Plan References: `v20` Phase 05, `v23` Phase 03-04
- Scope: Autosave interval, dirty gating, pending-save indicators, error handling
- Out of Scope: Cloud sync scheduling
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookSessionManager.h`
- Related Features / Systems / Components: Dirty state, autosave, pending-save status
- Current Behavior: Autosave logic is surface-specific and incomplete.
- Intended Completed Behavior: One autosave contract applies to all artifact families with family-specific serializers only.
- Missing Pieces: Shared autosave coordinator, family adapters, failure policy
- Technical Approach: Promote autosave to an artifact-lifecycle service with pluggable family persistence backends.
- Implementation Steps: Define autosave coordinator; attach family adapters; unify pending/failure states; update status UI.
- Validation Steps: Edit text, notebook, and canvas artifacts; verify autosave timing and failure surfacing.
- Acceptance Criteria: Autosave timing and state reporting are consistent across artifact families.
- Dependencies: `P03-T01`
- Parallelization Notes: Can proceed alongside `P03-T03`.
- Risks / Failure Modes: Background save overlap can corrupt state without proper serialization and locking rules.
- Cleanup / Migration Notes: Retire canvas-only or shell-only autosave assumptions once shared coordinator exists.
- Observability / Diagnostics Notes: Track pending, saving, failed, retried, and completed autosave states.
- Rollback / Safety Notes: Allow autosave to be disabled globally during rollout if instability is detected.
- References / Context: Canvas autosave model, shell timer usage
- Example scenarios where useful: An unsaved notebook autosaves session state without being forced into an invalid path-based write.

### Task P03-T03

- Phase ID: `P03`
- Task ID: `P03-T03`
- Task Title: Complete session restore and crash recovery on top of artifact records
- Priority: `P0`
- Category: `Persistence / Restore`
- Objective: Restore open artifacts, active contexts, and dirty unsaved work reliably after restart or crash.
- Why This Matters Now: Product trust depends on restart continuity.
- Execution Gap Statement: Current session restore is still partial and too path/snapshot-oriented.
- User / Product Impact: Restarting the app can lose intent, layout, or unsaved work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v19` Phase 03-04, `v20` Phase 05, `v23` Phase 03-04
- Scope: Open artifacts, active editor group, canvas/notebook restoration, dirty buffers, restore prompts
- Out of Scope: Cross-machine sync restore
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`, `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`
- Related Features / Systems / Components: Session state, restore pipeline, crash recovery
- Current Behavior: Restore behavior is not yet authoritative across all artifact families.
- Intended Completed Behavior: Restart reconstructs shell state using artifact records and persisted session metadata.
- Missing Pieces: Session schema updates, restore ordering, dirty-buffer replay, recovery conflict handling
- Technical Approach: Restore shell from artifact session manifest rather than ad hoc path reopening.
- Implementation Steps: Extend session manifest; persist artifact records; restore groups and active contexts; handle missing resources gracefully.
- Validation Steps: Perform cold restart and simulated crash tests with mixed open artifacts.
- Acceptance Criteria: Open state, active state, dirty state, and visible shell context survive restart within defined restore rules.
- Dependencies: `P02-T01`, `P03-T01`
- Parallelization Notes: Requires artifact identity and save ownership to stabilize first.
- Risks / Failure Modes: Restore order bugs can reopen wrong surfaces or lose active-context propagation.
- Cleanup / Migration Notes: Retire restore code that assumes only file-path-based editor tabs.
- Observability / Diagnostics Notes: Emit restore manifest parse, replay, fallback, and failure logs.
- Rollback / Safety Notes: Preserve old restore manifest compatibility during migration.
- References / Context: Workspace session restore and open orchestrator
- Example scenarios where useful: After a crash, an unsaved canvas reopens with local unsaved state and the correct workbench mode active.

### Task P03-T04

- Phase ID: `P03`
- Task ID: `P03-T04`
- Task Title: Finish recent-items and reopen flows for all artifact families
- Priority: `P1`
- Category: `Artifact Lifecycle`
- Objective: Ensure recent artifacts and reopen flows represent real project continuity.
- Why This Matters Now: Recent-items lists are part of trust and recovery, not just convenience.
- Execution Gap Statement: Recent/open history is still too file-centric and weak around notebooks, canvases, and unsaved work.
- User / Product Impact: Users cannot reliably return to prior work or understand what the app considers restorable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `v20` Phase 05, `v22` Phase 16
- Scope: MRU items, reopen closed, startup/recent items, artifact family labeling
- Out of Scope: Cloud-based recent-history syncing
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`
- Related Features / Systems / Components: Navigation history, startup panel, recent items
- Current Behavior: Recent/open flows are incomplete and inconsistent by artifact type.
- Intended Completed Behavior: Recent items and reopen logic treat files, notebooks, and canvases as first-class items with accurate metadata.
- Missing Pieces: Unified MRU data model and reopen handling
- Technical Approach: Attach recent-history recording to artifact lifecycle events and restore services.
- Implementation Steps: Define MRU model; update startup surface; support reopen-closed artifact actions; persist metadata.
- Validation Steps: Open/close/save/reopen all artifact types and verify MRU continuity.
- Acceptance Criteria: Recent items and reopen behavior are accurate, stable, and artifact-aware.
- Dependencies: `P03-T03`
- Parallelization Notes: Can trail restore work but should land before startup polish and release gates.
- Risks / Failure Modes: Mixed artifact/path IDs can create duplicate or stale recent entries.
- Cleanup / Migration Notes: Remove file-only MRU assumptions.
- Observability / Diagnostics Notes: Track MRU insert/update/remove events.
- Rollback / Safety Notes: Maintain migration logic for prior MRU storage formats.
- References / Context: Startup and navigation history surfaces
- Example scenarios where useful: Reopen Closed reopens the last closed notebook, not only text files.

### Task P03-T05

- Phase ID: `P03`
- Task ID: `P03-T05`
- Task Title: Add explicit recovery UX and failure-state rules for persistence paths
- Priority: `P1`
- Category: `Diagnostics / Recovery`
- Objective: Make save/autosave/restore failures visible, actionable, and recoverable.
- Why This Matters Now: Trustworthy persistence requires more than success paths.
- Execution Gap Statement: Current persistence flows lack enough structured recovery behavior and user-facing failure handling.
- User / Product Impact: Failures can become silent data-loss events or confusing no-ops.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Prior Plan References: `v19` feedback harnesses, `v20` diagnostics/validation, `v23` diagnostics/recovery
- Scope: Save errors, autosave failures, restore conflicts, retry/reveal/save-as fallback actions
- Out of Scope: Remote sync conflict UX
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/Events.h`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Features / Systems / Components: Notifications, status bar, recovery dialogs
- Current Behavior: Failure handling is uneven and often widget-local.
- Intended Completed Behavior: Every persistence failure has structured state, user feedback, and a recovery path.
- Missing Pieces: Structured failure events, retry policies, UI contracts
- Technical Approach: Emit persistence-result objects and bind them to notifications, status indicators, and recovery actions.
- Implementation Steps: Define result/failure model; publish events; attach UI affordances; add retry/reveal/save-as actions.
- Validation Steps: Force write failures, missing path conflicts, and corrupt restore manifests.
- Acceptance Criteria: Persistence failures are visible, actionable, and logged with reproducible diagnostics.
- Dependencies: `P03-T01` through `P03-T03`
- Parallelization Notes: Can progress with validation-harness work.
- Risks / Failure Modes: Excessive error noise can degrade UX if severity and deduplication are poor.
- Cleanup / Migration Notes: Remove silent or log-only failure branches.
- Observability / Diagnostics Notes: Persist failure reasons and recovery actions invoked.
- Rollback / Safety Notes: Keep destructive recovery actions explicitly confirmed by the user.
- References / Context: Notification and status surfaces
- Example scenarios where useful: Disk-full on autosave surfaces a retry action and preserves the dirty artifact in memory.
