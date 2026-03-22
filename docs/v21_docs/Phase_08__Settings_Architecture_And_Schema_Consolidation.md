# Phase 08: Settings Architecture And Schema Consolidation

## Outcome

Establish one authoritative settings architecture so settings schema, ownership, scope, deep links, and runtime application stop competing with each other.

## Improvement Count

132 atomic improvements across 6 execution tasks.

### P08-T01

- Phase ID: P08
- Task ID: P08-T01
- Task Title: Choose one canonical settings state owner and remove lifecycle contradictions
- Priority: P0
- Category: Settings Application
- Atomic Improvements Covered: 22
- Objective: Make one component responsible for staged settings state, apply, cancel, undo, and export/import orchestration.
- Why This Matters Now: The settings stack currently contradicts itself at the architecture level.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` writes settings immediately while `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` presents staged apply/cancel semantics and `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp` claims canonical ownership.
- User Impact: Users cannot trust whether changes are live, pending, reversible, or already persisted.
- Scope: Settings ownership, staged state, apply/cancel/revert semantics, undo/redo boundaries, import/export orchestration.
- Out of Scope: Final settings UI polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`
- Related Systems / Components: Settings panel, settings dialog, config, live settings reactor, settings import/export.
- Current Behavior: Multiple settings owners exist simultaneously and do not agree on lifecycle semantics.
- Intended Behavior: One authoritative state owner governs staged values, apply/cancel semantics, persistence boundaries, and runtime apply notifications.
- Technical Approach: Promote one owner, adapt UI components to it, and remove direct config mutation from presentation widgets.
- Implementation Steps: Select canonical owner; define authoritative state API; migrate UI callers; remove immediate-write side effects from `SettingsPanel`; document lifecycle rules.
- Validation Steps: Change settings in the UI, cancel, apply, undo, and reopen; verify all state transitions match the authoritative model.
- Acceptance Criteria: No settings surface mutates persisted config directly outside the canonical settings-owner contract.
- Dependencies: Phase 01.
- Risks / Failure Modes: Existing settings widgets may depend on direct mutation side effects for live preview.
- UX Notes: Users must understand clearly whether they are previewing, staging, or applying settings.
- Settings / Panel / Control Notes where relevant: This is the foundational settings contract every settings control must obey.
- Observability / Diagnostics Notes: Add settings-state transition traces for stage, apply, cancel, revert, import, and export.
- Rollback / Safety Notes: Keep compatibility adapters while migrating old widgets to the canonical owner.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp` calls itself canonical, but the UI still mutates config directly.
- Example scenarios where useful: Toggling a setting in the dialog, pressing Cancel, and reopening shows the original persisted value rather than the staged value.

### P08-T02

- Phase ID: P08
- Task ID: P08-T02
- Task Title: Consolidate settings schema registration and defaults into one authoritative catalog
- Priority: P0
- Category: Settings UI
- Atomic Improvements Covered: 22
- Objective: Eliminate duplicate schema ownership between core and UI registration paths.
- Why This Matters Now: A settings UI cannot be trustworthy if multiple schema registries define overlapping keys and defaults.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp`, and built-in registration in `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` overlap in schema and defaults.
- User Impact: Setting labels, defaults, descriptions, categories, and validation can drift between storage and UI.
- Scope: Setting definitions, categories, defaults, validation metadata, restart-required metadata, visibility rules, migration hooks.
- Out of Scope: Addition of entirely new settings unrelated to the existing surfaces.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Related Systems / Components: Settings catalog, config defaults, settings UI generation, live settings reactor.
- Current Behavior: The schema and UI metadata are defined in more than one place.
- Intended Behavior: One catalog owns every setting definition, default, category, scope, validator, and presentation hint.
- Technical Approach: Merge schema definitions into a single authoritative catalog and have UI read from it instead of re-registering built-ins.
- Implementation Steps: Inventory duplicated setting definitions; choose authoritative model shape; migrate UI to catalog-driven rendering; remove duplicate registration.
- Validation Steps: Compare exported catalog entries, defaults, and UI categories before and after consolidation.
- Acceptance Criteria: Every setting key and its metadata can be traced to one catalog source only.
- Dependencies: P08-T01.
- Risks / Failure Modes: Existing code may depend on side effects from `Config::set_defaults()` that need to be mirrored carefully.
- UX Notes: Category naming and descriptions should become more consistent after consolidation.
- Settings / Panel / Control Notes where relevant: Settings search, category navigation, and reset-to-default depend on a trustworthy catalog.
- Observability / Diagnostics Notes: Add duplicate-key and duplicate-metadata detection to the settings bootstrap path.
- Rollback / Safety Notes: Keep a migration audit that compares old and new defaults during the transition.
- References / Context: The repo currently has both core-side and UI-side built-in settings registration.
- Example scenarios where useful: The same setting key reports the same default and help text in the UI, JSON editor, and reset flow.

