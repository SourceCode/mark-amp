# Phase 18 - Accessibility Visuals Contrast Focus And Reduced Motion Redesign

## Goal

Redesign accessibility-visible cues so the new visual system is not only beautiful, but also clearer, more navigable, and more inclusive.

## Definition Of Done

- focus visibility is redesigned and consistent across all custom surfaces
- critical states are distinguishable beyond subtle color differences
- reduced-motion treatment is integrated into the redesigned interaction system

## Tasks

### Phase ID: V27-P18
### Task ID: V27-P18-T01
### Task Title: Redesign The Focus Ring And Keyboard Current Item System Across All Major Surfaces
- Priority: P0
- Category: Accessibility Visuals
- Objective: rebuild focus and keyboard current-item visuals so they feel intentionally designed and strong enough for custom UI.
- Why This Matters Now: `v27` redesigns many custom surfaces, and custom UI often fails keyboard users unless focus is redesigned deliberately.
- Visual Gap Statement: focus visibility is likely uneven across custom shell, tabs, settings, canvas, notebook, and panel controls.
- User Experience Impact: stronger focus visibility improves usability, confidence, and professional accessibility quality.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` accessibility phase, `v26` Phase 18
- Scope: focus ring style, current-item indication, icon-only control focus, list and panel focus, overlay focus visibility
- Out of Scope: focus-routing logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: controls, tabs, panels, dialogs, notebook, canvas, dense rows
- Current Visual Behavior / Appearance: focus visibility exists in places, but a redesigned system-wide focus language is not yet guaranteed.
- Intended Visual Behavior / Appearance: every major interactive surface exposes a clearly redesigned, premium focus treatment.
- Icon / Visual Design Direction: crisp accessible focus system that looks intentionally part of the new design, not bolted on.
- Technical Styling Approach: define shared focus roles and apply them to every custom-painted or custom-composed surface class.
- Implementation Steps:
  1. Audit focus visibility across redesigned surfaces.
  2. Redesign focus ring and current-item indicators.
  3. Apply the new focus system across core shell, control, and content-adjacent surfaces.
  4. Validate keyboard-only workflows across representative journeys.
- Validation Steps:
  1. Navigate shell, settings, dialogs, and panels by keyboard.
  2. Review icon-only and dense-row focus states.
  3. Compare focus clarity in both theme families.
- Acceptance Criteria: keyboard focus is consistently visible and visually integrated across all major redesigned surfaces.
- Dependencies: redesigned control, shell, and panel phases
- Parallelization Notes: can proceed as redesigned surfaces stabilize.
- Risks / Failure Modes: focus styling that is too subtle will fail accessibility; too aggressive can visually fight the redesign.
- Accessibility / Readability Notes: focus visibility is a core success criterion for `v27`.
- Theme / Styling Notes: focus must remain reliable across all backgrounds and themes.
- Motion / Interaction Notes where relevant: focus transitions later should remain quick and reduced-motion compatible.
- Cleanup / Consolidation Notes where relevant: retire any local focus hacks once the new system is adopted.
- Rollback / Safety Notes: do not remove existing focus indicators before redesigned replacements are validated.
- References / Context: a premium IDE-style app must support keyboard navigation clearly across custom UI.
- Example scenarios where useful: tabbing through a dialog and then into a settings list should feel visually obvious and coherent.

### Phase ID: V27-P18
### Task ID: V27-P18-T02
### Task Title: Redesign Contrast And Non Color State Differentiation In Dense And Interactive UI
- Priority: P1
- Category: Accessibility Visuals
- Objective: improve the readability and state clarity of the redesigned interface without breaking its premium restraint.
- Why This Matters Now: redesign work often trends toward subtlety; this phase ensures subtlety does not become ambiguity.
- Visual Gap Statement: dense rows, disabled states, warnings, and metadata can become too subtle after a premium redesign unless explicitly checked.
- User Experience Impact: stronger contrast and state differentiation reduce confusion and make the redesign more usable for more people.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` accessibility and density work, `v26` Phase 18
- Scope: selected/disabled/hover/current states, metadata contrast, warning/error differentiation, table and tree readability
- Out of Scope: formal accessibility certification workflows
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: dense rows, settings, feedback, editor states, structured data surfaces
- Current Visual Behavior / Appearance: after redesign, some low-noise states may still need explicit non-color reinforcement.
- Intended Visual Behavior / Appearance: important distinctions remain obvious without reverting to noisy or harsh styling.
- Icon / Visual Design Direction: accessible precision with controlled but reliable differentiation.
- Technical Styling Approach: add secondary visual cues such as border, weight, shape, or placement where color alone is insufficient.
- Implementation Steps:
  1. Review redesigned surfaces for subtle-state failures.
  2. Add or tune non-color cues where necessary.
  3. Validate dense and compact views under realistic use.
  4. Recheck against feedback and theme parity systems.
