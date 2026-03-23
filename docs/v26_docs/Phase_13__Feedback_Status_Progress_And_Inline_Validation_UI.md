# Phase 13 - Feedback Status Progress And Inline Validation UI

## Goal

Make status, progress, validation, and feedback presentation feel coherent, trustworthy, and elegantly non-disruptive.

## Definition Of Done

- status bar, progress indicators, banners, alerts, and inline validation use one feedback language
- severity and state are clear without visual harshness
- feedback surfaces feel premium and integrated with the rest of the shell

## Tasks

### Phase ID: V26-P13
### Task ID: V26-P13-T01
### Task Title: Standardize Feedback Severity, Messaging Hierarchy, And Inline Validation Styling
- Priority: P0
- Category: Feedback UI
- Objective: Create one visual system for success, warning, error, and info messaging across inline and overlay contexts.
- Why This Matters Now: Feedback is everywhere and currently risks drifting across settings, notebook, dialogs, and notifications.
- UI Gap Statement: Severity handling and validation styling often vary by surface, reducing trust and coherence.
- User Experience Impact: Better feedback reduces confusion and makes the product feel more dependable and refined.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Prior Plan References: `v22` feedback UI work, `v25` diagnostics and release-path closure
- Scope: message hierarchy, severity colors, inline validation rows, warning banners, success confirmations, compact feedback shells
- Out of Scope: validation logic or notification routing
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Related Screens / Components / Surfaces: settings validation, notebook feedback, notifications, inline warnings
- Current UI Behavior / Appearance: feedback likely works functionally but does not yet read as one unified visual system.
- Intended UI Behavior / Appearance: feedback surfaces communicate severity clearly with restrained, premium styling.
- Visual / Interaction Design Direction: calm, trustworthy feedback with strong clarity and minimal disruption.
- Technical Styling Approach: define shared feedback tokens and compact banner/inline-message roles.
- Implementation Steps:
  1. Inventory feedback and validation styles across surfaces.
  2. Define severity hierarchy and message layout rules.
  3. Refactor inline and compact feedback UI to shared styles.
  4. Validate message clarity in dense forms and notebooks.
- Validation Steps:
  1. Trigger success, warning, error, and info states.
  2. Compare inline and overlay variants.
  3. Verify severity remains clear without oversaturation.
- Acceptance Criteria: all major feedback states feel visually related and product-grade.
- Dependencies: V26-P01-T03, V26-P11-T03
- Parallelization Notes: can run while status-bar work begins.
- Risks / Failure Modes: over-subtle severity can reduce clarity; over-strong severity can feel harsh.
- Accessibility / Readability Notes: do not rely solely on color to convey severity.
- Theme / Styling Notes: severity tokens must remain theme-safe and legible.
- Motion / Interaction Notes where relevant: feedback appearance should be quick and restrained.
- Cleanup / Consolidation Notes where relevant: retire duplicate warning and validation shells.
- Rollback / Safety Notes: preserve semantic meaning and existing feedback timing.
- References / Context: this task underpins settings, notebook, and notifications polish.
- Example scenarios where useful: a settings validation warning and a notebook execution error should feel different in severity but related in design.

