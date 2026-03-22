# Phase 12 - Build, Run, Terminal, Debug, And Output Workflows

## Phase Goal

Finish the lower workbench surfaces that users expect in a serious IDE, while explicitly gating incomplete capabilities that should not yet ship as normal production behavior.

## Measurable Outcome

- Terminal, debug, build, and output panels are accurately represented in shell readiness.
- Build/run workflows persist logs and state correctly.
- Incomplete capabilities are gated instead of implied.

### Task P12-T01

- Phase ID: `P12`
- Task ID: `P12-T01`
- Task Title: Complete terminal panel and service integration or gate it decisively
- Priority: `P0`
- Category: `Panel Completion`
- Objective: Ensure terminal functionality is either real or not represented as a standard product panel.
- Why This Matters Now: Terminal is a core IDE expectation and the shell already exposes deferred terminal seams.
- Execution Gap Statement: `LayoutManager` registers terminal as deferred and `TerminalService` remains a stub area in the extension/service stack.
- User / Product Impact: Users can infer capabilities that are not fully implemented.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- Prior Plan References: `v21` Phase 07, `v23` Phase 09 and 18
- Scope: Terminal panel host, terminal service injection, readiness gating
- Out of Scope: Full terminal multiplexing feature expansion
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h`
- Related Features / Systems / Components: Terminal panel, shell panel lifecycle
- Current Behavior: Terminal capability remains deferred and conditionally absent.
- Intended Completed Behavior: Terminal is either fully wired and stable or explicitly gated from standard production use.
- Missing Pieces: Real host/service bridge, readiness policy
- Technical Approach: Decide completion path; implement terminal integration or hard-gate the panel until real.
- Implementation Steps: Audit terminal service; complete host hookup; add lifecycle tests or gate panel access.
- Validation Steps: Open terminal, run commands, persist panel state, and restore layout.
- Acceptance Criteria: Terminal panel no longer exists in ambiguous deferred production state.
- Dependencies: `P05-T01`, `P05-T05`
- Parallelization Notes: Can proceed with build/output work.
- Risks / Failure Modes: Platform differences can complicate terminal integration.
- Cleanup / Migration Notes: Remove deferred placeholder registration once decision is implemented.
- Observability / Diagnostics Notes: Emit terminal service readiness and panel initialization diagnostics.
- Rollback / Safety Notes: Prefer gating over shipping partial terminal behavior.
- References / Context: Layout manager deferred panel registration
- Example scenarios where useful: Toggling terminal from the View menu either opens a working terminal or is absent from standard release UI.

### Task P12-T02

- Phase ID: `P12`
- Task ID: `P12-T02`
- Task Title: Finish build output persistence, refresh, and panel lifecycle
- Priority: `P1`
- Category: `Panel Completion`
- Objective: Make build logs and build panel behavior durable and useful.
- Why This Matters Now: Build surfaces are present but still lean on stubby persistence behavior.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp` still contains file-I/O stubs.
- User / Product Impact: Build diagnostics and history can be lost or feel unreliable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/BuildPanel.cpp`
- Prior Plan References: `v23` Phase 09
- Scope: Build log storage, panel refresh, status transitions, retained history
- Out of Scope: Full task runner ecosystem
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- Related Features / Systems / Components: Build panel, output panel, log manager
- Current Behavior: Build logs are not yet durably managed.
- Intended Completed Behavior: Build runs produce stable log history, panel updates, and restart continuity where appropriate.
- Missing Pieces: Real persistence semantics and log lifecycle rules
- Technical Approach: Implement structured build-log storage and integrate panel consumers with it.
- Implementation Steps: Replace stub log I/O; model build sessions; wire build panel refresh and output linkage.
- Validation Steps: Run builds, reopen output/history, and verify persisted logs and status transitions.
- Acceptance Criteria: Build output survives normal lifecycle expectations and updates visible panels accurately.
- Dependencies: `P03-T01`, `P05-T01`
- Parallelization Notes: Can progress independently of terminal if build execution path is separate.
- Risks / Failure Modes: Excessive log volume can create storage and UI performance issues.
- Cleanup / Migration Notes: Remove stub persistence comments and fake refresh assumptions.
- Observability / Diagnostics Notes: Emit build run IDs, log sizes, and persistence errors.
- Rollback / Safety Notes: Cap log retention and preserve truncation diagnostics.
- References / Context: Build log and panel stack
- Example scenarios where useful: A failed build reopens with its log intact and navigable from the build panel.

### Task P12-T03

- Phase ID: `P12`
- Task ID: `P12-T03`
- Task Title: Align output, debug console, and run-status surfaces with canonical action/state rules
- Priority: `P1`
- Category: `Menu / Command Completion`
- Objective: Make run/build/debug surfaces obey the same action readiness and panel lifecycle rules as the rest of the shell.
- Why This Matters Now: These surfaces often remain shell outliers and accumulate local logic quickly.
- Execution Gap Statement: Deferred build/debug/output panels still rely on shell-local assumptions and thin host logic.
- User / Product Impact: Users see surfaces that look integrated but behave inconsistently.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- Prior Plan References: `v21` Phase 06-07, `v23` Phase 09
- Scope: Panel toggles, action enablement, status indications, command routing
- Out of Scope: Deep debugger protocol implementation beyond current shell completion
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`
- Related Features / Systems / Components: Output/debug/build panels, shell controls
- Current Behavior: These surfaces are more present than fully governed.
- Intended Completed Behavior: Run/build/debug surfaces are consistently bound to readiness and action-state models.
- Missing Pieces: Context keys, readiness metadata, canonical handlers
- Technical Approach: Register these surfaces in the same action and panel readiness systems as other shell panels.
- Implementation Steps: Add command/state metadata; normalize toggles; wire status bar and panel actions to canonical handlers.
- Validation Steps: Open/toggle/build/run from menus, buttons, and shortcuts with consistent results.
- Acceptance Criteria: Run/build/debug surfaces no longer bypass shell control governance.
- Dependencies: `P04-T01`, `P05-T01`
- Parallelization Notes: Can progress with source-control and settings work.
- Risks / Failure Modes: Weak runtime state can make action enablement unstable.
- Cleanup / Migration Notes: Remove duplicate panel-toggle logic from local widgets.
- Observability / Diagnostics Notes: Emit panel-state and action-state diagnostics for build/run/debug surfaces.
- Rollback / Safety Notes: Disable uncertain actions rather than exposing dead controls.
- References / Context: Panel and action audit infrastructure
- Example scenarios where useful: `View > Output` and a status-bar output toggle open the same panel and active channel.

