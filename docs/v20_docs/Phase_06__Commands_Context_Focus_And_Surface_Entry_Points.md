# Phase 06: Commands Context Focus And Surface Entry Points

## Outcome

Unify command routing, focus context, menus, palette, toolbars, shortcuts, context menus, and empty-state actions so every artifact workflow enters and behaves through the same product rules.

## Improvement Count

204 atomic improvements across 6 execution tasks.

### P06-T01

- Phase ID: P06
- Task ID: P06-T01
- Task Title: Consolidate creation and lifecycle commands into one canonical command model
- Priority: P0
- Category: Command / Event Wiring
- Atomic Improvements Covered: 34
- Objective: Stop registering creation and lifecycle behavior separately in palette code, menus, and widgets.
- Why This Matters Now: Command duplication is one reason core creation paths diverged.
- Problem Statement: `MainFrame` still manually registers commands that bypass deeper command services.
- User Impact: Different entry points can trigger different code paths for the same user intent.
- Scope: New/open/save/rename/duplicate/delete commands for files, notebooks, and boards; command metadata; enablement.
- Out of Scope: Third-party command extension API redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.cpp`
- Related Systems / Components: Command registry, palette, menus, toolbar, shortcuts.
- Current Behavior: Commands are duplicated and sometimes invoke direct callbacks instead of shared actions.
- Intended Behavior: One command definition resolves one shell-aware action regardless of entry point.
- Technical Approach: Centralize command descriptors and action dispatch behind artifact-aware services.
- Implementation Steps: Audit duplicates; add canonical IDs; migrate palette and menus; remove widget-local behaviors.
- Validation Steps: Invoke the same command from menu, shortcut, and palette and compare traces.
- Acceptance Criteria: New/open/save commands resolve through one command path.
- Dependencies: Phase 01.
- Risks / Failure Modes: Partial migration leaves stale menu handlers.
- UX Notes: Command names and categories should become more discoverable and consistent.
- Styling / Highlighting Notes where relevant: Command labels and icons need shared semantics.
- Observability / Diagnostics Notes: Log command source, resolved target artifact, and execution result.
- Rollback / Safety Notes: Keep legacy alias commands temporarily but route them to canonical implementations.
- References / Context: `MainFrame.cpp` manually registers `New File`, `New Notebook`, and `Canvas: New Board`.
- Example scenarios where useful: `Cmd+N` and the palette `New File` command use the exact same backend.

### P06-T02

- Phase ID: P06
- Task ID: P06-T02
- Task Title: Rebuild active-context propagation across editor, notebook, canvas, panels, and shell
- Priority: P0
- Category: Docking / Panels
- Atomic Improvements Covered: 34
- Objective: Make the active artifact, active surface, active selection, and active focus target agree everywhere.
- Why This Matters Now: Command correctness depends on a reliable context model.
- Problem Statement: Surface switching, tab switching, and panel focus currently do not share one authoritative context chain.
- User Impact: Save, copy, search, and context-menu actions can hit the wrong target.
- Scope: Focus events, selection context, shell context keys, panel activation, active artifact updates.
- Out of Scope: Full accessibility tree redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Systems / Components: Shell controller, tabs, panels, command enablement.
- Current Behavior: Focus and active context are fragmented by widget and subsystem.
- Intended Behavior: The workbench can answer “what is active?” deterministically at all times.
- Technical Approach: Introduce context propagation contracts tied to shell artifact state.
- Implementation Steps: Define active-context object; adapt editor/notebook/canvas hosts; sync tab activation; update panel listeners.
- Validation Steps: Move focus between code editor, notebook cell, canvas, explorer, and search panel while invoking commands.
- Acceptance Criteria: Command targeting follows visible focus and active artifact consistently.
- Dependencies: P06-T01, Phase 01.
- Risks / Failure Modes: Context churn causing flicker or repeated events.
- UX Notes: Focus should be visible and understandable, especially after creation flows.
- Styling / Highlighting Notes where relevant: Active, focused, and selected visual states must become distinct and consistent.
- Observability / Diagnostics Notes: Add context-change timeline logs in debug builds.
- Rollback / Safety Notes: Provide context inspector tooling for migration debugging.
- References / Context: Current shell controller owns mode but not full artifact or focus context.
- Example scenarios where useful: User clicks explorer, then notebook cell, then canvas object and save/search target changes correctly each time.

### P06-T03

- Phase ID: P06
- Task ID: P06-T03
- Task Title: Normalize all creation entry points including menus, toolbars, context menus, shortcuts, and empty states
- Priority: P0
- Category: Core Creation Flow
- Atomic Improvements Covered: 34
- Objective: Ensure every visible affordance for creating artifacts points to the same workflow.
- Why This Matters Now: Product discoverability depends on entry-point consistency.
- Problem Statement: Some entry points are present but misleading or dead.
- User Impact: Users cannot build reliable habits in the application.
- Scope: Main menu, toolbar, palette, explorer context menu, tab bar button, welcome screen, panel empty states, keyboard shortcuts.
- Out of Scope: Plugin-defined custom creation entries.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Systems / Components: Commands, toolbar, explorer, tabs, welcome UI.
- Current Behavior: Creation affordances are redundant but not equivalent.
- Intended Behavior: Every create affordance triggers the same artifact creation flow with entry-point-specific placement hints only.
- Technical Approach: Audit and rewire all creation affordances to canonical commands.
- Implementation Steps: Inventory affordances; replace dead/misleading actions; add missing notebook/canvas create entry points; unify keyboard shortcuts.
- Validation Steps: Test every visible create affordance in the shell.
- Acceptance Criteria: No create affordance bypasses the canonical command path.
- Dependencies: P06-T01, Phase 02, Phase 03, Phase 04.
- Risks / Failure Modes: Overlooked affordances leave inconsistent behavior behind.
- UX Notes: Empty states should become first-class workflow surfaces, not decorative dead ends.
- Styling / Highlighting Notes where relevant: Affordances need consistent iconography and hierarchy.
- Observability / Diagnostics Notes: Track create command source by surface.
- Rollback / Safety Notes: Leave compatibility aliases but not alternate implementations.
- References / Context: `MainFrame.cpp`, `TabBar.cpp`, and `FileTreeCtrl.cpp` all currently own part of create behavior.
- Example scenarios where useful: User right-clicks explorer, hits palette, or clicks the welcome CTA and gets the same notebook creation flow.

### P06-T04

- Phase ID: P06
- Task ID: P06-T04
- Task Title: Repair command enablement, context menus, and shortcut behavior by artifact type
- Priority: P1
- Category: Command / Event Wiring
- Atomic Improvements Covered: 34
- Objective: Make actions appear only when valid and target the current artifact correctly.
- Why This Matters Now: Broken enablement makes the IDE feel unreliable even when functions exist.
- Problem Statement: Context-key and command systems are not yet fully connected to shell artifact state.
- User Impact: Users see irrelevant actions or action failures.
- Scope: Enablement rules, when-clauses, shortcut routing, context menus for file/notebook/board.
- Out of Scope: Extension-specific menu ecosystems.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService*`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Context keys, menu model, shortcuts, palette.
- Current Behavior: Action availability is likely inconsistent across surfaces.
- Intended Behavior: Menus and shortcuts are artifact-aware, focus-aware, and predictable.
- Technical Approach: Populate context keys from shell active state and reuse them for enablement across UI surfaces.
- Implementation Steps: Define context schema; wire update sources; rebuild context menus; test shortcut precedence; remove stale actions.
- Validation Steps: Inspect menus and shortcuts in editor, notebook, canvas, explorer, and search.
- Acceptance Criteria: Invalid actions are hidden or disabled consistently with clear rationale.
- Dependencies: P06-T01, P06-T02.
- Risks / Failure Modes: Context rules become too complex to debug.
- UX Notes: Disabled actions should still teach discoverability when appropriate.
- Styling / Highlighting Notes where relevant: Disabled, hovered, and destructive menu states need consistent styling.
- Observability / Diagnostics Notes: Add a context-debug overlay or log dump.
- Rollback / Safety Notes: Keep command fallbacks visible in debug tooling only.
- References / Context: Artifact-aware command targeting is currently too implicit.
- Example scenarios where useful: Rename board is visible in canvas context menu but not in plain text editor context menu.

