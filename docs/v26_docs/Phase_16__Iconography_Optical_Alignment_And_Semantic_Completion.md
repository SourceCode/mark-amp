# Phase 16 - Iconography Optical Alignment And Semantic Completion

## Goal

Finish icon system adoption so icon use feels deliberate, aligned, and semantically consistent across the entire app.

## Definition Of Done

- unicode stand-ins, emoji-like prefixes, and inconsistent icon sizing are removed from visible paths
- icon-only and icon-plus-text controls use shared alignment and spacing rules
- icon semantics are stable across shell, settings, panels, and specialty surfaces

## Tasks

### Phase ID: V26-P16
### Task ID: V26-P16-T01
### Task Title: Replace Unicode And Emoji Stand-Ins With Final Iconography
- Priority: P0
- Category: Iconography
- Objective: Remove the last visibly temporary icon treatments from the main UX path.
- Why This Matters Now: Placeholder iconography is one of the fastest ways to break premium perception.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` still expose unicode or emoji-like stand-ins.
- User Experience Impact: Final iconography materially improves polish, clarity, and trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/resources/icons/lucide/`
- Prior Plan References: `v22` iconography pass, `v25` syntax/theme/icon visible closure
- Scope: visible unicode icons, emoji prefixes, placeholder icon labels, missing semantic icon replacements
- Out of Scope: hidden debug-only tooling
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/resources/icons/lucide/`
- Related Screens / Components / Surfaces: canvas tool rail, settings categories, panel actions
- Current UI Behavior / Appearance: some visible surfaces still communicate state or category through stand-in glyphs rather than final icon assets.
- Intended UI Behavior / Appearance: every visible iconographic cue uses the final icon system and aligns with the product’s premium tone.
- Visual / Interaction Design Direction: clean modern icon system with disciplined semantics and no novelty glyphs.
- Technical Styling Approach: use the existing icon asset pipeline consistently and remove local text-prefix substitutes.
- Implementation Steps:
  1. Inventory visible stand-in icons and emoji prefixes.
  2. Map them to final icon assets with clear semantics.
  3. Update affected UI surfaces and spacing.
  4. Validate theme correctness and sizing.
- Validation Steps:
  1. Review startup, settings, canvas, and panel action areas.
  2. Confirm no unicode or emoji stand-ins remain on visible product paths.
  3. Verify icons render crisply in both themes.
- Acceptance Criteria: visible placeholder iconography is eliminated from the main UI path.
- Dependencies: V26-P10-T01, V26-P09-T01
- Parallelization Notes: can begin as soon as target surfaces are identified.
- Risks / Failure Modes: icon replacement without spacing retuning can create alignment drift.
- Accessibility / Readability Notes: icon-only meaning must remain supported by labels, tooltips, or context.
- Theme / Styling Notes: final icons must inherit theme colors correctly.
- Motion / Interaction Notes where relevant: icon-state changes should remain subtle and consistent.
- Cleanup / Consolidation Notes where relevant: remove old stand-in mapping code and prefix tables.
- Rollback / Safety Notes: confirm icon replacements preserve semantic meaning before removing old glyphs.
- References / Context: this task has high visible impact with limited scope.
- Example scenarios where useful: the canvas tool palette should stop feeling like a prototype once final icons replace glyph stand-ins.

### Phase ID: V26-P16
### Task ID: V26-P16-T02
### Task Title: Normalize Icon Size, Optical Alignment, And Icon-Text Spacing Across Core Surfaces
- Priority: P1
- Category: Iconography
- Objective: Make icon usage feel refined through consistent size, baseline alignment, and spacing.
- Why This Matters Now: Even with final icons, inconsistent optical alignment can keep the UI feeling amateur.
- UI Gap Statement: Shell, toolbar, panel, and settings surfaces likely use different icon metrics and alignment assumptions.
- User Experience Impact: Better optical alignment makes the whole interface feel more expensive and intentional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` icon polish, `v25` icon visible closure
- Scope: icon sizes, icon-button metrics, label gaps, optical centering, badge/icon balance
- Out of Scope: adding new icons
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ActivityBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: toolbar, activity bar, settings rows, panel headers, notifications
- Current UI Behavior / Appearance: icon use is more advanced than earlier passes but likely still uneven in size and spacing.
- Intended UI Behavior / Appearance: icons feel optically aligned, balanced with text, and uniform in meaning and scale.
- Visual / Interaction Design Direction: disciplined iconography that supports the layout rather than competing with it.
- Technical Styling Approach: define shared icon metrics and alignment roles for common control families.
- Implementation Steps:
  1. Audit icon sizes and baseline placement across core surfaces.
  2. Define standard icon metrics by control family.
  3. Refactor spacing and paint/layout code to those metrics.
  4. Validate on high-density and icon-only surfaces.
