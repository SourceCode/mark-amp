# Phase 04: Docking Panels Focus And Visibility Lifecycle

## Phase Purpose
Remove panel drift so sidebars, bottom panels, focus restore, and visibility persistence behave like one designed shell instead of loosely related widgets.

## Measurable Outcome
- Panel visibility and activation use shared policy.
- Placeholder or misleading side panels are either completed or demoted.
- Focus traversal and restore are dependable across shell chrome and overlays.

## Tasks

### P04-T01
- Phase ID: P04
- Task ID: P04-T01
- Task Class: Foundational
- Task Title: Make PanelAreaModel And PanelLifecycleManager The Real Panel State Owners
- Priority: Critical
- Objective: Centralize panel registration, visibility, order, active-tab selection, and persisted snapshots.
- Why This Matters Now: Panel state is still partly local to widgets and partly local to `LayoutManager`, which keeps layout restore brittle.
- Problem Statement: `PanelAreaModel` and `PanelLifecycleManager` exist, but sidebars and bottom panels still have separate local logic and visibility toggles.
- Scope: primary sidebar, secondary sidebar, bottom panel, panel tab activation, panel order, and snapshot restore.
- Out of Scope: Multi-window layout sync.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelAreaModel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelLifecycleManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/PanelService.cpp`
- Related Systems / Components: panel registration, panel persistence, plugin-contributed panels, shell layout state.
- Current Behavior: Models exist, but `LayoutManager` still directly manages a large amount of panel lifecycle policy.
- Intended Behavior: Panel models own state, and `LayoutManager` renders that state.
- Technical Approach: Move panel mutations into shared model APIs and persist from there.
- Implementation Steps:
  1. Inventory all panel visibility and activation mutations.
  2. Route them through `PanelAreaModel` and `PanelLifecycleManager`.
  3. Persist snapshots per workspace and per shell layout policy.
  4. Ensure plugin-contributed panels use the same state flow.
- Validation Steps:
  1. Toggle panels in different orders and relaunch.
  2. Register and unregister plugin panels while the shell is live.
  3. Verify active panel switches away correctly when a visible panel is hidden.
- Acceptance Criteria: Panel state is no longer owned by ad hoc local widget logic.
- Dependencies: P01-T01, P03-T02.
- Risks / Failure Modes: Plugin panels may expose assumptions about activation ordering.
- UX Notes: Visibility and restore must match user intent exactly.
- Observability / Diagnostics Notes: Add panel-state diffs on every visibility/activation mutation.
- Rollback / Safety Notes: Keep a snapshot-reset path to recover from bad saved layouts.
- References / Context:
  - `PanelLifecycleManager::builtin_defaults()` is useful but not enough until it is the actual state owner.
- Example Scenarios Where Useful:
  - Hiding terminal, showing problems, reordering panels, and relaunching should restore identically.

### P04-T02
- Phase ID: P04
- Task ID: P04-T02
- Task Class: Workflow
- Task Title: Retire Placeholder Secondary Panels And Misleading Panel Surfaces
- Priority: High
- Objective: Stop presenting secondary sidebar and shell panels as complete when they still hold placeholder content or incomplete behavior.
- Why This Matters Now: Users can already reach these surfaces, so partial hosts actively reduce trust.
- Problem Statement: `LayoutManager` still creates ad hoc secondary panels such as outline, backlinks, and graph placeholders directly in registration code.
- Scope: secondary sidebar registration, placeholder hosts, unsupported panels, and fallback empty states.
- Out of Scope: Full domain feature implementation for every panel.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.h`
- Related Systems / Components: secondary sidebar registry, graph panel, outline, backlinks, empty states, activity bar discoverability.
- Current Behavior: Several panels are reachable but still render placeholder summaries or simplistic widgets.
- Intended Behavior: Supported panels are integrated and trustworthy; unsupported ones are clearly marked or hidden from primary affordances.
- Technical Approach: Classify each panel as complete, incomplete-but-useful, or not-ready, then wire or demote accordingly.
- Implementation Steps:
  1. Audit secondary sidebar registrations and mark panel readiness.
  2. Replace ad hoc placeholder implementations with real typed panels where available.
  3. Hide or gate unsupported panels from primary discoverability.
  4. Apply consistent empty/loading/error states to panels that remain visible.
- Validation Steps:
  1. Open every activity bar and secondary sidebar entry.
  2. Verify each visible surface either works or clearly states its supported scope.
- Acceptance Criteria: No major shell panel presents placeholder behavior as if it were complete.
- Dependencies: P04-T01.
- Risks / Failure Modes: Hiding panels may expose missing navigation alternatives.
- UX Notes: It is better to present a scoped, honest surface than a misleading pseudo-feature.
- Observability / Diagnostics Notes: Track panel activation for not-ready surfaces during rollout to measure user demand.
- Rollback / Safety Notes: Demoted panels can remain behind feature flags if needed.
- References / Context:
  - `LayoutManager.cpp` still contains inlined placeholder panel factories.
- Example Scenarios Where Useful:
  - Opening graph in the secondary sidebar should either show the real graph surface or a clearly scoped not-ready state, not a faux minimap.

### P04-T03
- Phase ID: P04
- Task ID: P04-T03
- Task Class: Foundational
- Task Title: Finish Shell Focus Traversal Restore And Hidden-Panel Safety Rules
- Priority: High
- Objective: Make keyboard traversal and focus restore follow one stable shell policy.
- Why This Matters Now: The shell has a `FocusManager`, but the surrounding UI still relies on many local focus assumptions.
- Problem Statement: Focus snapshots, traps, and traversal order exist, but overlays, prompts, sidebars, and panel visibility changes are not consistently using them.
- Scope: activity bar, sidebar, secondary sidebar, editor area, bottom panel, status bar, command palette, quick pick, input box, and settings host.
- Out of Scope: Full screen-reader authoring for every custom widget.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/FocusManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`
- Related Systems / Components: keyboard navigation, focus snapshots, overlays, accessibility, hidden panel transitions.
- Current Behavior: Focus-safe primitives exist, but many shell transitions do not use them.
- Intended Behavior: Opening and closing panels or overlays never strands focus on hidden surfaces and always restores predictably.
- Technical Approach: Make `FocusManager` the explicit policy owner and update shell surfaces to register entry/exit behavior.
- Implementation Steps:
  1. Define focus entry points for each shell zone.
  2. Push and restore snapshots around overlays and prompts.
  3. Disable or redirect focus when panels are hidden.
  4. Add keyboard-only regression cases for panel and overlay flows.
- Validation Steps:
  1. Navigate shell without mouse.
  2. Open and cancel command palette, quick pick, and settings.
  3. Hide a focused panel and verify focus recovery.
- Acceptance Criteria: Focus does not land on hidden or destroyed surfaces and restore behavior is consistent.
- Dependencies: P02-T03, P04-T01.
- Risks / Failure Modes: Custom widgets may not expose enough focus hooks initially.
- UX Notes: Elite IDE feel requires invisible predictability here.
- Observability / Diagnostics Notes: Add debug-only focus transition tracing with zone and widget identity.
- Rollback / Safety Notes: Keep simple fallback routing to editor area if a target focus surface disappears.
- References / Context:
  - `FocusManager` already supports snapshots and traps but needs broader shell adoption.
- Example Scenarios Where Useful:
  - Open command palette, cancel, then press sidebar toggle and continue keyboard navigation without losing place.
