# Phase 09: Feedback Observability And Regression Harnesses

## Phase Purpose
Make integration quality measurable and user-visible behavior consistent across progress, status, failure, and recovery paths.

## Measurable Outcome
- Status bar, notifications, panel states, and command diagnostics are unified enough to trust.
- The app has repeatable smoke paths for the highest-risk workflows.
- Integration regressions become visible quickly instead of hiding in shell event drift.

## Tasks

### P09-T01
- Phase ID: P09
- Task ID: P09-T01
- Task Class: Workflow
- Task Title: Finish Status Bar Item Projection And Click Routing
- Priority: High
- Objective: Make extension and service-contributed status items appear in the real status bar and execute real commands when clicked.
- Why This Matters Now: The plugin system can create status items, but the visible shell still mostly ignores them.
- Problem Statement: `StatusBarItemService` is wired into plugins, `StatusBarAdapter` exists, but `StatusBarPanel` still renders mostly internal state without that bridge.
- Scope: projected status items, ordering, alignment, click routing, and fallback overflow policy.
- Out of Scope: Rich custom status bar widgets from extensions.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/StatusBarItemService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/StatusBarAdapter.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`
- Related Systems / Components: status bar, plugin contributions, command dispatcher, shell feedback.
- Current Behavior: Status bar contributions can be created in services but are not fully projected into the visible status bar.
- Intended Behavior: The visible status bar merges first-party and extension items coherently.
- Technical Approach: Promote `StatusBarAdapter` into the real runtime path and give `StatusBarPanel` a projected-item render lane.
- Implementation Steps:
  1. Instantiate `StatusBarAdapter` in the app bootstrap.
  2. Refresh projected items from `StatusBarItemService`.
  3. Render projected items in `StatusBarPanel`.
  4. Route clicks through shared command execution rather than info-only notifications.
- Validation Steps:
  1. Activate built-in and sample plugins that create status items.
  2. Verify item ordering, visibility, and click execution.
- Acceptance Criteria: Extension-created status items appear and behave like part of the shell.
- Dependencies: P01-T02, P02-T02.
- Risks / Failure Modes: Status bar crowding may require overflow behavior sooner than expected.
- UX Notes: Status bar should stay calm and useful, not become noisy chrome.
- Observability / Diagnostics Notes: Log projected item count, hidden overflow count, and click outcomes.
- Rollback / Safety Notes: Cap extension item count and preserve first-party item priority.
- References / Context:
  - `StatusBarAdapter.cpp` currently only sorts items and posts informational notifications on click.
- Example Scenarios Where Useful:
  - A plugin-created sync item should appear beside first-party status indicators and trigger its command when clicked.

### P09-T02
- Phase ID: P09
- Task ID: P09-T02
- Task Class: Foundational
- Task Title: Apply Shared Panel State Patterns To Shell Critical Surfaces
- Priority: High
- Objective: Standardize empty, loading, failed, and recoverable states for the most visible shell panels.
- Why This Matters Now: Several panels already exist but still express failure and no-data states inconsistently or not at all.
- Problem Statement: `PanelStateModel` exists, but major surfaces still use ad hoc labels, hidden widgets, or nothing.
- Scope: explorer, search, settings host, notebook host, canvas host, and startup-related empty/error states.
- Out of Scope: Deep redesign of every secondary panel.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/PanelStateModel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Related Systems / Components: panel states, empty states, load states, retry actions, shell feedback.
- Current Behavior: Some panels have `EmptyPanelState`, some use custom text, and some use no shared state model.
- Intended Behavior: Core surfaces share recognizable and actionable state patterns.
- Technical Approach: Use `PanelStateModel` as the contract and `EmptyPanelState` or equivalent renderers as the visual language.
- Implementation Steps:
  1. Define shared panel-state render rules for core shell surfaces.
  2. Add retry and action hooks where meaningful.
  3. Replace ad hoc placeholder labels in core surfaces.
  4. Tie panel state transitions to workspace load, search, notebook, and canvas host state.
- Validation Steps:
  1. Put each core surface into loading, empty, and failed states.
  2. Verify retry and messaging consistency.
- Acceptance Criteria: Core shell surfaces express empty/loading/error states with one coherent UX language.
- Dependencies: P03-T01, P07-T01, P08-T02.
- Risks / Failure Modes: Over-standardization may flatten necessary domain nuance if the renderer is too rigid.
- UX Notes: State surfaces should help users recover, not merely state that something went wrong.
- Observability / Diagnostics Notes: Record panel-state transitions for core shell surfaces.
- Rollback / Safety Notes: Preserve surface-specific content areas while standardizing only the state shell.
- References / Context:
  - `SearchSidebarPanel` and `CanvasWorkspacePanel` still contain local placeholder and empty-state handling.
- Example Scenarios Where Useful:
  - Running search with no workspace open should give a guided no-workspace state instead of just empty results.

### P09-T03
- Phase ID: P09
- Task ID: P09-T03
- Task Class: Foundational
- Task Title: Add High Value Smoke Paths And Integration Diagnostics
- Priority: Critical
- Objective: Create repeatable validation for the workflows most likely to regress during the v19 wave.
- Why This Matters Now: This plan changes shared shell plumbing. Without validation harnesses, regressions will hide behind event chains.
- Problem Statement: Existing command and observability helpers exist, but they are not yet a decisive integration safety net.
- Scope: smoke paths, command execution logging, shell transition logging, focus tracing, and launch-to-workflow validation.
- Out of Scope: Full end-to-end UI automation coverage of the entire product.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/core/CommandExecutionLog.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ObservabilityCommands.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
  - `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Systems / Components: smoke test script, command execution tracing, shell transition tracing, regression tests.
- Current Behavior: Pieces of execution logging and observability exist, but the highest-risk shell flows are not protected as a coherent suite.
- Intended Behavior: The team can run a small set of shell-focused validations after each major integration change.
- Technical Approach: Add a v19 smoke matrix that covers shell, search, settings, notebook, canvas, and icon-critical flows.
- Implementation Steps:
  1. Define a smoke matrix for launch, workspace open, palette command, search, settings deep link, notebook open, canvas open, and restore.
  2. Wire command execution logging into shared dispatch.
  3. Add shell transition, focus, and restore diagnostics behind debug toggles.
  4. Extend `scripts/smoke_test.sh` or add dedicated smoke helpers.
- Validation Steps:
  1. Run the smoke suite locally and in CI.
  2. Confirm failures point to actionable logs rather than generic crashes.
- Acceptance Criteria: Shared-shell regressions can be caught quickly with repeatable diagnostics.
- Dependencies: P01-T01, P02-T02.
- Risks / Failure Modes: Low-signal logs can drown the useful signals if instrumentation is too noisy.
- UX Notes: Better diagnostics indirectly protect product polish by catching shell regressions earlier.
- Observability / Diagnostics Notes: This task is the observability task. Keep logs structured and source-tagged.
- Rollback / Safety Notes: Gate verbose tracing behind config or debug flags.
- References / Context:
  - `CommandExecutionLog.cpp` and `ObservabilityCommands.cpp` provide starting points but are not yet the full integration harness.
- Example Scenarios Where Useful:
  - After changing shell-controller sequencing, the smoke suite should immediately tell whether startup, search, settings, notebook, or canvas flows broke.