### P06-T05

- Phase ID: P06
- Task ID: P06-T05
- Task Title: Align panel activation, docking behavior, and focus restoration with artifact workflows
- Priority: P1
- Category: Docking / Panels
- Atomic Improvements Covered: 34
- Objective: Stop panels from breaking flow during create/open/save transitions.
- Why This Matters Now: Artifact workflows depend on stable focus and predictable panel visibility.
- Problem Statement: Panel activation and shell changes are not fully coordinated around artifacts.
- User Impact: Search, explorer, outline, or status panels may interrupt editing or hide important feedback.
- Scope: Explorer activation after create, properties/inspector activation, status updates, bottom panel behavior, focus return after dialogs.
- Out of Scope: Entire docking system rewrite.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
- Related Systems / Components: Docking, explorer, search, status, shell.
- Current Behavior: Panel behavior likely varies by surface and action.
- Intended Behavior: Panels support workflows without stealing ownership of the active artifact.
- Technical Approach: Tie panel activation and focus-return logic to shell context transitions.
- Implementation Steps: Define panel activation rules; normalize focus return; update create/save/search flows; fix bottom-panel and sidebar consistency.
- Validation Steps: Create artifacts, rename them, search in them, and save them while panels are open and docked differently.
- Acceptance Criteria: Panels no longer derail artifact workflows.
- Dependencies: P06-T02.
- Risks / Failure Modes: Focus ping-pong between panels and artifacts.
- UX Notes: Side panels should help users complete workflows, not fragment them.
- Styling / Highlighting Notes where relevant: Active panel chrome should be quieter than active artifact chrome.
- Observability / Diagnostics Notes: Log focus-return decisions after dialogs and panel actions.
- Rollback / Safety Notes: Keep fallback focus target if preferred artifact target is unavailable.
- References / Context: Prior passes already identified docking and panel lifecycle inconsistency.
- Example scenarios where useful: After Save As, focus returns to the new active editor or notebook cell, not to the explorer.

