# Phase 13: Settings, Config, Scope, Deep Link, And Export Closure

## Phase Intent

Finish the settings and config work still needed for dependable release behavior.

## Release-Ready Exit Criteria

- settings staging, apply, cancel, export, import, scope, and deep links are coherent,
- config persistence and workspace overrides are validated,
- no visible settings path bypasses canonical ownership.

## Task Count

3

## Task P13-T01

- Phase ID: P13
- Task ID: P13-T01
- Task Title: Complete `SettingsStateOwner` adoption, export behavior, and config persistence wiring
- Priority: P0
- Category: Settings Hardening
- Objective: make `SettingsStateOwner` the actual owner of visible settings lifecycle, including export and persistence.
- Why This Matters Now: the placeholder export behavior proves this subsystem is still incomplete on the release path.
- Release Gap Statement: settings ownership is still split between direct mutation and partially adopted state ownership.
- User / Product Impact: users need dependable settings apply, restore, and export behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Config.h`
- Prior Plan References: `v21 Phase 08`; `v21 Phase 09`; `v24 Phase 13`
- Scope: staged edits, export/import of visible settings, persisted apply path, dirty indicators.
- Out of Scope: remote settings sync.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.h`; `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Features / Systems / Components: config; settings state owner; settings host
- Current Behavior: export remains placeholder and visible settings adoption is incomplete.
- Intended Release-Ready Behavior: the same owner stages, validates, applies, exports, and restores settings.
- Missing Pieces: real export inventory; visible host adoption; persistence integration.
- Technical Approach: extend `SettingsStateOwner` from a helper into the canonical release-path settings engine.
- Implementation Steps:
1. Replace placeholder export with real config key iteration for release-path settings.
2. Route visible settings surfaces through the owner.
3. Add settings export/import and apply/cancel tests.
- Validation Steps:
1. Stage changes, export, restart, import, and re-apply.
2. Verify the same values persist and restore correctly.
- Acceptance Criteria: no visible settings workflow bypasses `SettingsStateOwner`.
- Dependencies: P05-T03
- Parallelization Notes: scope/deep-link work can proceed after host adoption contracts are defined.
- Risks / Failure Modes: duplicated key definitions; export omissions.
- Observability / Diagnostics Notes: emit staged/applied/exported key counts and failures.
- Rollback / Safety Notes: export/import should fail safely and never corrupt current config.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`

## Task P13-T02

- Phase ID: P13
- Task ID: P13-T02
- Task Title: Finish workspace-scope overrides and deep-link routing for real settings hosts
- Priority: P1
- Category: Settings Hardening
- Objective: make scoped settings and deep links behave like first-class product workflows.
- Why This Matters Now: scoped configuration is part of release-grade IDE behavior, not optional polish.
- Release Gap Statement: workspace settings and deep-link routing remain partially implemented or weakly integrated.
- User / Product Impact: users need predictable scope and direct navigation to relevant settings.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ScopedConfig.h`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
- Prior Plan References: `v21 Phase 08`; `v23 Phase 10`; `v24 Phase 13`
- Scope: global versus workspace overrides, host routing, category selection, restart persistence.
- Out of Scope: cross-workspace sync.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ScopedConfig.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: scoped config; deep links; settings host; workspace settings
- Current Behavior: deep links log and route loosely, while workspace settings still lag in completeness.
- Intended Release-Ready Behavior: users can open scoped settings directly and trust persistence and restore semantics.
- Missing Pieces: workspace settings load/save completion; host routing integration; scope validation.
- Technical Approach: treat settings scope and deep-link paths as first-class shell navigation flows backed by the canonical settings host.
- Implementation Steps:
1. Complete workspace settings persistence behavior.
2. Route deep links into the canonical host with category and scope context.
3. Add scoped settings smoke scenarios.
- Validation Steps:
1. Change global and workspace-scoped values and restart.
2. Invoke deep links from visible entry points and verify correct section and scope.
- Acceptance Criteria: scoped settings and deep links are deterministic and restart-safe.
- Dependencies: P05-T03; P13-T01
- Parallelization Notes: can proceed while config schema cleanup happens.
- Risks / Failure Modes: wrong scope precedence; deep links opening stale hosts.
- Observability / Diagnostics Notes: log scope resolution and deep-link targets.
- Rollback / Safety Notes: when scope resolution fails, fall back to read-only or global view with explicit messaging.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`

## Task P13-T03

- Phase ID: P13
- Task ID: P13-T03
- Task Title: De-duplicate settings schema and validation ownership
- Priority: P1
- Category: Architecture Consolidation
- Objective: remove drift between catalog bootstrap, panel registration, and config validation definitions.
- Why This Matters Now: schema duplication is a recurring source of partial completion and inconsistent settings behavior.
- Release Gap Statement: settings definitions remain duplicated across multiple layers.
- User / Product Impact: duplicated schema increases the risk of missing defaults, wrong labels, and inconsistent validation.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v21 Phase 08`; `v24 Phase 13`
- Scope: release-path built-in settings definitions, validation metadata, defaults, UI descriptors.
- Out of Scope: extension-contributed settings beyond release-critical integration.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ConfigAuditTrail.cpp`
- Related Features / Systems / Components: settings catalog; defaults; validation; migrations
- Current Behavior: multiple sources define overlapping built-in settings behavior.
- Intended Release-Ready Behavior: one canonical schema drives defaults, UI, validation, export, and migration behavior.
- Missing Pieces: schema source-of-truth selection; duplicate removal; validation tests.
- Technical Approach: choose one catalog source of truth and make other layers consume it.
- Implementation Steps:
1. Inventory duplicated setting definitions.
2. Consolidate schema and defaults into one canonical registry.
3. Update UI and validation consumers to read from it.
- Validation Steps:
1. Compare visible settings list, defaults, and validation behavior before and after consolidation.
2. Add tests for missing-schema or duplicate-key failure.
- Acceptance Criteria: release-path settings definitions are not duplicated across competing bootstrap paths.
- Dependencies: P13-T01
- Parallelization Notes: low-conflict refactor after host behavior is stable.
- Risks / Failure Modes: missing settings after consolidation; migration drift.
- Observability / Diagnostics Notes: export schema summary and duplicate-key audit.
- Rollback / Safety Notes: preserve compatibility aliases during migration if required.
- References / Context: `docs/v21_docs/ASSESSMENT__MarkAmp_V21_Control_Panels_And_Settings_Assessment.md`
