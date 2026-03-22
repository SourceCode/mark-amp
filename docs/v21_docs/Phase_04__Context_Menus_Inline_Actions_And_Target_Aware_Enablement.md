# Phase 04: Context Menus Inline Actions And Target Aware Enablement

## Outcome

Make right-click actions, tab menus, inline action menus, panel header actions, and target-specific controls trustworthy, themed, and context-aware.

## Improvement Count

126 atomic improvements across 6 execution tasks.

### P04-T01

- Phase ID: P04
- Task ID: P04-T01
- Task Title: Replace ad hoc `wxMenu` context menus with the registry/model system
- Priority: P0
- Category: Context Menu
- Atomic Improvements Covered: 21
- Objective: Make the context-menu registry and themed menu system the real path for context actions.
- Why This Matters Now: The repo already has a registry/model system but most real surfaces still bypass it.
- Problem Statement: `PopupMenu(&menu)` is still the dominant pattern in production surfaces.
- User Impact: Context menus are inconsistent in content, style, and enablement.
- Scope: File tree, tab bar, output, terminal, source control, git log, problems, editor, breadcrumb.
- Out of Scope: Extension-contributed menu ecosystems.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ContextMenuRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Systems / Components: Context menu registry, menu builder, themed context menu, commands.
- Current Behavior: The context-menu registry exists but is not the primary runtime path.
- Intended Behavior: Context menus are declared once and rendered consistently through the shared system.
- Technical Approach: Migrate real surfaces to registry-backed models and canonical action execution.
- Implementation Steps: Audit each context menu; define model entries; replace raw `wxMenu`; wire actions and enablement through the command system.
- Validation Steps: Compare menu contents and execution traces before and after migration.
- Acceptance Criteria: High-traffic context menus no longer build their own one-off menu trees.
- Dependencies: Phase 01.
- Risks / Failure Modes: Platform-native expectations may need selective exceptions.
- UX Notes: Keep keyboard navigation, separators, and mnemonic clarity.
- Settings / Panel / Control Notes where relevant: Settings-related context actions should deep-link correctly.
- Observability / Diagnostics Notes: Log context type, target type, and action resolution.
- Rollback / Safety Notes: Allow temporary fallbacks for surfaces with platform-specific menu blockers.
- References / Context: `ContextMenuRegistry.cpp` already defines default menus that are not yet driving the real UI.
- Example scenarios where useful: Right-clicking a tab shows the same canonical actions whether invoked from native or themed surfaces.

### P04-T02

- Phase ID: P04
- Task ID: P04-T02
- Task Title: Make context-menu content target-aware and state-correct
- Priority: P0
- Category: Enablement / Visibility Logic
- Atomic Improvements Covered: 21
- Objective: Ensure context menus reflect what is actually valid for the clicked target.
- Why This Matters Now: Incorrect context menus are a fast way to create misleading UI.
- Problem Statement: Current context menus often decide availability locally and incompletely.
- User Impact: Users see impossible or irrelevant actions.
- Scope: File vs folder vs empty explorer, saved vs unsaved tab, active vs inactive problem, terminal selection state, output selection state.
- Out of Scope: LSP-driven semantic context menus.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- Related Systems / Components: Context keys, active context, explorer, tabs, diagnostics.
- Current Behavior: Target-state rules are duplicated and incomplete.
- Intended Behavior: Context menus show only valid actions for the clicked target and state.
- Technical Approach: Pass rich target context into menu generation and command predicates.
- Implementation Steps: Define target context payloads; refactor menu generation; add per-target enablement rules; remove stale items.
- Validation Steps: Right-click different target types and compare action sets.
- Acceptance Criteria: Context menus no longer expose invalid actions for the current target.
- Dependencies: P04-T01, Phase 01.
- Risks / Failure Modes: Missing context payload fields can silently hide needed actions.
- UX Notes: Keep menu length tight; hide more often than disable when the action is irrelevant.
- Settings / Panel / Control Notes where relevant: Panel header menus should understand whether a panel is pinnable, closable, or docked.
- Observability / Diagnostics Notes: Emit target-type and enabled-item counts for menu traces.
- Rollback / Safety Notes: Keep default-safe hidden states for unsupported targets.
- References / Context: File tree and tab bar currently hand-roll target-specific enablement.
- Example scenarios where useful: Unsaved tabs do not advertise “Reveal in Finder” until they have a real path.

