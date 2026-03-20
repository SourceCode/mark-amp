# Phase 05: Persistence Autosave Restore And Recovery

## Outcome

Make save, autosave, reopen, session restore, crash recovery, and dirty-state handling durable enough to support a real IDE across all artifact types.

## Improvement Count

204 atomic improvements across 6 execution tasks.

### P05-T01

- Phase ID: P05
- Task ID: P05-T01
- Task Title: Replace optimistic save orchestration with authoritative persistence outcomes
- Priority: P0
- Category: State / Persistence
- Atomic Improvements Covered: 34
- Objective: Ensure save only marks artifacts clean after verified write success.
- Why This Matters Now: Current save orchestration is too optimistic for a professional IDE.
- Problem Statement: `DocumentPersistenceOrchestrator::save` publishes a save event and immediately marks clean.
- User Impact: Users can be told work is saved when it is not.
- Scope: Verified save results, write errors, save transactions, artifact-kind-aware persistence callbacks.
- Out of Scope: Cloud sync semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Persistence orchestrator, layout manager, board save, notebook save.
- Current Behavior: Dirty state can be cleared before durable persistence is confirmed.
- Intended Behavior: Clean state follows confirmed save success only.
- Technical Approach: Introduce save result objects, per-artifact save executors, and transactional dirty-state updates.
- Implementation Steps: Add save callbacks; propagate detailed failures; delay clean-state transitions; unify save notifications.
- Validation Steps: Force write failures and verify artifacts remain dirty with actionable errors.
- Acceptance Criteria: No artifact is marked clean after a failed save.
- Dependencies: Phases 01 to 04.
- Risks / Failure Modes: Existing event consumers may assume fire-and-forget save.
- UX Notes: Failure messages should offer retry, save as, and reveal details.
- Styling / Highlighting Notes where relevant: Dirty/clean indicators need synchronized visual states.
- Observability / Diagnostics Notes: Record save result, duration, bytes written, and failure type.
- Rollback / Safety Notes: Preserve legacy save path behind debug-only shims during migration.
- References / Context: `DocumentPersistenceOrchestrator.cpp` marks clean immediately after publishing save events.
- Example scenarios where useful: Disk permission error leaves the file dirty and offers Save As instead of silently succeeding.

### P05-T02

- Phase ID: P05
- Task ID: P05-T02
- Task Title: Add atomic writes, backup snapshots, and recovery journals
- Priority: P0
- Category: Artifact Lifecycle
- Atomic Improvements Covered: 34
- Objective: Protect users from partial writes and crash-time data loss.
- Why This Matters Now: Core creation work is meaningless if persistence can still lose data.
- Problem Statement: File and board saves write directly to final destinations.
- User Impact: Crashes or disk issues can corrupt artifacts.
- Scope: Temp-file saves, fsync strategy where supported, backup rotation, recovery metadata, recovery prompts.
- Out of Scope: Full distributed sync conflict resolution.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Related Systems / Components: Persistence, recovery, autosave, crash handling.
- Current Behavior: Direct writes and limited recovery policy.
- Intended Behavior: All primary artifact writes are crash-resilient and recoverable.
- Technical Approach: Add per-artifact safe-write helpers and local recovery journals.
- Implementation Steps: Implement atomic write utility; connect file/notebook/board saves; add journal cleanup and restore prompts.
- Validation Steps: Simulate interrupted writes and verify recovery options.
- Acceptance Criteria: Recovery data exists for unsaved or partially written primary artifacts.
- Dependencies: P05-T01.
- Risks / Failure Modes: Performance regressions if journaling is too heavy.
- UX Notes: Recovery prompts should be calm, specific, and easy to resolve.
- Styling / Highlighting Notes where relevant: Recovery surfaces need polished warning and restore states.
- Observability / Diagnostics Notes: Track journal creation, replay, and cleanup outcomes.
- Rollback / Safety Notes: Keep journals isolated per workspace and per artifact.
- References / Context: `LayoutManager::SaveFile` and `CanvasWorkbench::save_board` currently use direct `ofstream`.
- Example scenarios where useful: App crashes during board save, then reopens with a recoverable board draft.

