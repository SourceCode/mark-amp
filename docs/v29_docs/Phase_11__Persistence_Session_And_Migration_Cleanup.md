# Phase 11: Persistence Session And Migration Cleanup

## Task Count

- 2 tasks

## Task V29-P11-T01

- Phase ID: V29-P11
- Task ID: V29-P11-T01
- Task Title: Remove retired-feature session restore, recent-item, reopen, and layout persistence
- Priority: Critical
- Category: Persistence / Migration Cleanup
- Objective: Ensure session restore, recent items, panel restore, and saved layout data no longer reopen retired artifacts or views.
- Why This Matters Now: Removal without restore cleanup turns old user state into crash or dead-end territory.
- Removal Gap Statement: Earlier plans explicitly expanded session restore and panel persistence for these surfaces, and current shell code still persists sidebar modes and workbench state.
- User / Product Impact: Preserves startup stability for existing users upgrading into the smaller product.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2445`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp:2476`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v18_docs/Phase_03__Shell_Layout_Panels_And_Session_Continuity.md`; `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_05__Persistence_Autosave_Restore_And_Recovery.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_03__Persistence_Autosave_Restore_Recovery_And_Session_Trust.md`
- Scope: Reopen logic, recent items, active sidebar mode, workbench mode, persisted tab sets, and saved panel/layout state.
- Out of Scope: Deleting old serialized file formats themselves unless they are actively loaded at startup.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`
- Related Features / Systems / Components: Startup, restore, recent items, panel persistence, active artifact.
- Current Behavior / Presence: The app still knows how to think in terms of notebook/canvas artifact kinds and sidebar modes during restore.
- Intended Post-Removal Behavior: Old removed-feature state is ignored or downgraded safely and users land in retained views.
- Removal / Simplification Direction: Prefer graceful drop with optional notification over silent crash or preserving dead restore branches.
- Technical Approach: Add compatibility readers or downgrade rules, then delete active runtime reopen paths.
- Implementation Steps: Identify persisted keys and restore call sites; map retired modes to retained defaults; skip retired artifacts; clear dead recent items; update tests.
- Validation Steps: Upgrade from old persisted state fixtures containing canvas, notebook, flashcard, and task references; confirm startup succeeds and lands in retained views.
- Acceptance Criteria: Old state containing retired features no longer breaks startup or restore.
- Dependencies: Phases 04 through 10.
- Parallelization Notes: Must coordinate with feature deletion because it references removed types during the compatibility window.
- Risks / Failure Modes: Numeric enums or string mode IDs may be deserialized before compatibility mapping is applied.
- Migration / Compatibility Notes: This is the phase that defines the only allowed temporary compatibility readers.
- UX / Layout Cleanup Notes: User notification should be calm and explicit if removed surfaces are dropped during restore.
- Cleanup / Consolidation Notes: Remove persisted keys entirely once compatibility windows are satisfied.
- Rollback / Safety Notes: Test with real upgrade fixtures before deleting legacy readers.
- References / Context: Earlier session-continuity work expands the exact blast radius this phase must now retire.
- Example Scenarios: A saved layout that previously reopened Canvas now opens Explorer and text tabs instead.

## Task V29-P11-T02

- Phase ID: V29-P11
- Task ID: V29-P11-T02
- Task Title: Retire removed-feature storage, serializers, and compatibility readers after safe downgrade behavior exists
- Priority: High
- Category: Persistence / Migration Cleanup
- Objective: Remove or quarantine persisted storage paths for boards, notebooks, flashcards, FSRS review logs, and task caches once upgrade safety is covered.
- Why This Matters Now: Deleting UI and services without cleaning storage readers leaves hidden compatibility burden behind indefinitely.
- Removal Gap Statement: Flashcards persist under workspace storage and notebook/canvas lifecycles still express saved/opened artifact semantics.
- User / Product Impact: Prevents stale data from being re-read and simplifies workspace storage ownership.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_18__Migration_Retirement_Placeholder_Removal_And_Dead_Path_Cleanup.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_18__Migration_Stub_And_Dead_Path_Retirement.md`
- Scope: Workspace storage directories, serializer/deserializer entry points, compatibility readers, and migration notices.
- Out of Scope: Generic retained document persistence.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/fsrs/FlashcardStore.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp`
- Related Features / Systems / Components: Storage, serialization, workspace data, migrations.
- Current Behavior / Presence: Removed features still have dedicated persisted formats and storage models.
- Intended Post-Removal Behavior: Storage readers either safely ignore old data or are fully removed after compatibility handling.
- Removal / Simplification Direction: Minimize long-lived compatibility burden and shrink storage ownership to retained features.
- Technical Approach: Add a short-lived compatibility layer where needed, then delete serializers and storage readers from production code.
- Implementation Steps: Audit stored file and directory formats; decide ignore-versus-notify behavior; remove serializers from build; add tests proving graceful handling.
- Validation Steps: Start from fixtures containing old board/notebook/flashcard/task state and confirm stable downgrade.
- Acceptance Criteria: No active production serializer or storage reader remains for retired features beyond explicit compatibility handling.
- Dependencies: V29-P11-T01.
- Parallelization Notes: Can proceed alongside Phase 15 build pruning.
- Risks / Failure Modes: Removing readers too early can make upgrade tests fail or break startup.
- Migration / Compatibility Notes: Keep the compatibility window narrow and documented.
- UX / Layout Cleanup Notes: If notifying users, use one concise migration message rather than repeated warnings.
- Cleanup / Consolidation Notes: Remove retired storage paths from docs and support scripts.
- Rollback / Safety Notes: Only delete compatibility readers after tests cover downgrade fixtures.
- References / Context: This phase converts prior migration-retirement plans into explicit removal-safe downgrade work.
- Example Scenarios: Old flashcard review-log data is ignored without breaking workspace open.
