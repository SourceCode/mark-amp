# Phase 12 - Settings Preferences Theme Management And Control Redesign

## Goal

Redesign settings, preferences, theme management, and control-heavy interfaces so they feel elegant, premium, and purpose-built rather than utility-like.

## Definition Of Done

- settings and theme management surfaces feel fully redesigned
- all control families use the new icon and visual systems consistently
- no emoji-prefix, placeholder, or ad hoc control language remains on the visible settings path

## Tasks

### Phase ID: V27-P12
### Task ID: V27-P12-T01
### Task Title: Redesign Settings Shell Categories And Preference Row Architecture
- Priority: P0
- Category: Settings UI
- Objective: rebuild settings structure, category presentation, row hierarchy, and supporting information architecture.
- Why This Matters Now: settings are one of the strongest tests of whether the redesign reaches beyond hero surfaces.
- Visual Gap Statement: settings still carry emoji-prefix and local styling debt that directly undermines premium quality.
- User Experience Impact: better settings presentation improves trust, readability, and perceived product maturity.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Prior Plan References: `v21` settings work, `v22` settings phase, `v26` Phase 10
- Scope: category sidebar, search/filter area, row layout, label/help hierarchy, reset and warning placements, advanced-group structure
- Out of Scope: settings persistence or schema changes
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Screens / Components / Surfaces: settings home, categories, settings rows, advanced groups
- Current Visual Behavior / Appearance: settings work but still expose emoji prefixes, local layout decisions, and utility-style row architecture.
- Intended Visual Behavior / Appearance: settings feel like a premium configuration environment with redesigned hierarchy and stronger scannability.
- Icon / Visual Design Direction: sophisticated productivity settings surface with clear sections, canonical icons, and elegant information density.
- Technical Styling Approach: reauthor settings shell and row patterns around the redesigned control, icon, typography, and spacing systems.
- Implementation Steps:
  1. Remove emoji-prefix and legacy category icon patterns.
  2. Redesign settings shell, category rail, and search/filter header.
  3. Rebuild preference row architecture and supporting help hierarchy.
  4. Validate large categories and mixed control types.
- Validation Steps:
  1. Browse several categories with different control densities.
  2. Search within settings and inspect filtered states.
  3. Confirm row rhythm and hierarchy feel clearly redesigned.
- Acceptance Criteria: settings no longer read as a utility sheet and no visible placeholder iconography remains on the path.
- Dependencies: Phase 01 through Phase 05
- Parallelization Notes: can proceed alongside dialog/control redesign.
- Risks / Failure Modes: excessive spaciousness can reduce efficiency in large settings categories.
- Accessibility / Readability Notes: support text and warnings must stay readable and well grouped.
- Theme / Styling Notes: settings must align with the redesigned shell and overlay systems.
- Motion / Interaction Notes where relevant: later control transitions should be subtle and consistent.
- Cleanup / Consolidation Notes where relevant: remove old category-prefix and local font/color logic after redesign.
- Rollback / Safety Notes: keep settings semantics stable while redesigning presentation.
- References / Context: settings should become one of the strongest “premium product” surfaces, not one of the weakest.
- Example scenarios where useful: moving from the shell into settings should feel like entering a more focused but equally premium environment.

