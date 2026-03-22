# Phase 19: Motion Microinteractions Drag Resize And Focus Transfer Polish

## Outcome

Define a restrained, premium motion language and apply it to hover, activation, panel transitions, drag states, resize states, focus transfers, and content-surface mode changes.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P19-T01

- Phase ID: P19
- Task ID: P19-T01
- Task Title: Define a product-wide motion language for shell, controls, overlays, and content surfaces
- Priority: P0
- Category: Motion / Animation
- Atomic Improvements Covered: 60
- Objective: Replace local animation tuning with one intentional motion system.
- Why This Matters Now: Motion is already present across tabs, palette, dialogs, toasts, tooltips, and minimap, but it is not yet clearly one language.
- UI Problem Statement: [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp), [CommandPalette.cpp](/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp), [ThemedDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp), and [NotificationManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp) all define motion locally.
- User Experience Impact: The product can feel slightly inconsistent or less refined in how it moves.
- Scope: Duration bands, easing bands, hover transitions, enter/exit motion, microfeedback, gesture feedback, reduced-motion mapping.
- Out of Scope: Low-level animation engine internals beyond visible system rules.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Related Screens / Components / Surfaces: Entire animated UI.
- Current UI Behavior / Appearance: Many surfaces animate, but not yet from one clearly shared motion grammar.
- Intended UI Behavior / Appearance: Motion feels coherent, premium, and restrained across all surfaces.
- Visual / Interaction Design Direction: Calm, intentional movement that supports clarity rather than spectacle.
- Technical Styling Approach: Define motion tokens and family-specific motion roles, then map all existing animations onto them.
- Implementation Steps: Inventory current animation timings and easings; define motion roles; align surface-specific animations; document reduced-motion alternates; remove ornamental motion that lacks purpose.
- Validation Steps: Review animated states across shell, overlays, editor, notebook, and canvas interactions.
- Acceptance Criteria: Motion language reads as one system rather than local taste.
- Dependencies: Phase 01, Phase 17.
- Risks / Failure Modes: Over-standardizing motion can remove useful surface-specific nuance.
- Accessibility / Readability Notes: Motion must never be the only state communication channel.
- Theme / Styling Notes: Motion should support, not compete with, theme and contrast hierarchy.
- Motion / Interaction Notes where relevant: This task defines the system that later polish tasks depend on.
- Observability / Diagnostics Notes where relevant: Add a motion inventory and timing board to the UI atlas.
- Rollback / Safety Notes: Maintain current functionality while converging motion behavior gradually.
- References / Context: Premium interfaces are often distinguished by the discipline of their motion more than by the amount of it.
- Example scenarios where useful: The palette, tab strip, and toasts all move differently in detail but clearly belong to one motion family.

### P19-T02