### P04-T03

- Phase ID: P04
- Task ID: P04-T03
- Task Title: Complete panel-header, inline, and item-row action wiring
- Priority: P1
- Category: Button Wiring
- Atomic Improvements Covered: 21
- Objective: Make small inline controls as trustworthy as top-level menus.
- Why This Matters Now: Dead inline affordances are easy to miss and highly frustrating.
- Problem Statement: Panel headers, row actions, and inline controls often use local callbacks and partial implementations.
- User Impact: Users click small action glyphs expecting quick actions and get inconsistent behavior.
- Scope: Panel header buttons, explorer row actions, extension cards, notebook/canvas inline actions, quick-action menus.
- Out of Scope: New inline-action feature sets.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/PanelHeader.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExplorerToolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ExtensionCard.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp`
- Related Systems / Components: Panel headers, inline controls, context menus, commands.
- Current Behavior: Many inline controls are bespoke and locally validated.
- Intended Behavior: Inline controls participate in the same action and state system as menus.
- Technical Approach: Attach canonical action IDs and target payloads to inline controls.
- Implementation Steps: Inventory inline controls; map actions; remove direct lambdas where possible; add hover/focus/disabled semantics.
- Validation Steps: Activate inline controls across explorer, extensions, code actions, and panel headers.
- Acceptance Criteria: No inline control remains misleading or orphaned.
- Dependencies: Phase 01 and P04-T01.
- Risks / Failure Modes: Small controls may require richer target context than menus.
- UX Notes: Hover and focus affordances must make clickability clear.
- Settings / Panel / Control Notes where relevant: Panel header actions must respect panel lifecycle state.
- Observability / Diagnostics Notes: Add inline-action activation traces.
- Rollback / Safety Notes: Disable or hide controls rather than leaving dead handlers.
- References / Context: Explorer toolbar, panel headers, and extension cards all define their own small-action behavior.
- Example scenarios where useful: Clicking an extension gear opens a real extension settings surface or does not render as available.

### P04-T04

- Phase ID: P04
- Task ID: P04-T04
- Task Title: Make no-op and partially implemented contextual actions explicit blockers
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 21
- Objective: Remove or gate contextual actions that still target stubs or comments instead of behavior.
- Why This Matters Now: Small dead controls accumulate faster than top-level dead controls.
- Problem Statement: Some contextual actions are visibly present while the underlying behavior is still placeholder-only.
- User Impact: Users cannot trust local quick actions.
- Scope: Problems quick fixes, problem navigation, search actions, notebook/canvas context actions, source control context actions.
- Out of Scope: Full downstream feature implementation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`
- Related Systems / Components: Problems, search, source control, context menus.
- Current Behavior: Several contextual actions are placeholders or comments.
- Intended Behavior: No contextual control remains exposed as if complete when it still fronts a stub.
- Technical Approach: Tag incomplete contextual actions in the control manifest and remove or gate them.
- Implementation Steps: Audit partial actions; classify blocker vs follow-on; hide, disable-with-rationale, or implement.
- Validation Steps: Search for placeholder action handlers and verify UI exposure matches support status.
- Acceptance Criteria: Users no longer see dead contextual quick actions in production state.
- Dependencies: P04-T01 through P04-T03.
- Risks / Failure Modes: Over-gating could temporarily reduce discoverability.
- UX Notes: If an action is intentionally unavailable, explain why only when the user would reasonably expect it.
- Settings / Panel / Control Notes where relevant: Settings-related contextual actions should not link to non-existent panels.
- Observability / Diagnostics Notes: Report contextual actions still marked partial.
- Rollback / Safety Notes: Use feature flags for near-ready actions.
- References / Context: `ProblemsPanel.cpp` currently has placeholder quick-fix behavior.
- Example scenarios where useful: “Quick Fix…” is hidden until a provider actually exists.

