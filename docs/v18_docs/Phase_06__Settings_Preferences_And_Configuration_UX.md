# Phase 06: Settings Preferences And Configuration UX

## Phase Goal
Make settings feel intentional, deep-linkable, scope-aware, and truly connected to live product behavior.

## Tasks
### Task 1
- Phase ID: P06
- Task ID: P06-T01
- Task Title: Wire SettingsOpenRequest Deep Links End To End
- Objective: Make setting IDs, queries, and scopes actually drive the visible settings UI.
- Problem Statement: `MainFrame` logs deep-link data but does not complete navigation into the settings surface.
- Scope: `SettingsOpenRequestEvent`, sidebar/settings host, search prefill, scroll-to-setting, scope selection.
- Out of Scope: Rebuilding settings taxonomy.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: EventBus, settings panel/dialog, activity bar settings surface.
- Current Behavior: Deep-link requests do not fully materialize in the UI.
- Intended Behavior: Any settings request can open the right host, preselect scope, search, and focus the requested setting.
- Technical Approach: Add a real shell mediator between the event and the active settings host.
- Implementation Steps:
  1. Choose canonical settings host surface.
  2. Route request event there.
  3. Implement query/setting/scope application order.
  4. Return focus correctly when closed.
- Edge Cases / Failure Modes: Request arrives before settings host exists; setting ID not found; request while modal dialog open.
- UX Considerations: Deep links should feel instant and precise.
- Dependencies: P02-T02 and P03-T03.
- Validation Steps: Fire requests with setting ID, query only, and scope only.
- Acceptance Criteria: Settings deep links visibly work.
- Rollback / Safety Notes: Fall back to general settings landing state when a target is missing.
- References / Context: `MainFrame.cpp` handler currently stops at logs and sidebar selection.
- Example interactions or usage scenarios where helpful: Choosing “Preferences: Open Keybinding Settings” should land directly in the relevant settings area.

