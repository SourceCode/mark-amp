# Phase 14: Dialogs Popovers Tooltips Notifications And Transient Overlays

## Outcome

Unify every transient surface into one premium overlay language with stronger shells, better spacing, more elegant motion, and clearer state communication.

## Improvement Count

360 atomic improvements across 6 execution tasks.

### P14-T01

- Phase ID: P14
- Task ID: P14-T01
- Task Title: Redesign modal dialog shells, backdrops, headers, and footer action balance
- Priority: P0
- Category: Dialog / Overlay UI
- Atomic Improvements Covered: 60
- Objective: Make modals feel premium, structurally calm, and consistent across confirms, inputs, progress, and wizard flows.
- Why This Matters Now: Dialogs are high-stakes surfaces where weak visual hierarchy is especially noticeable.
- UI Problem Statement: [ThemedDialog.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp) provides a basic shell, but modal hierarchy, spacing rhythm, and footer treatment remain minimal.
- User Experience Impact: Modals feel more like functional containers than polished decision surfaces.
- Scope: Backdrop, modal shell, header spacing, body rhythm, footer action grouping, danger/info/warning variants, compact modal states.
- Out of Scope: Dialog logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.h`; `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DialogModel.cpp`
- Related Screens / Components / Surfaces: Confirm, input, progress, wizard, settings, import/export, theme dialogs.
- Current UI Behavior / Appearance: Dialog infrastructure exists, but shells and footer hierarchies are not yet flagship quality.
- Intended UI Behavior / Appearance: Dialogs feel calm, deliberate, and premium under all decision states.
- Visual / Interaction Design Direction: Refined overlays with strong information hierarchy and polished action balance.
- Technical Styling Approach: Define modal shell, backdrop, and footer variants that all dialog families consume.
- Implementation Steps: Audit dialog families; redesign shell and backdrop; refine header/body/footer spacing; standardize primary/secondary/destructive action layouts; tune warning and info variants.
- Validation Steps: Review confirm, input, progress, and wizard dialogs across dark and light themes.
- Acceptance Criteria: Dialogs become visibly more premium and structurally consistent.
- Dependencies: Phase 01, Phase 03.
- Risks / Failure Modes: Overly spacious dialogs can feel wasteful in compact workflows.
- Accessibility / Readability Notes: Focus order, title/body/action hierarchy, and contrast must be strong.
- Theme / Styling Notes: Backdrops and shells need enough separation without feeling heavy or muddy.
- Motion / Interaction Notes where relevant: Modal open/close transitions should be elegant and reduced-motion aware.
- Observability / Diagnostics Notes where relevant: Add modal family screenshot boards and state matrices.
- Rollback / Safety Notes: Keep existing dialog behavior stable while shell and footer visuals evolve.
- References / Context: Dialog polish is one of the fastest ways to raise perceived product quality.
- Example scenarios where useful: A destructive confirmation dialog feels serious and refined without becoming visually alarming.

### P14-T02

- Phase ID: P14
- Task ID: P14-T02
- Task Title: Upgrade popovers, dropdowns, anchored panels, and rich tooltip shells
- Priority: P1
- Category: Dialog / Overlay UI
- Atomic Improvements Covered: 60
- Objective: Make anchored transient surfaces feel consistent, elegant, and spatially stable.
- Why This Matters Now: Popovers and dropdowns are among the easiest places for visual inconsistency to accumulate.
- UI Problem Statement: The repo includes [ThemedDropdown.cpp](/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.cpp), [BreadcrumbDropdown.cpp](/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbDropdown.cpp), [TooltipWindow.cpp](/Users/ryanrentfro/code/markamp/src/ui/TooltipWindow.cpp), and [RichTooltip.cpp](/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp), but they do not yet read as one transient family.
- User Experience Impact: Small transient surfaces can feel more piecemeal than polished.
- Scope: Shell frames, corner radii, placement offsets, shadows, arrow affordances if used, item spacing, helper-text hierarchy, rich tooltip content structure.
- Out of Scope: Dropdown data or selection semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbDropdown.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TooltipWindow.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp`
- Related Screens / Components / Surfaces: Dropdowns, breadcrumbs, rich help popovers, inline anchored surfaces.
- Current UI Behavior / Appearance: These surfaces function, but not yet under one highly refined visual grammar.
- Intended UI Behavior / Appearance: Anchored transient surfaces feel spatially related and visually premium.
- Visual / Interaction Design Direction: Clean, floating information surfaces with precise edges and restrained depth.
- Technical Styling Approach: Define anchored-overlay variants and placement rules that unify popovers and rich tooltips.
- Implementation Steps: Audit anchored surfaces; align shell geometry and placement; refine content spacing and iconography; normalize hover and selected states for dropdown items.
- Validation Steps: Review anchored surfaces from toolbar, breadcrumbs, settings, and inline editor contexts.
- Acceptance Criteria: Small anchored surfaces no longer feel like separate widget families.
- Dependencies: Phase 01, Phase 04.
- Risks / Failure Modes: Too much depth or animation can make small overlays feel heavy.
- Accessibility / Readability Notes: Placement and focus return must remain clear for keyboard users.
- Theme / Styling Notes: Overlay contrast and shadows need careful tuning in dark themes.
- Motion / Interaction Notes where relevant: Popover and tooltip entry motion should be subtle and consistent.
- Observability / Diagnostics Notes where relevant: Add anchored-overlay screenshots with placement variants.
- Rollback / Safety Notes: Preserve current open/close logic while visual shells are normalized.
- References / Context: Anchored surfaces are small but highly visible indicators of polish maturity.
- Example scenarios where useful: Opening a breadcrumb dropdown or a themed dropdown feels like the same transient design system.

