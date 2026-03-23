# Phase 16 - Startup Welcome Onboarding And Empty State Redesign

## Goal

Redesign startup, welcome, onboarding, and empty states so every low-content moment feels premium, intentional, and visually complete.

## Definition Of Done

- startup and welcome surfaces feel fully redesigned and flagship-quality
- empty states across the product share a more polished, more premium language
- onboarding and rediscovery moments feel helpful without feeling noisy

## Tasks

### Phase ID: V27-P16
### Task ID: V27-P16-T01
### Task Title: Redesign Startup And Welcome As A Flagship First Impression Surface
- Priority: P0
- Category: Empty State / Onboarding UI
- Objective: rebuild startup into a premium first-run and reopen surface that reflects the full new visual identity.
- Why This Matters Now: the app’s first impression must visibly prove that the redesign is real and comprehensive.
- Visual Gap Statement: startup still carries duplicated shell logic, fallback styling, and a less premium presentation than the target product identity requires.
- User Experience Impact: a stronger startup surface raises trust and excitement before users even enter the workspace.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Prior Plan References: `v22` startup/onboarding phase, `v26` Phase 14
- Scope: startup shell, recent items, create/open affordances, supporting text, visual atmosphere, integration with shell chrome
- Out of Scope: recent-item persistence behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Screens / Components / Surfaces: welcome screen, recent items, startup actions
- Current Visual Behavior / Appearance: startup is functional but still exposes local styling debt and duplicated shell assumptions.
- Intended Visual Behavior / Appearance: startup feels premium, calm, and unmistakably “new” compared with the previous product generation.
- Icon / Visual Design Direction: elegant productivity welcome surface with refined actions, stronger hierarchy, and restrained atmosphere.
- Technical Styling Approach: redesign startup as its own flagship surface using the new shell, control, icon, and onboarding systems rather than adapting old shell fragments.
- Implementation Steps:
  1. Remove old startup-specific visual debt and fallback patterns.
  2. Redesign startup layout, hierarchy, and recent-item presentation.
  3. Rebuild create/open actions using the new control and icon systems.
  4. Validate startup with and without recent content.
- Validation Steps:
  1. Open the app into empty and recent-item states.
  2. Compare startup visually with the shell and settings surfaces.
  3. Confirm startup feels clearly redesigned rather than cleaned up.
- Acceptance Criteria: startup functions as a flagship proof point for the redesign and no longer reveals old visual DNA.
- Dependencies: Phase 02, Phase 04, Phase 12
- Parallelization Notes: can run alongside empty-state system redesign after common patterns are defined.
- Risks / Failure Modes: too much mood or atmosphere can make startup feel less professional.
- Accessibility / Readability Notes: recent-item metadata and action labels must remain crisp and high-contrast.
- Theme / Styling Notes: startup should not depend on special fallback accent logic.
- Motion / Interaction Notes where relevant: later startup micro-motion should remain subtle and optional.
- Cleanup / Consolidation Notes where relevant: remove duplicated shell logic and startup-only fallback styling where the redesign supersedes it.
- Rollback / Safety Notes: preserve startup functionality and navigation while redesigning the surface.
- References / Context: startup is one of the highest-impact first-impression surfaces in the app.
- Example scenarios where useful: launching the app with no open content should still feel premium and finished.

### Phase ID: V27-P16
### Task ID: V27-P16-T02
### Task Title: Redesign Empty States Across Panels Notebook Canvas And Structured Surfaces
- Priority: P0
- Category: Empty State / Onboarding UI
- Objective: give the product one redesigned empty-state language scaled appropriately for full-page, panel, and compact contexts.
- Why This Matters Now: empty states appear everywhere and still expose inconsistent tone and visual maturity.
- Visual Gap Statement: different surfaces still likely use different empty-state patterns, including thin placeholders and utility copy.
- User Experience Impact: stronger empty states improve perceived completeness and reduce the feeling of unfinished UI.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Prior Plan References: `v22` Phase 16, `v26` Phase 14
- Scope: full-page empties, panel empties, compact list/table empties, iconography, actions, supporting copy hierarchy
- Out of Scope: tutorial or walkthrough engines
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- Related Screens / Components / Surfaces: panel empty states, notebook empty states, canvas empty states, startup, empty tables/lists
- Current Visual Behavior / Appearance: empties are likely partly improved but still inconsistent in shell quality, iconography, and tone.
- Intended Visual Behavior / Appearance: every empty state feels clearly related to the redesigned product and appropriately scaled for its context.
- Icon / Visual Design Direction: calm, premium empty-state family with canonical iconography and disciplined supporting text.
- Technical Styling Approach: build a redesigned empty-state template system with full-page, panel, and compact variants.
- Implementation Steps:
  1. Inventory empty-state surfaces across major subsystems.
  2. Redesign scalable empty-state templates and icon usage.
  3. Replace thin or inconsistent empties across the app.
  4. Validate tone and spacing in varied contexts.
