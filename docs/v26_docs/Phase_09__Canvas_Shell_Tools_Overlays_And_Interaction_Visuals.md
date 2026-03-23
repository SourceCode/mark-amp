# Phase 09 - Canvas Shell Tools Overlays And Interaction Visuals

## Goal

Make the canvas feel like a flagship premium surface through better shell chrome, tool presentation, overlays, affordances, and onboarding quality.

## Definition Of Done

- canvas shell and tool surfaces match the quality of the app shell
- object selection, handles, guides, overlays, and zoom/navigation controls feel precise and premium
- visible placeholder and stand-in elements are removed from the canvas UI path

## Tasks

### Phase ID: V26-P09
### Task ID: V26-P09-T01
### Task Title: Replace Canvas Placeholder Chrome With A Premium Workspace Shell
- Priority: P0
- Category: Canvas UI
- Objective: Eliminate placeholder-looking canvas shell elements and create a polished workspace presentation.
- Why This Matters Now: Canvas is a flagship differentiator, and its visible placeholder shell currently undercuts product quality.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` still contains unicode icon placeholders and placeholder inspector text.
- User Experience Impact: A premium canvas shell makes the entire product feel more ambitious, capable, and complete.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` canvas UI work, `v25` release-path canvas hardening
- Scope: workspace shell, visible tool rail, inspector placeholder replacement, header/footer chrome
- Out of Scope: canvas data model or interaction engine logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: canvas workspace, tool rail, inspector host, empty canvas state
- Current UI Behavior / Appearance: canvas shell is functional but visibly unfinished in iconography and supporting chrome.
- Intended UI Behavior / Appearance: canvas workspace reads as a first-class premium surface with refined shell framing and no placeholder UI.
- Visual / Interaction Design Direction: high-precision creative workspace with restrained but confident chrome.
- Technical Styling Approach: replace stand-ins with shared iconography, shell tokens, and panel/palette styling rules.
- Implementation Steps:
  1. Remove unicode icon stand-ins and placeholder inspector text.
  2. Apply shared shell, toolbar, and panel styling conventions to canvas chrome.
  3. Tune spacing and visual grouping in the canvas workspace frame.
  4. Validate light/dark parity and empty-canvas presentation.
- Validation Steps:
  1. Open the canvas workspace empty and populated.
  2. Compare canvas chrome with editor and notebook tabs.
  3. Verify no placeholder-looking UI remains.
- Acceptance Criteria: canvas shell no longer exposes visibly temporary UI and feels premium at first glance.
- Dependencies: V26-P01-T01, V26-P01-T03, V26-P16-T01
- Parallelization Notes: can begin once shared icon and shell rules are known.
- Risks / Failure Modes: overly heavy chrome could reduce canvas openness.
- Accessibility / Readability Notes: tool labels or affordances must remain clear if icon-only controls are used.
- Theme / Styling Notes: canvas shell should harmonize with but remain distinct from the editor shell.
- Motion / Interaction Notes where relevant: none yet; handled later.
- Cleanup / Consolidation Notes where relevant: remove placeholder strings and stand-in assets entirely.
- Rollback / Safety Notes: keep canvas interaction entry points unchanged while shell visuals are replaced.
- References / Context: this is one of the clearest high-impact `v26` UI tasks.
- Example scenarios where useful: opening a blank canvas should feel premium immediately, not like entering an unfinished workspace.

### Phase ID: V26-P09
### Task ID: V26-P09-T02
### Task Title: Refine Canvas Tool Palette, Selection Handles, Guides, And Zoom Controls
- Priority: P0
- Category: Canvas UI
- Objective: Give core canvas interaction affordances the same precision and tactile quality as premium design tools.
- Why This Matters Now: Canvas quality is judged heavily by selection visuals and tool affordances, not only by raw feature count.
- UI Gap Statement: Tool palette and manipulation visuals can easily feel generic or utilitarian if they are not explicitly refined.
- User Experience Impact: Better affordances improve confidence during creation, editing, and navigation.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h`, `/Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h`
- Prior Plan References: `v22` canvas refinement, `v25` release-path canvas closure
- Scope: tool palette presentation, active tool state, selection outline styling, resize handles, guide/snap visuals, zoom control styling
- Out of Scope: snapping logic or transform behavior changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h`, `/Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h`
- Related Screens / Components / Surfaces: tool palette, selection overlays, guide overlays, zoom UI
- Current UI Behavior / Appearance: interaction visuals are likely serviceable but not yet at premium creative-tool quality.
- Intended UI Behavior / Appearance: controls and manipulation affordances feel precise, modern, and unmistakably intentional.
- Visual / Interaction Design Direction: premium canvas tooling with clean geometry and excellent visual restraint.
- Technical Styling Approach: define canvas-specific interaction tokens aligned with shared focus and action-state language.
- Implementation Steps:
  1. Audit current tool and selection affordances.
  2. Define visual roles for tool states, handles, guides, and zoom controls.
  3. Refine paint logic and sizing for precision and clarity.
  4. Validate at multiple zoom levels and object densities.
