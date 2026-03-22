# Phase 04 - Shell Command, Action, Menu, And Control Authority

## Phase Goal

Make every visible control a thin delegate over the same command and action model so enablement, routing, shortcuts, and UI feedback stop diverging.

## Measurable Outcome

- Menus, toolbars, context actions, and keybindings share one action contract.
- Direct widget-only control logic is retired from the critical path.
- Control audits become enforceable release checks.

### Task P04-T01

- Phase ID: `P04`
- Task ID: `P04-T01`
- Task Title: Establish one authoritative action manifest for the shell
- Priority: `P0`
- Category: `Menu / Command Completion`
- Objective: Make all visible control surfaces derive from a single action registry and manifest.
- Why This Matters Now: Control integrity cannot be trusted while surfaces register actions differently.
- Execution Gap Statement: Main menu bindings, toolbar behavior, palette commands, and direct callbacks still diverge.
- User / Product Impact: Users encounter no-op actions, inconsistent enablement, and shortcut drift.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Prior Plan References: `v21` Phase 01-04, `v23` Phase 02
- Scope: Command/action manifest, handler registration, enablement metadata
- Out of Scope: Surface-specific layout polish
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`, `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h`
- Related Features / Systems / Components: Commands, menus, shortcuts, toolbar
- Current Behavior: Multiple command universes coexist.
- Intended Completed Behavior: One action manifest defines labels, handlers, visibility, enablement, shortcuts, and surface presence.
- Missing Pieces: Canonical action manifest adoption and shell-wide binding
- Technical Approach: Promote action manifest to the source of truth and make menus/toolbars/context menus declarative views over it.
- Implementation Steps: Normalize action IDs; unify metadata; bind surfaces from manifest; remove duplicate registrations.
- Validation Steps: Compare menus, palette, and toolbar against the manifest automatically.
- Acceptance Criteria: No primary shell control ships outside the canonical action manifest.
- Dependencies: `P01-T04`
- Parallelization Notes: Blocks most control and settings host work.
- Risks / Failure Modes: Partial adoption can leave hybrid routing that is harder to debug.
- Cleanup / Migration Notes: Remove direct shadow command registries after migration.
- Observability / Diagnostics Notes: Emit manifest coverage and missing-handler reports.
- Rollback / Safety Notes: Support temporary manifest aliases for renamed action IDs.
- References / Context: Existing command and audit models
- Example scenarios where useful: `File > New File`, toolbar new-file button, and shortcut all route through the same action definition.

### Task P04-T02

- Phase ID: `P04`
- Task ID: `P04-T02`
- Task Title: Retire direct widget-only command paths from `MainFrame`, `Toolbar`, and `TabBar`
- Priority: `P0`
- Category: `Cleanup / De-duplication`
- Objective: Remove local logic that bypasses the action model.
- Why This Matters Now: Shell trust depends on uniform routing, not just shared labels.
- Execution Gap Statement: High-visibility controls still perform behavior directly inside widget classes.
- User / Product Impact: Behavior differs depending on where users trigger the same action.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Prior Plan References: `v21` assessment, `v23` control-readiness closure
- Scope: Direct menu binds, toolbar click handlers, tab-bar new-file behavior
- Out of Scope: Non-command low-level paint/input handling
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp`
- Related Features / Systems / Components: UI controls, shell action routing
- Current Behavior: Widgets often publish events or perform behavior directly.
- Intended Completed Behavior: Widgets dispatch canonical action IDs and consume routed state.
- Missing Pieces: Adapter layer and migration plan
- Technical Approach: Convert widgets to action dispatchers with read-only state bindings.
- Implementation Steps: Identify direct handlers; map to action IDs; replace local branches; wire state refresh.
- Validation Steps: Trigger the same action from multiple surfaces and confirm identical handler path and state changes.
- Acceptance Criteria: High-visibility shell controls no longer own business logic directly.
- Dependencies: `P04-T01`
- Parallelization Notes: Can proceed surface by surface once manifest exists.
- Risks / Failure Modes: UI may appear wired but lose instant feedback if state propagation is not updated.
- Cleanup / Migration Notes: Delete duplicate event publications that shadow canonical commands.
- Observability / Diagnostics Notes: Trace action dispatch source surface and resolved handler.
- Rollback / Safety Notes: Maintain temporary fallback only until dispatch parity is proven.
- References / Context: `ToolbarModel` and action auditor
- Example scenarios where useful: Double-clicking empty tab bar dispatches `file.new`, not local untitled-tab creation code.

### Task P04-T03