### Task P12-T04

- Phase ID: `P12`
- Task ID: `P12-T04`
- Task Title: Gate unimplemented run/debug/task capabilities behind explicit policy
- Priority: `P1`
- Category: `Placeholder / Stub Removal`
- Objective: Stop partially represented execution features from leaking into standard product expectations.
- Why This Matters Now: Some advanced execution surfaces still depend on stub services or future placeholders.
- Execution Gap Statement: Task runner and debug-related contribution/service seams are broader than their completed backend reality.
- User / Product Impact: Users see hints of capability that are not yet dependable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/TaskRunnerService.h`, `/Users/ryanrentfro/code/markamp/src/core/ExtensionManifest.h`
- Prior Plan References: `v23` Phase 18 and 20
- Scope: Feature flags, visibility rules, menu/panel gating, release policy
- Out of Scope: Finishing the full debug/task ecosystem in this phase
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.h`
- Related Features / Systems / Components: Feature flags, debug/task capabilities, release policy
- Current Behavior: Some unfinished run/debug/task capabilities may still be too visible.
- Intended Completed Behavior: Only capabilities backed by real workflows appear as standard product features.
- Missing Pieces: Policy mapping from unfinished capability to shell visibility
- Technical Approach: Use explicit capability readiness policy rather than incidental visibility.
- Implementation Steps: Inventory unfinished execution capabilities; wire gates to controls/panels; update docs and tests.
- Validation Steps: Verify gated capabilities do not appear in release-mode menus or panels.
- Acceptance Criteria: No unfinished execution capability remains implied as production-ready without a tracked waiver.
- Dependencies: `P01-T04`, `P05-T05`
- Parallelization Notes: Can land before or during deeper execution work.
- Risks / Failure Modes: Hidden features can still be reachable through legacy shortcuts or restore state.
- Cleanup / Migration Notes: Remove stale menu items and hidden commands when capabilities remain deferred.
- Observability / Diagnostics Notes: Log attempts to invoke gated capabilities.
- Rollback / Safety Notes: Developer-mode overrides should remain explicit and non-default.
- References / Context: Feature registry and capability gating
- Example scenarios where useful: A not-yet-complete debugger panel is not shown in the default View menu until its runtime is real.

### Task P12-T05

- Phase ID: `P12`
- Task ID: `P12-T05`
- Task Title: Add workbench execution-surface smoke tests
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Protect terminal/build/output/debug surface wiring during ongoing completion work.
- Why This Matters Now: These surfaces sit at the intersection of panels, commands, persistence, and status UI.
- Execution Gap Statement: Regression protection for execution surfaces is still too weak relative to their shell complexity.
- User / Product Impact: Regressions in these areas are highly visible and harm IDE credibility.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`
- Prior Plan References: `v19` Phase 09, `v23` Phase 19
- Scope: Panel toggles, log visibility, build-run state, restore-safe panel opening
- Out of Scope: Full debugger scenario automation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Features / Systems / Components: Smoke testing, execution surfaces
- Current Behavior: Coverage is insufficient for these cross-surface interactions.
- Intended Completed Behavior: Execution surfaces are covered by deterministic shell smoke paths.
- Missing Pieces: Fixtures, scenarios, gating thresholds
- Technical Approach: Add shell-level smoke workflows that exercise panel lifecycles and status updates.
- Implementation Steps: Define scenarios; add fixtures; wire to CI; attach diagnostics capture.
- Validation Steps: Intentionally regress panel toggles or status updates and confirm smoke failures.
- Acceptance Criteria: Core execution surfaces are covered by repeatable smoke scenarios.
- Dependencies: `P12-T01` through `P12-T04`
- Parallelization Notes: Can scaffold early and harden as implementation stabilizes.
- Risks / Failure Modes: Environment-sensitive surfaces may require fallback mocks for deterministic smoke.
- Cleanup / Migration Notes: Replace informal manual checks with automated smoke where practical.
- Observability / Diagnostics Notes: Capture panel state snapshots and output logs on failure.
- Rollback / Safety Notes: Keep smoke environment isolated from developer machines.
- References / Context: Workflow smoke infrastructure
- Example scenarios where useful: Toggle Output, run a build, inspect log, hide panel, restore layout, reopen app, verify panel state and logs.
