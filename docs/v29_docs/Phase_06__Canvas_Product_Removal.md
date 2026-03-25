# Phase 06: Canvas Product Removal

## Task Count

- 2 tasks

## Task V29-P06-T01

- Phase ID: V29-P06
- Task ID: V29-P06-T01
- Task Title: Delete canvas lifecycle, board creation/open/save flows, and workbench integration
- Priority: Critical
- Category: Canvas Removal
- Objective: Remove canvas artifact lifecycle, board workflows, and all shell integration routes that still treat boards as openable project artifacts.
- Why This Matters Now: Canvas remains one of the largest product subsystems and cannot be retired by shell cleanup alone.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp` still supports create/open/restore/save/save-as/rename/duplicate/remove/close for boards.
- User / Product Impact: Ensures users cannot create or reopen boards and that the app stops pretending boards are part of the retained workflow.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasNavigationIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasSessionContext.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v20_docs/Phase_04__Canvas_Creation_And_Board_Workbench_Integration.md`; `/Users/ryanrentfro/code/markamp/docs/v24_docs/Phase_09__Canvas_Artifact_Workbench_And_Interaction_Completion.md`; `/Users/ryanrentfro/code/markamp/docs/v25_docs/Phase_09__Canvas_Shell_Workbench_Persistence_And_Interaction_Integrity.md`
- Scope: Canvas lifecycle, open/save/restore hooks, shell integration, navigation helpers, and workbench adapters.
- Out of Scope: Low-level canvas engine deletion handled in V29-P06-T02.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.h`; `/Users/ryanrentfro/code/markamp/src/core/CanvasNavigationIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasShellIntegration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasSessionContext.cpp`
- Related Features / Systems / Components: Artifact lifecycle, shell navigation, recent items, restore.
- Current Behavior / Presence: Boards behave like first-class editable artifacts with save and restore semantics.
- Intended Post-Removal Behavior: Board lifecycle operations do not exist in the shipping product.
- Removal / Simplification Direction: Delete board lifecycle services and map old persisted board references to a safe ignore path.
- Technical Approach: Remove the lifecycle classes and any caller routes after shared artifact collapse and migration handling are in place.
- Implementation Steps: Delete lifecycle and shell integration classes; remove call sites; replace restore/open references with compatibility drops; update tests and CMake.
- Validation Steps: Search for `CanvasArtifactLifecycle`, `BoardOpenRequestEvent`, and `CanvasShellIntegration`; confirm no retained production references remain.
- Acceptance Criteria: No production code path can create, open, save, or restore a board.
- Dependencies: Phases 04 and 05.
- Parallelization Notes: Coordinate with V29-P11 migration work.
- Risks / Failure Modes: Old workspace state could still try to call deleted board routes unless migration is landed.
- Migration / Compatibility Notes: Old board records should be skipped with non-fatal logging or notification.
- UX / Layout Cleanup Notes: Any “recent board” or “board template” copy must disappear.
- Cleanup / Consolidation Notes: Remove board-specific DTOs and helper contexts that become unreachable.
- Rollback / Safety Notes: Stage deletions after feature entry points are already gone.
- References / Context: Canvas was heavily expanded in v19-v27 and now becomes a full retirement track.
- Example Scenarios: Reopening a workspace with a board no longer recreates a canvas tab.

## Task V29-P06-T02

- Phase ID: V29-P06
- Task ID: V29-P06-T02
- Task Title: Delete canvas engine, UI panels, collaboration, templates, and plugin-derived canvas subfeatures
- Priority: Critical
- Category: Canvas Removal
- Objective: Remove the canvas engine itself, its UI panels, collaboration stack, templates, and canvas-derived subfeatures such as mind map and canvas kanban.
- Why This Matters Now: Canvas source footprint is large enough that keeping it in tree after lifecycle deletion preserves dead code, build cost, and documentation drift.
- Removal Gap Statement: `/Users/ryanrentfro/code/markamp/src/canvas` contains 330 files and `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp` still exposes canvas collaboration, apps, kanban, and mind map as built-in plugin features.
- User / Product Impact: Delivers the actual smaller product rather than a hidden feature pile.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasCommentsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasFacilitationPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`
- Prior Plan References: `/Users/ryanrentfro/code/markamp/docs/v19_docs/Phase_08__Canvas_Workbench_And_Cross_Surface_Integration.md`; `/Users/ryanrentfro/code/markamp/docs/v22_docs/Phase_10__Canvas_Workspace_Tools_Object_Chrome_And_Interaction_Visuals.md`; `/Users/ryanrentfro/code/markamp/docs/v27_docs/Phase_10__Canvas_Shell_Tooling_Overlay_And_Creative_Surface_Redesign.md`
- Scope: All canvas engine files, canvas UI panels, collaboration files, template services, export services, search/indexing tied only to canvas, mind map, board template, and canvas-specific kanban paths.
- Out of Scope: AV kanban data view, which belongs to retained structured-data workflows and must be preserved.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasCollabService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuiltInPlugins.cpp`
- Related Features / Systems / Components: Canvas renderer, input, tools, collab, templates, mind map, canvas kanban, plugins.
- Current Behavior / Presence: Canvas remains a deep product subsystem with many satellite features.
- Intended Post-Removal Behavior: None of the canvas engine or its derived creative subfeatures ship in the application.
- Removal / Simplification Direction: Delete the entire subsystem and any derived plugin entries, while preserving non-canvas structured-data features explicitly.
- Technical Approach: Remove canvas directories and feature-owned files, then repair retained code by deleting references instead of stubbing them.
- Implementation Steps: Delete `/src/canvas` and canvas UI wrappers; remove built-in plugins and feature flags for canvas derivatives; audit help text, tests, and docs; preserve AV kanban by separating it from canvas kanban references.
- Validation Steps: Build search for `Canvas`, `Board`, `MindMap`, and canvas kanban types in production code; confirm only explicit migration handling remains.
- Acceptance Criteria: Canvas, mind map, and canvas-derived plugin subfeatures are absent from shipping code and UI.
- Dependencies: V29-P06-T01 and Phase 10 planning.
- Parallelization Notes: Plugin cleanup can run in parallel if ownership is split by file set.
- Risks / Failure Modes: Confusing AV kanban with canvas kanban could over-delete retained data-view capabilities.
- Migration / Compatibility Notes: Any persisted board-template or collab session state must be ignored safely.
- UX / Layout Cleanup Notes: Remove any empty states, icons, or labels that refer to boards, templates, minimap, comments, or facilitation.
- Cleanup / Consolidation Notes: Delete canvas token headers and chrome coordinators in the same wave.
- Rollback / Safety Notes: Keep deletions segmented so AV and editor behavior are not accidentally reverted with them.
- References / Context: v22-v27 invested heavily in making canvas premium; v29 explicitly retires that direction.
- Example Scenarios: No `CanvasWorkspacePanel`, no canvas collaboration plugin, no `Canvas: New Board`, no mind map workflow.
