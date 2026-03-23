# Phase 10 - Canvas Shell Tooling Overlay And Creative Surface Redesign

## Goal

Redesign the canvas into a flagship creative surface with premium chrome, stronger tool identity, cleaner overlays, and more intentional spatial design.

## Definition Of Done

- canvas no longer exposes placeholder-looking shell or iconography
- tool palette, overlays, handles, guides, and empty states feel redesigned and premium
- canvas identity is strong enough to stand beside the editor and notebook

## Tasks

### Phase ID: V27-P10
### Task ID: V27-P10-T01
### Task Title: Replace Canvas Placeholder And Glyph Based Shell With A Redesigned Creative Workspace
- Priority: P0
- Category: Canvas UI
- Objective: remove all placeholder and glyph-based canvas shell treatments and rebuild the canvas frame as a premium creative workspace.
- Why This Matters Now: canvas is one of the product’s strongest visual differentiators and one of its most obvious remaining weak points.
- Visual Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` still exposes unicode tool icons and placeholder inspector text.
- User Experience Impact: a redesigned canvas shell immediately raises ambition, trust, and product distinctiveness.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` Phase 10, `v25` canvas release-path work, `v26` Phase 09
- Scope: canvas shell, tool rail, inspector shell, workspace frame, empty host chrome, icon replacement
- Out of Scope: canvas document model or persistence behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: canvas workspace, tool rail, inspector side surface, empty canvas state
- Current Visual Behavior / Appearance: canvas shell is functional but clearly behind the rest of the product in icon quality and shell maturity.
- Intended Visual Behavior / Appearance: canvas looks like a fully redesigned, premium creative product surface from first open.
- Icon / Visual Design Direction: refined creative workspace with canonical tool icons, better shell depth, and stronger spatial calm.
- Technical Styling Approach: replace unicode tool controls and placeholder text with canonical icon-driven controls and a redesigned shell system.
- Implementation Steps:
  1. Remove unicode and placeholder shell artifacts from the canvas path.
  2. Rebuild tool rail, header, and inspector shell around canonical icons and redesigned geometry.
  3. Apply the redesigned visual language to the full canvas frame.
  4. Validate empty and populated canvas states.
- Validation Steps:
  1. Open blank and content-filled boards.
  2. Compare canvas shell against editor and notebook shells.
  3. Confirm no placeholder-looking UI remains on the visible canvas path.
- Acceptance Criteria: canvas shell no longer looks provisional and feels like a flagship redesigned surface.
- Dependencies: Phase 01 through Phase 05
- Parallelization Notes: can run in parallel with notebook redesign after foundations stabilize.
- Risks / Failure Modes: too much chrome can reduce the spaciousness expected from a canvas tool.
- Accessibility / Readability Notes: tool discoverability must remain strong if controls become more icon-led.
- Theme / Styling Notes: canvas shell must stay distinct while still belonging to the redesigned product.
- Motion / Interaction Notes where relevant: later tool and overlay transitions should reinforce the new shell.
- Cleanup / Consolidation Notes where relevant: remove canvas-specific placeholder strings and glyph mappings completely.
- Rollback / Safety Notes: preserve core canvas access patterns while redesigning shell visuals.
- References / Context: this is one of the highest-visibility redesign tasks in `v27`.
- Example scenarios where useful: opening a blank canvas should feel premium, aspirational, and finished.

### Phase ID: V27-P10
### Task ID: V27-P10-T02
### Task Title: Redesign Canvas Tool Palette Selection Handles Guides And Zoom Navigation
- Priority: P0
- Category: Canvas UI
- Objective: reauthor the canvas interaction language so tool controls and object manipulation feel more precise, modern, and premium.
- Why This Matters Now: creative-surface quality is judged heavily by tool affordances and manipulation feedback.
- Visual Gap Statement: current canvas interaction visuals likely work, but they do not yet match the premium standard implied by the rest of the redesign.
- User Experience Impact: stronger interaction visuals improve confidence and make canvas work more enjoyable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h`, `/Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h`
- Prior Plan References: `v22` canvas phase, `v26` canvas interaction visuals
- Scope: tool palette, active tool state, selection outlines, resize handles, snap guides, zoom controls, layer/utility controls if present
- Out of Scope: snapping or transform logic itself
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h`, `/Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h`
- Related Screens / Components / Surfaces: canvas tools, object selection, drag handles, guide overlays, zoom controls
- Current Visual Behavior / Appearance: interaction affordances likely feel functional but not yet at a best-in-class design-tool level.
- Intended Visual Behavior / Appearance: canvas interactions feel refined, tactile, and visually exact.
- Icon / Visual Design Direction: premium creative-tool interaction language with crisp geometry and stronger affordance clarity.
- Technical Styling Approach: redesign interaction affordances as one canvas-specific state family built on the global visual and icon systems.
- Implementation Steps:
  1. Redesign tool iconography, states, and control shells.
  2. Rebuild selection outlines, handles, and guide treatments.
  3. Redesign zoom and utility controls to fit the new creative-surface identity.
  4. Validate at multiple zoom levels and object densities.