- Phase ID: P19
- Task ID: P19-T02
- Task Title: Polish microinteractions for hover, press, active, save, and completion feedback
- Priority: P1
- Category: Motion / Animation
- Atomic Improvements Covered: 60
- Objective: Make the interface feel more tactile and responsive without adding noise.
- Why This Matters Now: Small interaction responses are one of the clearest marks of premium fit and finish.
- UI Problem Statement: Hover and active states exist across controls, but many remain purely static or use locally tuned motion in isolation.
- User Experience Impact: The product feels less alive and less intentional in the hand than top-tier IDEs.
- Scope: Button hover/press, toolbar save pulse, tab fade, badge updates, selection settle, focus transfer, inline action reveal.
- Out of Scope: Large layout transitions.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`
- Related Screens / Components / Surfaces: Buttons, tabs, badges, row actions, status updates.
- Current UI Behavior / Appearance: Microfeedback exists, but not yet with one premium tactile philosophy.
- Intended UI Behavior / Appearance: Small interactions feel satisfyingly responsive and product-wide consistent.
- Visual / Interaction Design Direction: Tactile but understated microfeedback.
- Technical Styling Approach: Map microinteraction roles to motion tokens and control states, then refine or remove local one-offs.
- Implementation Steps: Audit microinteraction patterns; normalize hover and press transitions; refine save and completion cues; harmonize badge and state updates; remove noisy effects.
- Validation Steps: Interact with controls across shell, settings, panels, and editor utilities.
- Acceptance Criteria: The UI feels more tactile and polished without becoming animated for animation’s sake.
- Dependencies: P19-T01, Phase 04.
- Risks / Failure Modes: Too much microfeedback can make dense surfaces feel jittery.
- Accessibility / Readability Notes: Motion must always be paired with clear static states.
- Theme / Styling Notes: Microfeedback should preserve contrast and not blur text or icon clarity.
- Motion / Interaction Notes where relevant: This task covers the smallest but most frequent motion behaviors.
- Observability / Diagnostics Notes where relevant: Add microinteraction reference captures and timing notes.
- Rollback / Safety Notes: Make microfeedback optically additive rather than structurally necessary.
- References / Context: Premium fit and finish is often perceived most strongly through microinteraction quality.
- Example scenarios where useful: Pressing a toolbar action feels crisp and responsive instead of abruptly toggling state with no tactile cue.

### P19-T03

- Phase ID: P19
- Task ID: P19-T03
- Task Title: Improve drag, drop, reorder, and insertion-state visuals across tabs, panels, trees, notebooks, and canvas
- Priority: P0
- Category: Motion / Animation
- Atomic Improvements Covered: 60
- Objective: Make spatial rearrangement interactions visually precise and confidence-inspiring.
- Why This Matters Now: Drag and reorder cues are central to a sophisticated IDE/canvas environment and currently vary widely.
- UI Problem Statement: Drag indicators exist in [TabBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp), [DropIndicatorOverlay.cpp](/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.cpp), [ActivityBar.cpp](/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp), and canvas systems, but not yet under one visual philosophy.
- User Experience Impact: Rearrangement interactions can feel rough, ambiguous, or lower-trust than they should.
- Scope: Drag previews, insertion markers, reorder hints, docking targets, canvas drag states, list-row drag states, notebook cell insertion cues.
- Out of Scope: Drag/drop logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DropIndicatorOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolStateMachine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookCellController.cpp`
- Related Screens / Components / Surfaces: Tabs, sidebars, dock targets, canvas objects, notebook cells, tree rows.
- Current UI Behavior / Appearance: Rearrangement cues are present but visually inconsistent and sometimes under-emphasized.
- Intended UI Behavior / Appearance: Drag and insertion interactions feel expertly signposted and visually stable.
- Visual / Interaction Design Direction: Precision spatial cues with minimal ambiguity and low clutter.
- Technical Styling Approach: Define drag and insertion-state variants shared across reorderable surfaces, with surface-specific geometry adapters.
- Implementation Steps: Audit drag-state families; refine insertion markers and preview cards; align docking targets and reorder lines; tune canvas and notebook insertion cues.
- Validation Steps: Drag tabs, reorder rows, move notebook cells, and manipulate canvas objects across themes and densities.
- Acceptance Criteria: Spatial interaction states are more legible and more premium across the product.
- Dependencies: Phase 02, Phase 05, Phase 09, Phase 10, Phase 15.
- Risks / Failure Modes: Over-visible drag states can clutter idle surfaces if too much state leaks before interaction.
- Accessibility / Readability Notes: Drag targets and insertion cues must be visible for users with reduced precision and low vision.
- Theme / Styling Notes: Insertion cues need strong contrast on both bright and dark backgrounds.
- Motion / Interaction Notes where relevant: Drag-state motion should communicate momentum and destination without latency.
- Observability / Diagnostics Notes where relevant: Add drag and insertion screenshot boards for every major reorderable surface.
- Rollback / Safety Notes: Preserve current drag hit zones and behavior while visual feedback is improved.
- References / Context: Spatial interaction quality is one of the clearest indicators of elite product polish.
- Example scenarios where useful: Reordering notebook cells and dragging tabs both feel like versions of one precise spatial feedback system.

### P19-T04