- Phase ID: `P04`
- Task ID: `P04-T03`
- Task Title: Centralize enablement, visibility, and focus-context evaluation
- Priority: `P0`
- Category: `Enablement / Visibility Logic`
- Objective: Ensure controls are enabled, disabled, or hidden for the same reasons everywhere.
- Why This Matters Now: Shared routing is still not enough if surfaces compute state differently.
- Execution Gap Statement: Focus- and selection-sensitive actions still have inconsistent availability across shell surfaces.
- User / Product Impact: Users see commands enabled where they fail, or hidden where they should work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WhenClause.h`, `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v19` command context contracts, `v21` Phase 04
- Scope: Focus model, selection model, active artifact context, panel and workbench mode context
- Out of Scope: Accessibility hints unrelated to action state
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ActiveDocumentTracker.h`, `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.h`
- Related Features / Systems / Components: Context keys, enablement rules, visibility rules
- Current Behavior: Surfaces infer action state locally and inconsistently.
- Intended Completed Behavior: One state-evaluation layer powers menus, buttons, shortcuts, and palette entries.
- Missing Pieces: Context normalization and shared evaluation consumption
- Technical Approach: Feed action state from context keys and active-shell models rather than widget-local checks.
- Implementation Steps: Define required context keys; bind active shell state; update surface refresh hooks.
- Validation Steps: Snapshot action states while changing focus, selection, active panel, and workbench mode.
- Acceptance Criteria: The same action has the same enablement and visibility result across all surfaces for the same context.
- Dependencies: `P04-T01`, `P02-T04`
- Parallelization Notes: Can advance alongside panel and explorer host work.
- Risks / Failure Modes: Stale context propagation can freeze action states.
- Cleanup / Migration Notes: Remove per-widget enablement branches after central state is trustworthy.
- Observability / Diagnostics Notes: Log state-evaluation inputs for failed control audits.
- Rollback / Safety Notes: Provide state-diff diagnostics before deleting old branches.
- References / Context: Context key and active-document services
- Example scenarios where useful: `Save` is enabled for unsaved notebooks and canvases, not just text editors with filesystem paths.

### Task P04-T04

- Phase ID: `P04`
- Task ID: `P04-T04`
- Task Title: Make context menus and inline actions declarative over the same action model
- Priority: `P1`
- Category: `Menu / Command Completion`
- Objective: Remove bespoke right-click and inline-menu wiring from critical surfaces.
- Why This Matters Now: Context menus remain one of the largest sources of duplicate control logic.
- Execution Gap Statement: Many surfaces still build `wxMenu` objects manually and bypass centralized routing.
- User / Product Impact: Context actions drift from menus, shortcuts, and toolbar behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v21` Phase 04, `v23` Phase 02
- Scope: Explorer, tab, editor, output, source control, terminal, canvas, notebook context menus
- Out of Scope: Surface-specific visual styling beyond required parity indicators
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`
- Related Features / Systems / Components: Context menu registry, target-aware actions
- Current Behavior: Context menus are mostly built per-surface by hand.
- Intended Completed Behavior: Context menus declare target types and action groups, then resolve through the shared action model.
- Missing Pieces: Target-aware declarative menu generation
- Technical Approach: Use surface target descriptors plus action manifest filters to generate menu models.
- Implementation Steps: Define target contracts; migrate high-value surfaces; verify parity with main action system.
- Validation Steps: Compare context menus to canonical action manifest for matching targets.
- Acceptance Criteria: Context actions no longer bypass command routing or enablement logic.
- Dependencies: `P04-T01`, `P04-T03`
- Parallelization Notes: Can run surface-by-surface after action-state work stabilizes.
- Risks / Failure Modes: Missing target context can generate incomplete menus.
- Cleanup / Migration Notes: Remove direct menu assembly code as surfaces migrate.
- Observability / Diagnostics Notes: Record generated menu IDs and dropped actions for diagnostics.
- Rollback / Safety Notes: Keep legacy context menu path only until migrated surfaces are verified.
- References / Context: ContextMenuRegistry and ThemedContextMenu
- Example scenarios where useful: Right-clicking a folder in the explorer shows the same rename/delete/create actions as the command palette would allow for that target.

### Task P04-T05

- Phase ID: `P04`
- Task ID: `P04-T05`
- Task Title: Enforce action readiness in CI and pre-release smoke
- Priority: `P1`
- Category: `Testing / Regression Protection`
- Objective: Turn control integrity from a planning concern into a gate.
- Why This Matters Now: The repository already contains action/panel audits that should now fail unfinished states.
- Execution Gap Statement: Missing or stub handlers are still detectable but not yet consistently blocking.
- User / Product Impact: Dead UI can still reach users.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_control_audit.cpp`, `/Users/ryanrentfro/code/markamp/tests/unit/test_v23_action_readiness.cpp`
- Prior Plan References: `v21` Phase 10, `v23` Phase 19-20
- Scope: Manifest coverage, missing handlers, stub handlers, gated-action policy
- Out of Scope: Full UI screenshot testing
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`, `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`
- Related Features / Systems / Components: Control audit, CI gates, release readiness
- Current Behavior: Audit coverage exists but is not yet a universal release blocker.
- Intended Completed Behavior: Action readiness failures stop merges or release candidates unless explicitly waived.
- Missing Pieces: CI enforcement, allowlist policy, actionable reports
- Technical Approach: Promote audit results to structured CI artifacts with severity thresholds.
- Implementation Steps: Wire tests into release jobs; add reports; add waiver format with expiration.
- Validation Steps: Inject missing-handler and stub-handler cases to verify gate failure.
- Acceptance Criteria: Shipping a dead menu item or stub action requires an explicit tracked waiver.
- Dependencies: `P01-T04`, `P04-T01`
- Parallelization Notes: Can progress incrementally while action migrations land.
- Risks / Failure Modes: Poorly managed waivers can weaken the gate.
- Cleanup / Migration Notes: Retire passive audit-only reporting once CI gates are live.
- Observability / Diagnostics Notes: Publish per-build action readiness summaries.
- Rollback / Safety Notes: Start as warning-plus-report, then promote to hard block.
- References / Context: Existing V21/V23 readiness tests
- Example scenarios where useful: A toolbar button with no manifest action fails the readiness suite before release.