### Phase ID: V27-P12
### Task ID: V27-P12-T02
### Task Title: Redesign Theme Gallery And Theme Management As A Showcase Surface
- Priority: P0
- Category: Settings UI
- Objective: turn theme management into a showcase-quality visual experience that reinforces the new visual identity.
- Why This Matters Now: theme selection is both a settings task and a visible proof point of the redesign’s confidence.
- Visual Gap Statement: the current theme gallery is useful but still carries local styling and utility-tool feel.
- User Experience Impact: a better theme gallery increases delight, trust, and the perceived sophistication of the product’s visual system.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Prior Plan References: `v22` theme gallery work, `v26` Phase 10 and Phase 15
- Scope: gallery shell, preview cards, theme metadata, category browsing, selection states, theme-management actions
- Out of Scope: theme import/export backend behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/IconGalleryDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`
- Related Screens / Components / Surfaces: theme gallery, theme preview cards, icon gallery if retained as a developer/tooling surface
- Current Visual Behavior / Appearance: gallery presentation is functional but not yet a true showcase-grade product surface.
- Intended Visual Behavior / Appearance: theme browsing feels aspirational, elegant, and strongly aligned with the redesigned visual identity.
- Icon / Visual Design Direction: curated showcase UI with richer previews, better card composition, and stronger premium framing.
- Technical Styling Approach: redesign gallery cards and browsing shells using the new visual system and control language; ensure icon-related tooling surfaces do not lag behind.
- Implementation Steps:
  1. Redesign gallery frame and card system.
  2. Rebuild preview composition, metadata, and action layout.
  3. Align gallery selection and browsing states with the redesigned control/icon system.
  4. Validate gallery behavior across theme families.
- Validation Steps:
  1. Browse several themes in light and dark contexts.
  2. Compare card hierarchy and selection clarity.
  3. Confirm the gallery feels more like a premium showcase than a utility browser.
- Acceptance Criteria: theme management feels visibly redesigned and helps sell the new visual system rather than lagging behind it.
- Dependencies: Phase 02, Phase 17
- Parallelization Notes: can overlap with settings shell work once common control patterns are established.
- Risks / Failure Modes: over-rich previews can feel noisy or misleading.
- Accessibility / Readability Notes: preview metadata and selection indicators must remain legible in all themes.
- Theme / Styling Notes: the gallery must remain correct while previewing alternate themes.
- Motion / Interaction Notes where relevant: card hover and selection transitions later should remain subtle.
- Cleanup / Consolidation Notes where relevant: retire local card-style fragments that no longer fit the redesigned system.
- Rollback / Safety Notes: preserve theme browsing usability while upgrading visual richness.
- References / Context: theme management is a signature product-quality moment.
- Example scenarios where useful: choosing a theme should feel luxurious and decisive, not like browsing a settings utility.

### Phase ID: V27-P12
### Task ID: V27-P12-T03
### Task Title: Redesign Core Control Families For Settings Dialogs And Embedded Forms
- Priority: P1
- Category: Control Styling
- Objective: rebuild buttons, toggles, selects, dropdowns, inputs, chips, and compact form controls around the new icon and visual systems.
- Why This Matters Now: control families carry the redesign into every settings, dialog, and panel surface.
- Visual Gap Statement: even improved controls can still read as partially inherited rather than newly designed if their geometry and icon behavior remain incremental.
- User Experience Impact: more refined controls improve trust, usability, and the tactile quality of daily interactions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.h`
- Prior Plan References: `v22` settings/control work, `v26` Phase 10
- Scope: button families, icon buttons, toggles, selects, chips, inline forms, form spacing and validation states
- Out of Scope: non-visible input handling logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.h`
- Related Screens / Components / Surfaces: settings forms, dialogs, startup actions, inspectors, filter bars, compact form zones
- Current Visual Behavior / Appearance: controls are partly systematized, but not yet fully reauthored around the new icon and visual identity.
- Intended Visual Behavior / Appearance: controls feel more premium, better balanced, and more semantically coherent across all surfaces.
- Icon / Visual Design Direction: elegant desktop controls with stronger icon/text balance and more tactile precision.
- Technical Styling Approach: redesign control metrics and state models around the new icon usage contract and visual language.
- Implementation Steps:
  1. Define redesigned control families and their icon roles.
  2. Rebuild button, toggle, select, chip, and input patterns.
  3. Update embedded form surfaces across settings and dialogs.
  4. Validate compact and standard-density usage.
- Validation Steps:
  1. Compare control families across settings, dialogs, and panel filters.
  2. Verify icon-only, icon-plus-text, disabled, and focused variants.
  3. Confirm the redesigned controls feel related everywhere.
- Acceptance Criteria: controls across the product feel fully redesigned and no longer expose mixed icon or geometry assumptions.
- Dependencies: Phase 01 through Phase 03, V27-P12-T01
- Parallelization Notes: can proceed alongside dialog redesign after the family direction is approved.
- Risks / Failure Modes: redesigning controls too heavily can reduce familiarity and efficiency.
- Accessibility / Readability Notes: focus, disabled, and validation states must remain highly legible.
- Theme / Styling Notes: controls must be semantically theme-driven and parity-ready.
- Motion / Interaction Notes where relevant: later tactile and motion polish should use these control families as the base.
- Cleanup / Consolidation Notes where relevant: remove control-specific local icon gaps and local sizing hacks.
- Rollback / Safety Notes: preserve control semantics and platform expectations where applicable.
- References / Context: control redesign spreads the new visual identity across the whole product.
- Example scenarios where useful: a toggle in settings and an action button in a dialog should feel like members of the same new design family.
