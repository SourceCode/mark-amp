# Phase 19 - Motion Micro Interactions And Icon Driven Control Redesign

## Goal

Finish the redesigned interface with restrained premium motion, better tactile feel, and controls whose structure improves because the icon system itself is better.

## Definition Of Done

- motion and tactile response reinforce the redesigned visual system
- icon-driven control redesign materially improves clarity, not just aesthetics
- drag, resize, focus-transfer, hover, and press behavior feel more polished across the app

## Tasks

### Phase ID: V27-P19
### Task ID: V27-P19-T01
### Task Title: Redesign Motion Language For Panels Tabs Menus Dialogs And Surface Transitions
- Priority: P0
- Category: Motion / Animation
- Objective: define and apply a new restrained motion language that makes the redesigned app feel more responsive and cohesive.
- Why This Matters Now: once the visible redesign is in place, motion becomes one of the main signals of finish quality.
- Visual Gap Statement: static redesign alone will still feel incomplete if transitions and response timings remain mixed or abrupt.
- User Experience Impact: better motion improves perceived responsiveness and gives the interface a more premium tactile quality.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v22` motion phase, `v26` Phase 17
- Scope: panel transitions, tab transitions, dialog/menu/popover entry, selection transitions, region-switch transitions, lightweight opacity/position/elevation cues
- Out of Scope: decorative animation unrelated to clarity or feel
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: tabs, panels, dialogs, menus, palette, shell regions
- Current Visual Behavior / Appearance: some motion exists or is implied, but a redesign-grade motion language is not yet clearly systematized.
- Intended Visual Behavior / Appearance: transitions feel smoother, more premium, and clearly part of the new visual identity.
- Icon / Visual Design Direction: restrained desktop motion with minimal travel and fast, polished timing.
- Technical Styling Approach: define shared timing/easing tiers and apply them only where they materially improve continuity.
- Implementation Steps:
  1. Audit current transitions and abrupt state changes.
  2. Define the redesigned motion vocabulary and surface-specific usage rules.
  3. Apply the motion system to tabs, panels, dialogs, menus, and palette.
  4. Validate reduced-motion compatibility.
- Validation Steps:
  1. Open/close or switch between representative surfaces repeatedly.
  2. Compare motion coherence across different surface classes.
  3. Confirm improved feel without introducing sluggishness.
- Acceptance Criteria: the redesigned UI feels smoother and more premium without becoming animated for animation’s sake.
- Dependencies: major surface redesign phases complete
- Parallelization Notes: can begin once core visual redesign is materially stable.
- Risks / Failure Modes: too much travel, opacity, or duration will make the UI feel sluggish or theatrical.
- Accessibility / Readability Notes: reduced-motion support must remain first-class.
- Theme / Styling Notes: motion should reinforce surface depth and state hierarchy, not compensate for weak static design.
- Motion / Interaction Notes where relevant: this task defines the motion system.
- Cleanup / Consolidation Notes where relevant: retire local animation constants and bespoke transition patterns.
- Rollback / Safety Notes: keep motion easy to tune centrally if responsiveness is affected.
- References / Context: motion is a finishing layer after the redesign is visually stable.
- Example scenarios where useful: switching tabs or opening a dialog should feel noticeably smoother but still immediate.

### Phase ID: V27-P19
### Task ID: V27-P19-T02
### Task Title: Redesign Hover Press Drag Resize And Focus Transfer Micro Interactions
- Priority: P1
- Category: Motion / Animation
- Objective: improve the tactile feel of everyday interactions so the redesigned UI feels more precise and luxurious.
- Why This Matters Now: tactile polish is one of the clearest remaining differentiators once the static redesign is complete.
- Visual Gap Statement: many interaction states likely remain mixed in timing, emphasis, or visual language across custom surfaces.
- User Experience Impact: better micro-interactions improve confidence and make the app feel more expensive and intentional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` and `v26` micro-interaction phases
- Scope: hover and press feel, drag states, resize states, focus transfer, panel activation, control responsiveness
- Out of Scope: underlying interaction mechanics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: controls, tabs, rows, shell splitters, canvas tools, drag and resize affordances
- Current Visual Behavior / Appearance: interaction feedback exists, but it is not yet fully redesigned as one tactile system.
- Intended Visual Behavior / Appearance: interactions feel tighter, more refined, and more consistent across the app.
- Icon / Visual Design Direction: subtle tactile feedback that complements the redesigned visual hierarchy.
- Technical Styling Approach: align micro-state transitions and emphasis cues with the new control, icon, and shell state systems.
- Implementation Steps:
  1. Inventory high-frequency interaction states and movement-heavy cues.
  2. Redesign hover, press, drag, resize, and focus-transfer treatments.
  3. Apply the new micro-interaction patterns across shell, settings, notebook, and canvas.
  4. Validate reduced-motion behavior.
