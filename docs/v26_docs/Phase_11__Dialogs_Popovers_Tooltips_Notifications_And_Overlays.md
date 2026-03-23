# Phase 11 - Dialogs Popovers Tooltips Notifications And Overlays

## Goal

Create a coherent transient-surface language so dialogs, popovers, tooltips, notifications, and overlay panels feel like one premium system.

## Definition Of Done

- transient surfaces share consistent shell, spacing, hierarchy, and action patterns
- modal and non-modal overlays have clear depth and focus treatment
- tooltips and notifications no longer feel like afterthoughts next to the shell

## Tasks

### Phase ID: V26-P11
### Task ID: V26-P11-T01
### Task Title: Upgrade Themed Dialog Shells And Modal Hierarchy
- Priority: P0
- Category: Dialog / Overlay UI
- Objective: Turn dialog shells into polished, premium modal surfaces with strong hierarchy and consistent action placement.
- Why This Matters Now: Dialog quality is one of the clearest visible differences between polished desktop products and utilitarian tools.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` currently appears thin relative to the rest of the custom shell.
- User Experience Impact: Better dialogs improve trust during confirmations, settings, and important workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Prior Plan References: `v22` dialogs and overlays, `v25` visible UI closure
- Scope: modal frame, title hierarchy, body spacing, action row balance, backdrop treatment, warning/error/info variants
- Out of Scope: modal workflow logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Screens / Components / Surfaces: settings dialog, confirmations, import/export dialogs, modal overlays
- Current UI Behavior / Appearance: dialogs are functional but visually lightweight compared with shell chrome expectations.
- Intended UI Behavior / Appearance: dialogs feel premium, readable, and clearly modal without excessive decoration.
- Visual / Interaction Design Direction: elegant desktop modal with crisp hierarchy and controlled depth.
- Technical Styling Approach: define shared transient-surface shell roles and modal action layouts.
- Implementation Steps:
  1. Audit current dialog frame, title, and button arrangement.
  2. Standardize modal padding, title hierarchy, and footer layout.
  3. Add consistent backdrop and severity-state styling.
  4. Validate several dialog types against shell and command palette.
- Validation Steps:
  1. Open settings and confirmation dialogs in both themes.
  2. Compare action button alignment and body spacing.
  3. Verify focus visibility and backdrop quality.
- Acceptance Criteria: dialogs feel like first-class premium surfaces and share a clear modal grammar.
- Dependencies: V26-P01-T01, V26-P01-T02, V26-P02-T02
- Parallelization Notes: can begin after transient-surface layer rules are defined.
- Risks / Failure Modes: over-framing dialogs can feel heavy or old-fashioned.
- Accessibility / Readability Notes: ensure title-body-action hierarchy is clear and focus trapping remains visible.
- Theme / Styling Notes: dialog shells must feel cohesive in both themes and remain distinct from background layers.
- Motion / Interaction Notes where relevant: dialog entry/exit motion should be subtle and fast.
- Cleanup / Consolidation Notes where relevant: consolidate modal shell code instead of per-dialog special styling.
- Rollback / Safety Notes: preserve modal semantics and action ordering while refining visuals.
- References / Context: dialogs must now match the quality of the app shell, not lag behind it.
- Example scenarios where useful: a destructive confirmation dialog should feel serious and premium without becoming visually loud.

### Phase ID: V26-P11
### Task ID: V26-P11-T02
### Task Title: Unify Popovers, Tooltips, And Lightweight Context Overlays
- Priority: P1
- Category: Dialog / Overlay UI
- Objective: Create a consistent language for anchored overlays and compact transient help surfaces.
- Why This Matters Now: Smaller overlays are used constantly and quickly reveal inconsistencies in spacing and depth.
- UI Gap Statement: Without explicit unification, tooltips and popovers often drift into one-off component styling.
- User Experience Impact: Consistent lightweight overlays improve discoverability and make interaction feel more refined.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: `v22` popovers/tooltips, `v25` shell UI closure
- Scope: tooltip shell, popover spacing, anchor treatment, compact menus, inline help surfaces
- Out of Scope: content strategy for help text
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: tooltips, anchored menus, inline help popovers, compact overlays
- Current UI Behavior / Appearance: some overlays likely inherit different shells and spacing depending on implementation path.
- Intended UI Behavior / Appearance: all lightweight overlays share the same depth, padding, and hierarchy conventions.
- Visual / Interaction Design Direction: precise floating surfaces with crisp alignment and subtle emphasis.
- Technical Styling Approach: reuse transient-surface shell tiers with compact variants for tooltips and popovers.
- Implementation Steps:
  1. Inventory popover and tooltip variants.
  2. Define compact overlay metrics and anchor rules.
  3. Normalize hover/focus/selection states inside those surfaces.
  4. Validate overlap with shell chrome and panels.
- Validation Steps:
  1. Trigger tooltips and popovers from several surfaces.
  2. Compare padding and anchor alignment.
  3. Review theme contrast and background separation.
- Acceptance Criteria: popovers and tooltips look like members of the same transient-surface family.
- Dependencies: V26-P11-T01
- Parallelization Notes: can run once modal shell rules establish the overlay family.
- Risks / Failure Modes: too much visual treatment can make small overlays distracting.
- Accessibility / Readability Notes: tooltip text must remain readable and not overly condensed.
- Theme / Styling Notes: floating-surface contrast must stay safe against varying backgrounds.
- Motion / Interaction Notes where relevant: tooltip and popover transitions should be near-instant and subtle.
- Cleanup / Consolidation Notes where relevant: remove component-specific overlay shells where a shared one works.
- Rollback / Safety Notes: preserve existing trigger timing and accessibility semantics.
- References / Context: this task reduces “miscellaneous UI” feel across the app.
- Example scenarios where useful: a toolbar tooltip and a settings help popover should feel related.

### Phase ID: V26-P11
### Task ID: V26-P11-T03
### Task Title: Refine Notifications, Toasts, And Overlay Feedback Presentation
- Priority: P1
- Category: Dialog / Overlay UI
- Objective: Bring notification and toast surfaces up to the same polish level as dialogs and command surfaces.
- Why This Matters Now: Feedback overlays are highly visible during normal work and strongly shape perceived refinement.
- UI Gap Statement: Notification surfaces often remain less systematized than shell or dialog surfaces unless explicitly unified.
- User Experience Impact: Better toasts and notification overlays improve trust and reduce visual interruption.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Prior Plan References: `v22` feedback UI, `v25` diagnostics and release-path trustworthiness
- Scope: toast shell, severity hierarchy, action buttons, stack spacing, progress feedback overlays
- Out of Scope: notification routing logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Screens / Components / Surfaces: toast notifications, banner-like overlays, inline overlay feedback
- Current UI Behavior / Appearance: notifications likely work but may not yet share the same premium transient-surface language.
- Intended UI Behavior / Appearance: notifications feel elegant, informative, and visually consistent with dialogs, menus, and status UI.
- Visual / Interaction Design Direction: restrained, trustworthy overlays with clear severity and low interruption cost.
- Technical Styling Approach: apply shared overlay shell rules plus a small set of severity-aware feedback tokens.
- Implementation Steps:
  1. Audit notification shells and severity variants.
  2. Standardize spacing, typography, and actions.
  3. Refine stack layout and screen placement behavior.
  4. Validate interaction with dialogs and bottom status areas.
- Validation Steps:
  1. Trigger success, warning, error, and info notifications.
  2. Compare with dialogs and inline feedback components.
  3. Verify readability at small widths and on dense screens.
- Acceptance Criteria: notification overlays feel polished, theme-correct, and part of the same transient-surface family.
- Dependencies: V26-P11-T01, V26-P13-T01
- Parallelization Notes: can run in parallel with broader feedback UI work once shared overlay rules are set.
- Risks / Failure Modes: too much chrome can make notifications intrusive.
- Accessibility / Readability Notes: severity should be distinguishable beyond color alone.
- Theme / Styling Notes: preserve contrast and layer separation in both themes.
- Motion / Interaction Notes where relevant: toast motion should be quick, subtle, and respectful of reduced-motion settings.
- Cleanup / Consolidation Notes where relevant: consolidate notification shells and severity styling.
- Rollback / Safety Notes: preserve notification timing and dismiss behavior while changing visuals.
- References / Context: this phase gives transient feedback the same level of polish as persistent UI.
- Example scenarios where useful: a save-success toast should feel polished and non-disruptive, not generic.
