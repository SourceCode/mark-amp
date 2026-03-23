# Phase 18 - Accessibility Visual Contrast Focus And Reduced Motion

## Goal

Improve visible accessibility quality so the UI is easier to read, easier to navigate by keyboard, and more comfortable without compromising the premium visual language.

## Definition Of Done

- focus states are consistently visible across all major surfaces
- contrast and non-color state differentiation are materially improved
- reduced-motion treatment is deliberate and complete on animated surfaces

## Tasks

### Phase ID: V26-P18
### Task ID: V26-P18-T01
### Task Title: Standardize Focus Ring And Keyboard Navigation Visibility Across Major Surfaces
- Priority: P0
- Category: Accessibility Visuals
- Objective: Create one strong, theme-correct focus treatment across controls, lists, tabs, editor-adjacent UI, notebook, and canvas chrome.
- Why This Matters Now: Focus visibility is a quality and usability requirement, and it often drifts badly in custom desktop UI.
- UI Gap Statement: Multiple custom surfaces exist, but consistent keyboard-focus presentation is not yet guaranteed.
- User Experience Impact: Better focus visibility improves accessibility, confidence, and professional usability.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` accessibility visual work, `v25` release-readiness focus on trustworthiness
- Scope: focus ring style, keyboard current-item indicators, focus visibility in overlays, focus for icon-only controls
- Out of Scope: keyboard command routing changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Screens / Components / Surfaces: toolbar, tabs, settings controls, dense rows, canvas tools, dialogs
- Current UI Behavior / Appearance: focus treatment likely varies by surface and may be too subtle or absent in custom-painted components.
- Intended UI Behavior / Appearance: keyboard focus is always clearly visible and visually aligned with the app’s premium styling.
- Visual / Interaction Design Direction: crisp accessible focus treatment that feels designed, not bolted on.
- Technical Styling Approach: define shared focus tokens and apply them to custom-painted and standard controls alike.
- Implementation Steps:
  1. Audit focus visibility across custom surfaces.
  2. Define shared focus ring and current-item styles.
  3. Apply focus styling to high-traffic surfaces first.
  4. Validate keyboard-only navigation across the shell.
- Validation Steps:
  1. Navigate toolbar, tabs, settings, and dialogs by keyboard.
  2. Compare focus visibility in light and dark themes.
  3. Verify icon-only controls have clear focus indication.
- Acceptance Criteria: keyboard focus is obvious and consistent across all major surfaces.
- Dependencies: V26-P01-T03, V26-P15-T03
- Parallelization Notes: can run once shared interaction-state conventions are stable.
- Risks / Failure Modes: too much focus styling can look noisy; too little hurts accessibility.
- Accessibility / Readability Notes: this is the primary accessibility-visual task for keyboard users.
- Theme / Styling Notes: focus contrast must be preserved in both themes and on all surface tiers.
- Motion / Interaction Notes where relevant: focus transitions should be quick and respect reduced motion.
- Cleanup / Consolidation Notes where relevant: remove per-surface focus hacks once a shared system exists.
- Rollback / Safety Notes: ensure custom focus paint never obscures essential content.
- References / Context: focus visibility is a non-negotiable quality bar for a premium IDE-style product.
- Example scenarios where useful: tabbing through settings should always reveal a clear focus target.

### Phase ID: V26-P18
### Task ID: V26-P18-T02
### Task Title: Improve Contrast And Non-Color Differentiation For Dense And Interactive States
- Priority: P1
- Category: Accessibility Visuals
- Objective: Strengthen readability and state distinction in areas where low-noise styling may currently be too subtle.
- Why This Matters Now: Premium restraint should not come at the cost of clarity, especially in dense productivity surfaces.
- UI Gap Statement: Selected, disabled, warning, and metadata states in dense panels and controls may still rely too heavily on color nuance.
- User Experience Impact: Better state differentiation reduces errors and improves comfort for low-vision and fatigued users.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Prior Plan References: `v22` accessibility and density work, `v25` visible closure
- Scope: contrast tuning, disabled-state clarity, metadata legibility, selected-row indicators, warning and error differentiation
- Out of Scope: full WCAG certification process
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
- Related Screens / Components / Surfaces: dense rows, settings forms, editor dynamic states, warning/error feedback
- Current UI Behavior / Appearance: some low-noise states may now be too subtle after aesthetic refinement.
- Intended UI Behavior / Appearance: all important states remain readable and distinguishable without breaking the premium tone.
- Visual / Interaction Design Direction: accessible precision with restrained but reliable state cues.
- Technical Styling Approach: adjust contrast tiers and add secondary shape or border cues where color alone is insufficient.
- Implementation Steps:
  1. Review low-contrast and dense interactive states across major surfaces.
  2. Identify states that rely too heavily on subtle color shifts.
  3. Add complementary border, shape, or typography cues where needed.
  4. Re-test in both themes and common display conditions.
- Validation Steps:
  1. Compare disabled, selected, warning, and error states side by side.
  2. Test on standard and dim displays where possible.
  3. Verify readability in narrow panels and dense tables.
- Acceptance Criteria: important states remain clearly distinguishable without sacrificing visual restraint.
- Dependencies: V26-P13-T01, V26-P15-T03
- Parallelization Notes: can run alongside focus-ring work.
- Risks / Failure Modes: increasing clarity too aggressively can reintroduce visual noise.
- Accessibility / Readability Notes: prioritize real readability improvements, not just theoretical token compliance.
- Theme / Styling Notes: contrast adjustments must remain theme-balanced.
- Motion / Interaction Notes where relevant: do not rely on animation to communicate critical state differences.
- Cleanup / Consolidation Notes where relevant: consolidate ad hoc contrast overrides into shared tokens.
- Rollback / Safety Notes: review all contrast changes in context to avoid isolated overcorrection.
- References / Context: this is the main safeguard against premium minimalism becoming ambiguity.
- Example scenarios where useful: a selected search result and an error problem row should remain immediately distinguishable.

### Phase ID: V26-P18
### Task ID: V26-P18-T03
### Task Title: Complete Reduced-Motion Visual Fallbacks For Animated Surfaces
- Priority: P1
- Category: Accessibility Visuals
- Objective: Ensure animated or transitioning surfaces remain fully usable and premium when motion is reduced.
- Why This Matters Now: Motion has been added deliberately in `v26`; it now needs an equally deliberate accessibility fallback.
- UI Gap Statement: Without explicit fallback work, reduced-motion behavior tends to be partial and inconsistent.
- User Experience Impact: Users who prefer reduced motion still get a refined, coherent interface.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Prior Plan References: `v22` motion/accessibility work
- Scope: dialogs, palette, menus, hover transitions, progress motion, toast motion
- Out of Scope: system-level preference detection if not already available in the platform layer
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- Related Screens / Components / Surfaces: command palette, dialogs, notifications, hover states, progress indicators
- Current UI Behavior / Appearance: reduced-motion support is not yet guaranteed across newly refined motion surfaces.
- Intended UI Behavior / Appearance: animated surfaces gracefully fall back to near-instant state changes while preserving hierarchy and polish.
- Visual / Interaction Design Direction: accessible calm without feeling visually degraded.
- Technical Styling Approach: centralize motion enable/disable handling and ensure every animated surface can use static-state clarity alone.
- Implementation Steps:
  1. Inventory all animated or transitioning surfaces.
  2. Add reduced-motion branches or shared helpers where missing.
  3. Verify static fallbacks still communicate surface changes clearly.
  4. Test with reduced-motion enabled.
- Validation Steps:
  1. Open dialogs, palette, and notifications in reduced-motion mode.
  2. Compare hover and press response on high-frequency controls.
  3. Confirm no state relies on animation to be understood.
- Acceptance Criteria: all major animated surfaces have clean reduced-motion behavior without visible quality loss.
- Dependencies: V26-P17-T01, V26-P17-T02, V26-P17-T03
- Parallelization Notes: naturally follows motion work and can be finalized alongside final QA.
- Risks / Failure Modes: incomplete fallback coverage can produce inconsistent responsiveness.
- Accessibility / Readability Notes: reduced-motion mode must remain a first-class experience, not an afterthought.
- Theme / Styling Notes: reduced-motion should not require separate theme styling.
- Motion / Interaction Notes where relevant: disable or sharply shorten non-essential transitions.
- Cleanup / Consolidation Notes where relevant: consolidate motion gating logic centrally.
- Rollback / Safety Notes: keep motion defaults configurable while validating fallbacks.
- References / Context: this task completes the accessibility side of the motion layer.
- Example scenarios where useful: opening the command palette in reduced-motion mode should still feel polished and immediate.