- Validation Steps:
  1. Interact repeatedly with representative controls and resize/drag flows.
  2. Compare tactile feel across surface types.
  3. Confirm the redesign adds quality without visible latency.
- Acceptance Criteria: everyday interactions feel more coherent and premium than the current implementation.
- Dependencies: V27-P19-T01, Phase 12, Phase 10
- Parallelization Notes: can run after the motion language and control family redesigns are stable.
- Risks / Failure Modes: overly strong micro-feedback can become distracting or slow.
- Accessibility / Readability Notes: critical states must still be clear without motion.
- Theme / Styling Notes: micro-interactions should remain visually coherent in both theme families.
- Motion / Interaction Notes where relevant: ensure motion never substitutes for clear static state.
- Cleanup / Consolidation Notes where relevant: remove local micro-interaction hacks that no longer match the new system.
- Rollback / Safety Notes: revert any tactile effect that harms responsiveness.
- References / Context: micro-interactions are the final feel layer on top of the visual redesign.
- Example scenarios where useful: dragging a panel splitter and pressing a toolbar button should feel like parts of one carefully tuned interaction system.

### Phase ID: V27-P19
### Task ID: V27-P19-T03
### Task Title: Redesign Icon Driven Controls Rows Chips And Badges Around Improved Semantics
- Priority: P0
- Category: Control Styling
- Objective: use the improved icon system to structurally redesign controls and row layouts where iconography changes balance, hierarchy, or discoverability.
- Why This Matters Now: `v27` is not successful if icon replacement only swaps assets without improving the UI built around those icons.
- Visual Gap Statement: many controls and rows are currently shaped by older icon assumptions, glyph widths, or weak icon semantics.
- User Experience Impact: better icon-driven control design improves clarity, reduces clutter, and strengthens premium feel.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.h`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v22` iconography and control work, `v26` Phase 16 and Phase 19
- Scope: icon-only buttons, icon-plus-text controls, row icon columns, chips, pills, badges, toggle rows, compact utility controls
- Out of Scope: icon asset creation itself
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/ui/PanelHeaderIcons.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.h`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: toolbar actions, panel actions, settings rows, chips, badges, filters, compact actions
- Current Visual Behavior / Appearance: some controls likely still reflect glyph-era spacing or icon semantics that limit clarity and polish.
- Intended Visual Behavior / Appearance: controls and rows become structurally better because icon roles are stronger, cleaner, and more stable.
- Icon / Visual Design Direction: iconography that improves interface architecture, not just decoration.
- Technical Styling Approach: revisit layout, balance, and affordance patterns wherever canonical icon replacement changes available clarity and space.
- Implementation Steps:
  1. Identify controls and rows whose structure is constrained by old icon assumptions.
  2. Redesign their layout, emphasis, and affordance balance using canonical icon semantics.
  3. Update chips, badges, and compact action controls to the new balance rules.
  4. Validate scannability and discoverability after redesign.
- Validation Steps:
  1. Compare old and new icon-driven controls side by side.
  2. Verify icon-only controls remain clear and balanced.
  3. Confirm rows and badges gain clarity rather than just different icons.
- Acceptance Criteria: icon replacement materially improves control and row architecture across the product.
- Dependencies: Phase 01 through Phase 03, Phase 05, Phase 12
- Parallelization Notes: can run as the final control and micro-interaction polish layer.
- Risks / Failure Modes: changing control balance too aggressively can harm familiarity or density.
- Accessibility / Readability Notes: icon-only and compact controls must preserve discoverability and focus clarity.
- Theme / Styling Notes: chips and badges must remain theme-safe and severity-aware where relevant.
- Motion / Interaction Notes where relevant: redesigned icon-driven controls should later inherit the new tactile and motion language.
- Cleanup / Consolidation Notes where relevant: remove old spacing logic based on glyph width or local icon hacks.
- Rollback / Safety Notes: keep semantic meaning stable while redesigning structure.
- References / Context: this task makes icon replacement functionally valuable to the visible redesign.
- Example scenarios where useful: a filter chip or panel action row should feel cleaner and more understandable because the new icon system is better, not just newer.