- Phase ID: P19
- Task ID: P19-T04
- Task Title: Polish resize, split, collapse, and focus-transfer transitions across workbench zones
- Priority: P1
- Category: Motion / Animation
- Atomic Improvements Covered: 60
- Objective: Make structural workbench transitions feel smoother and more intentional.
- Why This Matters Now: Even with perfect static layout, rough structural transitions can make the UI feel heavy or brittle.
- UI Problem Statement: Shell resizing, panel opening, minimap toggling, and focus transfers currently use local motion or none at all.
- User Experience Impact: The interface can feel abrupt when its structure changes.
- Scope: Sidebar show/hide, bottom panel reveal, minimap slide, split changes, panel activation emphasis, focus transfers, editor/canvas/notebook mode switches.
- Out of Scope: Layout persistence logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MinimapPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PanelContainer.cpp`
- Related Screens / Components / Surfaces: Shell layout, minimap, sidebars, bottom panels, active content surface transitions.
- Current UI Behavior / Appearance: Structural transitions vary in polish and sometimes feel abrupt.
- Intended UI Behavior / Appearance: Structural changes feel smooth, stable, and product-wide coherent.
- Visual / Interaction Design Direction: Subtle structural motion that protects spatial understanding.
- Technical Styling Approach: Apply structural motion roles from the motion system to shell and content-surface transitions.
- Implementation Steps: Audit structural transitions; tune show/hide and focus-transfer motion; align minimap and panel transitions; improve active-region emphasis after structural change.
- Validation Steps: Open and close shell regions, toggle minimap, switch active content modes, and compare the resulting feel.
- Acceptance Criteria: Structural transitions feel more polished and less mechanically abrupt.
- Dependencies: Phase 02, Phase 06, Phase 19 motion foundation.
- Risks / Failure Modes: Structural motion can become distracting if durations are too long.
- Accessibility / Readability Notes: Reduced-motion alternatives must preserve spatial clarity.
- Theme / Styling Notes: Transition visuals should not depend on theme-specific artifacts that disappear in some modes.
- Motion / Interaction Notes where relevant: This task governs medium-scale structural motion.
- Observability / Diagnostics Notes where relevant: Add structural transition captures and timing notes.
- Rollback / Safety Notes: Keep transitions short and easy to disable if they interfere with responsiveness.
- References / Context: Structural polish is critical to making the shell feel expensive rather than merely feature-rich.
- Example scenarios where useful: Toggling the minimap feels smooth and spatially stable instead of popping the layout abruptly.

### P19-T05

- Phase ID: P19
- Task ID: P19-T05
- Task Title: Refine notebook, canvas, and editor mode-transition choreography
- Priority: P2
- Category: Motion / Animation
- Atomic Improvements Covered: 60
- Objective: Make transitions among major content modes feel coherent and intentional rather than abrupt context switches.
- Why This Matters Now: MarkAmp’s identity depends on moving between different content surfaces without feeling fragmented.
- UI Problem Statement: Surface transitions among editor, notebook, canvas, preview, and hybrid states are not yet clearly choreographed as one experience.
- User Experience Impact: Users can experience product-mode changes as jumps between tools rather than shifts inside one environment.
- Scope: Surface activation transitions, title and shell cue updates, contextual toolbar changes, notebook/canvas reveal, preview/editor mode transitions.
- Out of Scope: Surface switching logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Related Screens / Components / Surfaces: Editor, notebook, canvas, preview, shell mode switches.
- Current UI Behavior / Appearance: Surface changes work functionally, but the visual handoff is not yet a polished choreography.
- Intended UI Behavior / Appearance: Switching modes feels smooth and spatially coherent without dramatic animation.
- Visual / Interaction Design Direction: Calm mode handoffs that preserve orientation and shell continuity.
- Technical Styling Approach: Use coordinated shell and content-surface state transitions tied to the broader motion system.
- Implementation Steps: Audit mode-switch visuals; align shell cue changes; refine content-surface reveal/hide transitions; preserve context markers; tune timing for mode-specific chrome updates.
- Validation Steps: Switch repeatedly among editor, notebook, canvas, and preview states in representative workflows.
- Acceptance Criteria: Major content-mode changes feel integrated rather than product-fragmenting.
- Dependencies: Phase 05, Phase 09, Phase 10, P19-T01.
- Risks / Failure Modes: Too much choreography can slow expert workflows or create motion fatigue.
- Accessibility / Readability Notes: Mode changes must remain clear even with reduced motion.
- Theme / Styling Notes: Transition cues must maintain enough contrast and continuity in every theme mode.
- Motion / Interaction Notes where relevant: This task is about multi-surface choreography, not flourish.
- Observability / Diagnostics Notes where relevant: Add mode-switch journey captures to the UI atlas.
- Rollback / Safety Notes: Keep transitions short and easily adjustable.
- References / Context: Cross-surface continuity is one of MarkAmp’s strongest opportunities for UI differentiation.
- Example scenarios where useful: Jumping from a markdown editor to a notebook or canvas still feels like staying in one coherent environment.

### P19-T06

- Phase ID: P19
- Task ID: P19-T06
- Task Title: Add motion and interaction-state galleries plus reduced-motion parity checks
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Make motion quality and restraint visible, reviewable, and stable.
- Why This Matters Now: Motion is easy to overdo or fragment unless it is documented and regression-checked.
- UI Problem Statement: The product needs explicit motion references and reduced-motion parity captures across shell, overlay, and content-surface states.
- User Experience Impact: Motion quality can drift or become inconsistent without strong governance.
- Scope: Microinteraction captures, structural transition captures, drag/reorder state boards, reduced-motion comparisons, timing references.
- Out of Scope: Animation engine internals.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Related Screens / Components / Surfaces: Entire motion system.
- Current UI Behavior / Appearance: Motion intent exists but is not yet governed by one visual review asset.
- Intended UI Behavior / Appearance: Motion is documented, referenceable, and reviewed for both animated and reduced-motion experiences.
- Visual / Interaction Design Direction: Disciplined motion governance.
- Technical Styling Approach: Build motion boards, timing tables, and paired reduced-motion captures for representative interactions.
- Implementation Steps: Define motion fixtures; capture key animated states; publish timing and easing references; add reduced-motion comparison boards; include motion blockers in UI QA.
- Validation Steps: Perturb transition duration or easing and verify the motion board exposes the regression.
- Acceptance Criteria: Motion quality becomes easier to review, compare, and maintain.
- Dependencies: P19-T01 through P19-T05, Phase 17.
- Risks / Failure Modes: Capturing motion in review assets can be cumbersome without careful fixture design.
- Accessibility / Readability Notes: Reduced-motion parity must be first-class in the motion boards.
- Theme / Styling Notes: Motion captures should cover at least one dark and one light theme context.
- Motion / Interaction Notes where relevant: This task formalizes motion as a governed design system.
- Observability / Diagnostics Notes where relevant: Attach timing metadata and fixture IDs to every motion board.
- Rollback / Safety Notes: Keep the motion board additive if animation tuning is still evolving rapidly.
- References / Context: World-class polish depends as much on the discipline of motion as on the quality of static screens.
- Example scenarios where useful: A regression that makes toast dismiss motion too floaty is surfaced before release.

