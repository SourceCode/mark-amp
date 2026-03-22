# MarkAmp V24 Execution Synthesis Master Index

## Purpose

`v24` is the execution-synthesis and completion-sequencing pass. It consolidates the strongest findings from `v18` through `v23`, removes repeated planning noise, and defines the next dependency-aware implementation wave for finishing MarkAmp as a trustworthy IDE / canvas / notebook product.

## What Changed From Prior Planning Generations

- `v18` and `v19` established the integration problem.
- `v20` elevated broken artifact creation and lifecycle integrity into product blockers.
- `v21` elevated menus, controls, panels, and settings into trust blockers.
- `v22` elevated UI-system inconsistency into a product-quality blocker.
- `v23` proved the repository still contains a large amount of explicit unfinished work.
- `v24` merges those findings into one operational sequence, de-duplicates repeated workstreams, and defines what must land before the product is release-credible.

## What Was Merged, De-Duplicated, Or Re-Sequenced

- Creation, dirty state, save, autosave, restore, recovery, and recent-items continuity are now one artifact-lifecycle program instead of several separate tracks.
- Menu, toolbar, button, context-menu, and panel toggle work is merged into one shell action-governance stream.
- Settings schema, settings UI, deep links, apply/revert, persistence, and workspace scope work is merged into one settings-completion stream.
- Search, preview, diagnostics, and editor overlay work is sequenced behind the artifact and command foundations they depend on.
- Notebook and canvas work is sequenced after the artifact spine so they can converge on the same shell rules.
- UI quality work is attached to subsystem completion, not run as a detached styling backlog.
- Placeholder, stub, and migration cleanup is clustered by product ownership rather than by comment marker alone.

## Highest-Priority Next Implementation Moves

1. Replace split artifact creation paths with one shell-owned artifact lifecycle.
2. Make command, menu, toolbar, and context-routing authoritative across the shell.
3. Finish persistence, restore, autosave, recovery, and session continuity.
4. Complete notebook and canvas on top of the same artifact and shell contracts.
5. Eliminate production stubs in search, Git, repository, settings export, and cloud/network transports.
6. Finish panels, settings, and explorer workflows that still imply more than they actually deliver.
7. Turn smoke, regression, and restart/recovery validation into real release gates.

## Critical Path Dependencies

- Phase 01 establishes the program ledger, definition-of-finished rules, and merged backlog ownership.
- Phase 02 through Phase 05 form the core path: artifact lifecycle, persistence, shell action integrity, and panel/settings host completion.
- Phase 06 through Phase 10 build user-facing completion on top of that spine: editor/search, syntax/theme, notebook, canvas, and explorer/project flows.
- Phase 11 through Phase 17 are subsystem completion tracks that can run in parallel once the shell and lifecycle foundations stabilize.
- Phase 18 through Phase 20 close migration retirement, validation gates, and release-readiness proof.

## Parallelizable Workstreams

- Notebook and canvas can proceed in parallel after the artifact and command spine is stable.
- Search/editor and source-control/build tracks can proceed in parallel after persistence and shell routing are stable.
- Settings/config, plugin/extension, and cloud/security tracks can proceed in parallel once ownership boundaries are clarified.
- Rendering/import/export and advanced domain completion can proceed in parallel once shared persistence and command assumptions stop changing.
- Validation harness work should start early but becomes a hard gate near the end of each subsystem track.

## Highest-Risk Unfinished Subsystems

- `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp`

## Highest-Risk UX And Workflow Gaps

- New file creation still diverges between tabs, menus, and file tree.
- Save and restore behavior still depends too heavily on direct `LayoutManager` file I/O.
- Notebook creation and execution still stop short of product-grade lifecycle ownership.
- Canvas shell behavior still lags behind canvas workbench capability.
- Settings staging, apply, export, and workspace scope remain inconsistent.
- Some visible panels still imply real product depth while remaining thin or stubbed.

## Highest-Risk Architecture And Migration Gaps

- Direct widget callbacks versus command/action manifest routing
- Legacy pseudo untitled paths versus unsaved artifact records
- Panel lifecycle models versus actual registered panel factories
- Settings state owner versus visible settings widgets
- Newer design-system and theme-token infrastructure versus local surface overrides
- Audit helpers that diagnose incomplete work without yet enforcing completion

