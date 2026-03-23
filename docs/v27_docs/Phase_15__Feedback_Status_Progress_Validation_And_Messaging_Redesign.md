# Phase 15 - Feedback Status Progress Validation And Messaging Redesign

## Goal

Redesign the product’s feedback language so status, validation, progress, alerts, and messaging feel more elegant, more trustworthy, and more integrated with the new visual identity.

## Definition Of Done

- feedback presentation feels redesigned rather than utility-like
- severity, progress, and non-blocking guidance are visually coherent across surfaces
- status and feedback no longer weaken the product’s premium feel

## Tasks

### Phase ID: V27-P15
### Task ID: V27-P15-T01
### Task Title: Redesign Feedback Severity Language And Inline Validation Systems
- Priority: P0
- Category: Feedback UI
- Objective: rebuild the visual language for success, warning, error, info, and validation states across inline and embedded contexts.
- Why This Matters Now: feedback design touches settings, notebook, editor, dialogs, and panels, so it needs to align with the full redesign.
- Visual Gap Statement: feedback is likely still partly surface-specific and can expose inconsistent iconography, spacing, and severity emphasis.
- User Experience Impact: clearer and more elegant feedback improves trust and reduces interruption cost.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Prior Plan References: `v22` feedback phases, `v26` Phase 13
- Scope: inline validation, compact alerts, severity iconography, message hierarchy, action placement for non-modal feedback
- Out of Scope: validation logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Screens / Components / Surfaces: settings validation, notebook error/status surfaces, inline warnings, compact alerts
- Current Visual Behavior / Appearance: feedback likely works but still appears partly inherited and not fully redesigned as a system.
- Intended Visual Behavior / Appearance: every feedback state feels premium, clear, and unified across the full app.
- Icon / Visual Design Direction: restrained but expressive severity design using canonical icons and stronger hierarchy.
- Technical Styling Approach: redesign feedback tokens and templates first, then apply them across embedded surfaces.
- Implementation Steps:
  1. Redesign severity hierarchy and inline feedback shell patterns.
  2. Replace older severity icon or banner patterns with canonical roles.
  3. Rebuild inline validation placement and message hierarchy.
  4. Validate dense, compact, and rich-content contexts.
- Validation Steps:
  1. Trigger several severity states across settings, notebook, and panel surfaces.
  2. Compare iconography, hierarchy, and tone.
  3. Confirm redesigned feedback remains noticeable without becoming loud.
- Acceptance Criteria: feedback severity and validation presentation feel like one newly designed system across the app.
- Dependencies: Phase 02, Phase 03, Phase 13
- Parallelization Notes: can proceed while status and progress redesign is underway.
- Risks / Failure Modes: too much softness can reduce urgency; too much emphasis can damage product calm.
- Accessibility / Readability Notes: severity must remain distinguishable beyond color alone.
- Theme / Styling Notes: redesigned feedback roles must remain stable across theme families.
- Motion / Interaction Notes where relevant: later feedback transitions should reinforce, not substitute for, clear static states.
- Cleanup / Consolidation Notes where relevant: retire per-surface validation shells that no longer fit the redesigned feedback system.
- Rollback / Safety Notes: preserve message semantics and actions while redesigning visuals.
- References / Context: the redesign should make feedback feel more intentional and less generic.
- Example scenarios where useful: a settings warning and a notebook execution error should feel related in design while remaining appropriately different in severity.

### Phase ID: V27-P15
### Task ID: V27-P15-T02
### Task Title: Redesign Status Bar And Background Activity Presentation
- Priority: P1
- Category: Feedback UI
- Objective: turn persistent status and background activity surfaces into premium low-noise product UI.
- Why This Matters Now: persistent feedback is always visible and can quietly drag down the overall shell if it stays utilitarian.
- Visual Gap Statement: the status bar is customized but still carries older visual DNA, including visible emoji-based AI state prefixes.
- User Experience Impact: stronger persistent feedback improves trust while preserving focus on primary work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Prior Plan References: `v22` status/feedback work, `v26` Phase 13
- Scope: status segments, iconography, background activity, progress, transient status messages, alignment with shell chrome
- Out of Scope: background-task orchestration logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Screens / Components / Surfaces: status bar, progress indicators, activity states, AI/provider segment
- Current Visual Behavior / Appearance: status presentation is customized but still inherits older icon and messaging patterns.
- Intended Visual Behavior / Appearance: status bar feels redesigned, more refined, and fully integrated with the shell and feedback system.
- Icon / Visual Design Direction: discreet premium utility bar with canonical icons, cleaner grouping, and better calm.
- Technical Styling Approach: redesign status segment roles, icon usage, and activity visuals rather than only cleaning up individual labels.
- Implementation Steps:
  1. Replace old icon-prefix and segment styling patterns.
  2. Redesign status grouping and activity presentation.
  3. Align progress and provider/connection states with the new feedback language.
  4. Validate narrow-width and busy-status scenarios.