- Validation Steps:
  1. Trigger empty states across startup, canvas, notebook, explorer, and AV surfaces.
  2. Compare full-page and compact variants.
  3. Confirm the family feels coherent without becoming repetitive.
- Acceptance Criteria: empty states across the app feel like one redesigned system and no longer expose legacy placeholder aesthetics.
- Dependencies: Phase 03, Phase 14, V27-P16-T01
- Parallelization Notes: can proceed while specific surface redesign phases are wrapping up.
- Risks / Failure Modes: over-articulated empties can feel too marketing-heavy for a productivity app.
- Accessibility / Readability Notes: icons must support rather than replace clear text guidance.
- Theme / Styling Notes: empty-state contrast and emphasis must remain restrained but visible.
- Motion / Interaction Notes where relevant: later subtle reveal/loading motion must remain optional and quiet.
- Cleanup / Consolidation Notes where relevant: remove one-off empty-state fragments after the new templates are adopted.
- Rollback / Safety Notes: preserve context-specific clarity even while unifying the family.
- References / Context: empty-state redesign connects almost every other `v27` surface phase.
- Example scenarios where useful: an empty explorer and an empty canvas should feel like members of one product family without becoming identical.

### Phase ID: V27-P16
### Task ID: V27-P16-T03
### Task Title: Redesign Onboarding Hints And Contextual Guidance For Premium Discoverability
- Priority: P1
- Category: Empty State / Onboarding UI
- Objective: improve first-use and rediscovery cues so they feel more elegant, better integrated, and less ad hoc.
- Why This Matters Now: discoverability must improve with the redesign, but premium products solve it with restraint.
- Visual Gap Statement: onboarding hints and small educational cues can easily remain locally improvised and visually disconnected.
- User Experience Impact: users learn the product faster without feeling visually nagged or overloaded.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp`
- Prior Plan References: `v22` onboarding work, `v26` Phase 14
- Scope: subtle hints, rediscovery prompts, contextual empty-state guidance, first-run support surfaces
- Out of Scope: product-tour logic or analytics-driven onboarding
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/FirstRunWizard.cpp`
- Related Screens / Components / Surfaces: startup hints, canvas onboarding, first-run wizard, command palette guidance
- Current Visual Behavior / Appearance: educational surfaces likely vary and may still include local or glyph-based cues.
- Intended Visual Behavior / Appearance: guidance feels subtle, premium, and fully integrated with the redesigned visual system.
- Icon / Visual Design Direction: elegant supportive guidance with restrained iconography and low visual noise.
- Technical Styling Approach: redesign hint and guidance components using the new empty-state, feedback, and control systems rather than standalone visuals.
- Implementation Steps:
  1. Audit visible hint and onboarding-cue surfaces.
  2. Redesign their shells, hierarchy, and icon usage.
  3. Replace any outdated or over-literal visual cues.
  4. Validate that guidance remains helpful without dominating the UI.
- Validation Steps:
  1. Review the app as a first-time user across startup, canvas, and command surfaces.
  2. Check that hints remain discoverable but subtle.
  3. Confirm the redesigned guidance feels related to the rest of the product.
- Acceptance Criteria: onboarding and contextual guidance feel intentionally designed and premium rather than tutorial-like or improvised.
- Dependencies: V27-P16-T02, Phase 13, Phase 15
- Parallelization Notes: can proceed once the empty-state and feedback families are defined.
- Risks / Failure Modes: over-designed hints can clutter the product; under-designed hints can reduce discoverability.
- Accessibility / Readability Notes: supporting text and actions must remain clear and not icon-dependent.
- Theme / Styling Notes: hints should remain low-noise in both themes.
- Motion / Interaction Notes where relevant: any attention-directing motion later should be minimal and reduced-motion friendly.
- Cleanup / Consolidation Notes where relevant: merge duplicate hint styles and remove ad hoc onboarding visuals.
- Rollback / Safety Notes: keep essential first-use guidance available while redesigning presentation.
- References / Context: discoverability should improve in `v27`, but without sliding into generic onboarding design.
- Example scenarios where useful: a first-use canvas hint should feel elegant and product-grade, not like a temporary tooltip.