- Validation Steps:
  1. Compare toolbar, activity bar, settings, and panels side by side.
  2. Review icon-plus-text and icon-only controls.
  3. Confirm alignment under light and dark themes.
- Acceptance Criteria: icons across the shell and major surfaces feel consistently aligned and sized.
- Dependencies: V26-P16-T01, V26-P03-T01, V26-P10-T03
- Parallelization Notes: can run concurrently with later cleanup once shared metrics are agreed.
- Risks / Failure Modes: forcing one icon size on every context can hurt legibility or density.
- Accessibility / Readability Notes: icon-only controls still need clear discoverability through tooltip or context support.
- Theme / Styling Notes: icon contrast should remain strong without appearing heavier than text.
- Motion / Interaction Notes where relevant: state changes should not shift icon alignment.
- Cleanup / Consolidation Notes where relevant: remove local icon offset hacks after shared metrics land.
- Rollback / Safety Notes: verify hit targets and labels remain correct when icon metrics change.
- References / Context: this is a premium-finish task more than a raw functionality task.
- Example scenarios where useful: a settings category icon and a toolbar icon should feel related without becoming identical in scale.

### Phase ID: V26-P16
### Task ID: V26-P16-T03
### Task Title: Rationalize Icon Semantics Across Navigation, Actions, Feedback, And Empty States
- Priority: P1
- Category: Iconography
- Objective: Ensure that the same conceptual meaning uses the same icon family and visual tone across the app.
- Why This Matters Now: Semantic inconsistency creates subtle friction even when individual icons look good.
- UI Gap Statement: Incremental implementation across many surfaces can cause semantic icon drift.
- User Experience Impact: Consistent icon semantics improve learnability and reduce hesitation.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v22` icon system work
- Scope: navigation icons, settings category icons, feedback severity icons, empty-state support icons
- Out of Scope: icon asset creation beyond what is needed to fill true semantic gaps
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: toolbar, startup, notifications, settings, empty states
- Current UI Behavior / Appearance: icon choices may be individually acceptable but semantically inconsistent across surfaces.
- Intended UI Behavior / Appearance: icon meaning is stable and instantly recognizable across the product.
- Visual / Interaction Design Direction: coherent icon semantics with restrained expressive variation.
- Technical Styling Approach: create a semantic icon map for high-frequency concepts and update visible outliers.
- Implementation Steps:
  1. Build a semantic inventory of repeated UI concepts.
  2. Pick canonical icons for each concept.
  3. Replace inconsistent choices on visible surfaces.
  4. Document the map for future UI work.
- Validation Steps:
  1. Review icon usage across shell, settings, notifications, and empty states.
  2. Check for accidental semantic collisions.
  3. Confirm consistency in both themes and at small sizes.
- Acceptance Criteria: repeated concepts use stable iconography across major surfaces.
- Dependencies: V26-P16-T01
- Parallelization Notes: can run in parallel with optical-alignment work after icon replacements are known.
- Risks / Failure Modes: over-standardizing could reduce helpful contextual nuance.
- Accessibility / Readability Notes: maintain label support where icon semantics may still be ambiguous.
- Theme / Styling Notes: semantic icons should not depend on unique colors to communicate meaning.
- Motion / Interaction Notes where relevant: icon swaps or state transitions should remain subtle.
- Cleanup / Consolidation Notes where relevant: remove old icon-selection branches and icon-prefix code.
- Rollback / Safety Notes: verify semantic replacements with the surrounding action labels.
- References / Context: this task prevents visual coherence from drifting after `v26`.
- Example scenarios where useful: “search,” “settings,” and “warning” should not use different icon meanings depending on surface.
