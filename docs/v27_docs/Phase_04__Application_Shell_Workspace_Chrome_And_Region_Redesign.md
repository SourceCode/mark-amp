# Phase 04 - Application Shell Workspace Chrome And Region Redesign

## Goal

Redesign the full shell, workspace composition, region chrome, splitters, and empty-shell states around the new icon and visual systems.

## Definition Of Done

- the shell feels visibly redesigned rather than incrementally improved
- side regions, center work area, bottom regions, and split handles use one premium chrome system
- shell entry, empty, and resize states feel intentional and high-end

## Tasks

### Phase ID: V27-P04
### Task ID: V27-P04-T01
### Task Title: Redesign Main Frame Composition, Region Balance, And Workspace Layering
- Priority: P0
- Category: Layout System
- Objective: re-author shell geometry, layering, and balance so the app feels like a new premium workspace.
- Why This Matters Now: The shell is the visual frame for every other redesign phase.
- Visual Gap Statement: shell chrome exists and has improved, but it still reads as accumulated refinement rather than a visibly new visual generation.
- User Experience Impact: A redesigned shell immediately raises perceived product quality and coherence.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Prior Plan References: `v22` shell chrome phases, `v26` shell refinement phases
- Scope: shell proportions, region balance, workspace layering, empty-shell framing, splitter integration, background depth
- Out of Scope: non-visual docking or persistence behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Screens / Components / Surfaces: whole application shell, primary sidebar, secondary sidebar, center workspace, bottom panels
- Current Visual Behavior / Appearance: the shell is stronger than many interior surfaces but still not fully reauthored around a new visual identity.
- Intended Visual Behavior / Appearance: the shell feels unmistakably redesigned, with stronger balance, depth, and region transitions.
- Icon / Visual Design Direction: premium integrated workspace with confident layering and more luxurious composition.
- Technical Styling Approach: redesign shell metrics and surface tiers first, then update region hosts and split containers.
- Implementation Steps:
  1. Reassess shell region proportions and layer hierarchy.
  2. Define the redesigned shell composition model.
  3. Update shell hosts, splitters, and region containers to the new model.
  4. Validate default, sparse, and dense workspace states.
- Validation Steps:
  1. Review shell at narrow, medium, and wide sizes.
  2. Compare empty, single-pane, and multi-panel arrangements.
  3. Confirm redesigned shell identity is visible before opening any specific tool.
- Acceptance Criteria: the shell reads as a new high-end workspace rather than a tuned legacy shell.
- Dependencies: Phase 01 through Phase 03 foundations
- Parallelization Notes: can run with action-surface redesign once the new visual system is defined.
- Risks / Failure Modes: redesigning region balance without testing density can hurt real workflows.
- Accessibility / Readability Notes: preserve navigation clarity and target accessibility in compact shell layouts.
- Theme / Styling Notes: shell layering must remain equally strong in light and dark themes.
- Motion / Interaction Notes where relevant: splitter and region transitions later should reinforce the new shell hierarchy.
- Cleanup / Consolidation Notes where relevant: old shell fallback styling should be marked for removal.
- Rollback / Safety Notes: keep visual and layout changes separable from behavior changes.
- References / Context: this is a redesign task, not only a polish task.
- Example scenarios where useful: opening the app into a busy workspace should still feel calm and visually composed.

