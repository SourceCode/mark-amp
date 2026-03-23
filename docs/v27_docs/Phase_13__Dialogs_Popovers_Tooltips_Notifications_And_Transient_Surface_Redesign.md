# Phase 13 - Dialogs Popovers Tooltips Notifications And Transient Surface Redesign

## Goal

Redesign all transient and modal surfaces into one elegant, premium family that matches the rest of the visual overhaul.

## Definition Of Done

- dialogs, popovers, tooltips, and notifications feel visibly redesigned
- transient surfaces share one coherent shell, icon, spacing, and action language
- no transient surface looks older or thinner than the redesigned shell

## Tasks

### Phase ID: V27-P13
### Task ID: V27-P13-T01
### Task Title: Redesign The Modal Shell And Dialog Family
- Priority: P0
- Category: Dialog / Overlay UI
- Objective: reauthor modal shells, headers, bodies, footers, and severity variants so dialogs feel premium and authoritative.
- Why This Matters Now: dialogs are a key visible benchmark against high-end desktop applications.
- Visual Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp` still reads as less mature than the custom shell and toolbar surfaces.
- User Experience Impact: better dialogs improve trust and make important workflows feel more polished.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Prior Plan References: `v22` overlay phase, `v26` Phase 11
- Scope: modal shell, header hierarchy, action row balance, severity styling, backdrop and surface depth, icon usage
- Out of Scope: modal workflow logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp`
- Related Screens / Components / Surfaces: settings dialog, confirmations, warnings, import/export dialogs, modal workflows
- Current Visual Behavior / Appearance: dialogs function but still look less authored and less premium than the best shell surfaces.
- Intended Visual Behavior / Appearance: dialogs feel newly designed, premium, and visually authoritative without becoming heavy.
- Icon / Visual Design Direction: elegant modal family with better hierarchy, canonical icons, and refined severity treatment.
- Technical Styling Approach: rebuild transient-shell roles and action layouts around the redesigned visual and icon systems.
- Implementation Steps:
  1. Redesign modal frame, title/body/footer structure, and backdrop behavior.
  2. Rebuild severity-state iconography and action balance.
  3. Align settings and general dialogs under the same family.
  4. Validate compact, wide, destructive, and informational variants.
- Validation Steps:
  1. Open several dialog classes in both themes.
  2. Compare hierarchy, spacing, and action placement.
  3. Confirm dialogs feel aligned with the redesigned shell rather than like an older subsystem.
- Acceptance Criteria: the dialog family feels visibly redesigned and premium across common modal patterns.
- Dependencies: Phase 02, Phase 05, Phase 12
- Parallelization Notes: can run with notification and tooltip redesign.
- Risks / Failure Modes: overly dramatic dialogs can conflict with desktop productivity tone.
- Accessibility / Readability Notes: focus visibility and action ordering must remain extremely clear.
- Theme / Styling Notes: modal shells must have strong parity and controlled depth in all themes.
- Motion / Interaction Notes where relevant: later dialog transitions should remain restrained and respectful.
- Cleanup / Consolidation Notes where relevant: retire per-dialog shell exceptions where the new family is sufficient.
- Rollback / Safety Notes: preserve modal semantics and accessibility behavior while redesigning visuals.
- References / Context: dialogs must stop feeling like a visual tier below the shell.
- Example scenarios where useful: a destructive confirmation should feel serious and premium, not generic or theatrical.