## Highest-Risk Validation And Regression Gaps

- No single artifact smoke suite covers file, notebook, and canvas creation through restore.
- Restart/recovery flows are not yet strong enough to gate release confidence.
- Many tests still normalize placeholder or stub behavior instead of forbidding it.
- Action/panel audit helpers are not yet release blockers.
- UI and syntax-highlighting quality regressions lack enough subsystem-level snapshots and smoke paths.

## Phase Order

1. `Phase_01__Program_Spine_Backlog_Deduplication_And_Definition_Of_Finished.md`
2. `Phase_02__Artifact_Lifecycle_And_Unsaved_Document_Spine.md`
3. `Phase_03__Persistence_Autosave_Restore_Recovery_And_Recent_Items.md`
4. `Phase_04__Shell_Command_Action_Menu_And_Control_Authority.md`
5. `Phase_05__Panels_Docking_Explorer_And_Settings_Host_Completion.md`
6. `Phase_06__Editor_Search_Preview_And_Diagnostics_Completion.md`
7. `Phase_07__Syntax_Highlighting_Theme_Icon_And_Visual_System_Closure.md`
8. `Phase_08__Notebook_Artifact_Runtime_And_Execution_Completion.md`
9. `Phase_09__Canvas_Artifact_Workbench_And_Interaction_Completion.md`
10. `Phase_10__Explorer_Project_Navigation_And_Workspace_Flows.md`
11. `Phase_11__Source_Control_Repository_History_And_Diff_Completion.md`
12. `Phase_12__Build_Run_Terminal_Debug_And_Output_Workflows.md`
13. `Phase_13__Settings_Config_Workspace_Scope_And_Deep_Link_Completion.md`
14. `Phase_14__Plugin_Extension_Contribution_And_Theme_Marketplace_Completion.md`
15. `Phase_15__Cloud_Sync_Security_Vault_And_Transport_Completion.md`
16. `Phase_16__Rendering_Import_Export_PDF_Media_And_Print_Completion.md`
17. `Phase_17__Advanced_Domain_Workflows_AV_Graph_Tasks_Presentation_And_Node_Editor.md`
18. `Phase_18__Migration_Retirement_Placeholder_Removal_And_Dead_Path_Cleanup.md`
19. `Phase_19__Diagnostics_Smoke_E2E_Fuzz_And_Regression_Gates.md`
20. `Phase_20__Release_Readiness_Subsystem_Signoff_And_Final_Completion_Gates.md`

## Task Counts

| Phase | Task Count |
| --- | ---: |
| Phase 01 | 5 |
| Phase 02 | 5 |
| Phase 03 | 5 |
| Phase 04 | 5 |
| Phase 05 | 5 |
| Phase 06 | 5 |
| Phase 07 | 5 |
| Phase 08 | 5 |
| Phase 09 | 5 |
| Phase 10 | 5 |
| Phase 11 | 5 |
| Phase 12 | 5 |
| Phase 13 | 5 |
| Phase 14 | 5 |
| Phase 15 | 5 |
| Phase 16 | 5 |
| Phase 17 | 5 |
| Phase 18 | 5 |
| Phase 19 | 5 |
| Phase 20 | 5 |
| Total | 100 |

## Overall Acceptance Criteria For The V24 Wave

- One authoritative artifact lifecycle exists for text files, notebooks, and canvases.
- Direct widget-only control paths are retired or reduced to thin delegates over canonical commands.
- Panels, menus, buttons, and settings hosts no longer expose dead or misleading production UI.
- Core production stubs in search, execution, Git, repository, settings export, and transport layers are completed or gated out of release paths.
- Notebook and canvas behave as first-class shell artifacts with save, restore, and command integrity.
- Shared UI, theme, syntax-highlighting, and icon systems are applied through the authoritative surfaces that remain after migration cleanup.
- Audit helpers and validation harnesses are wired into release readiness rather than left as optional diagnostics.
- Each subsystem has an explicit definition of finished and a signoff path.
