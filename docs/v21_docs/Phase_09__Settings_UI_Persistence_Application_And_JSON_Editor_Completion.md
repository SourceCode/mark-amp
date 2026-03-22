# Phase 09: Settings UI Persistence Application And JSON Editor Completion

## Outcome

Finish the settings user experience so every settings control saves correctly, applies correctly, restores correctly, and exposes an honest advanced JSON workflow.

## Improvement Count

132 atomic improvements across 6 execution tasks.

### P09-T01

- Phase ID: P09
- Task ID: P09-T01
- Task Title: Rebuild `SettingsPanel` around the canonical staged-settings contract
- Priority: P0
- Category: Settings UI
- Atomic Improvements Covered: 22
- Objective: Make the visible settings panel reflect the chosen settings lifecycle instead of mutating config opportunistically.
- Why This Matters Now: The visible settings surface still embodies the settings-ownership contradiction.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` combines pending-change APIs with direct `config_.set(...)` and `config_.save(...)` calls.
- User Impact: Users cannot predict whether edits are pending, saved, applied, or reversible.
- Scope: Settings field widgets, change tracking, reset buttons, category tree, search results, scope tabs, pending state indicators.
- Out of Scope: Full settings visual redesign beyond correctness and completion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`
- Related Systems / Components: Settings panel, settings dialog, catalog, state owner, config.
- Current Behavior: `SettingsPanel` stages and mutates at the same time.
- Intended Behavior: `SettingsPanel` displays and edits staged values only, with persistence and runtime apply happening only through the canonical owner.
- Technical Approach: Remove direct config mutation from the panel and bind all widgets to staged state plus explicit apply/cancel/revert commands.
- Implementation Steps: Refactor widget binding; replace direct writes with staged updates; update change indicators; normalize reset/default handling; remove hidden save side effects.
- Validation Steps: Edit multiple settings, navigate categories, cancel, apply, reopen, and verify visible values and persisted values remain correct.
- Acceptance Criteria: `SettingsPanel` cannot mutate persisted settings except through the canonical apply path.
- Dependencies: Phase 08.
- Risks / Failure Modes: Some existing widgets may expect immediate side effects for previews and need explicit preview contracts.
- UX Notes: Pending changes must be obvious but unobtrusive.
- Settings / Panel / Control Notes where relevant: This task governs the behavior of every settings control rendered inside the main panel.
- Observability / Diagnostics Notes: Add widget-level staged-change traces and direct-write guard assertions.
- Rollback / Safety Notes: Maintain compatibility adapters for legacy setting widgets during the transition.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` currently calls `config_.save()` inside change handlers.
- Example scenarios where useful: Typing in a font-size field updates pending state, but quitting without Apply leaves persisted settings unchanged.

### P09-T02

- Phase ID: P09
- Task ID: P09-T02
- Task Title: Complete `SettingsDialog` apply, cancel, revert, close-confirmation, and dirty-state behavior
- Priority: P0
- Category: Settings UI
- Atomic Improvements Covered: 22
- Objective: Make the settings dialog trustworthy as a staged editing surface.
- Why This Matters Now: The dialog already advertises rich staged behavior, so any mismatch with actual state handling is especially misleading.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` exposes Apply/Cancel/OK and close-confirmation behavior, but the underlying panel currently writes directly.
- User Impact: Users may cancel changes that have already been persisted or fail to understand what the dialog will keep.
- Scope: Apply/OK/Cancel buttons, close interception, dirty indicators, undo/redo hooks, scope switching, query navigation while dirty.
- Out of Scope: Cross-window multi-host settings editing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.h`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Systems / Components: Settings dialog, settings panel, staged settings owner, deep-link router.
- Current Behavior: Dialog semantics and actual persistence behavior are not fully aligned.
- Intended Behavior: Dialog controls faithfully manage staged changes and clearly communicate whether state is pending, applied, reverted, or abandoned.
- Technical Approach: Bind dialog actions to canonical settings-owner operations and reconcile navigation/close logic with staged state.
- Implementation Steps: Rewire button handlers; unify dirty-state detection; align close-confirmation with actual pending changes; preserve deep-link navigation while dirty.
- Validation Steps: Modify settings, switch categories, close with and without applying, reopen specific deep-linked categories, and verify all resulting states.
- Acceptance Criteria: Dialog close and button behavior match the true state of staged changes in every tested path.
- Dependencies: P09-T01.
- Risks / Failure Modes: Cross-dialog reentrancy or multi-open settings hosts may expose new ownership bugs.
- UX Notes: Confirmation prompts should be rare, clear, and only shown when there is genuine staged data at risk.
- Settings / Panel / Control Notes where relevant: Apply/Cancel/OK/Reset are among the highest-risk settings controls in the app.
- Observability / Diagnostics Notes: Trace dialog lifecycle with dirty state, action taken, and resulting apply/cancel outcome.
- Rollback / Safety Notes: Keep the old close prompt wording behind a compatibility constant only if external documentation depends on it.
- References / Context: The current dialog is richer than the actual underlying state model, which makes it especially risky.
- Example scenarios where useful: User changes three settings, closes the dialog, chooses Discard, and reopens to see none of the changes persisted.

### P09-T03

- Phase ID: P09
- Task ID: P09-T03
- Task Title: Finish settings persistence, restore, import, export, and migration correctness
- Priority: P0
- Category: Settings Persistence
- Atomic Improvements Covered: 22
- Objective: Make settings write/read behavior durable, scoped, and reversible across restarts and imports.
- Why This Matters Now: Even a clean UI is untrustworthy if persistence and restore are incomplete or stub-backed.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` uses simplified persistence, and `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp` still contains placeholder export logic.
- User Impact: Users lose preferences, import/export invalid data, or see settings revert unexpectedly on startup.
- Scope: Save/apply persistence, startup restore, scoped file locations, import/export, reset-to-default, migration from legacy keys, recent profile integrity.
- Out of Scope: Cloud settings sync across machines.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Systems / Components: Settings persistence, scoped config, import/export, startup restore, migration.
- Current Behavior: Persistence and export/import capabilities are partially implemented and partially placeholder-backed.
- Intended Behavior: Applied settings persist at the correct scope, restore correctly on startup, and import/export flows are structurally valid and migration-safe.
- Technical Approach: Complete persistence backends, formalize export/import formats, and add versioned migration handling to the settings stack.
- Implementation Steps: Replace placeholder export logic; finish workspace/project save/load; add import validation and rollback; implement migration mapping for legacy keys; verify restore ordering at startup.
- Validation Steps: Apply settings at different scopes, restart, export, import into a fresh profile, and compare effective values.
- Acceptance Criteria: Settings persistence survives restart and round-trips cleanly through export/import without data loss or scope confusion.
- Dependencies: Phase 08.
- Risks / Failure Modes: Legacy config files may contain malformed or ambiguous values that need careful fallback handling.
- UX Notes: Import/export flows should communicate scope, format, and failure causes clearly.
- Settings / Panel / Control Notes where relevant: Reset and restore behavior must feel safe and reversible.
- Observability / Diagnostics Notes: Add persistence traces with scope, file path, migration version, and validation status.
- Rollback / Safety Notes: Write imports through temporary files and transactional replace behavior where possible.
- References / Context: The current settings stack still contains placeholder export and simplified workspace persistence.
- Example scenarios where useful: User exports settings from one profile, imports them into another, and sees the same effective values after restart.

