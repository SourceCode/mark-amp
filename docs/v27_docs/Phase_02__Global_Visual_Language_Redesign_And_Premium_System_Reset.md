# Phase 02 - Global Visual Language Redesign And Premium System Reset

## Goal

Replace the current convergence-first visual language with a redesigned premium system covering spacing, typography, density, depth, borders, radii, and interaction states.

## Definition Of Done

- one redesigned visual system is explicitly defined and adopted as the product baseline
- old local style assumptions are superseded by stronger system rules
- later surface redesign phases no longer invent their own local visual language

## Tasks

### Phase ID: V27-P02
### Task ID: V27-P02-T01
### Task Title: Redesign The Global Spacing, Density, Radius, Border, And Depth Model
- Priority: P0
- Category: Visual Language
- Objective: Replace the current mixed geometry and density model with a more deliberate premium layout and surface system.
- Why This Matters Now: The app cannot feel fully redesigned if it still uses an inherited convergence-era geometry model.
- Visual Gap Statement: Shared layout primitives exist, but visible surfaces still reveal accumulated local spacing, border, and density decisions.
- User Experience Impact: A stronger geometry model makes the whole app feel calmer, more intentional, and more premium.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`, `/Users/ryanrentfro/code/markamp/src/ui/SpacingGrid.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v22` Phase 01, `v26` Phase 01
- Scope: spacing scale, density tiers, corner families, divider thickness, surface depth tiers, panel and control padding baselines
- Out of Scope: behavioral layout changes unrelated to visible redesign
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/UILayoutTokens.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutMetrics.h`, `/Users/ryanrentfro/code/markamp/src/ui/SpacingGrid.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Screens / Components / Surfaces: shell, toolbars, tabs, panels, dialogs, settings, notebook, canvas
- Current Visual Behavior / Appearance: spacing and depth are improved versus earlier passes, but still feel partially inherited and not fully reauthored.
- Intended Visual Behavior / Appearance: the full application uses a redesigned geometry and surface-depth language that clearly feels new and premium.
- Icon / Visual Design Direction: premium desktop design with stronger rhythm, cleaner grouping, and more intentional surface hierarchy.
- Technical Styling Approach: redefine shared layout tokens and component metrics first, then use them as the baseline for all later phases.
- Implementation Steps:
  1. Audit current geometry and density usage across core surfaces.
  2. Define the redesigned spacing, radius, border, divider, and depth model.
  3. Update shared layout/token files to encode the new model.
  4. Publish adoption rules for later surface phases.
- Validation Steps:
  1. Compare old and new shell screenshots.
  2. Validate density in compact and standard modes if supported.
  3. Confirm panel, dialog, and toolbar geometry all align to the same system.
- Acceptance Criteria: the redesigned geometry model is explicit, reusable, and strong enough to drive the rest of the overhaul.
- Dependencies: none
- Parallelization Notes: foundational and should begin immediately.
- Risks / Failure Modes: redesigning geometry without later adoption will create another partial system.
- Accessibility / Readability Notes: density changes must preserve readability and target clarity.
- Theme / Styling Notes: depth and border language must work across themes without bespoke overrides.
- Motion / Interaction Notes where relevant: movement later must respect the new geometry and depth rules.
- Cleanup / Consolidation Notes where relevant: identify legacy geometry systems to retire in Phase 20.
- Rollback / Safety Notes: keep token changes explicit and documented so later tuning is controlled.
- References / Context: this is a redesign task, not a minor metrics cleanup.
- Example scenarios where useful: the shell, a dialog, and a settings page should all clearly inherit the same new geometry language.

### Phase ID: V27-P02
### Task ID: V27-P02-T02
### Task Title: Redesign Typography, Hierarchy, And Information Rhythm Across The Product
- Priority: P0
- Category: Typography
- Objective: Replace the current practical typography system with a more premium, better-structured hierarchy for shell, dense surfaces, settings, overlays, and onboarding.
- Why This Matters Now: Typography is one of the clearest signals of a full redesign versus an incremental polish pass.
- Visual Gap Statement: The repo has typography infrastructure, but many surfaces still use local font emphasis and inherited hierarchy patterns.
- User Experience Impact: Better typography improves readability, orientation, and perceived sophistication across the entire app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TypographyScale.h`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Prior Plan References: `v22` typography work, `v26` Phase 01
- Scope: title hierarchy, section hierarchy, metadata hierarchy, control labels, supporting text, dense-row text roles
- Out of Scope: code editor content fonts and syntax colors, handled in later phases
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/TypographyScale.h`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StartupPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Related Screens / Components / Surfaces: shell headers, settings, command palette, dialogs, panels, onboarding, feedback
- Current Visual Behavior / Appearance: hierarchy is improved but still partially local and not yet clearly reauthored as one premium typography system.
- Intended Visual Behavior / Appearance: every screen uses a redesigned typography system with stronger hierarchy discipline and more elegant rhythm.
- Icon / Visual Design Direction: restrained premium desktop typography with stronger role separation and calmer metadata styling.
- Technical Styling Approach: expand shared typography roles and migrate surfaces toward role-based usage instead of local font construction.
- Implementation Steps:
  1. Define redesigned typography roles for every major UI text class.
  2. Map the old hierarchy to the new role system.
  3. Refactor high-traffic surfaces first.
  4. Validate hierarchy across compact and spacious surfaces.
- Validation Steps:
  1. Compare settings, shell, dialogs, and onboarding surfaces side by side.
  2. Test long labels and localized strings where practical.
  3. Confirm dense panels remain readable.
- Acceptance Criteria: typography across the product clearly reads as a redesigned system rather than a tuned accumulation.
- Dependencies: V27-P02-T01
- Parallelization Notes: can proceed while geometry redesign is being finalized.
- Risks / Failure Modes: over-stylized hierarchy can reduce speed in dense information views.
- Accessibility / Readability Notes: do not over-reduce metadata or support text.
- Theme / Styling Notes: text roles should be independent from theme-specific color choices.
- Motion / Interaction Notes where relevant: none
- Cleanup / Consolidation Notes where relevant: remove duplicated local font-building logic from high-traffic surfaces.
- Rollback / Safety Notes: preserve semantic hierarchy even if size/weight tuning changes later.
- References / Context: this is essential to making the app feel newly designed.
- Example scenarios where useful: settings rows, command palette rows, and panel headers should show consistent primary, secondary, and metadata hierarchy.

### Phase ID: V27-P02
### Task ID: V27-P02-T03
### Task Title: Redesign Color, Surface, And Interaction-State Language For A Stronger Premium Identity
- Priority: P0
- Category: Color / Theme
- Objective: Replace the current mixed surface and state language with a more distinctive, consistent, and premium system.
- Why This Matters Now: Surface and state treatment defines whether the product feels redesigned or merely tidied up.
- Visual Gap Statement: Theme tokens exist, but local colors, legacy shells, and partially migrated surfaces still weaken identity and consistency.
- User Experience Impact: A stronger surface and state language improves clarity, tactility, and overall perceived quality.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: `v22` theme consistency, `v26` Phase 01 and Phase 15
- Scope: surface tiers, border and divider language, hover/focus/pressed/selected/disabled states, warning/error/info/success visual roles
- Out of Scope: syntax token mapping specifics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ThemeTokens.h`, `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: shell, controls, tabs, dialogs, menus, feedback, settings, panels
- Current Visual Behavior / Appearance: surface and state treatment is partly tokenized but still carries inherited conventions and local overrides.
- Intended Visual Behavior / Appearance: every surface and interactive state participates in one redesigned, premium visual identity.
- Icon / Visual Design Direction: calm but distinctive desktop visual language with stronger material hierarchy and clearer states.
- Technical Styling Approach: redesign semantic tokens and per-state rules before broad surface migration rather than patching surfaces individually.
- Implementation Steps:
  1. Audit current surface and state roles.
  2. Redesign semantic surface and interaction-state tokens.
  3. Update helper paint logic and adoption guidance.
  4. Validate on representative shell, panel, overlay, and feedback surfaces.
- Validation Steps:
  1. Review normal, hover, focus, pressed, selected, disabled, warning, and error states side by side.
  2. Compare shell and overlay depth language.
  3. Confirm redesigned states remain calm and legible in both themes.
- Acceptance Criteria: the product has one redesigned surface and state language strong enough to guide all later surface work.
- Dependencies: V27-P02-T01
- Parallelization Notes: should complete before action-surface and control redesign begins.
- Risks / Failure Modes: over-emphasis can make the UI loud; under-emphasis can make the redesign invisible.
- Accessibility / Readability Notes: redesigned states must remain distinguishable without relying only on subtle color shifts.
- Theme / Styling Notes: design the system semantically so parity work later is manageable.
- Motion / Interaction Notes where relevant: state transitions later should reinforce this static state hierarchy.
- Cleanup / Consolidation Notes where relevant: mark local state-color overrides for retirement.
- Rollback / Safety Notes: keep state roles semantic and reversible to avoid brittle per-surface fixes.
- References / Context: this is the color/state counterpart to the icon-system foundation.
- Example scenarios where useful: tab selection, toolbar hover, settings focus, and toast severity should all feel clearly related.