### P06-T06

- Phase ID: P06
- Task ID: P06-T06
- Task Title: Add command-routing and focus-model validation harnesses
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 34
- Objective: Catch context and command regressions quickly as workflows are unified.
- Why This Matters Now: Command routing bugs often surface only in multi-surface scenarios.
- Problem Statement: Visual testing alone will not catch wrong-target command execution.
- User Impact: Users lose trust when familiar shortcuts behave inconsistently.
- Scope: Command source matrix, focus shifts, context-key assertions, panel interaction smoke cases, keyboard-first workflows.
- Out of Scope: Full plugin command matrix.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchCommands.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`
- Related Systems / Components: Tests, commands, focus, shell.
- Current Behavior: Cross-surface command correctness is likely under-tested.
- Intended Behavior: CI verifies that commands target the right artifact in realistic focus scenarios.
- Technical Approach: Add shell-context fixtures and command-execution assertions.
- Implementation Steps: Create focus-transition helpers; test save/new/search/copy/delete/rename; assert correct target artifact IDs.
- Validation Steps: Run command-routing suite locally and in CI.
- Acceptance Criteria: Wrong-target command bugs become reproducible and release-blocking.
- Dependencies: P06-T01 through P06-T05.
- Risks / Failure Modes: Tests may be brittle if focus hooks are not explicit.
- UX Notes: Include tests for keyboard-only flows and screen-reader-relevant focus transitions.
- Styling / Highlighting Notes where relevant: Validate active/focused chrome states during command routing tests.
- Observability / Diagnostics Notes: Store per-command context snapshots on failure.
- Rollback / Safety Notes: Keep helper APIs test-only to avoid leaking them into production paths.
- References / Context: Current command path duplication in `MainFrame.cpp` is a major regression vector.
- Example scenarios where useful: Press `Cmd+S` after clicking a canvas object and verify the board, not the last file, receives the save request.