### Phase ID: V26-P13
### Task ID: V26-P13-T02
### Task Title: Refine Status Bar, Progress Indicators, And Background Activity Presentation
- Priority: P1
- Category: Feedback UI
- Objective: Upgrade persistent and semi-persistent feedback so it feels informative and polished rather than purely utilitarian.
- Why This Matters Now: The status bar and progress indicators are constant touchpoints in a professional desktop application.
- UI Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` is custom and strong, but still part of the broader convergence and premium-finish gap.
- User Experience Impact: Better persistent feedback improves trust while reducing visual distraction.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Prior Plan References: `v22` status and feedback UI, `v25` release-readiness UI closure
- Scope: status bar spacing, item grouping, progress visuals, activity indicators, background task cues
- Out of Scope: task orchestration logic
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Related Screens / Components / Surfaces: status bar, progress spinners, background activity, transient status messages
- Current UI Behavior / Appearance: status bar is customized but may still need premium finishing and clearer integration with other feedback surfaces.
- Intended UI Behavior / Appearance: status and progress UI feel elegant, subtle, and highly readable.
- Visual / Interaction Design Direction: low-profile professional status system with crisp grouping and clear state cues.
- Technical Styling Approach: align status bar and progress visuals with feedback tokens and shell chrome rules.
- Implementation Steps:
  1. Audit status-bar item roles and spacing.
  2. Tune progress and activity visuals for subtlety and clarity.
  3. Align status item hover/active treatment with shell controls where interactive.
  4. Validate mixed states during active tasks.
- Validation Steps:
  1. Trigger background activity and progress states.
  2. Compare persistent status cues with toast and inline feedback.
  3. Verify narrow-width status-bar composition.
- Acceptance Criteria: status and progress presentation feels premium and integrated with the broader feedback language.
- Dependencies: V26-P01-T01, V26-P03-T01, V26-P13-T01
- Parallelization Notes: can run with notification refinement.
- Risks / Failure Modes: too much subtlety can hide important progress or error information.
- Accessibility / Readability Notes: progress and state changes need visible differentiation beyond animation.
- Theme / Styling Notes: status contrast should remain comfortable, especially in dark themes.
- Motion / Interaction Notes where relevant: activity motion should be minimal and respect reduced-motion settings.
- Cleanup / Consolidation Notes where relevant: consolidate status and progress styling helpers.
- Rollback / Safety Notes: keep status content accurate while changing presentation.
- References / Context: the status bar is part of the shell and part of the feedback system.
- Example scenarios where useful: background indexing progress should be visible without pulling attention away from editing.

### Phase ID: V26-P13
### Task ID: V26-P13-T03
### Task Title: Standardize Banners, Inline Alerts, And Non-Blocking Recovery Messaging
- Priority: P1
- Category: Feedback UI
- Objective: Make non-modal alerts and recovery prompts feel elegant, readable, and visually aligned with the rest of the product.
- Why This Matters Now: Non-blocking alerts often carry the most visible UI inconsistency across mature apps.
- UI Gap Statement: Banner-style surfaces typically arise across settings, panels, and editors and can easily diverge in tone and structure.
- User Experience Impact: Consistent alerts improve trust and reduce interruption cost.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v22` inline feedback work, `v25` release-readiness validation UX
- Scope: banner shells, inline alert layout, compact recovery actions, severity alignment, placement rules
- Out of Scope: recovery logic itself
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Related Screens / Components / Surfaces: settings warnings, editor inline alerts, panel banners
- Current UI Behavior / Appearance: non-modal alerts likely vary in spacing, icon use, and severity emphasis.
- Intended UI Behavior / Appearance: banners and inline alerts look calm, coherent, and clearly actionable.
- Visual / Interaction Design Direction: low-interruption alerting with disciplined hierarchy and calm severity cues.
- Technical Styling Approach: extend the shared feedback system with banner and inline-alert templates.
- Implementation Steps:
  1. Inventory banner and inline-alert variants.
  2. Define shared layout and action placement rules.
  3. Refactor visible alert surfaces to those templates.
  4. Validate in dense and narrow layouts.
- Validation Steps:
  1. Trigger representative warning and recovery banners.
  2. Compare settings, editor, and panel variants.
  3. Verify readability and action discoverability.
- Acceptance Criteria: non-modal alerts feel consistent and premium across the app.
- Dependencies: V26-P13-T01
- Parallelization Notes: can run after severity hierarchy is defined.
- Risks / Failure Modes: over-subtle banners can be missed; over-strong banners can feel alarming.
- Accessibility / Readability Notes: include clear icons/text hierarchy and preserve action target clarity.
- Theme / Styling Notes: banner backgrounds should remain theme-safe and not overpower surrounding surfaces.
- Motion / Interaction Notes where relevant: appearance/dismiss transitions should be soft and non-distracting.
- Cleanup / Consolidation Notes where relevant: retire per-surface banner variants where a shared template works.
- Rollback / Safety Notes: preserve message content and action semantics while updating visuals.
- References / Context: this is the final consolidation step for feedback surfaces outside the status bar and notifications.
- Example scenarios where useful: a workspace warning banner should feel coherent with a settings inline validation message.