### P08-T03

- Phase ID: P08
- Task ID: P08-T03
- Task Title: Define explicit user, workspace, and project settings-scope contracts
- Priority: P0
- Category: Settings Persistence
- Atomic Improvements Covered: 22
- Objective: Make scope semantics explicit and correctly implemented rather than partially represented in the UI only.
- Why This Matters Now: Scope selectors and tabs are misleading if the underlying persistence contract is incomplete.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` still uses simplified stub load/save behavior while `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` exposes scoped editing surfaces.
- User Impact: Users cannot trust where a setting is stored or which value will win at runtime.
- Scope: User/workspace/project scope ownership, precedence rules, persistence paths, UI scope selectors, scope-specific reset behavior.
- Out of Scope: Multi-machine sync semantics for settings.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ScopedConfig.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Systems / Components: Workspace settings, scoped config, settings UI, persistence, startup restore.
- Current Behavior: The UI advertises multiple scopes, but the persistence backend and precedence behavior are only partially implemented.
- Intended Behavior: Scope tabs and selectors correspond to a real precedence model with correct read/write/apply semantics.
- Technical Approach: Define authoritative scope precedence and persistence contracts, then align UI scope controls and storage code with them.
- Implementation Steps: Document precedence rules; implement durable scope storage; align scope selectors and labels; add scope-aware reset and export behavior.
- Validation Steps: Modify the same setting at different scopes and verify runtime precedence, persistence location, and restore behavior.
- Acceptance Criteria: Settings scope behavior is deterministic, documented in code, and reflected accurately in the UI.
- Dependencies: P08-T01, P08-T02.
- Risks / Failure Modes: Existing workspaces may contain legacy values that require migration and conflict resolution.
- UX Notes: Users need clear scope labels and conflict cues when higher-precedence values override lower-precedence ones.
- Settings / Panel / Control Notes where relevant: Scope selectors should not render a scope option that is not actually supported in the current workspace mode.
- Observability / Diagnostics Notes: Add scope-resolution traces that show which layer supplied the effective value.
- Rollback / Safety Notes: Support legacy workspace settings files during one migration cycle.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` currently advertises capabilities beyond its implemented persistence.
- Example scenarios where useful: A workspace-specific editor font override wins over the user default only inside that workspace and survives restart.

### P08-T04

