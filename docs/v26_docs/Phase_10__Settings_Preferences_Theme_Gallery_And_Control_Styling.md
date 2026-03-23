# Phase 10 - Settings Preferences Theme Gallery And Control Styling

## Goal

Upgrade settings, preferences, theme gallery, and control-heavy surfaces so they feel sophisticated, clear, and unmistakably premium.

## Definition Of Done

- settings information architecture and control styling are visually cohesive
- theme gallery feels premium and visually aligned with settings
- no emoji-prefix or visibly temporary settings presentation remains

## Tasks

### Phase ID: V26-P10
### Task ID: V26-P10-T01
### Task Title: Remove Settings Visual Debt And Establish A Premium Preference Row System
- Priority: P0
- Category: Settings UI
- Objective: Replace inconsistent settings-row presentation with a standardized, high-clarity preference system.
- Why This Matters Now: Settings is one of the strongest visible signals of UI maturity in a desktop IDE.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` still contains emoji icon-prefix patterns and heavy local styling logic.
- User Experience Impact: Better settings UI improves trust and reduces friction when configuring the product.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Prior Plan References: `v22` settings UI, `v25` release-readiness settings closure
- Scope: category presentation, preference row layout, labels/help text, toggles, inputs, selects, warnings, reset affordances
- Out of Scope: settings schema or persistence logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Screens / Components / Surfaces: settings dialog, settings categories, settings rows, advanced settings groups
- Current UI Behavior / Appearance: settings are functional but visually inconsistent and still contain placeholder-like icon treatments.
- Intended UI Behavior / Appearance: settings feel crisp, readable, premium, and dense without appearing cluttered.
- Visual / Interaction Design Direction: premium preferences surface with disciplined hierarchy and calm form styling.
- Technical Styling Approach: define shared settings-row roles and retire emoji or local-prefix-based presentation.
- Implementation Steps:
  1. Remove emoji/category-prefix styling and replace with proper iconography or typography hierarchy.
  2. Standardize row spacing, label/help layouts, and control alignment.
  3. Normalize warning/reset/advanced-setting treatments.
  4. Validate large settings categories and search-filtered results.
- Validation Steps:
  1. Browse several categories with varied control types.
  2. Search within settings and review filtered presentation.
  3. Confirm no row feels visually improvised.
- Acceptance Criteria: settings rows use one high-quality pattern and no visible placeholder iconography remains.
- Dependencies: V26-P01-T01, V26-P01-T02, V26-P16-T01
- Parallelization Notes: should start early because control styling patterns are reused elsewhere.
- Risks / Failure Modes: excessive visual polish can reduce density efficiency; keep settings practical.
- Accessibility / Readability Notes: help text and warning states must stay legible at common DPI scales.
- Theme / Styling Notes: control states must remain consistent with global action-state rules.
- Motion / Interaction Notes where relevant: toggles and selects can adopt shared subtle motion later.
- Cleanup / Consolidation Notes where relevant: remove local font/color helper duplication from settings code.
- Rollback / Safety Notes: preserve form behavior and validation logic while changing presentation.
- References / Context: settings should feel as premium as the editor, not like an internal utility page.
- Example scenarios where useful: a long advanced settings category should remain readable and calm.

### Phase ID: V26-P10
### Task ID: V26-P10-T02
### Task Title: Bring Theme Gallery And Theme Browsing Surfaces To Showcase Quality
- Priority: P0
- Category: Settings UI
- Objective: Turn theme browsing into a premium showcase surface instead of a utility grid with local styling.
- Why This Matters Now: Theme browsing is both a settings task and a product-marketing moment inside the app.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp` contains heavy local font/color handling and does not yet read as a fully systematized premium gallery.
- User Experience Impact: A strong gallery improves delight, confidence in theme quality, and the perceived sophistication of the entire app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Prior Plan References: `v22` theme gallery and UI polish, `v25` syntax/theme/icon closure
- Scope: gallery card styling, preview fidelity, category hierarchy, selection states, metadata and action layout
- Out of Scope: theme-import backend logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Related Screens / Components / Surfaces: theme gallery, theme previews, theme selection states
- Current UI Behavior / Appearance: gallery styling is functional but still contains substantial local visual logic and uneven premium finish.
- Intended UI Behavior / Appearance: the gallery feels aspirational, elegant, and consistent with the rest of the app’s design system.
- Visual / Interaction Design Direction: showcase-style gallery with refined previews, clear hierarchy, and restrained richness.
- Technical Styling Approach: introduce shared gallery-card and preview-shell patterns tied to theme tokens.
- Implementation Steps:
  1. Audit current gallery layout and card styling.
  2. Standardize typography, card spacing, and preview framing.
  3. Improve selected, hovered, and focused states.
  4. Validate preview accuracy across theme families.
