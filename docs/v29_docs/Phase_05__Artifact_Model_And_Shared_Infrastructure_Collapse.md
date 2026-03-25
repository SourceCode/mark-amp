# Phase 05: Artifact Model And Shared Infrastructure Collapse

## Task Count

- 2 tasks

## Task V29-P05-T01

- Phase ID: V29-P05
- Task ID: V29-P05-T01
- Task Title: Collapse shared artifact infrastructure to retained artifact families only
- Priority: Critical
- Category: Service / Model Removal
- Objective: Remove notebook and canvas from the canonical artifact model, creation service, naming policy branches, and related shared counters.
- Why This Matters Now: Shared infrastructure still encodes removed features as first-class peers, which keeps lifecycle and restore complexity alive even after UI removal.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h` and `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp` still explicitly model notebook and canvas kinds.
- User / Product Impact: Simplifies creation, restore, recent-item logic, and shell reasoning around what the app can open.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h:24`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp:80`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp:113`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp:124`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_01__Artifact_Creation_Spine_And_Shell_Ownership.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_02__Artifact_Lifecycle_And_Unsaved_Document_Spine.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_02__Artifact_Lifecycle_Unification_And_Unsaved_Ownership_Closure.md`
- Scope: `ArtifactKind`, record helpers, creation service, naming policy, counters, and shared artifact-facing APIs.
- Out of Scope: Feature-owned lifecycle classes handled in Phases 06 and 07.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.h`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactCreationService.h`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactNamingPolicy.cpp`
- Related Features / Systems / Components: Creation spine, registry, naming, active artifact tracking.
- Current Behavior / Presence: Text files, notebooks, and canvases share one first-class artifact system.
- Intended Post-Removal Behavior: Retained document types alone remain first-class artifacts.
- Removal / Simplification Direction: Delete kinds and branches instead of leaving dormant enum values.
- Technical Approach: Remove `kNotebook` and `kCanvas`, delete helper methods and counters, and update downstream call sites to retained abstractions.
- Implementation Steps: Refactor enum and switch statements; delete create helpers for notebook/canvas; update naming defaults; fix compile errors in dependent code by removing retired paths.
- Validation Steps: Build the app and verify all artifact service tests and shell integrations compile against the reduced model.
- Acceptance Criteria: Shared artifact code no longer exposes notebook or canvas as active artifact families.
- Dependencies: Phases 01 through 04.
- Parallelization Notes: Must land before full notebook/canvas lifecycle deletion is complete.
- Risks / Failure Modes: Restore or tab logic may assume the old enum values still exist.
- Migration / Compatibility Notes: Introduce compatibility readers before deleting any persisted numeric or textual kind mappings.
- UX / Layout Cleanup Notes: Reduced artifact model should simplify “new/open/save/reopen” messaging.
- Cleanup / Consolidation Notes: Collapse utility helpers that only existed for multi-artifact branching.
- Rollback / Safety Notes: Change in one controlled phase to minimize cascading merge conflicts.
- References / Context: This is the architectural simplification pivot that unlocks cleaner deletions later.
- Example Scenarios: Artifact creation no longer has `create_notebook()` or `create_canvas()`.

## Task V29-P05-T02

- Phase ID: V29-P05
- Task ID: V29-P05-T02
- Task Title: Remove feature-specific shell integration adapters and reduce shared tab/tree branching
- Priority: High
- Category: Service / Model Removal
- Objective: Delete notebook/canvas shell adapters and collapse tab/tree/reopen logic onto retained document behavior.
- Why This Matters Now: Even after artifact kinds are removed, adapter classes can keep stale assumptions alive in shell composition.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.cpp` and notebook host/lifecycle support classes still model removed surfaces in shell tabs and tree nodes.
- User / Product Impact: Makes tabs, tree views, and shell state more predictable and easier to maintain.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_07__Notebook_Host_Execution_And_Document_Trust.md`; `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_03__Notebook_Creation_And_Computational_Document_Flows.md`; `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_04__Canvas_Creation_And_Board_Workbench_Integration.md`
- Scope: Shell adapter classes, tab descriptors, tree-node builders, host abstractions, and active-item routing that only exist for notebooks/canvas.
- Out of Scope: Flashcard and task-specific cleanup.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.h`
- Related Features / Systems / Components: Tabs, tree nodes, shell restore, active artifact state.
- Current Behavior / Presence: Specialized shell adapters still exist for removed surfaces.
- Intended Post-Removal Behavior: Shared shell logic only reasons about retained document types and panels.
- Removal / Simplification Direction: Delete special-purpose shell adapters and fold retained behavior into generic shell paths.
- Technical Approach: Remove adapter classes and update call sites to use the reduced artifact model or retained editors directly.
- Implementation Steps: Delete canvas shell integration; delete notebook host abstraction; remove related includes and tests; simplify shell caller expectations.
- Validation Steps: Open, save, rename, and close retained documents; verify tabs and tree behavior still work without feature adapters.
- Acceptance Criteria: No shell adapter remains whose sole purpose is notebook or canvas integration.
- Dependencies: V29-P05-T01.
- Parallelization Notes: Can overlap with Phases 06 and 07 if ownership is clearly split.
- Risks / Failure Modes: Tab or tree code may require explicit retained-path replacement before deletion.
- Migration / Compatibility Notes: Restore mapping for removed tabs must be handled in Phase 11.
- UX / Layout Cleanup Notes: Tabs and tree views should no longer imply heterogeneous artifact families.
- Cleanup / Consolidation Notes: Reduce shell-specific DTOs and helper builders.
- Rollback / Safety Notes: Keep retained document open/close flows covered by smoke tests.
- References / Context: Earlier plans aimed to deepen shell ownership for these surfaces; v29 intentionally reverses that.
- Example Scenarios: The file tree no longer asks canvas integration for board nodes.
