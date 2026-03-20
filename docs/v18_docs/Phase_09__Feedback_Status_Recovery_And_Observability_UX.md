# Phase 09: Feedback Status Recovery And Observability UX

## Phase Goal
Make the workbench communicate state, progress, errors, and recovery consistently across all major flows.

## Tasks
### Task 1
- Phase ID: P09
- Task ID: P09-T01
- Task Title: Standardize Notification Progress And Status Patterns
- Objective: Define one feedback language for toasts, status items, progress, and inline state.
- Problem Statement: MarkAmp has notifications, status bar, progress service, and panel-specific feedback but they are not unified.
- Scope: NotificationEvent, NotificationManager, status bar states, progress service, command feedback conventions.
- Out of Scope: Full telemetry backend work.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ProgressService.h`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: notifications, progress, status bar, accessibility announcer.
- Current Behavior: Feedback exists in multiple forms with inconsistent usage.
- Intended Behavior: Success, warning, progress, and error states are expressed through a deliberate hierarchy.
- Technical Approach: Define severity, duration, placement, and ownership rules and apply them to shell services.
- Implementation Steps:
  1. Establish feedback taxonomy.
  2. Map task types to feedback channels.
  3. Add helpers/adapters to reduce ad hoc notifications.
  4. Ensure feedback is visible and accessible.
- Edge Cases / Failure Modes: Notification spam; conflicting progress indicators; stale status messages.
- UX Considerations: Feedback should be calm but unmistakable.
- Dependencies: P02-T05.
- Validation Steps: Review representative workflows and verify the right feedback channel is used.
- Acceptance Criteria: Core workbench flows follow one feedback language.
- Rollback / Safety Notes: Throttle repeated low-value feedback.
- References / Context: Needed to support later integration verification and polish.
- Example interactions or usage scenarios where helpful: Export should show progress and completion in a consistent pattern across document and canvas workflows.

### Task 2
- Phase ID: P09
- Task ID: P09-T02
- Task Title: Integrate Extension Status Bar Contributions Into Visible Status UI
- Objective: Make contributed status items visible, clickable, and theme-compatible in the real status bar.
- Problem Statement: Plugins can create `StatusBarItemService` items, but `StatusBarPanel` does not appear to consume them.
- Scope: Service-to-status UI adapter, item ordering, alignment, click handling, hover/tooltips, theme integration.
- Out of Scope: Extension marketplace UX.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/StatusBarItemService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/StatusBarItemService.h`
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Related Systems / Components: plugin manager, status bar item service, status bar panel, command routing.
- Current Behavior: Extension items are created in service state but are not clearly rendered by the visible status bar.
- Intended Behavior: Extension status items behave like first-class shell items.
- Technical Approach: Add a status bar adapter that reads service items and projects them into `StatusBarPanel`.
- Implementation Steps:
  1. Attach status bar panel to the item service.
  2. Merge service-backed items with built-in items.
  3. Route clicks through shared commands.
  4. Handle lifecycle on plugin activate/deactivate.
- Edge Cases / Failure Modes: Duplicate IDs; too many items; plugin unload during click.
- UX Considerations: Extension items should not visually destabilize the bar.
- Dependencies: P02-T02 and P09-T01.
- Validation Steps: Activate a plugin contributing status bar items and verify visible behavior.
- Acceptance Criteria: Status bar contributions are actually visible and interactive.
- Rollback / Safety Notes: Cap item count and truncate safely.
- References / Context: Plugin manager already wires status bar contributions into the service.
- Example interactions or usage scenarios where helpful: An extension-contributed status item should appear on the correct side of the bar and execute its command when clicked.

### Task 3
- Phase ID: P09
- Task ID: P09-T03
- Task Title: Normalize Error Empty And Recovery States Across Panels
- Objective: Replace ad hoc empty/error messaging with reusable, consistent panel-state patterns.
- Problem Statement: Search, explorer, settings, canvas, and placeholder panels each represent no-data and failure states differently.
- Scope: Empty states, loading states, recoverable failure states, retry affordances, panel-level notifications.
- Out of Scope: Fatal crash UI.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/EmptyPanelState.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Related Systems / Components: empty state components, notification manager, shell state models.
- Current Behavior: Several panels use custom placeholder text or no explicit state at all.
- Intended Behavior: Panels communicate empty/loading/error/retry states consistently.
- Technical Approach: Introduce shared panel-state rendering patterns and migrate major surfaces.
- Implementation Steps:
  1. Define panel state model.
  2. Apply to explorer, search, settings, canvas, and notebook hosts.
  3. Add retry or recovery actions where useful.
  4. Connect accessibility announcements.