### Phase ID: V27-P04
### Task ID: V27-P04-T02
### Task Title: Redesign Activity Bar Sidebar Hosts And Region Switching Chrome
- Priority: P0
- Category: Navigation UI
- Objective: redesign region-switching surfaces so activity selection, sidebar modes, and side-surface transitions feel best-in-class.
- Why This Matters Now: The activity and sidebar system is one of the most visible icon-heavy shells in the product.
- Visual Gap Statement: the shell uses strong custom components, but sidebar registration and region switching still leak legacy icon-char and mixed treatment paths.
- User Experience Impact: Better activity and sidebar chrome improves orientation, navigation confidence, and premium feel.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v22` activity/sidebar phases, `v26` shell and icon phases
- Scope: activity slots, active/hover state, sidebar host framing, secondary sidebar tabs, region-switching visual cues
- Out of Scope: panel lifecycle logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: activity bar, primary sidebar host, secondary sidebar, panel switchers
- Current Visual Behavior / Appearance: strong custom shell exists, but region-switching surfaces still carry legacy icon-char assumptions and uneven interior-shell treatment.
- Intended Visual Behavior / Appearance: activity and sidebar surfaces feel sharper, cleaner, and clearly redesigned around the canonical icon and visual systems.
- Icon / Visual Design Direction: crisp icon-led navigation with precise active-state emphasis and refined rail geometry.
- Technical Styling Approach: replace icon-char ownership, redesign active slot and host shells, and unify region-switching visuals across primary and secondary side surfaces.
- Implementation Steps:
  1. Replace icon-char dependencies with semantic icon rendering.
  2. Redesign rail metrics, slot geometry, and host shells.
  3. Align primary and secondary region-switching treatments.
  4. Validate in high-density multi-panel layouts.
- Validation Steps:
  1. Switch rapidly between several activity items.
  2. Test primary and secondary sidebar visibility combinations.
  3. Confirm icons, labels, and active states feel coherent and premium.
- Acceptance Criteria: region switching no longer exposes icon chars or older shell styling and feels like one modern navigation system.
- Dependencies: Phase 03
- Parallelization Notes: can run in parallel with main shell composition work.
- Risks / Failure Modes: too much emphasis can make side navigation noisy.
- Accessibility / Readability Notes: active and focus states must remain highly visible.
- Theme / Styling Notes: rail contrast and slot states must remain theme-safe.
- Motion / Interaction Notes where relevant: activity/region transitions later should build on the new slot-state hierarchy.
- Cleanup / Consolidation Notes where relevant: retire icon-char sidebar APIs once migration completes.
- Rollback / Safety Notes: preserve activity semantics and keyboard navigation while redesigning visuals.
- References / Context: activity/sidebar redesign is both an icon and shell priority in `v27`.
- Example scenarios where useful: moving from Explorer to Search to AI should feel like switching between products within one coherent premium shell.

### Phase ID: V27-P04
### Task ID: V27-P04-T03
### Task Title: Redesign Split Handles Resize Affordances And Sparse Shell States
- Priority: P1
- Category: Layout System
- Objective: make shell edges, splitters, resize affordances, and sparse states feel intentionally designed and tactile.
- Why This Matters Now: Sparse and transitional shell states are often where legacy visuals remain most visible.
- Visual Gap Statement: split and resize states are functional, but they still read more like utility behaviors than polished design features.
- User Experience Impact: Better split and sparse-state design improves trust and perceived quality during real work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `v22` shell and motion phases, `v26` shell refinement
- Scope: split handles, resize cues, sparse-workspace composition, empty-shell transitions, region boundary language
- Out of Scope: actual splitter behavior logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Screens / Components / Surfaces: split panels, empty shell, sparse workspace, startup-hosted shell
- Current Visual Behavior / Appearance: resize and sparse states remain less designed than the shell’s best steady-state views.
- Intended Visual Behavior / Appearance: even sparse or actively resizing shell states look premium, controlled, and intentional.
- Icon / Visual Design Direction: low-noise precision with visible craftsmanship in edges and transitions.
- Technical Styling Approach: define redesigned splitter and sparse-state visual rules tied to the new shell geometry and state language.
- Implementation Steps:
  1. Audit visible splitter and sparse-state affordances.
  2. Define redesigned handle, hover, drag, and empty-shell visuals.
  3. Apply them to shell containers and startup-hosted sparse views.
  4. Validate in narrow, wide, and low-content shells.
- Validation Steps:
  1. Drag splitters repeatedly in mixed layouts.
  2. Open the app in sparse and dense states.
  3. Confirm shell quality remains high when few surfaces are visible.
- Acceptance Criteria: split and sparse states no longer reveal a weaker visual generation than the fully populated shell.
- Dependencies: V27-P04-T01
- Parallelization Notes: can run after the main shell composition is defined.
- Risks / Failure Modes: overemphasized splitters can add clutter.
- Accessibility / Readability Notes: resize affordances need enough visibility and hit clarity to remain usable.
- Theme / Styling Notes: edges and handles must work on all shell background tiers.
- Motion / Interaction Notes where relevant: drag-state polish later should build on these visuals.
- Cleanup / Consolidation Notes where relevant: retire legacy splitter styling and sparse-state fallback treatments.
- Rollback / Safety Notes: keep interaction behavior untouched while redesigning visible affordances.
- References / Context: sparse-state quality strongly affects first impression and perceived completeness.
- Example scenarios where useful: a nearly empty workspace on first launch should still feel deliberate and expensive.
