# Phase 06: Settings Menus Toolbars And Surface Discoverability

## Phase Purpose
Finish settings and top-level interaction surfaces so they are consistent, searchable, deep-linkable, and honest about what they do.

## Measurable Outcome
- Settings deep links actually navigate and focus the right controls.
- Menus and toolbars stop surfacing misleading or partially wired actions.
- High-frequency top-level affordances become consistent with command and state models.

## Tasks

### P06-T01
- Phase ID: P06
- Task ID: P06-T01
- Task Class: Workflow
- Task Title: Complete Settings Deep Link Navigation And Scope Routing
- Priority: High
- Objective: Finish settings-open behavior so deep links actually search, focus, scope-switch, and scroll to the target setting.
- Why This Matters Now: Settings appears present and mature, but deep-link behavior is still mostly logs and intent.
- Problem Statement: `SettingsDeepLinkRouter` logs routes, and `MainFrame` subscribes to `SettingsOpenRequestEvent`, but the end-to-end navigation path is incomplete.
- Scope: settings open, query prefill, scope tabs, category selection, target setting reveal, and focus placement.
- Out of Scope: New settings domains beyond current catalog.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/SettingsDeepLinkRouter.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.h`
- Related Systems / Components: settings panel, query search, scope tabs, activity bar, command palette.
- Current Behavior: Settings can open, but deep-link fields do not reliably drive the actual UI state.
- Intended Behavior: A settings-open request with `setting_id`, `query`, and `scope` deterministically lands the user on the right surface.
- Technical Approach: Give `SettingsDeepLinkRouter` a real host bridge and add explicit API methods to `SettingsPanel` for target reveal and scope selection.
- Implementation Steps:
  1. Define a settings-host interface consumed by the router.
  2. Add setting reveal and scope-switch APIs to `SettingsPanel`.
  3. Remove placeholder logic in `MainFrame` that only republishes or logs.
  4. Add keyboard focus policy for search-first and target-first flows.
- Validation Steps:
  1. Open settings generally, by query, by scope, and by explicit setting ID.
  2. Trigger from menu, palette, and in-app buttons.
- Acceptance Criteria: Settings deep links are real behaviors, not log statements.
- Dependencies: P01-T02, P02-T02.
- Risks / Failure Modes: Category tree selection and scroll positioning may race if the panel is not fully laid out.
- UX Notes: Search-first settings navigation should feel as good as VS Code or better.
- Observability / Diagnostics Notes: Add a settings-route trace with target, scope, focus result, and fallback outcome.
- Rollback / Safety Notes: If target reveal fails, land on search with the query prefilled instead of doing nothing.
- References / Context:
  - `SettingsPanel` already exposes `SetSearchText()`, but routing is not yet completed end to end.
- Example Scenarios Where Useful:
  - `Preferences: Open Font Settings` from the palette should open settings and place the user on `editor.fontSize`.

### P06-T02
- Phase ID: P06
- Task ID: P06-T02
- Task Class: Cleanup
- Task Title: Normalize Toolbar And Header Actions To Match Real Product Capability
- Priority: High
- Objective: Remove or repair top-level affordances whose labels suggest capabilities that are incomplete, duplicated, or context-blind.
- Why This Matters Now: The product already looks feature-rich, so misleading top-level actions are more damaging than missing features.
- Problem Statement: Toolbar buttons, panel header actions, and shell surface affordances are not consistently backed by the same state and command paths.
- Scope: main toolbar, panel headers, activity bar discoverability, contextual shell actions, and surface-local shortcuts shown to users.
- Out of Scope: Deep redesign of the overall visual language.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: toolbar, panel headers, context actions, command dispatcher, shell context.
- Current Behavior: Some top-level actions are static, editor-centric, or not obviously tied to current context.
- Intended Behavior: Visible actions expose only supported behavior and accurately reflect current context and enablement.
- Technical Approach: Bind header and toolbar actions through the shared command dispatcher and context rules, then remove unsupported affordances.
- Implementation Steps:
  1. Audit all top-level actions for real command backing.
  2. Convert supported actions to shared command IDs.
  3. Hide or gate misleading actions until they are truly integrated.
  4. Standardize hover text and shortcut display.
- Validation Steps:
  1. Compare toolbar and header actions across editor, canvas, notebook, and no-workspace states.
  2. Verify disabled-state behavior and tooltip explanations.
- Acceptance Criteria: No major top-level affordance lies about its readiness or context support.
- Dependencies: P02-T02.
- Risks / Failure Modes: Removing visible actions without replacement may reduce discoverability.
- UX Notes: Fewer, accurate actions are preferable to a crowded unreliable chrome.
- Observability / Diagnostics Notes: Instrument action usage before and after cleanup to see which affordances matter most.
- Rollback / Safety Notes: Put demoted affordances behind feature flags rather than deleting immediately.
- References / Context:
  - `Toolbar.cpp` currently owns several static buttons and state assumptions.
- Example Scenarios Where Useful:
  - Opening the app with no active editor should not leave editor-only actions appearing fully actionable.

### P06-T03
- Phase ID: P06
- Task ID: P06-T03
- Task Class: Polish
- Task Title: Standardize Menu Labels Shortcut Strings And Discoverability Copy
- Priority: Medium
- Objective: Remove label drift and inconsistent terminology across command surfaces.
- Why This Matters Now: Once commands are centralized, inconsistent naming becomes a visible polish failure.
- Problem Statement: Menus, palette entries, tooltips, and settings copy still use mixed naming conventions and shortcut strings.
- Scope: command titles, menu labels, tooltip copy, shortcut display text, and panel names.
- Out of Scope: Marketing copy or external documentation.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: command metadata, tooltips, menu system, palette, status bar.
- Current Behavior: Some labels are technical, some are user-facing, and shortcut strings are not always normalized.
- Intended Behavior: The app uses one consistent interaction language across all visible surfaces.
- Technical Approach: Treat command metadata as the primary user-facing copy source where possible.
- Implementation Steps:
  1. Audit visible labels against command metadata.
  2. Normalize inconsistent shortcut formatting and capitalization.
  3. Remove duplicate or conflicting names for the same action.
  4. Add a small copy review checklist for future commands.
- Validation Steps:
  1. Compare representative actions across menu, palette, toolbar, and tooltip surfaces.
  2. Run a terminology diff for common actions like save, reveal, search, and settings.
- Acceptance Criteria: Common actions and surfaces use consistent language and shortcut display.
- Dependencies: P02-T01, P06-T02.
- Risks / Failure Modes: Renaming commands without aliases may affect extension or automation compatibility.
- UX Notes: Consistent language is part of perceived quality, not just documentation.
- Observability / Diagnostics Notes: None beyond command inventory review.
- Rollback / Safety Notes: Preserve stable internal command IDs while cleaning user-facing copy.
- References / Context:
  - Palette text in `MainFrame` and command titles in `WorkbenchCommands.cpp` currently drift.
- Example Scenarios Where Useful:
  - Users should not see `Open Preferences`, `Settings`, and `Preferences` used inconsistently for the same surface.