### P05-T03

- Phase ID: P05
- Task ID: P05-T03
- Task Title: Rebuild session restore around artifact records instead of loose path snapshots
- Priority: P0
- Category: State / Persistence
- Atomic Improvements Covered: 34
- Objective: Make reopen and restore trustworthy for files, notebooks, and canvases.
- Why This Matters Now: Current restore is too memory-oriented and too path-only.
- Problem Statement: `WorkspaceSessionRestore` stores snapshots but does not fully apply them and lacks artifact-kind awareness.
- User Impact: Workspaces do not reopen into a dependable working state.
- Scope: Artifact-aware snapshots, active artifact restore, split-group restore, unsaved draft restore, surface restore.
- Out of Scope: Cross-device session sync.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Session restore, shell, persistence, split view.
- Current Behavior: Restore is only partially serialized and partially applied.
- Intended Behavior: Restored workspaces reopen the same artifact set with correct active/focus state.
- Technical Approach: Store artifact records, layout state, and unsaved-draft references in durable snapshots.
- Implementation Steps: Extend snapshot schema; restore through shell artifact service; restore active groups and active artifact last; handle missing-path recovery.
- Validation Steps: Restart after mixed file/notebook/canvas session and verify same working set returns.
- Acceptance Criteria: Restore opens the right artifact types in the right surfaces with the right active context.
- Dependencies: Phases 01 to 04.
- Risks / Failure Modes: Legacy snapshots may deserialize incompletely.
- UX Notes: Missing artifacts should show recovery choices, not silent drops.
- Styling / Highlighting Notes where relevant: Restore progress and partial-failure states need polished visual treatment.
- Observability / Diagnostics Notes: Save and replay restore traces to per-workspace logs.
- Rollback / Safety Notes: Maintain legacy snapshot reader while new format rolls out.
- References / Context: `WorkspaceOpenOrchestrator.cpp` restores by publishing `ActiveFileChangedEvent` after `latest_snapshot`.
- Example scenarios where useful: User reopens a workspace and returns to one file, one notebook, and one canvas in the previous arrangement.

### P05-T04

- Phase ID: P05
- Task ID: P05-T04
- Task Title: Make autosave and dirty-state policy coherent across artifact types
- Priority: P1
- Category: Artifact Lifecycle
- Atomic Improvements Covered: 34
- Objective: Unify when and how files, notebooks, and boards become dirty and autosave.
- Why This Matters Now: Mixed dirty semantics undermine product trust.
- Problem Statement: Dirty tracking is fragmented by subsystem and not obviously policy-driven.
- User Impact: Users may not know what is saved, what is pending, or what will be restored.
- Scope: Dirty-state transitions, autosave timers/triggers, idle-save rules, save-on-focus-loss policy, user settings.
- Out of Scope: Collaborative merge policies.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Dirty tracking, autosave, settings, status bar.
- Current Behavior: Dirty semantics vary by subsystem and can clear too early.
- Intended Behavior: Dirty and autosave behavior follow one clear product policy with artifact-specific nuance.
- Technical Approach: Add a shared persistence policy layer with artifact adapters.
- Implementation Steps: Define dirty transitions; wire autosave triggers; expose user settings; unify status and notifications.
- Validation Steps: Edit file, notebook, and board under autosave-on and autosave-off policies.
- Acceptance Criteria: Dirty and autosave states are consistent and predictable.
- Dependencies: P05-T01 through P05-T03.
- Risks / Failure Modes: Overeager autosave hurts performance or surprises users.
- UX Notes: Status indicators should clearly show saving, saved, unsaved, and error states.
- Styling / Highlighting Notes where relevant: Dirty markers, autosave spinners, and error badges need consistent design.
- Observability / Diagnostics Notes: Log autosave reasons, skips, durations, and failures.
- Rollback / Safety Notes: Keep autosave opt-out and per-workspace overrides.
- References / Context: `LayoutManager.cpp` uses timer-based autosave while notebooks and boards have separate semantics.
- Example scenarios where useful: User edits a board and a file, pauses, and sees both autosave with clear status.