- Phase ID: P08
- Task ID: P08-T04
- Task Title: Finish settings deep-link routing and host resolution
- Priority: P0
- Category: Settings UI
- Atomic Improvements Covered: 22
- Objective: Ensure every settings entry point opens the real settings host at the requested category, query, or setting ID.
- Why This Matters Now: Many menus and commands currently publish settings-open events that do not route to a real destination.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp` logs deep-link intent, but does not robustly route to `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` capabilities such as `OpenToSetting` and `OpenWithQuery`.
- User Impact: Settings commands feel shallow or inconsistent depending on where they are invoked.
- Scope: Preferences menu, toolbar settings button, command palette settings commands, deep-link query routing, host fallback behavior.
- Out of Scope: Visual redesign of settings navigation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`
- Related Systems / Components: Settings deep links, command routing, settings dialog, main frame, shell controller.
- Current Behavior: Settings open requests often resolve only to sidebar mode changes or log statements rather than a real deep-linked host.
- Intended Behavior: Every settings entry point opens the same settings host and lands at the requested query or setting key when possible.
- Technical Approach: Introduce a real settings-host resolver and make settings-open requests carry canonical deep-link payloads through it.
- Implementation Steps: Define host-resolution rules; bind router to the real settings host; support `setting_id`, `query`, and `scope`; remove weak sidebar-only fallbacks where appropriate.
- Validation Steps: Trigger settings from menu, toolbar, palette, and inline actions and verify correct host plus navigation state.
- Acceptance Criteria: No settings entry point ends at a pseudo-settings surface or silent no-op route.
- Dependencies: P08-T01.
- Risks / Failure Modes: Multiple settings-host concepts may coexist during migration and cause routing ambiguity.
- UX Notes: Deep-linked settings should preserve user context and make the destination obvious on arrival.
- Settings / Panel / Control Notes where relevant: If settings remain available as a panel as well as a dialog, host resolution rules must be explicit and consistent.
- Observability / Diagnostics Notes: Log settings-open source, requested destination, chosen host, and fallback reason.
- Rollback / Safety Notes: Keep a controlled fallback to the generic settings root while deep-link coverage is completed.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` already exposes `OpenToSetting` and `OpenWithQuery`, but the router path does not fully use them.
- Example scenarios where useful: `Preferences: Open Font Settings` from the command palette opens the settings host directly to the editor font category instead of only switching the activity bar.

### P08-T05

- Phase ID: P08
- Task ID: P08-T05
- Task Title: Align runtime setting application with the canonical catalog and state owner
- Priority: P1
- Category: Settings Application
- Atomic Improvements Covered: 22
- Objective: Make live setting changes apply through one explicit runtime path with clear restart-required semantics.
- Why This Matters Now: Settings that save but do not apply, or apply through hidden side effects, are a major trust failure.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/core/LiveSettingsReactor.cpp` exists, but live-apply behavior is incomplete and not the sole runtime application path.
- User Impact: Users change settings and see inconsistent, delayed, or missing runtime effects.
- Scope: Live settings reactor usage, restart-required metadata, runtime apply hooks for editor/panels/themes/layout, staged preview boundaries.
- Out of Scope: New setting types beyond those already surfaced in the app.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/LiveSettingsReactor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: Live settings reactor, config, events, panels, editor, themes.
- Current Behavior: Runtime setting application is partly event-driven, partly local, and only partially declared in one place.
- Intended Behavior: Every setting declares whether it applies live, on host reopen, or on restart, and changes flow through one runtime apply path.
- Technical Approach: Attach application mode metadata to catalog entries and drive runtime updates from the canonical owner through the live reactor.
- Implementation Steps: Add application-mode metadata; register missing handlers; remove local side-effect paths; surface restart-required indicators where needed.
- Validation Steps: Change representative editor, theme, layout, and panel settings and verify live updates or restart-required messaging.
- Acceptance Criteria: Users can tell which settings apply immediately, and actual runtime behavior matches that declaration.
- Dependencies: P08-T01, P08-T02.
- Risks / Failure Modes: Some legacy settings may still have hidden side effects outside the reactor and need careful removal.
- UX Notes: Restart-required settings should be explicit and never feel arbitrary.
- Settings / Panel / Control Notes where relevant: Panel and control settings need the same live-apply rigor as editor settings.
- Observability / Diagnostics Notes: Add runtime-apply traces with handler name, setting key, and outcome.
- Rollback / Safety Notes: Keep a compatibility bridge that emits legacy `SettingChangedEvent` notifications during the migration.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/LiveSettingsReactor.cpp` currently handles only a narrow subset of live application behavior.
- Example scenarios where useful: Changing the sidebar width preference or terminal wrapping setting updates the corresponding surface immediately and predictably.

### P08-T06

- Phase ID: P08
- Task ID: P08-T06
- Task Title: Add settings-architecture diagnostics and migration gates
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Make schema duplication, rogue direct writes, scope drift, and broken deep links mechanically detectable.
- Why This Matters Now: The settings stack is complex enough that architecture drift will recur unless it is gated.
- Problem Statement: Without diagnostics, direct config mutation and duplicate schema ownership can easily reappear in new UI work.
- User Impact: Settings trust erodes gradually through invisible drift until users encounter inconsistent behavior.
- Scope: Duplicate-schema detection, rogue direct-write detection, deep-link trace validation, scope-resolution diagnostics, migration audit reports.
- Out of Scope: End-user telemetry analytics beyond internal diagnostics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Systems / Components: Settings architecture, diagnostics, CI, config mutation, migration tools.
- Current Behavior: Architectural problems require code archaeology to detect.
- Intended Behavior: CI and debug builds can report duplicate settings ownership, direct writes outside the canonical owner, and unresolved deep-link requests.
- Technical Approach: Instrument settings APIs, add architectural assertions, and generate migration/completeness reports.
- Implementation Steps: Add direct-write guardrails; generate duplicate-key reports; verify deep-link destinations; add scope-resolution audit output; wire checks into CI.
- Validation Steps: Intentionally add a duplicate key or a direct config write path and verify diagnostics catch it.
- Acceptance Criteria: Settings architectural regressions become mechanically visible and release-blocking where critical.
- Dependencies: P08-T01 through P08-T05.
- Risks / Failure Modes: Diagnostics may produce too much noise if legacy paths are not triaged carefully.
- UX Notes: Internal diagnostics should improve user trust indirectly by preventing silent settings drift.
- Settings / Panel / Control Notes where relevant: Reports should separate schema, persistence, UI, and runtime-apply failures.
- Observability / Diagnostics Notes: Emit structured diagnostics suitable for CI artifacts and local developer inspection.
- Rollback / Safety Notes: Gate strict failures gradually if the initial diagnostics surface too much legacy debt at once.
- References / Context: v21 treats misleading settings behavior as a release blocker, so the architecture needs enforcement, not just guidance.
- Example scenarios where useful: CI fails when a new settings widget writes directly to `Config` instead of using the canonical state owner.