### P14-T03

- Phase ID: P14
- Task ID: P14-T03
- Task Title: Redesign notifications, banners, alerts, and progress feedback for stronger trust and visual calm
- Priority: P0
- Category: Feedback UI
- Atomic Improvements Covered: 60
- Objective: Make feedback surfaces informative, beautiful, and non-disruptive.
- Why This Matters Now: Notifications and alerts are often where products either feel polished or noisy.
- UI Problem Statement: [NotificationManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp), [NotificationModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp), and progress-related surfaces need stronger visual hierarchy and family coherence.
- User Experience Impact: Success, warning, and error feedback can feel abrupt or too mechanically styled.
- Scope: Toast shells, inline alerts, status banners, progress bars, spinner variants, action buttons, dismiss controls, stacked notification rhythm.
- Out of Scope: Notification semantics or routing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/NotificationModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedProgressBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/animation/ProgressBar.cpp`
- Related Screens / Components / Surfaces: Toasts, alerts, banners, progress indicators, activity feedback.
- Current UI Behavior / Appearance: Notifications and progress exist, but their shells and state language are not yet fully premium.
- Intended UI Behavior / Appearance: Feedback surfaces become visually calm, trustworthy, and clearly prioritized.
- Visual / Interaction Design Direction: Minimal, elegant feedback with strong hierarchy and low interruption.
- Technical Styling Approach: Define feedback surface variants by urgency and persistence, then unify toast, banner, and progress presentation.
- Implementation Steps: Audit feedback families; redesign shell and message hierarchy; align icons, buttons, and progress tracks; improve stack spacing and edge placement; tune severity cues.
- Validation Steps: Trigger info, success, warning, error, and progress states across themes and shell contexts.
- Acceptance Criteria: Feedback surfaces feel like a polished product system rather than a set of local utilities.
- Dependencies: Phase 01, Phase 17, Phase 19.
- Risks / Failure Modes: Over-softening error feedback can weaken urgency.
- Accessibility / Readability Notes: Message contrast, action affordances, and severity distinctions must remain strong.
- Theme / Styling Notes: Feedback colors need distinct but disciplined calibration across themes.
- Motion / Interaction Notes where relevant: Toast and progress motion should be smooth and reduced-motion aware.
- Observability / Diagnostics Notes where relevant: Add feedback family screenshot boards and stacked-toast states.
- Rollback / Safety Notes: Preserve current notification queueing and dismiss behavior while redesigning shells.
- References / Context: High-quality feedback is part of premium fit and finish, not secondary polish.
- Example scenarios where useful: A success toast and a long-running progress surface both feel elegant and consistent instead of unrelated.

### P14-T04

- Phase ID: P14
- Task ID: P14-T04
- Task Title: Unify spotlight, shortcut, quick-pick, and transient teaching overlays under one visual family
- Priority: P2
- Category: Dialog / Overlay UI
- Atomic Improvements Covered: 60
- Objective: Make instructional and transient command overlays feel polished and clearly related.
- Why This Matters Now: These overlays are visible quality markers for expert workflows and discoverability.
- UI Problem Statement: Spotlight, shortcut, quick-pick, and related overlays are present but visually fragmented.
- User Experience Impact: Teaching and expert-helper surfaces can feel detached from the premium shell the rest of `v22` is building.
- Scope: Spotlight shells, shortcut overlays, quick-pick list presentation, helper copy hierarchy, overlay iconography, backdrop and dismiss cues.
- Out of Scope: Search or picker logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SpotlightOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlay.cpp`; `/Users/ryanrentfro/code/markamp/src/core/QuickPickService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DialogStackManager.cpp`
- Related Screens / Components / Surfaces: Spotlight, quick pick, shortcut overlays, transient teaching surfaces.
- Current UI Behavior / Appearance: Overlays function, but they do not yet clearly belong to one polished transient family.
- Intended UI Behavior / Appearance: Transient helper surfaces feel cohesive, calm, and expertly tuned.
- Visual / Interaction Design Direction: Lightweight, elegant teaching overlays that respect the user’s focus.
- Technical Styling Approach: Reuse dialog and popover systems while defining compact overlay-specific shell and row variants.
- Implementation Steps: Audit transient helper overlays; align shell, copy, and list treatments; improve backdrop and action hierarchy; normalize overlay density.
- Validation Steps: Open multiple overlay families in succession and compare shell and item coherence.
- Acceptance Criteria: Teaching and helper overlays no longer feel like separate historical experiments.
- Dependencies: Phase 04, Phase 08, Phase 19.
- Risks / Failure Modes: Heavy overlay styling can feel intrusive.
- Accessibility / Readability Notes: Keyboard focus and current selection cues must remain very clear in helper overlays.
- Theme / Styling Notes: Overlay backgrounds and emphasis colors need consistent contrast behavior across themes.
- Motion / Interaction Notes where relevant: Entry/exit motion should be subtle and never block cognitive flow.
- Observability / Diagnostics Notes where relevant: Add helper-overlay screenshots and compact-width variants.
- Rollback / Safety Notes: Maintain current overlay invocation behavior while visual unification is applied.
- References / Context: Discoverability surfaces should support, not weaken, the premium UI direction.
- Example scenarios where useful: A quick-pick overlay and a shortcut overlay look like siblings rather than unrelated tools.