- Edge Cases / Failure Modes: Rapid state changes causing flicker; conflicting inline and toast errors.
- UX Considerations: Empty states should guide action, not just describe absence.
- Dependencies: P03-T03 and P09-T01.
- Validation Steps: Put panels into no-data and error modes and verify consistency.
- Acceptance Criteria: Major surfaces share recognizable recovery patterns.
- Rollback / Safety Notes: Prefer reusable state components over one-off art/text.
- References / Context: Placeholder-heavy surfaces make this phase high leverage.
- Example interactions or usage scenarios where helpful: A failed workspace open should show a retry path in the relevant panel plus a lightweight notification.

### Task 4
- Phase ID: P09
- Task ID: P09-T04
- Task Title: Add Integration Focused Instrumentation For UI Chains
- Objective: Add targeted observability for broken chains without turning the product into a logging experiment.
- Problem Statement: Integration bugs often hide in event sequencing and stale state, and they are hard to verify without lightweight instrumentation.
- Scope: Command execution tracing, panel mode changes, session restore events, prompt lifecycle, board/notebook/editor mode switches.
- Out of Scope: Full analytics platform deployment.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`
  - `/Users/ryanrentfro/code/markamp/src/core/Logger.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp`
- Related Systems / Components: logger, event bus, shell mode transitions, command execution.
- Current Behavior: Some logging exists, but targeted integration instrumentation is inconsistent.
- Intended Behavior: Engineers can trace a failing UI chain quickly using structured logs and limited counters.
- Technical Approach: Add scoped integration logs around state transitions and command lifecycles, with debug-only verbosity where appropriate.
- Implementation Steps:
  1. Define essential integration trace points.
  2. Add structured fields for command ID, surface, target resource, and result.
  3. Add restore and prompt lifecycle logs.
  4. Document usage for regression verification.
- Edge Cases / Failure Modes: Excessive log noise; privacy-sensitive content leakage.
- UX Considerations: No user-facing behavior change except more reliable debugging.
- Dependencies: P01-T01.
- Validation Steps: Reproduce key workflows and confirm traceability.
- Acceptance Criteria: High-value integration failures become diagnosable without code archaeology.
- Rollback / Safety Notes: Avoid logging document contents or secrets.
- References / Context: This phase supports deterministic AI-agent validation.
- Example interactions or usage scenarios where helpful: A failed quick pick should leave a clear trace of request, host creation, cancel/result, and focus restoration.

### Task 5
- Phase ID: P09
- Task ID: P09-T05
- Task Title: Complete Accessibility And Feedback Cohesion For Shell State Changes
- Objective: Ensure major shell state changes produce coherent visual and accessibility feedback.
- Problem Statement: Accessibility helpers exist, but shell transitions and feedback surfaces need consistent announcement and focus behavior.
- Scope: Live announcer, keyboard mode, shell mode changes, notifications, prompt open/close, panel toggles, status changes.
- Out of Scope: Full WCAG audit beyond practical shell integration.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/accessibility/LiveAnnouncer.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/accessibility/AccessibilityController.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Related Systems / Components: accessibility controller, live announcer, focus manager, prompt host.
- Current Behavior: Accessibility support exists but is uneven across shell state transitions.
- Intended Behavior: Major changes are perceivable, focus-safe, and not overly noisy.
- Technical Approach: Define announcement policy and focus policy for shell events and apply them consistently.
- Implementation Steps:
  1. Identify state changes that need announcement.
  2. Standardize message phrasing and throttling.
  3. Ensure focus restoration works with announcements.
  4. Add regression checks for keyboard-only flows.
- Edge Cases / Failure Modes: Announcement spam during rapid state updates; missing context in terse announcements.
- UX Considerations: Accessibility should clarify the UI, not overload it.
- Dependencies: P02-T03 and P09-T01.
- Validation Steps: Exercise keyboard-only flows with prompt, panel, mode, and notification changes.
- Acceptance Criteria: Shell state changes are accessible and coherent.
- Rollback / Safety Notes: Throttle repeated announcements.
- References / Context: `MarkAmpApp` already publishes keyboard-mode changes globally.
- Example interactions or usage scenarios where helpful: Switching to canvas mode should restore focus to the canvas surface and announce the mode change once.

