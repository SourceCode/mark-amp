# Phase 13 - Settings, Config, Workspace Scope, And Deep Link Completion

## Phase Goal

Finish the settings and configuration system so schema, UI, persistence, runtime application, and workspace overrides finally behave as one product feature.

## Measurable Outcome

- Settings definitions are not duplicated across competing pathways.
- User and workspace settings apply, persist, export, and restore correctly.
- Deep-link and JSON-editor settings flows are trustworthy.

### Task P13-T01

- Phase ID: `P13`
- Task ID: `P13-T01`
- Task Title: Consolidate settings schema ownership into one catalog pipeline
- Priority: `P0`
- Category: `Settings Completion`
- Objective: Remove duplicated setting definitions and bootstrap pathways.
- Why This Matters Now: Settings correctness and UI trust depend on one schema source.
- Execution Gap Statement: Settings schema currently exists across catalog, bootstrap, and UI-local registration paths.
- User / Product Impact: Settings can drift in labels, defaults, validation, and persistence behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v21` Phase 08-09, `v23` Phase 10
- Scope: Setting definitions, defaults, validation metadata, grouping
- Out of Scope: New settings categories beyond current scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/Config.h`
- Related Features / Systems / Components: Settings catalog, config defaults, settings UI
- Current Behavior: Definitions are duplicated and can diverge.
- Intended Completed Behavior: One settings catalog defines all user-facing settings and metadata.
- Missing Pieces: Schema authority and UI adoption
- Technical Approach: Make the catalog the only source of setting definitions; generate UI and JSON metadata from it.
- Implementation Steps: Inventory duplicates; migrate UI-local definitions; update bootstrap/default logic; add schema assertions.
- Validation Steps: Compare catalog entries to rendered settings UI and config defaults.
- Acceptance Criteria: No production setting is defined independently in multiple ownership paths.
- Dependencies: `P05-T03`
- Parallelization Notes: Foundation for export, workspace scope, and deep-link work.
- Risks / Failure Modes: Migrating schema can break existing saved config interpretation.
- Cleanup / Migration Notes: Remove `RegisterBuiltinSettings()` duplication from UI code.
- Observability / Diagnostics Notes: Emit catalog coverage and duplicate-key diagnostics.
- Rollback / Safety Notes: Keep config migration support for renamed or moved settings.
- References / Context: Settings catalog and panel files
- Example scenarios where useful: A setting appears with the same key, label, default, and validation in both the main settings UI and JSON editor.

### Task P13-T02

- Phase ID: `P13`
- Task ID: `P13-T02`
- Task Title: Complete workspace-vs-user settings persistence and restore behavior
- Priority: `P0`
- Category: `Settings Completion`
- Objective: Make settings scope real where the product implies it exists.
- Why This Matters Now: Workspace settings remain a clear unfinished seam.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` is still a stubbed area and scope handling is incomplete.
- User / Product Impact: Users cannot trust whether a setting is personal, project-local, or restorable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`
- Prior Plan References: `v21` assessment, `v23` Phase 10
- Scope: User scope, workspace scope, defaults, precedence, restore
- Out of Scope: Team/cloud-scoped settings beyond current scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ScopedConfig.h`
- Related Features / Systems / Components: Config, scoped config, workspace settings
- Current Behavior: Scope semantics are present conceptually but incompletely implemented.
- Intended Completed Behavior: Settings scope behaves predictably and persists correctly across sessions and workspaces.
- Missing Pieces: Workspace settings load/save, precedence application, UI surfacing
- Technical Approach: Complete workspace config storage and precedence resolution behind shared settings host APIs.
- Implementation Steps: Implement storage; apply precedence rules; update settings UI to show/edit scope cleanly.
- Validation Steps: Modify settings at user and workspace scope and verify startup/runtime application.
- Acceptance Criteria: Workspace settings persist, override correctly, and restore with the workspace.
- Dependencies: `P13-T01`, `P03-T03`
- Parallelization Notes: Can proceed with export and deep-link work.
- Risks / Failure Modes: Scope precedence bugs can produce hard-to-debug configuration behavior.
- Cleanup / Migration Notes: Remove placeholder workspace-setting branches after full implementation.
- Observability / Diagnostics Notes: Log resolved value, scope source, and override chain for debugging.
- Rollback / Safety Notes: Preserve global fallback if workspace config is unreadable.
- References / Context: Config and scoped-config infrastructure
- Example scenarios where useful: Word wrap enabled for one workspace does not silently change the global default.

### Task P13-T03

- Phase ID: `P13`
- Task ID: `P13-T03`
- Task Title: Finish settings export, import, JSON edit, and deep-link behavior
- Priority: `P1`
- Category: `Settings Completion`
- Objective: Remove placeholder export behavior and weak deep-link/JSON-editor paths.
- Why This Matters Now: Settings portability and advanced editing still lag behind the host’s visible affordances.
- Execution Gap Statement: `SettingsStateOwner::export_settings()` is a placeholder and JSON/deep-link flows remain weak.
- User / Product Impact: Advanced settings users cannot trust import/export or navigation to specific settings.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
- Prior Plan References: `v21` Phase 09, `v23` Phase 10
- Scope: Export, import, JSON validation, deep links, search query routing
- Out of Scope: Cloud-synced settings profiles
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Features / Systems / Components: Settings host, state owner, JSON editor
- Current Behavior: Export is placeholder; JSON editing and deep links are incomplete.
- Intended Completed Behavior: Settings can be exported/imported accurately, edited safely in JSON, and opened directly by query or setting ID.
- Missing Pieces: Config key enumeration, validation, deep-link host routing
- Technical Approach: Build all advanced settings flows on top of the canonical catalog and state owner.
- Implementation Steps: Implement export enumeration; harden JSON validation; connect deep-link router to live host contexts.
- Validation Steps: Export/import real settings sets, deep-link into nested settings, and validate JSON edits.
- Acceptance Criteria: Advanced settings flows no longer rely on placeholder behavior or fake formatting paths.
- Dependencies: `P05-T03`, `P13-T01`
- Parallelization Notes: Can progress with runtime-application work.
- Risks / Failure Modes: Incorrect export/import mapping can silently lose user configuration.
- Cleanup / Migration Notes: Remove fake format and weak validation shortcuts from JSON editor.
- Observability / Diagnostics Notes: Emit import/export counts, validation failures, and deep-link resolution traces.
- Rollback / Safety Notes: Keep backup files before destructive imports.
- References / Context: Settings state owner and JSON editor
- Example scenarios where useful: `settings://editor.wordWrap` opens the settings dialog to the correct setting and applying changes exports correctly.

