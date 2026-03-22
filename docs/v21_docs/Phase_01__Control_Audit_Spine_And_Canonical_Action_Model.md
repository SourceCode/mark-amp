# Phase 01: Control Audit Spine And Canonical Action Model

## Outcome

Create one authoritative action inventory and one canonical control-execution model so menus, buttons, panels, and settings surfaces stop diverging by widget and surface.

## Improvement Count

126 atomic improvements across 6 execution tasks.

### P01-T01

- Phase ID: P01
- Task ID: P01-T01
- Task Title: Build a full control-surface inventory with stable action IDs
- Priority: P0
- Category: Command Routing
- Atomic Improvements Covered: 21
- Objective: Inventory every menu item, button, toggle, status action, panel action, and settings control and assign a canonical action ID.
- Why This Matters Now: The repository currently exposes too many ad hoc entry points to reason about manually.
- Problem Statement: Menus, toolbar buttons, palette commands, and context actions are registered in multiple incompatible places.
- User Impact: Users encounter inconsistent behavior by surface.
- Scope: Main menu, palette, toolbar, status bar, panel headers, context menus, empty-state CTAs, settings controls.
- Out of Scope: Implementation of every action itself.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`
- Related Systems / Components: Commands, menus, toolbar, context menus, settings.
- Current Behavior: Control registration is scattered and partially duplicated.
- Intended Behavior: Every visible control maps to one action ID and one source-of-truth behavior record.
- Technical Approach: Generate or maintain a control manifest backed by action metadata and source references.
- Implementation Steps: Inventory controls; assign IDs; classify surfaces; record handler presence and status rules.
- Validation Steps: Diff the manifest against code search and UI screenshots.
- Acceptance Criteria: No visible control lacks a canonical action ID and owner.
- Dependencies: None.
- Risks / Failure Modes: Hidden controls in lazily created panels may be missed.
- UX Notes: Inventory should capture label, icon, tooltip, and expected feedback.
- Settings / Panel / Control Notes where relevant: Include settings scope selectors and panel header actions explicitly.
- Observability / Diagnostics Notes: Emit inventory completeness reports in CI.
- Rollback / Safety Notes: Keep the manifest additive and non-invasive initially.
- References / Context: `MainFrame.cpp`, `Toolbar.cpp`, and multiple panels currently define their own action models.
- Example scenarios where useful: `File > Save`, toolbar save, tab context-menu save, and palette save all share one action ID.

### P01-T02

- Phase ID: P01
- Task ID: P01-T02
- Task Title: Consolidate action execution behind the command registry instead of widget-local callbacks
- Priority: P0
- Category: Command Routing
- Atomic Improvements Covered: 21
- Objective: Make the command registry the actual execution backbone for visible controls.
- Why This Matters Now: Direct widget callbacks are a primary source of drift and no-op behavior.
- Problem Statement: `MainFrame.cpp`, `Toolbar.cpp`, and panels still execute behavior directly.
- User Impact: The same user intent behaves differently depending on how it is triggered.
- Scope: Menu handlers, toolbar clicks, status actions, panel buttons, palette commands.
- Out of Scope: Extension contribution redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Systems / Components: Command registry, menu model, palette, toolbar.
- Current Behavior: The registry exists but many visible controls bypass it.
- Intended Behavior: Visible controls invoke canonical commands, not custom widget lambdas.
- Technical Approach: Introduce a thin UI-action dispatcher that binds control surfaces to command IDs.
- Implementation Steps: Map controls to commands; replace direct callbacks; preserve context payload support.
- Validation Steps: Invoke identical actions from multiple surfaces and compare dispatch traces.
- Acceptance Criteria: No high-value global control bypasses the canonical command path.
- Dependencies: P01-T01.
- Risks / Failure Modes: Context payloads may be lost if the dispatcher is too generic.
- UX Notes: Error reporting should be consistent across surfaces once unified.
- Settings / Panel / Control Notes where relevant: Settings and panel toggle actions need the same treatment.
- Observability / Diagnostics Notes: Log command source surface and resolved target context.
- Rollback / Safety Notes: Migrate in slices and keep compatibility aliases temporarily.
- References / Context: `WorkbenchCommands.cpp` and manual `RegisterCommand` usage currently coexist.
- Example scenarios where useful: A menu click and a toolbar click for Settings both dispatch the same command.

### P01-T03

- Phase ID: P01
- Task ID: P01-T03
- Task Title: Define canonical enablement and visibility contracts for all controls
- Priority: P0
- Category: Enablement / Visibility Logic
- Atomic Improvements Covered: 21
- Objective: Centralize when controls are visible, enabled, checked, or hidden.
- Why This Matters Now: Incorrect enablement is one of the easiest ways to ship misleading UI.
- Problem Statement: Enablement is currently split across widget-local logic and event assumptions.
- User Impact: Users click disabled-by-mistake or enabled-by-mistake actions.
- Scope: Global actions, artifact-aware actions, panel-aware actions, settings scope controls, context-menu items.
- Out of Scope: Final accessibility polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActiveContextService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Systems / Components: Context keys, active context, commands, menus, tabs.
- Current Behavior: Enablement logic is not one clearly inspectable system.
- Intended Behavior: Control state derives from canonical context and command rules.
- Technical Approach: Extend command metadata or companion state rules with explicit predicates.
- Implementation Steps: Define state schema; wire active context; update menu and toolbar refresh logic; add checked-state support.
- Validation Steps: Change focus across editor, notebook, canvas, explorer, and panels while inspecting control states.
- Acceptance Criteria: Control state is correct and reproducible from context data.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Refresh churn or stale state if listeners are incomplete.
- UX Notes: Disabled actions should remain informative, not arbitrary.
- Settings / Panel / Control Notes where relevant: Settings Apply/Cancel/Reset states should use the same rule framework.
- Observability / Diagnostics Notes: Add a control-state inspector in debug builds.
- Rollback / Safety Notes: Keep state derivation declarative to avoid hidden side effects.
- References / Context: Toolbar and menu items currently rely on separate assumptions.
- Example scenarios where useful: `Run Cell` is enabled only when a runnable notebook cell is active.

### P01-T04

- Phase ID: P01
- Task ID: P01-T04
- Task Title: Introduce control execution tracing and dead-affordance detection
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Detect controls that render but do not execute a real workflow.
- Why This Matters Now: v21 treats dead UI as a release blocker.
- Problem Statement: A visible control can currently exist without clear execution evidence.
- User Impact: Users click controls that do nothing or partially do something.
- Scope: Control activation logs, no-op detection, missing-handler detection, duplicate-handler detection.
- Out of Scope: Full analytics rollout.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: Commands, diagnostics, control manifest.
- Current Behavior: The app logs many things, but not every control activation as a structured path.
- Intended Behavior: Every activation path can be traced from affordance to result.
- Technical Approach: Add structured control-activation events with result states.
- Implementation Steps: Instrument action dispatch; flag no handler / no state change / error result cases; surface reports in debug and CI.
- Validation Steps: Trigger representative controls and inspect traces.
- Acceptance Criteria: Dead controls are mechanically discoverable.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Overly noisy logs.
- UX Notes: Diagnostics must not leak into normal user workflows.
- Settings / Panel / Control Notes where relevant: Settings controls should log stage/apply/persist/apply-runtime separately.
- Observability / Diagnostics Notes: This is the backbone for later smoke tests.
- Rollback / Safety Notes: Gate verbose traces behind diagnostics settings.
- References / Context: Broken controls have already survived into production-state UI.
- Example scenarios where useful: Clicking a Git menu item without a bound handler is reported as dead UI.

### P01-T05

- Phase ID: P01
- Task ID: P01-T05
- Task Title: Normalize labels, tooltips, icons, and shortcut metadata across controls
- Priority: P1
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 21
- Objective: Make visible action metadata consistent with actual execution behavior.
- Why This Matters Now: Mislabelled or mismatched controls are another form of broken UI.
- Problem Statement: Control metadata is currently defined in many places and can drift.
- User Impact: Users learn the wrong labels, shortcuts, or expectations.
- Scope: Action labels, tooltips, icon semantics, shortcut hints, checked labels, panel titles.
- Out of Scope: Full design-system restyle.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/IconSemanticMapper.cpp`
- Related Systems / Components: Toolbar, menus, panel headers, icon system, shortcuts.
- Current Behavior: Metadata is partly hardcoded by widget and partly by commands.
- Intended Behavior: The same action has the same name, tooltip, icon, and shortcut hint everywhere.
- Technical Approach: Store visible action metadata with the canonical action model and feed all surfaces from it.
- Implementation Steps: Audit metadata; remove duplicates; map icons and tooltips centrally; update surfaces.
- Validation Steps: Compare the same action across menu, toolbar, panel header, and palette.
- Acceptance Criteria: No user-facing action metadata conflicts across surfaces.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Some surfaces may need concise aliases but not conflicting names.
- UX Notes: Metadata should aid discoverability and trust.
- Settings / Panel / Control Notes where relevant: Settings category selectors and panel actions need the same semantic cleanup.
- Observability / Diagnostics Notes: Add metadata consistency checks to the control manifest.
- Rollback / Safety Notes: Preserve old labels as hidden aliases only where compatibility matters.
- References / Context: Toolbar labels and menu labels currently come from different sources.
- Example scenarios where useful: “Open Preferences” is not called “Settings” in one place and “Preferences” in another without rationale.