- Validation Steps:
  1. Trigger background activity and provider-state changes.
  2. Compare redesigned status with shell and notifications.
  3. Ensure the bar remains low-noise while clearly informative.
- Acceptance Criteria: the status bar feels clearly redesigned and no longer exposes legacy icon or utility styling residue.
- Dependencies: Phase 03, Phase 04, V27-P15-T01
- Parallelization Notes: can run with overlay feedback redesign as long as the feedback language is aligned.
- Risks / Failure Modes: overdesigning persistent status can draw too much attention.
- Accessibility / Readability Notes: compact segments must remain readable and keyboard/current-state aware.
- Theme / Styling Notes: status contrast and separation must hold in both themes.
- Motion / Interaction Notes where relevant: activity motion later should be understated and reduced-motion compatible.
- Cleanup / Consolidation Notes where relevant: remove emoji-based status prefixes and old segment styling paths.
- Rollback / Safety Notes: preserve status semantics and message fidelity during redesign.
- References / Context: the status bar is a high-frequency shell surface and should match the rest of the overhaul.
- Example scenarios where useful: AI availability and background indexing progress should feel polished without dominating the workspace.

### Phase ID: V27-P15
### Task ID: V27-P15-T03
### Task Title: Redesign Banners Alerts Progress And Non Blocking Messaging Surfaces
- Priority: P1
- Category: Feedback UI
- Objective: align banner-style and progress messaging surfaces with the redesigned feedback identity.
- Why This Matters Now: non-modal messaging often remains visually inconsistent even after broader feedback redesign.
- Visual Gap Statement: banner and progress-style messaging can vary in tone and structure depending on surface and implementation age.
- User Experience Impact: better non-blocking messaging reduces interruption and increases confidence in the product’s statefulness.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Prior Plan References: `v22` feedback and overlay phases, `v26` Phase 13
- Scope: banners, progress rows, inline recovery messages, compact alert bars, messaging action placement
- Out of Scope: actual recovery workflows
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Screens / Components / Surfaces: panel banners, editor alerts, settings messages, progress bars, compact status surfaces
- Current Visual Behavior / Appearance: these messaging surfaces likely still vary and often inherit utility-style presentation.
- Intended Visual Behavior / Appearance: all non-blocking messages feel elegantly integrated into the redesigned shell and feedback system.
- Icon / Visual Design Direction: calm, premium guidance with canonical icons and stronger visual hierarchy.
- Technical Styling Approach: create a redesigned family of embedded feedback surfaces and progress treatments with shared geometry and icon/state behavior.
- Implementation Steps:
  1. Inventory banner and embedded progress/message variants.
  2. Redesign shell, iconography, and hierarchy for those variants.
  3. Align them with status, dialogs, and notifications.
  4. Validate dense and narrow contexts.
- Validation Steps:
  1. Trigger representative banners, progress rows, and inline messages.
  2. Compare their visual language across surfaces.
  3. Verify clarity without unnecessary interruption.
- Acceptance Criteria: embedded and non-modal messaging surfaces feel clearly redesigned and consistent with the rest of the product.
- Dependencies: V27-P15-T01, V27-P15-T02
- Parallelization Notes: can run after the redesigned feedback language is stable.
- Risks / Failure Modes: too much emphasis can create alert fatigue.
- Accessibility / Readability Notes: actions and message hierarchy must remain clear in compact spaces.
- Theme / Styling Notes: severity and progress states must remain theme-safe and legible.
- Motion / Interaction Notes where relevant: loading and progress motion later should remain restrained.
- Cleanup / Consolidation Notes where relevant: retire per-surface banner patterns that do not belong to the redesigned family.
- Rollback / Safety Notes: preserve important messaging semantics while redesigning appearance.
- References / Context: this is the final alignment step for feedback outside the status bar and toasts.
- Example scenarios where useful: a panel-level warning banner should feel clearly related to a compact settings validation message.