### P09-T04

- Phase ID: P09
- Task ID: P09-T04
- Task Title: Make settings search, category navigation, and deep-link arrival fully trustworthy
- Priority: P1
- Category: Settings UI
- Atomic Improvements Covered: 22
- Objective: Ensure users can find, filter, and land on settings predictably from every settings entry point.
- Why This Matters Now: A large settings surface is unusable without reliable discovery and navigation.
- Problem Statement: Search, categories, and deep-link commands exist, but they currently sit on top of an architecture that does not consistently route or stage settings.
- User Impact: Users cannot reliably find the intended setting or tell whether they have landed in the right scope/category.
- Scope: Search box behavior, category tree navigation, query highlighting, deep-link focus, settings IDs, scope-aware search results.
- Out of Scope: Full natural-language settings search.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
- Related Systems / Components: Settings UI, settings dialog, deep-link router, settings catalog.
- Current Behavior: Discovery and navigation capabilities exist, but their correctness depends on incomplete host and ownership behavior.
- Intended Behavior: Search and deep links always open to the right control, show the right scope, and preserve staged-state expectations.
- Technical Approach: Drive search and deep-link results from the authoritative settings catalog and staged-state owner.
- Implementation Steps: Normalize setting identifiers; bind search results to catalog entries; implement deterministic scroll/focus-to-setting behavior; preserve dirty-state rules during navigation.
- Validation Steps: Search for settings by label and ID, jump from command palette deep links, switch scopes, and verify navigation plus focus.
- Acceptance Criteria: Users can reliably navigate to the intended setting and understand where they are in the settings hierarchy.
- Dependencies: Phase 08, P09-T01, P09-T02.
- Risks / Failure Modes: Dynamic category visibility or scope filtering may make deep links land on hidden categories if rules are not explicit.
- UX Notes: Search results should reveal both the setting and its category path clearly.
- Settings / Panel / Control Notes where relevant: Search and category selectors are themselves high-risk controls and must obey the same state rules as the rest of the settings UI.
- Observability / Diagnostics Notes: Log settings-search query, result count, selected entry, and deep-link landing success.
- Rollback / Safety Notes: Fallback deep links may open the parent category when a leaf control is temporarily unavailable, but this must be explicit and logged.
- References / Context: `MainFrame.cpp` already publishes search/deep-link style settings-open events that deserve a real arrival path.
- Example scenarios where useful: `Preferences: Search Settings` opens the settings host with the query populated, results filtered, and the first matching setting focused.

### P09-T05