### Task P13-T04

- Phase ID: `P13`
- Task ID: `P13-T04`
- Task Title: Ensure runtime application of changed settings across shell and content surfaces
- Priority: `P1`
- Category: `Settings Completion`
- Objective: Make changed settings actually take effect at runtime on the correct surfaces.
- Why This Matters Now: Settings persistence without runtime application still feels broken.
- Execution Gap Statement: Some settings can be saved without consistently updating shell, editor, notebook, canvas, or panel behavior.
- User / Product Impact: Users cannot trust the settings UI because changes may appear ignored.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/Events.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v19` settings/preferences, `v21` settings lifecycle, `v22` theme parity
- Scope: Theme changes, editor changes, panel layout settings, accessibility and startup settings
- Out of Scope: Reboot-only settings that are explicitly documented as such
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Features / Systems / Components: Setting-changed events, runtime application, UI refresh
- Current Behavior: Runtime application is inconsistent across surfaces.
- Intended Completed Behavior: Settings changes apply immediately when supported, and restart-required cases are explicit.
- Missing Pieces: Per-setting application policy and subscriber coverage
- Technical Approach: Define runtime-application contracts by setting category and bind surfaces to structured change events.
- Implementation Steps: Annotate settings; update subscribers; add restart-required notices; verify per-surface refresh behavior.
- Validation Steps: Change representative settings across all major categories and verify runtime behavior.
- Acceptance Criteria: Runtime-applicable settings visibly take effect without restart, and restart-only cases are clearly labeled.
- Dependencies: `P13-T01`, `P04-T03`
- Parallelization Notes: Can proceed with visual-system and accessibility work.
- Risks / Failure Modes: Excessive refresh logic can cause performance issues or stale partial updates.
- Cleanup / Migration Notes: Remove settings listeners that duplicate or bypass the canonical state owner.
- Observability / Diagnostics Notes: Emit setting application timing and target-surface diagnostics.
- Rollback / Safety Notes: Guard expensive live-apply paths with debouncing where necessary.
- References / Context: Setting-changed events and surface subscribers
- Example scenarios where useful: Changing editor font size updates open editors immediately and persists across restart.

### Task P13-T05

- Phase ID: `P13`
- Task ID: `P13-T05`
- Task Title: Add settings persistence and application regression gates
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Protect settings correctness once the host and schema become authoritative.
- Why This Matters Now: Settings regressions can quietly break many subsystems at once.
- Execution Gap Statement: Existing settings tests do not yet fully gate host, scope, export, and runtime-apply behavior together.
- User / Product Impact: Broken settings degrade shell trust across the product.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`
- Prior Plan References: `v21` Phase 10, `v23` Phase 19
- Scope: Host lifecycle, scope precedence, import/export, runtime application, restart restore
- Out of Scope: Visual snapshot testing of settings UI
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_panel_lifecycle.cpp`
- Related Features / Systems / Components: Settings validation, regression testing
- Current Behavior: Coverage is partial and split across lifecycle and unit-level tests.
- Intended Completed Behavior: Settings behavior is covered by deterministic tests and smoke scenarios.
- Missing Pieces: Cross-scope fixtures and host-driven workflows
- Technical Approach: Add settings smoke scenarios and targeted integration coverage on top of existing unit tests.
- Implementation Steps: Create representative settings fixtures; add apply/cancel/import/export/restart tests; wire into CI.
- Validation Steps: Seed known failure modes and confirm test detection.
- Acceptance Criteria: Core settings workflows are regression-protected and release-gated.
- Dependencies: `P13-T01` through `P13-T04`
- Parallelization Notes: Can scaffold during implementation and harden at phase close.
- Risks / Failure Modes: Overly brittle tests can discourage legitimate UI refactors.
- Cleanup / Migration Notes: Retire tests that normalize placeholder export or weak JSON validation behavior.
- Observability / Diagnostics Notes: Emit captured settings diffs and scope resolution traces on failure.
- Rollback / Safety Notes: Keep fixture migrations aligned with schema changes.
- References / Context: Prior V21/V23 settings tests
- Example scenarios where useful: Changing a workspace-only setting, exporting settings, restarting, and reopening the workspace remains covered end to end.