- Validation Steps:
  1. Browse multiple built-in themes in light and dark mode contexts.
  2. Check preview-card density and metadata clarity.
  3. Verify selection feels premium and obvious without noise.
- Acceptance Criteria: theme gallery feels like a polished product showcase surface, not a utilitarian picker.
- Dependencies: V26-P01-T01, V26-P01-T02, V26-P15-T01
- Parallelization Notes: can run alongside settings-row work once shared control and card standards are known.
- Risks / Failure Modes: over-styled previews can become visually loud or misleading.
- Accessibility / Readability Notes: preview metadata and selection states must remain clear regardless of theme.
- Theme / Styling Notes: gallery itself must remain theme-correct while displaying alternate themes.
- Motion / Interaction Notes where relevant: card hover and selection motion should be subtle.
- Cleanup / Consolidation Notes where relevant: retire gallery-specific local styling constants where shared roles suffice.
- Rollback / Safety Notes: keep preview fidelity and performance intact while refining visuals.
- References / Context: theme gallery is a signature premium-surface opportunity.
- Example scenarios where useful: previewing a dark theme from a light shell should still look deliberate and trustworthy.

### Phase ID: V26-P10
### Task ID: V26-P10-T03
### Task Title: Unify Control Styling Across Settings, Dialogs, Panels, And Embedded Forms
- Priority: P1
- Category: Control Styling
- Objective: Standardize toggles, buttons, dropdowns, text inputs, segmented controls, and inline forms across the product.
- Why This Matters Now: Control inconsistency is one of the fastest ways to lose premium credibility.
- UI Gap Statement: Control-heavy surfaces have evolved independently and likely drift in spacing, shape, and state feedback.
- User Experience Impact: Consistent controls reduce friction and make every surface feel more trustworthy.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Prior Plan References: `v22` controls and settings work, `v25` visible closure
- Scope: shared control metrics, control label alignment, form spacing, disabled state, focus state, inline validation placement
- Out of Scope: control behavior rewrites
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`
- Related Screens / Components / Surfaces: settings forms, dialogs, startup actions, inspector controls
- Current UI Behavior / Appearance: some controls already look polished, but the overall control language is not yet fully unified.
- Intended UI Behavior / Appearance: controls everywhere share one premium family with predictable metrics and state behavior.
- Visual / Interaction Design Direction: refined desktop controls with subtle depth and excellent optical alignment.
- Technical Styling Approach: build a small set of shared control roles and migrate the highest-traffic controls first.
- Implementation Steps:
  1. Catalog control variants and current state styling.
  2. Define preferred metrics and state treatment per control family.
  3. Refactor settings, dialogs, and startup surfaces to shared roles.
  4. Validate mixed-control forms and compact layouts.
- Validation Steps:
  1. Compare controls across settings, dialogs, and startup surfaces.
  2. Verify focus, disabled, hover, and pressed states.
  3. Check alignment of labels and support text.
- Acceptance Criteria: controls across core surfaces feel clearly related and premium.
- Dependencies: V26-P01-T01, V26-P01-T03, V26-P10-T01
- Parallelization Notes: can proceed once the settings-row system is established.
- Risks / Failure Modes: excessive unification can ignore legitimate context differences.
- Accessibility / Readability Notes: control states and focus rings must remain obvious and keyboard-friendly.
- Theme / Styling Notes: controls should be fully theme-driven rather than color hardcoded.
- Motion / Interaction Notes where relevant: shared control motion can be layered later.
- Cleanup / Consolidation Notes where relevant: consolidate form/control helpers instead of proliferating custom implementations.
- Rollback / Safety Notes: retain control behavior while standardizing appearance.
- References / Context: this task prevents future surface-specific drift.
- Example scenarios where useful: a toggle in settings and a toggle in a dialog should feel like the same component family.