### Task 2
- Phase ID: P06
- Task ID: P06-T02
- Task Title: Attach Config To SettingsCatalog And Scope Aware Settings Models
- Objective: Make the live config system consume the settings catalog rather than operating as a parallel world.
- Problem Statement: `Config` supports `SettingsCatalog`, but bootstrap does not currently show repository-level wiring.
- Scope: Catalog registration, config validation/defaults, settings panel catalog mode, workspace/project scopes via `ScopedConfig`.
- Out of Scope: Inventing new settings.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Config.h`
  - `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ScopedConfig.cpp`
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Related Systems / Components: Config, SettingsCatalog, ScopedConfig, WorkspaceSettings, settings UI.
- Current Behavior: Catalog infrastructure exists, but bootstrap-level integration is unclear or absent.
- Intended Behavior: Settings defaults, validation, search, and scopes all reflect the same underlying schema.
- Technical Approach: Create one catalog at bootstrap, attach it to config/scoped config consumers, and feed the settings UI from it.
- Implementation Steps:
  1. Create and register built-in catalog on startup.
  2. Attach catalog to config and settings UI.
  3. Add scope-aware reads/writes where applicable.
  4. Remove duplicate hardcoded settings definitions.
- Edge Cases / Failure Modes: Missing schema entries; conflicting plugin setting contributions; workspace overrides without app defaults.
- UX Considerations: Settings must show correct defaults and scope origin.
- Dependencies: Phase 01 shared contract work.
- Validation Steps: Verify default resolution, validation rejection, and scope switching.
- Acceptance Criteria: One settings schema drives both storage and UI.
- Rollback / Safety Notes: Keep backward compatibility for existing persisted keys.
- References / Context: `Config::set_catalog()` exists but current bootstrap usage is not evident.
- Example interactions or usage scenarios where helpful: Changing a setting in workspace scope should reflect the effective value and origin immediately.

### Task 3
- Phase ID: P06
- Task ID: P06-T03
- Task Title: Make Settings Changes Live And Observable Across The Workbench
- Objective: Ensure settings edits update the real affected UI and services, not just persisted config.
- Problem Statement: Settings UI can stage/apply values, but not every subsystem necessarily reacts live and consistently.
- Scope: Theme, density, sidebar behavior, editor settings, preview/canvas toggles, AI provider settings where safe, status updates.
- Out of Scope: Settings requiring restart when already marked as such.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: SettingChangedEvent, theme engine, shell layout, editor panels.
- Current Behavior: Some settings are event-driven, but coverage and consistency need explicit integration.
- Intended Behavior: Applying a setting updates the actual subsystem promptly and predictably.
- Technical Approach: Define live-apply adapters per subsystem and standardize event emission from settings changes.
- Implementation Steps:
  1. Inventory settings with live effect.
  2. Map each to subscriber/update path.
  3. Add missing subscribers or direct apply hooks.
  4. Surface restart-required state distinctly where needed.
- Edge Cases / Failure Modes: Setting invalid for current workspace; partial apply failure; cascading theme relayout.
- UX Considerations: The user should see what changed and whether restart is required.
- Dependencies: P06-T02 and P09-T01.
- Validation Steps: Change representative settings and verify live UI updates.
- Acceptance Criteria: Settings feel connected to the product rather than to a config file only.
- Rollback / Safety Notes: Do not live-apply high-risk provider changes without validation.
- References / Context: `MainFrame` already listens for some setting changes like density profile.
- Example interactions or usage scenarios where helpful: Changing density or word wrap should update visible surfaces immediately.

### Task 4
- Phase ID: P06
- Task ID: P06-T04
- Task Title: Unify Settings Sidebar Dialog And JSON Editor Entry Points
- Objective: Prevent settings from behaving like multiple disconnected products.
- Problem Statement: Sidebar settings, modal dialog settings, and JSON settings editing can drift in ownership and behavior.
- Scope: Entry points, unsaved indicator, apply/cancel semantics, import/export, JSON editor round trip.
- Out of Scope: Removal of useful power-user entry points.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: settings host, dialog stack, settings JSON editor, command routing.
- Current Behavior: Multiple settings surfaces exist without a clear canonical relationship.
- Intended Behavior: Users can enter settings in multiple ways, but all entry points share state, navigation, and apply semantics.
- Technical Approach: Choose canonical settings state owner and make alternate surfaces adapters over it.
- Implementation Steps:
  1. Define primary host and secondary hosts.
  2. Share staged state and change tracking.
  3. Normalize import/export and undo/redo behavior.
  4. Align focus and close behavior.
- Edge Cases / Failure Modes: JSON editor bypassing validation; two settings surfaces open simultaneously.
- UX Considerations: Advanced entry points should not surprise users with different persistence rules.
- Dependencies: P06-T01 and P06-T02.
- Validation Steps: Open settings from toolbar, menu, palette, and JSON editor; compare state behavior.
- Acceptance Criteria: Settings entry points are coherent adapters over one model.
- Rollback / Safety Notes: Keep JSON editor for power users but validate before apply.
- References / Context: Settings dialog and panel already support staged edits and undo/redo.
- Example interactions or usage scenarios where helpful: Search in settings sidebar, then open JSON editor, then return without losing context or staging unexpectedly.

### Task 5
- Phase ID: P06
- Task ID: P06-T05
- Task Title: Surface Workspace And Project Level Configuration Clearly
- Objective: Make scope origin and effective value understandable in the UI.
- Problem Statement: Workspace settings infrastructure exists, but users need visible scope context and override behavior.
- Scope: App/workspace/project scopes, effective value display, reset behavior, conflict resolution messaging.
- Out of Scope: Enterprise policy systems.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ScopedConfig.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: WorkspaceSettings, ScopedConfig, settings UI, setting change events.
- Current Behavior: Scope support exists in core but is not yet guaranteed to be fully legible in the UI.
- Intended Behavior: Users can tell which scope owns a setting and what value will actually apply.
- Technical Approach: Add explicit scope badges, effective value resolution, and override/reset controls to settings UI.
- Implementation Steps:
  1. Expose scope origin from config models.
  2. Render origin and effective value in settings rows.
  3. Add reset/remove override actions.
  4. Validate event propagation on scope changes.
- Edge Cases / Failure Modes: Invalid workspace config; missing project config; fallback chains with no explicit value.
- UX Considerations: Scope explanation must be concise and obvious.
- Dependencies: P06-T02.
- Validation Steps: Set values in multiple scopes and verify displayed effective value and reset behavior.
- Acceptance Criteria: Scope-aware settings are understandable and trustworthy.
- Rollback / Safety Notes: Preserve old config format compatibility.
- References / Context: `WorkspaceSettings` and `ScopedConfig` already model override chains.
- Example interactions or usage scenarios where helpful: A workspace word-wrap override should visibly override the app default until reset.