- Validation Steps:
  1. Select, resize, and move several object types.
  2. Check guide visibility at different backgrounds and themes.
  3. Verify zoom controls remain clear without dominating the canvas.
- Acceptance Criteria: core canvas manipulation visuals feel premium, precise, and aligned with the app’s overall design language.
- Dependencies: V26-P09-T01, V26-P18-T01
- Parallelization Notes: can run while notebook and editor refinements are underway.
- Risks / Failure Modes: oversized handles or bright guides can feel toy-like or distracting.
- Accessibility / Readability Notes: selection affordances must remain discoverable and high-contrast enough to use comfortably.
- Theme / Styling Notes: guide and handle colors must be theme-safe and legible on varied canvas backgrounds.
- Motion / Interaction Notes where relevant: drag and resize transitions can be added later but must not mask state clarity.
- Cleanup / Consolidation Notes where relevant: consolidate object-manipulation styling instead of per-tool variants.
- Rollback / Safety Notes: preserve hit targets and manipulation reliability while tuning visuals.
- References / Context: users compare canvas polish against leading whiteboard and design tools.
- Example scenarios where useful: selecting a frame and resizing it should feel precise and visually satisfying.

### Phase ID: V26-P09
### Task ID: V26-P09-T03
### Task Title: Polish Canvas Background, Inline Editing, Overlays, And Empty-State Onboarding
- Priority: P1
- Category: Canvas UI
- Objective: Improve the passive visual quality of the canvas so it feels premium before any object interaction begins.
- Why This Matters Now: Backgrounds, overlays, and empty states set tone and perceived sophistication immediately.
- UI Gap Statement: Canvas onboarding and passive visual systems often remain less refined than active manipulation affordances.
- User Experience Impact: A stronger passive visual environment makes the canvas feel inviting and premium even before use.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h`
- Prior Plan References: `v22` canvas polish and onboarding, `v25` visible UI closure
- Scope: canvas background/grid, overlay treatments, inline editing shell, empty-state onboarding, layer depth cues
- Out of Scope: canvas rendering algorithm changes unrelated to presentation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h`
- Related Screens / Components / Surfaces: blank canvas, text editing on canvas, overlays, onboarding prompts
- Current UI Behavior / Appearance: passive canvas visuals likely work, but may still feel raw or generic.
- Intended UI Behavior / Appearance: the canvas environment feels spatially coherent, elegant, and product-grade even when empty.
- Visual / Interaction Design Direction: spacious creative surface with subtle depth and restrained guidance.
- Technical Styling Approach: build on shared background, overlay, and empty-state systems while preserving canvas-specific openness.
- Implementation Steps:
  1. Tune background and grid styling.
  2. Refine inline editing frame and overlay treatments.
  3. Design an elegant canvas empty/onboarding state.
  4. Validate with varied zoom and theme combinations.
- Validation Steps:
  1. Open a blank canvas in light and dark themes.
  2. Enter inline edit mode on text objects.
  3. Compare overlay visibility against other shell surfaces.
- Acceptance Criteria: the passive canvas experience feels premium, calm, and intentionally designed.
- Dependencies: V26-P09-T01, V26-P14-T02
- Parallelization Notes: can run after shell placeholder cleanup is complete.
- Risks / Failure Modes: too much background texture or onboarding content can reduce creative openness.
- Accessibility / Readability Notes: overlays and inline editing shells must remain legible against canvas backgrounds.
- Theme / Styling Notes: background grids must not create contrast fatigue.
- Motion / Interaction Notes where relevant: overlay fades and onboarding transitions should remain subtle.
- Cleanup / Consolidation Notes where relevant: remove temporary empty-canvas visuals and redundant overlay styling.
- Rollback / Safety Notes: keep canvas content legible and interaction-first while refining atmosphere.
- References / Context: this task closes the “raw creative tool” feel gap.
- Example scenarios where useful: creating a new canvas should feel aspirational and polished before the first object is added.
