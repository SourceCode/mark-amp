# Phase 10: Settings Config Deep Link And Runtime Application Completion

## Outcome

Finish settings ownership, schema, import/export, deep links, workspace scope, and runtime-application behavior so preferences stop being split between immediate mutation, staged mutation, and placeholder ownership models.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P10-T01

- Phase ID: P10
- Task ID: P10-T01
- Task Title: Make SettingsStateOwner the real settings authority and remove placeholder export behavior
- Priority: P0
- Category: Settings Completion
- Atomic Completion Tasks Covered: 60
- Objective: Consolidate staged settings behavior under one real owner.
- Why This Matters Now: Settings still contradict themselves between UI promises and backend behavior.
- Completion Gap Statement: The canonical settings owner still contains placeholder export behavior and is not yet the sole live authority.
- User / Product Impact: Apply, cancel, revert, import, and export remain less trustworthy than they appear.
- Repository Evidence: [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp), [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp), [SettingsDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp)
- Scope: Stage/apply/cancel, export/import, undo-last, changed-key publishing, and visible settings host behavior.
- Out of Scope: Pure settings UI styling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsControlModel.cpp`
- Related Features / Systems / Components: Settings, dialogs, config, staging.
- Current Behavior: Multiple settings ownership paths still coexist and the canonical owner is incomplete.
- Intended Completed Behavior: One settings owner handles all staged mutations, import/export, and commit semantics.
- Missing Pieces: Real export enumeration, unified authority, and adapter cleanup.
- Technical Approach: Route all settings edits through `SettingsStateOwner` and finish its missing responsibilities.
- Implementation Steps: Complete export/import; rebase settings panel actions; remove direct config writes in visible UI; align events and revert flows.
- Validation Steps: Edit settings, apply, cancel, import, export, and reopen in both user and workspace scopes.
- Acceptance Criteria: Settings no longer mutate through conflicting ownership models.
- Dependencies: Phase 02.
- Risks / Failure Modes: Some settings may rely on immediate mutation today and need careful staged-apply adaptation.
- Cleanup / Migration Notes where relevant: Delete direct-write code paths after parity is reached.
- Observability / Diagnostics Notes where relevant: Emit staged-change counts, apply batches, and failed-apply diagnostics.
- Rollback / Safety Notes: Preserve a compatibility bridge while staged ownership is rolled out to all controls.
- References / Context: This closes one of the clearest contradictions called out in `v21`.
- Example scenarios where useful: Exporting settings yields real persisted keys, not an empty placeholder map.

### P10-T02

- Phase ID: P10
- Task ID: P10-T02
- Task Title: Finish settings schema consolidation across catalogs, bootstrap, and visible registration paths
- Priority: P0
- Category: Settings Completion
- Atomic Completion Tasks Covered: 60
- Objective: Remove duplicated settings definitions and make validation authoritative.
- Why This Matters Now: Settings drift is likely while definitions are duplicated.
- Completion Gap Statement: Catalog, bootstrap, and panel-local registration still overlap instead of flowing from one schema source.
- User / Product Impact: Settings can display, validate, or persist inconsistently.
- Repository Evidence: [SettingsCatalog.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp), [SettingsCatalogBootstrap.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp), [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp)
- Scope: Schema ownership, defaults, descriptions, types, validation rules, visibility rules, and deep-link IDs.
- Out of Scope: Visual presentation details.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalogBootstrap.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ResolvedConfig.cpp`
- Related Features / Systems / Components: Settings schema, defaults, validation, UI generation.
- Current Behavior: Settings definitions can diverge by source.
- Intended Completed Behavior: One canonical settings schema drives all UI, validation, defaults, and persistence.
- Missing Pieces: Source-of-truth policy, migration plan, and catalog completeness.
- Technical Approach: Consolidate schema definitions into one authoritative catalog consumed everywhere else.
- Implementation Steps: Audit duplicates; pick canonical schema; migrate panel-local registrations; add schema-level validation and metadata for UI generation.
- Validation Steps: Compare generated settings UI and stored config before and after consolidation.
- Acceptance Criteria: No visible settings definition is owned only by a panel-local duplicate.
- Dependencies: P10-T01.
- Risks / Failure Modes: Schema consolidation can break existing config keys if migration is not explicit.
- Cleanup / Migration Notes where relevant: Add config migrations for renamed or merged settings.
- Observability / Diagnostics Notes where relevant: Report schema drift and unknown-setting usage.
- Rollback / Safety Notes: Support reading old keys during migration windows.
- References / Context: This phase removes one of the major sources of settings inconsistency.
- Example scenarios where useful: A setting appears once, validates once, and persists once no matter how it is edited.

### P10-T03