- Validation Steps:
  1. Compare selected, disabled, warning, and error states side by side.
  2. Review dense lists and settings forms at common display conditions.
  3. Confirm readability improves without making the UI visually busy.
- Acceptance Criteria: critical states are reliably distinguishable across redesigned surfaces without undermining overall aesthetic restraint.
- Dependencies: Phase 15, Phase 17
- Parallelization Notes: can proceed with focus redesign once enough surfaces are visually stable.
- Risks / Failure Modes: overcorrection can reintroduce visual noise.
- Accessibility / Readability Notes: this task guards against “premium minimalism” becoming reduced usability.
- Theme / Styling Notes: contrast and non-color cues must work equally well in both themes.
- Motion / Interaction Notes where relevant: state clarity must not depend on animation.
- Cleanup / Consolidation Notes where relevant: consolidate any ad hoc contrast overrides into shared semantic roles.
- Rollback / Safety Notes: compare changes in context before finalizing; isolated contrast improvements can distort the broader system.
- References / Context: accessibility visuals should feel designed, not merely compliant.
- Example scenarios where useful: a disabled settings control and a selected search result should both remain unmistakably identifiable.

### Phase ID: V27-P18
### Task ID: V27-P18-T03
### Task Title: Redesign Reduced Motion Visual Behavior For The New Interaction System
- Priority: P1
- Category: Accessibility Visuals
- Objective: ensure the redesigned motion and micro-interaction system degrades gracefully and elegantly when motion is reduced.
- Why This Matters Now: `v27` intends to improve tactile quality, so reduced-motion support must evolve with it.
- Visual Gap Statement: without explicit redesign, reduced-motion behavior often becomes patchy and visually second-class.
- User Experience Impact: users who prefer reduced motion still receive a complete, premium visual experience.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Prior Plan References: `v22` motion/accessibility work, `v26` Phase 18
- Scope: overlay transitions, hover/press response, progress/loading motion, panel transitions, drag/resize feedback alternatives
- Out of Scope: OS-level preference integration if not already supported
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Screens / Components / Surfaces: palette, dialogs, menus, toolbar, tabs, notifications, progress and drag states
- Current Visual Behavior / Appearance: redesigned motion could easily outpace reduced-motion treatment unless it is considered explicitly.
- Intended Visual Behavior / Appearance: reduced-motion mode preserves clarity and premium feel without resorting to abrupt or visually degraded state changes.
- Icon / Visual Design Direction: accessible calm that still feels designed and responsive.
- Technical Styling Approach: define alternate timing and static-state emphasis rules for every animated surface family.
- Implementation Steps:
  1. Inventory all redesigned animated or transitioning surfaces.
  2. Redesign reduced-motion equivalents for those surfaces.
  3. Validate that static-state clarity remains sufficient.
  4. Add reduced-motion review to the final QA matrix.
- Validation Steps:
  1. Review dialogs, palette, notifications, and controls in reduced-motion mode.
  2. Compare the feel of reduced-motion and standard-motion variants.
  3. Confirm no interaction depends on animation to be understood.
- Acceptance Criteria: reduced-motion mode feels intentional, complete, and clearly part of the redesigned system.
- Dependencies: Phase 19
- Parallelization Notes: best finalized alongside motion and micro-interaction redesign.
- Risks / Failure Modes: incomplete reduced-motion coverage creates inconsistent responsiveness and weakens accessibility quality.
- Accessibility / Readability Notes: reduced-motion mode must remain a first-class visual experience.
- Theme / Styling Notes: reduced-motion should not require separate visual styling branches beyond timing/state behavior.
- Motion / Interaction Notes where relevant: strip non-essential travel and opacity effects while preserving hierarchy.
- Cleanup / Consolidation Notes where relevant: centralize reduced-motion handling instead of scattering local exceptions.
- Rollback / Safety Notes: keep motion fallbacks configurable and easy to verify.
- References / Context: reduced-motion quality is part of the premium bar, not only an accessibility checklist.
- Example scenarios where useful: opening the command palette with reduced motion should still feel polished and immediate.