- Phase ID: P09
- Task ID: P09-T05
- Task Title: Replace weak JSON settings editing with a real validated advanced-editor workflow
- Priority: P1
- Category: Settings UI
- Atomic Improvements Covered: 22
- Objective: Make the advanced JSON editor a real expert workflow instead of a weakly validated direct-write escape hatch.
- Why This Matters Now: Advanced settings editing is one of the easiest ways for power users to encounter broken persistence behavior.
- Problem Statement: `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp` performs only simplistic brace/quote validation, fake formatting, and direct config mutation.
- User Impact: Power users can save malformed or misleading settings and lose trust in the entire settings system.
- Scope: JSON parsing, schema validation, formatting, diff/preview, apply/cancel behavior, scoped editing, error presentation.
- Out of Scope: Support for arbitrary alternative config syntaxes.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.h`; `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
- Related Systems / Components: Settings JSON editor, settings catalog, staged settings owner, validation.
- Current Behavior: JSON editing is weakly validated and bypasses the intended settings lifecycle.
- Intended Behavior: JSON editing parses real structured data, validates against the catalog/schema, previews changes, and applies through the same canonical settings owner as the visual UI.
- Technical Approach: Introduce proper parsing and schema-aware validation, then stage JSON edits through the canonical settings owner rather than direct mutation.
- Implementation Steps: Replace ad hoc validation; implement real formatting; add parse and catalog validation diagnostics; stage JSON changes; support apply/cancel/revert and scope-aware editing.
- Validation Steps: Edit valid and invalid JSON, format documents, apply changes, cancel changes, and verify resulting settings state and error messages.
- Acceptance Criteria: Invalid JSON cannot silently save, formatting is real, and JSON edits participate in the same persistence and apply lifecycle as visual settings controls.
- Dependencies: Phase 08, P09-T01, P09-T03.
- Risks / Failure Modes: Schema validation may surface many existing settings irregularities that need triage.
- UX Notes: Error messages should point to exact keys and lines, not generic failure text.
- Settings / Panel / Control Notes where relevant: The JSON editor is a control surface and must not be allowed to bypass canonical ownership.
- Observability / Diagnostics Notes: Add parse, validation, and apply-result traces with failing keys and line numbers where available.
- Rollback / Safety Notes: Keep the editor read-only or gated if validation completeness lags behind the UI rollout.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp` currently uses minimal validation and fake format behavior.
- Example scenarios where useful: User pastes invalid JSON, sees a precise parse error, fixes it, and then applies the resulting staged settings through the same canonical owner as the dialog.

### P09-T06

- Phase ID: P09
- Task ID: P09-T06
- Task Title: Add settings UI and persistence smoke coverage across all major control paths
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 22
- Objective: Make settings-control behavior, persistence, and deep-link correctness release-gated.
- Why This Matters Now: Settings are now broad enough that manual testing will not catch architecture drift or subtle staged-state regressions.
- Problem Statement: Without focused coverage, the settings stack can appear polished while still saving incorrectly or routing to the wrong host.
- User Impact: Broken settings flows erode trust everywhere because users cannot stabilize the app to their preferences.
- Scope: Visual settings controls, Apply/Cancel/OK, deep links, search/filter, import/export, JSON editor, user/workspace scope, live-apply behavior, restart-required messaging.
- Out of Scope: Exhaustive validation of every downstream feature affected by settings.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`
- Related Systems / Components: Settings UI, settings persistence, deep links, diagnostics, config.
- Current Behavior: Settings correctness is not comprehensively protected by focused smoke tests.
- Intended Behavior: CI catches direct-write regressions, broken staging, missing persistence, bad deep links, and malformed JSON-editor behavior before release.
- Technical Approach: Build settings smoke flows around canonical setting keys, scope fixtures, and staged/apply lifecycle assertions.
- Implementation Steps: Add per-surface test cases; verify staged state and persistence separately; add import/export round-trip checks; add JSON validation tests; wire into CI.
- Validation Steps: Run the settings suite against clean profile, restored profile, and workspace-scoped fixtures.
- Acceptance Criteria: Settings UI and persistence regressions become release-blocking with clear failure diagnostics.
- Dependencies: P09-T01 through P09-T05.
- Risks / Failure Modes: Tests may become brittle if they depend on widget layout instead of catalog-driven identifiers.
- UX Notes: Include keyboard-only flows for category navigation, search, and Apply/Cancel controls.
- Settings / Panel / Control Notes where relevant: Smoke coverage must treat settings controls as first-class high-risk controls, not just backend configuration.
- Observability / Diagnostics Notes: Attach staged-state snapshots, scope-resolution traces, and deep-link-routing results to failing tests.
- Rollback / Safety Notes: Keep tests aligned to stable setting IDs and catalog entries so UI restructuring does not invalidate them unnecessarily.
- References / Context: v21 defines settings completeness as a release criterion, so settings need dedicated harnesses, not incidental coverage.
- Example scenarios where useful: CI opens the dialog from the command palette deep link, changes a workspace setting, cancels it, applies a user setting, edits JSON, restarts, and verifies the effective values.
