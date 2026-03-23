# Phase 02 - Shell Frame Layout Rhythm And Surface Layering

## Goal

Refine the app shell so the window frame, workspace composition, sidebars, center content, bottom areas, and split views feel balanced, intentional, and premium at all common sizes.

## Definition Of Done

- shell zones have clear proportional balance and spacing rhythm
- split views, resizing, and overflow states stay composed
- surface layering and depth are visually coherent across the shell

## Tasks

### Phase ID: V26-P02
### Task ID: V26-P02-T01
### Task Title: Rebalance Main Shell Proportions And Split-View Composition
- Priority: P0
- Category: Layout System
- Objective: Tune sidebar widths, center content margins, bottom-area height behavior, and splitter composition for a calmer and more premium shell.
- Why This Matters Now: The shell defines perceived product quality before any specific tool does.
- UI Gap Statement: Core shell surfaces exist, but layout balance and density vary enough that the workspace can still feel crowded or mechanically assembled.
- User Experience Impact: Better balance improves focus, reduces fatigue, and makes the product feel more expensive and intentional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Prior Plan References: `v22` shell polish, `v25` shell hardening work
- Scope: shell proportions, split-view defaults, resize behavior, padding at shell boundaries
- Out of Scope: docking behavior changes that alter core functionality
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Screens / Components / Surfaces: main window, sidebar regions, editor work area, bottom panels, split views
- Current UI Behavior / Appearance: custom shell exists, but proportions and internal spacing are still partially driven by accumulated local decisions.
- Intended UI Behavior / Appearance: shell composition feels balanced at startup, during panel toggles, and while resizing between common laptop and large-desktop widths.
- Visual / Interaction Design Direction: premium desktop workspace with stable framing and intentional negative space.
- Technical Styling Approach: centralize preferred shell metrics in layout helpers and remove duplicated proportion logic.
- Implementation Steps:
  1. Review current default splitter sizes and zone metrics.
  2. Define preferred min, ideal, and max ranges for core shell regions.
  3. Update layout code to apply those ranges consistently.
  4. Test common workspace configurations and panel combinations.
- Validation Steps:
  1. Verify shell composition at narrow, medium, and wide widths.
  2. Open side panels and bottom panels in multiple combinations.
  3. Capture before/after screenshots to confirm balance improvements.
- Acceptance Criteria: default and resized shell layouts feel balanced without crowded seams or oversized dead space.
- Dependencies: V26-P01-T01
- Parallelization Notes: can run while action-surface polish begins.
- Risks / Failure Modes: over-optimizing for one monitor size can hurt another.
- Accessibility / Readability Notes: keep sufficient room for labels and dense lists at typical DPI scaling.
- Theme / Styling Notes: shell layering should remain clear in both themes.
- Motion / Interaction Notes where relevant: splitter interaction feedback is handled later but should not be blocked by this work.
- Cleanup / Consolidation Notes where relevant: remove duplicated shell sizing logic in startup and frame code where possible.
- Rollback / Safety Notes: keep layout changes metric-driven so they can be tuned without code churn.
- References / Context: layout polish should serve editor, notebook, and canvas equally.
- Example scenarios where useful: startup with a narrow file tree and bottom problems panel should still feel balanced.

### Phase ID: V26-P02
### Task ID: V26-P02-T02
### Task Title: Standardize Surface Layering, Backgrounds, And Work-Area Depth
- Priority: P1
- Category: Layout System
- Objective: Establish a consistent depth model for shell layers, panel layers, work surfaces, and transient overlays.
- Why This Matters Now: Layer ambiguity makes the UI feel flat in some places and noisy in others.
- UI Gap Statement: Some shell components use strong custom paint while others still collapse visually into adjacent surfaces.
- User Experience Impact: Clear depth cues improve orientation without resorting to heavy borders or visual clutter.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: `v22` shell and chrome phases, `v25` visible UI closure
- Scope: surface stacks, background contrast, edge separators, subtle elevation treatment
- Out of Scope: animation of those transitions, which belongs later
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: top bar, side chrome, content host, dialogs, popovers
- Current UI Behavior / Appearance: layer relationships are stronger in shell chrome than in dialogs and some interior surfaces.
- Intended UI Behavior / Appearance: every surface belongs to a clear depth tier with subtle, consistent visual separation.
- Visual / Interaction Design Direction: low-noise layering with crisp separators and restrained elevation.
- Technical Styling Approach: define reusable surface tiers and assign each component to one of them.
- Implementation Steps:
  1. Enumerate existing surface layers and map them to theme tokens.
  2. Add any missing background/border tokens only where necessary.
  3. Refactor shell and dialog paint paths to use those layers.
  4. Validate side-by-side with settings and overlays.