### P04-T05

- Phase ID: P04
- Task ID: P04-T05
- Task Title: Standardize context-menu keyboard parity and accessibility
- Priority: P1
- Category: Accessibility
- Atomic Improvements Covered: 21
- Objective: Ensure context actions are reachable and understandable by keyboard and assistive technology.
- Why This Matters Now: Context actions are often pointer-biased and therefore easy to leave incomplete.
- Problem Statement: Multiple context-menu implementations make keyboard and accessibility parity hard to guarantee.
- User Impact: Keyboard users miss access to local workflows.
- Scope: Keyboard invocation, arrow navigation, Enter/Escape behavior, labels, shortcut hints, focus return.
- Out of Scope: Full screen-reader review for every surface.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedContextMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CodeActionMenu.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- Related Systems / Components: Context menus, accessibility, focus management.
- Current Behavior: Keyboard parity depends on the specific implementation being used.
- Intended Behavior: Context actions are consistently keyboardable and labeled.
- Technical Approach: Define shared interaction and accessibility rules for all context menus.
- Implementation Steps: Audit menus; normalize key handling; add labels/tooltips; verify focus return after dismiss/execute.
- Validation Steps: Use keyboard-only flows to open and execute context menus on multiple surfaces.
- Acceptance Criteria: Context actions are fully usable without a mouse.
- Dependencies: P04-T01.
- Risks / Failure Modes: Mixed native/themed menu behavior may complicate consistent focus return.
- UX Notes: Preserve fast access while improving clarity.
- Settings / Panel / Control Notes where relevant: Settings-related context actions should preserve user context when dismissed.
- Observability / Diagnostics Notes: Add focus-return logging for context menu closes.
- Rollback / Safety Notes: Keep native fallback paths accessible if themed menu accessibility is incomplete during migration.
- References / Context: `CodeActionMenu.cpp` already has custom keyboard handling; other menus do not share one contract.
- Example scenarios where useful: Keyboard user opens the tab context menu and saves or closes the tab without pointer input.

### P04-T06

- Phase ID: P04
- Task ID: P04-T06
- Task Title: Add contextual-control smoke and regression coverage
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 21
- Objective: Make target-aware contextual actions testable and release-gated.
- Why This Matters Now: Context menus and inline actions are especially vulnerable to regressions.
- Problem Statement: Many contextual flows are local enough that they avoid broad smoke suites.
- User Impact: Local workflows silently break.
- Scope: Explorer menus, tab menus, terminal/output menus, problems menu, code actions, panel header actions.
- Out of Scope: Exhaustive downstream feature correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Systems / Components: Context menus, inline actions, command routing, diagnostics.
- Current Behavior: Contextual control correctness is underprotected.
- Intended Behavior: CI catches wrong-target, missing, or dead contextual actions.
- Technical Approach: Use target-context fixtures and command-trace assertions.
- Implementation Steps: Define target matrix; add invoke tests; verify enablement sets; capture focus and state-change expectations.
- Validation Steps: Run context-action suite on representative surfaces.
- Acceptance Criteria: Broken contextual actions become release-blocking.
- Dependencies: P04-T01 through P04-T05.
- Risks / Failure Modes: UI event simulation may be brittle if abstractions are weak.
- UX Notes: Include keyboard and pointer paths where both matter.
- Settings / Panel / Control Notes where relevant: Include context actions that open settings, panels, or reveal targets.
- Observability / Diagnostics Notes: Attach target payloads and action traces to failures.
- Rollback / Safety Notes: Prefer harnesses built around canonical action IDs over widget coordinates.
- References / Context: Context-menu drift is already visible in the codebase structure.
- Example scenarios where useful: Right-clicking a folder and choosing “New File…” creates a file in that folder through the canonical create flow.