- Phase ID: P10
- Task ID: P10-T03
- Task Title: Complete settings deep-linking, scope handling, and runtime apply semantics
- Priority: P1
- Category: Settings Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make settings navigation and runtime behavior match what the UI promises.
- Why This Matters Now: Deep links and live-apply semantics still stop early in the current implementation.
- Completion Gap Statement: Settings deep links, restart-required handling, and user/workspace scope application remain partial.
- User / Product Impact: Users can reach settings but not always the exact intended setting state or runtime effect.
- Repository Evidence: [SettingsDeepLinkRouter.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp), [LiveSettingsReactor.cpp](/Users/ryanrentfro/code/markamp/src/core/LiveSettingsReactor.cpp), [WorkspaceSettings.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp)
- Scope: Deep links, section/category focus, workspace scope, restart-required flows, runtime apply and rollback.
- Out of Scope: Theme/UI presentation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/LiveSettingsReactor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Features / Systems / Components: Settings navigation, live apply, workspace scope, restart prompts.
- Current Behavior: Deep links can open settings generally, but not always reach a fully integrated host or runtime target.
- Intended Completed Behavior: Every deep link, scope change, and restart-required setting behaves deterministically.
- Missing Pieces: Host resolution, scope persistence, restart queue handling, and runtime-apply coverage.
- Technical Approach: Couple deep-link routing to the real settings host and make live-apply semantics explicit per schema entry.
- Implementation Steps: Finish host routing; encode setting scopes in schema; apply runtime changes via reactors; surface restart-required state consistently.
- Validation Steps: Navigate from menus and prompts into specific settings, change user/workspace scope, and verify runtime behavior.
- Acceptance Criteria: Settings deep links and runtime apply flows no longer stop at placeholder host or scope behavior.
- Dependencies: P10-T01, P10-T02.
- Risks / Failure Modes: Some settings may need restart semantics even if current UI implies live apply.
- Cleanup / Migration Notes where relevant: Remove settings paths that bypass deep-link routing.
- Observability / Diagnostics Notes where relevant: Log deep-link misses, scope conflicts, and pending-restart keys.
- Rollback / Safety Notes: Keep pending-restart changes staged until explicitly accepted.
- References / Context: This is necessary to make settings feel finished, not just populated.
- Example scenarios where useful: Clicking a “Fix in Settings” action lands on the exact setting and applies the change in the correct scope.

### P10-T04

- Phase ID: P10
- Task ID: P10-T04
- Task Title: Finish settings JSON editor, import/export, and validation error handling
- Priority: P1
- Category: Settings Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make power-user settings paths real rather than weakly validated shortcuts.
- Why This Matters Now: JSON-editor pathways can undermine all other settings improvements if they remain shallow.
- Completion Gap Statement: JSON settings editing still uses weak validation and direct mutation patterns instead of full schema-aware handling.
- User / Product Impact: Advanced settings editing remains risky and inconsistent.
- Repository Evidence: [SettingsJsonEditor.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp), [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp), [SettingsControlModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsControlModel.cpp)
- Scope: JSON parsing, schema validation, formatting, staged edits, import/export, and error messaging.
- Out of Scope: Third-party configuration file formats unrelated to settings.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsControlModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ConfigMigration.cpp`
- Related Features / Systems / Components: JSON editor, advanced settings, import/export, validation.
- Current Behavior: JSON editing still relies on weak formatting and direct mutation shortcuts.
- Intended Completed Behavior: Advanced settings editing is schema-aware, staged, validated, and recoverable.
- Missing Pieces: Real parsing/formatting, validation integration, staged application, and error/recovery flows.
- Technical Approach: Route JSON editing through the same schema and staged-ownership system as the rest of settings.
- Implementation Steps: Replace weak brace counting; implement parser/formatter; integrate schema validation; stage rather than direct-write; add recovery for invalid edits.
- Validation Steps: Edit valid and invalid JSON, import/export settings, and verify precise error handling and staged application.
- Acceptance Criteria: The JSON editor is no longer a backdoor around settings completion rules.
- Dependencies: P10-T01, P10-T02.
- Risks / Failure Modes: Partial schema coverage can make JSON editing stricter or looser than the visible UI.
- Cleanup / Migration Notes where relevant: Remove direct config mutation shortcuts from the JSON editor.
- Observability / Diagnostics Notes where relevant: Record validation failures and recovery usage.
- Rollback / Safety Notes: Always allow reverting to the last valid settings snapshot.
- References / Context: This task finishes the power-user path instead of leaving it as a thin escape hatch.
- Example scenarios where useful: A malformed settings JSON edit is rejected with exact validation reasons and no live-config corruption.

### P10-T05

- Phase ID: P10
- Task ID: P10-T05
- Task Title: Add settings completion and persistence regression gates
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Protect the finished settings lifecycle with durable automated coverage.
- Why This Matters Now: Settings completion can regress quietly through any direct-write shortcut or schema drift.
- Completion Gap Statement: Existing settings tests do not yet guarantee full staged behavior, schema consistency, and restart/apply correctness.
- User / Product Impact: Users can lose trust quickly if settings apply unpredictably.
- Repository Evidence: [test_phase20_file_management.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp), [test_resolved_config.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_resolved_config.cpp), [test_v21_validation_harness.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v21_validation_harness.cpp)
- Scope: Apply/cancel, schema validation, deep links, import/export, workspace scope, restart-required flows, JSON editing.
- Out of Scope: Visual layout tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_resolved_config.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_validation_harness.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Settings, config, validation, tests.
- Current Behavior: Settings regressions can still hide behind partial schema and ownership tests.
- Intended Completed Behavior: Settings lifecycle completion is proven by automated regression coverage.
- Missing Pieces: Cross-scope fixtures, JSON-editor scenarios, restart-required checks, and deep-link tests.
- Technical Approach: Add mixed settings workflows that run through the visible UI contracts and backend ownership model together.
- Implementation Steps: Add staged-behavior tests; add schema-drift checks; validate workspace/user scope persistence; test JSON import/export and restart-required state.
- Validation Steps: Reintroduce direct mutation or empty export behavior and confirm tests fail.
- Acceptance Criteria: Settings completion regressions become blocking and obvious.
- Dependencies: P10-T01 through P10-T04.
- Risks / Failure Modes: Scope-heavy settings tests can become brittle if fixture config state is not isolated carefully.
- Cleanup / Migration Notes where relevant: Remove tests that only prove placeholder audit summaries rather than settings behavior.
- Observability / Diagnostics Notes where relevant: Emit settings lifecycle traces for failed regression runs.
- Rollback / Safety Notes: Keep tests hermetic and avoid mutating developer machine config.
- References / Context: This phase preserves settings completion once the backend contradictions are removed.
- Example scenarios where useful: A workspace-scoped setting change applies only to the workspace, survives restart, and can be exported correctly.