- Validation Steps:
  1. Review toolbar, tabs, editor host, panels, dialogs, and command surfaces together.
  2. Check that adjacent surfaces remain distinguishable without harsh lines.
  3. Verify light/dark parity.
- Acceptance Criteria: shell, content host, panels, and overlays read as a deliberate depth system instead of unrelated paint layers.
- Dependencies: V26-P01-T03
- Parallelization Notes: can run in parallel with V26-P02-T01 once shared state tokens are settled.
- Risks / Failure Modes: excessive depth cues can look glossy or dated.
- Accessibility / Readability Notes: low-contrast themes still need clear surface separation.
- Theme / Styling Notes: avoid theme-specific hacks; use shared tokens.
- Motion / Interaction Notes where relevant: layered surfaces should be ready for later transition work.
- Cleanup / Consolidation Notes where relevant: retire duplicate background derivation code after tier mapping lands.
- Rollback / Safety Notes: prefer token adjustments over per-surface overrides.
- References / Context: this phase should make later panel, dialog, and overlay work easier.
- Example scenarios where useful: the command palette should clearly float above the shell without feeling disconnected from it.

### Phase ID: V26-P02
### Task ID: V26-P02-T03
### Task Title: Refine Empty Shell, Overflow, And Resizing Presentation
- Priority: P1
- Category: Layout System
- Objective: Make sparse-shell, overflow, and resized states feel composed instead of improvised.
- Why This Matters Now: Premium quality is revealed most clearly in edge states, not only ideal states.
- UI Gap Statement: Layout work has focused on active workflows, leaving minimal-content and overflow cases visually uneven.
- User Experience Impact: Users perceive stronger quality when the app remains calm and legible during unusual panel mixes and smaller window sizes.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Prior Plan References: `v22` empty-state planning, `v25` shell hardening
- Scope: empty main work area, hidden panel combinations, narrow-width overflow, section truncation behavior
- Out of Scope: content-level empty-state copy, covered later
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
- Related Screens / Components / Surfaces: shell frame, empty workspace, narrow shell, collapsed sidebars
- Current UI Behavior / Appearance: empty and constrained states still expose some raw layout seams and density imbalance.
- Intended UI Behavior / Appearance: sparse or constrained layouts still feel purposefully composed, with graceful truncation and preserved hierarchy.
- Visual / Interaction Design Direction: resilient premium shell that looks intentional even when mostly empty.
- Technical Styling Approach: add shell-specific empty and overflow layout rules rather than per-panel hacks.
- Implementation Steps:
  1. Identify small-width and sparse-content shell cases.
  2. Define truncation, collapse, and padding behavior for those states.
  3. Adjust shell layout and affected panels to preserve rhythm.
  4. Validate against startup, single-editor, and empty-workspace cases.
- Validation Steps:
  1. Resize to narrow widths and observe shell seams.
  2. Open the app with minimal content and compare against active workspace states.
  3. Verify no abrupt crowding appears in shell headers or panel chrome.
- Acceptance Criteria: narrow and empty shell states retain alignment, spacing, and visual calm without ad hoc breakage.
- Dependencies: V26-P02-T01
- Parallelization Notes: can be executed after the main shell proportion work is stable.
- Risks / Failure Modes: overly aggressive collapse rules can hide important context.
- Accessibility / Readability Notes: truncation must still preserve keyboard reachability and label meaning.
- Theme / Styling Notes: empty shell states should still use the same surface layers as active shell states.
- Motion / Interaction Notes where relevant: later transitions should preserve the same structural rules.
- Cleanup / Consolidation Notes where relevant: consolidate empty-shell handling instead of duplicating it in panel implementations.
- Rollback / Safety Notes: test all shell states before removing old fallback rules.
- References / Context: users judge quality sharply when opening the app for the first time or when working in small windows.
- Example scenarios where useful: opening the app into a mostly empty workspace on a 13-inch laptop should still look premium.