### P05-T05

- Phase ID: P05
- Task ID: P05-T05
- Task Title: Complete close, quit, and workspace-switch protection for unsaved work
- Priority: P1
- Category: Core Creation Flow
- Atomic Improvements Covered: 34
- Objective: Ensure unsaved work is never discarded by accident during close flows.
- Why This Matters Now: Broken creation and thin persistence make close flows especially dangerous.
- Problem Statement: Close and quit prompts are unlikely to be artifact-kind-aware or restore-aware enough.
- User Impact: Users can lose files, notebook edits, or board changes during normal navigation.
- Scope: Close tab, close workspace, quit app, switch workspace, bulk-save prompts, cancel-safe cleanup.
- Out of Scope: Remote collaboration conflict prompts.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`
- Related Systems / Components: Main frame, shell, persistence, session restore.
- Current Behavior: Dirty prompts likely remain file-centric and inconsistent.
- Intended Behavior: Close flows enumerate all dirty artifacts and offer save, discard, cancel, and recoverable fallback behavior.
- Technical Approach: Build artifact-kind-aware close coordinator and aggregate unsaved-work prompts.
- Implementation Steps: Inventory dirty artifacts; sequence prompt UX; save selected items; preserve drafts on cancel; wire workspace-switch checks.
- Validation Steps: Attempt quit with dirty file, dirty notebook, and dirty canvas simultaneously.
- Acceptance Criteria: Users can safely back out of destructive close flows without losing state.
- Dependencies: P05-T01 through P05-T04.
- Risks / Failure Modes: Prompt storms or hidden auto-discard paths.
- UX Notes: Batch prompts should be fast, readable, and keyboard-friendly.
- Styling / Highlighting Notes where relevant: Unsaved-work dialogs need strong hierarchy and clear destructive emphasis.
- Observability / Diagnostics Notes: Log unsaved-work prompt outcomes and cancellation patterns.
- Rollback / Safety Notes: Preserve emergency recovery snapshots on forced shutdown.
- References / Context: Current lifecycle fragmentation makes close protection unreliable by default.
- Example scenarios where useful: User closes a workspace with unsaved notebook and board edits and chooses to save notebook, discard board, and cancel file close.

### P05-T06

- Phase ID: P05
- Task ID: P05-T06
- Task Title: Add persistence, recovery, and restore regression suites
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 34
- Objective: Turn save and restore integrity into measurable release criteria.
- Why This Matters Now: Core workflow reliability cannot remain anecdotal.
- Problem Statement: Persistence failures often surface only after restart or crash.
- User Impact: Data loss erodes trust faster than almost any other defect.
- Scope: Save failure tests, autosave tests, restore tests, recovery journal tests, close-protection tests, partial-missing-artifact tests.
- Out of Scope: Distributed sync tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/run_tests.sh`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Systems / Components: CI, persistence, restore, shell.
- Current Behavior: Persistence and restore do not appear fully release-gated.
- Intended Behavior: CI proves that the product protects user work across normal and failure paths.
- Technical Approach: Add artifact-lifecycle test fixtures plus crash/recovery smoke scripts.
- Implementation Steps: Create persistence matrix; add crash-simulated tests; add cross-artifact restore tests; add missing-path and permission-error cases.
- Validation Steps: Run recovery matrix on local debug builds and CI.
- Acceptance Criteria: Save/recovery regressions block release.
- Dependencies: P05-T01 through P05-T05.
- Risks / Failure Modes: Overmocked tests may not catch real file-system behavior.
- UX Notes: Include validation of user-visible prompts and recovery copy.
- Styling / Highlighting Notes where relevant: Add screenshot checks for save error and recovery dialogs.
- Observability / Diagnostics Notes: Persist test traces with artifact IDs and recovery journal locations.
- Rollback / Safety Notes: Tests must isolate temp directories and cleanup thoroughly.
- References / Context: `WorkspaceSessionRestore.cpp` and save flows are currently too thin to trust without deeper coverage.
- Example scenarios where useful: Simulate crash with unsaved file and saved board, then verify correct recovery choices on next launch.