### Phase ID: V27-P13
### Task ID: V27-P13-T02
### Task Title: Redesign Popovers Tooltips And Lightweight Anchored Surfaces
- Priority: P1
- Category: Dialog / Overlay UI
- Objective: bring anchored overlays and lightweight help surfaces to the same visual standard as redesigned dialogs and menus.
- Why This Matters Now: small transient surfaces are used constantly and can easily expose inconsistent icon, spacing, or depth rules.
- Visual Gap Statement: tooltips and popovers often inherit one-off visual treatment unless explicitly redesigned as a family.
- User Experience Impact: better anchored surfaces make the whole app feel cleaner, more precise, and more premium.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Prior Plan References: `v22` overlay work, `v26` Phase 11
- Scope: tooltip shell, popover geometry, anchor treatment, compact row hierarchy, support icons, help surfaces
- Out of Scope: tooltip content policy
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ThemedDialog.cpp`
- Related Screens / Components / Surfaces: tooltips, anchored popovers, inline help, compact menus, contextual inspectors
- Current Visual Behavior / Appearance: these surfaces likely vary by implementation path and have not yet been fully reauthored as one family.
- Intended Visual Behavior / Appearance: all lightweight anchored surfaces feel clearly redesigned and related to the modal/command families.
- Icon / Visual Design Direction: refined floating micro-surfaces with strong alignment, calm depth, and canonical icon usage.
- Technical Styling Approach: create a compact transient-surface family derived from the redesigned dialog and menu systems.
- Implementation Steps:
  1. Redesign compact overlay geometry and support icon usage.
  2. Standardize anchor spacing, border, and depth rules.
  3. Align tooltips, popovers, and lightweight contextual surfaces.
  4. Validate against shell, settings, and canvas uses.
- Validation Steps:
  1. Trigger tooltips and popovers across several surfaces.
  2. Compare shell, spacing, and anchor treatment.
  3. Verify compact surfaces feel premium without becoming visually dominant.
- Acceptance Criteria: anchored transient surfaces feel like a coherent redesigned family.
- Dependencies: V27-P13-T01
- Parallelization Notes: can proceed after modal family direction is stable.
- Risks / Failure Modes: too much styling can make tiny surfaces distract from primary work.
- Accessibility / Readability Notes: tooltip text and compact focus handling must remain readable and predictable.
- Theme / Styling Notes: compact overlays must remain legible on varied surface backgrounds.
- Motion / Interaction Notes where relevant: any transitions must remain extremely subtle and quick.
- Cleanup / Consolidation Notes where relevant: remove one-off tooltip and popover shells where the new family applies.
- Rollback / Safety Notes: preserve platform-friendly overlay behavior and accessibility semantics.
- References / Context: tooltip/popover quality is a hidden but important premium differentiator.
- Example scenarios where useful: a toolbar tooltip and a settings help popover should feel like members of the same new transient family.

### Phase ID: V27-P13
### Task ID: V27-P13-T03
### Task Title: Redesign Notifications Toasts And Non Modal Feedback Overlays
- Priority: P1
- Category: Feedback UI
- Objective: rebuild notifications and overlay feedback so they feel elegant, informative, and deeply integrated with the new visual system.
- Why This Matters Now: users see transient feedback constantly, and low-quality toasts can undercut the rest of the redesign.
- Visual Gap Statement: notification and toast surfaces tend to lag behind shell and dialog redesign unless explicitly addressed.
- User Experience Impact: better overlays reduce interruption cost and improve trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Prior Plan References: `v22` overlay and feedback phases, `v26` Phase 11 and Phase 13
- Scope: toast shell, severity iconography, action buttons, stack spacing, placement, non-modal overlay rhythm
- Out of Scope: notification routing behavior
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Screens / Components / Surfaces: toast notifications, inline overlay feedback, banners that behave like floating surfaces
- Current Visual Behavior / Appearance: notifications likely work but do not yet look like first-class redesigned product surfaces.
- Intended Visual Behavior / Appearance: notifications feel more elegant, less generic, and fully integrated with the redesigned feedback language.
- Icon / Visual Design Direction: subtle, trustworthy feedback overlays with canonical severity icons and refined action balance.
- Technical Styling Approach: build overlay feedback from the redesigned transient and feedback systems, not standalone utility styles.
- Implementation Steps:
  1. Redesign toast and overlay feedback shells.
  2. Replace any old severity or icon treatments with canonical mappings.
  3. Rebuild action and stack layout.
  4. Validate multi-toast and edge-placement behavior.
- Validation Steps:
  1. Trigger success, warning, error, and info notifications.
  2. Compare to dialogs and status feedback.
  3. Confirm the redesigned surfaces feel premium without stealing focus.
- Acceptance Criteria: transient feedback overlays feel newly designed and clearly belong to the same visual family as dialogs and menus.
- Dependencies: V27-P13-T01, Phase 15
- Parallelization Notes: can proceed once transient shell language is defined.
- Risks / Failure Modes: overly rich notification design can become distracting.
- Accessibility / Readability Notes: severity must be clear beyond color and remain readable in brief exposures.
- Theme / Styling Notes: overlay contrast must be safe in both themes.
- Motion / Interaction Notes where relevant: later toast motion should be minimal and reduced-motion friendly.
- Cleanup / Consolidation Notes where relevant: retire older notification shell variants.
- Rollback / Safety Notes: preserve timing and dismiss semantics while redesigning visual treatment.
- References / Context: transient feedback should feel like part of the new product identity, not generic OS-adjacent chrome.
- Example scenarios where useful: a save-success toast should feel polished and non-disruptive, while an error toast should feel serious but controlled.