### P01-T06

- Phase ID: P01
- Task ID: P01-T06
- Task Title: Create a release-gating control audit report
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Turn the control inventory into an actionable completion report for implementation teams.
- Why This Matters Now: The repository needs one place that says which controls are live, partial, dead, duplicate, or misleading.
- Problem Statement: Without a release-gating report, dead UI can be normalized.
- User Impact: Product completeness claims become unreliable.
- Scope: Control status matrix, blocker classification, ownership, validation status.
- Out of Scope: End-user documentation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v21_docs`; `/Users/ryanrentfro/code/markamp/src/ui`; `/Users/ryanrentfro/code/markamp/src/core`
- Related Systems / Components: Docs, diagnostics, CI, action manifest.
- Current Behavior: Control coverage is discoverable only by code archaeology.
- Intended Behavior: A generated report shows exactly what remains dead or partial.
- Technical Approach: Generate the report from the control manifest and runtime traces.
- Implementation Steps: Define statuses; export report; attach to CI artifacts; require review for unresolved P0/P1 dead controls.
- Validation Steps: Intentionally leave one action dead and verify the report catches it.
- Acceptance Criteria: The team can see unresolved control integrity work at a glance.
- Dependencies: P01-T01 through P01-T05.
- Risks / Failure Modes: Report becomes stale if not generated automatically.
- UX Notes: This is an internal quality tool, not a user-facing feature.
- Settings / Panel / Control Notes where relevant: Report should segment controls, panels, and settings.
- Observability / Diagnostics Notes: Use structured output suitable for automation.
- Rollback / Safety Notes: Keep reporting non-destructive and generation-only.
- References / Context: v21 needs authoritative execution guidance, not more subjective auditing.
- Example scenarios where useful: CI reports that 11 menu items are still unbound and 4 panel actions still route to placeholders.