### P14-T05

- Phase ID: P14
- Task ID: P14-T05
- Task Title: Refine progress, loading, and blocking-operation visual treatment across dialogs and panels
- Priority: P1
- Category: Feedback UI
- Atomic Improvements Covered: 60
- Objective: Make waiting states feel calm, informative, and premium instead of generic or anxiety-inducing.
- Why This Matters Now: Loading and progress are visible in many contexts and currently vary in treatment.
- UI Problem Statement: Progress bars, spinners, and blocking dialogs are present in multiple component families with uneven polish.
- User Experience Impact: Waiting states can feel rough, inconsistent, or too visually busy.
- Scope: Progress dialogs, inline progress strips, indeterminate spinners, blocking overlays, panel loading states, batch operation progress.
- Out of Scope: Operation scheduling or progress semantics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedProgressBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/animation/ProgressBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`
- Related Screens / Components / Surfaces: Dialogs, panels, notifications, long-running actions.
- Current UI Behavior / Appearance: Progress and loading cues exist, but not yet as one polished family.
- Intended UI Behavior / Appearance: Waiting states communicate status clearly and calmly across all contexts.
- Visual / Interaction Design Direction: Quiet confidence during waiting, with minimal visual agitation.
- Technical Styling Approach: Create loading/progress variants shared across dialogs, inline surfaces, and panel states.
- Implementation Steps: Audit progress indicators; align spinner and bar styles; improve message hierarchy; normalize indeterminate and determinate treatments; refine blocking overlays.
- Validation Steps: Trigger short and long progress states in modal, inline, and panel contexts.
- Acceptance Criteria: Loading and progress feel consistent and non-jarring throughout the product.
- Dependencies: Phase 01, Phase 16, Phase 19.
- Risks / Failure Modes: Over-stylized progress indicators can draw too much attention during routine operations.
- Accessibility / Readability Notes: Progress state must be legible for low-vision users and understandable without motion alone.
- Theme / Styling Notes: Progress tracks and indicators must retain contrast on all relevant backgrounds.
- Motion / Interaction Notes where relevant: Indeterminate motion should be smooth, subtle, and reduced-motion aware.
- Observability / Diagnostics Notes where relevant: Add progress-state screenshot and motion references.
- Rollback / Safety Notes: Preserve current blocking behavior and operation text while visual treatment is improved.
- References / Context: Waiting states are part of the product’s emotional polish, not just operational plumbing.
- Example scenarios where useful: Importing a theme or large project shows progress that feels calm and professional rather than generic.

### P14-T06

- Phase ID: P14
- Task ID: P14-T06
- Task Title: Add transient-surface family galleries and regression coverage
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 60
- Objective: Protect dialog, popover, tooltip, toast, and overlay polish with explicit cross-family references.
- Why This Matters Now: Transient surfaces are numerous and visually easy to regress.
- UI Problem Statement: The product lacks a unified gallery covering all major transient surface families.
- User Experience Impact: Overlay quality drifts and users experience the product as less coherent over time.
- Scope: Modals, popovers, dropdowns, tooltips, notifications, progress, quick picks, helper overlays.
- Out of Scope: Overlay logic correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`; `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Related Screens / Components / Surfaces: Entire transient-surface family.
- Current UI Behavior / Appearance: Transient surface quality is reviewed locally and inconsistently.
- Intended UI Behavior / Appearance: Every transient surface family has approved visual references and regression protection.
- Visual / Interaction Design Direction: Mature transient-surface governance across the product.
- Technical Styling Approach: Build cross-family screenshot suites with representative states and motion notes.
- Implementation Steps: Define transient fixtures; capture shells and state variants; publish family boards; add regression tests for core transient surfaces.
- Validation Steps: Intentionally perturb toast shell or dialog footer balance and verify the gallery catches the change.
- Acceptance Criteria: Transient-surface polish becomes durable and easier to review.
- Dependencies: P14-T01 through P14-T05.
- Risks / Failure Modes: Transient fixtures may be noisy if timing and placement are not tightly controlled.
- Accessibility / Readability Notes: Include focus, reduced-motion, and high-contrast transient states.
- Theme / Styling Notes: Capture all transient families in dark, light, and high-contrast themes.
- Motion / Interaction Notes where relevant: Store representative timing and motion references for animated surfaces.
- Observability / Diagnostics Notes where relevant: Keep fixture metadata with overlay family and state IDs.
- Rollback / Safety Notes: Start with reference boards before enforcing all transient regressions strictly in CI.
- References / Context: `v22` needs transient surfaces to feel like a coherent premium layer, not an accumulation of helpers.
- Example scenarios where useful: A quick-pick popover and a progress dialog can be compared instantly for shell and action consistency.