- Validation Steps:
  1. Select, drag, resize, and zoom across several object types.
  2. Check guide and handle readability on varied canvas backgrounds.
  3. Compare tool state clarity under hover, active, and disabled conditions.
- Acceptance Criteria: canvas manipulation visuals feel premium and clearly redesigned, not merely recolored.
- Dependencies: V27-P10-T01, Phase 03
- Parallelization Notes: can proceed while passive canvas atmosphere work begins.
- Risks / Failure Modes: oversized or flashy handles can make the canvas feel toy-like.
- Accessibility / Readability Notes: affordances must remain clear at common zoom levels and on varied backgrounds.
- Theme / Styling Notes: guide and handle visibility must stay reliable across themes.
- Motion / Interaction Notes where relevant: later drag/resize transitions should be subtle and informative.
- Cleanup / Consolidation Notes where relevant: consolidate per-tool styling into a unified canvas interaction system.
- Rollback / Safety Notes: preserve hit targets and manipulation reliability throughout visual redesign.
- References / Context: interaction visuals are the canvas equivalent of editor dynamic-state design.
- Example scenarios where useful: selecting a sticky note or frame should feel more like using a premium design tool than a utility panel.

### Phase ID: V27-P10
### Task ID: V27-P10-T03
### Task Title: Redesign Canvas Background Overlays Inline Editing And Onboarding Atmosphere
- Priority: P1
- Category: Canvas UI
- Objective: improve the passive visual identity of the canvas so it feels premium and spatially intentional even before heavy interaction begins.
- Why This Matters Now: atmosphere and passive visual design set emotional tone immediately in creative surfaces.
- Visual Gap Statement: canvas background, overlays, and onboarding states still feel less fully authored than the product’s strongest surfaces.
- User Experience Impact: better passive visual design makes the canvas more inviting and more clearly differentiated.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h`
- Prior Plan References: `v22` canvas and onboarding phases, `v26` Phase 09 and Phase 14
- Scope: background and grid treatment, overlay depth, inline editing shell, empty-state onboarding, passive atmosphere and visual calm
- Out of Scope: canvas rendering algorithms not related to visible presentation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.h`
- Related Screens / Components / Surfaces: blank canvas, inline text editing, passive overlays, empty-canvas onboarding
- Current Visual Behavior / Appearance: passive canvas visuals likely remain more generic or transitional than the rest of the redesign target.
- Intended Visual Behavior / Appearance: the canvas environment feels spatially rich, calm, and unmistakably premium before any object is selected.
- Icon / Visual Design Direction: refined creative atmosphere with restrained depth and elegant guidance.
- Technical Styling Approach: redesign canvas passive visuals using dedicated canvas atmospheric roles layered over the global visual system.
- Implementation Steps:
  1. Redesign the canvas background and grid language.
  2. Rebuild inline editing shells and passive overlays.
  3. Redesign empty/onboarding states for the canvas.
  4. Validate with several zoom levels and themes.
- Validation Steps:
  1. Open a blank canvas in both themes.
  2. Enter inline edit mode on text objects.
  3. Check whether passive overlays and background details remain calm, not distracting.
- Acceptance Criteria: canvas atmosphere and passive visuals feel fully redesigned and premium, not sparse or improvised.
- Dependencies: V27-P10-T01, Phase 16
- Parallelization Notes: can run after core shell and interaction redesign direction is stable.
- Risks / Failure Modes: too much visual texture can reduce openness and content focus.
- Accessibility / Readability Notes: backgrounds and overlays must never compromise object and text readability.
- Theme / Styling Notes: atmospheric treatment must remain elegant in both light and dark variants.
- Motion / Interaction Notes where relevant: subtle overlay or onboarding motion may be layered later, but the static design must stand on its own.
- Cleanup / Consolidation Notes where relevant: retire temporary empty-canvas and passive-overlay visuals.
- Rollback / Safety Notes: keep the canvas’s sense of openness intact during redesign.
- References / Context: this is the passive-identity complement to canvas interaction redesign.
- Example scenarios where useful: a new user opening the canvas should feel they entered a premium creative environment, not a raw utility board.
