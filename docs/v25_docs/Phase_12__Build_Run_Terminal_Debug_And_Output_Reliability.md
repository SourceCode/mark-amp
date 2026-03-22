# Phase 12: Build, Run, Terminal, Debug, And Output Reliability

## Phase Intent

Make execution-adjacent IDE surfaces dependable enough to support serious workflows.

## Release-Ready Exit Criteria

- visible build, run, terminal, debug, and output controls are real and context-correct,
- output and problems surfaces align with real execution state,
- no dead or misleading execution affordances remain on the release path.

## Task Count

3

## Task P12-T01

- Phase ID: P12
- Task ID: P12-T01
- Task Title: Audit and close dead execution-surface controls on the release path
- Priority: P1
- Category: Menu / Command Hardening
- Objective: ensure build/run/debug/terminal controls are either real, correctly gated, or removed from release scope.
- Why This Matters Now: execution surfaces strongly shape whether the product feels like a real IDE.
- Release Gap Statement: broad execution UI can still imply more depth than current implementations safely deliver.
- User / Product Impact: dead run or debug affordances create high-friction trust failures.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- Prior Plan References: `v21 Phase 07`; `v23 Phase 09`; `v24 Phase 12`
- Scope: run/build/debug menu items, toolbar actions, bottom panels, output routing.
- Out of Scope: advanced debugger capabilities beyond the release matrix.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`
- Related Features / Systems / Components: terminal; output; problems; debug actions; build actions
- Current Behavior: execution controls remain at risk of action duplication and thin panel depth.
- Intended Release-Ready Behavior: every visible execution affordance is honest and context-correct.
- Missing Pieces: release-path inventory; execution action manifest adoption; panel gating.
- Technical Approach: use action/panel readiness gates to classify execution surfaces and remove misleading ones.
- Implementation Steps:
1. Inventory execution controls and panels.
2. Map each to real handler ownership or gate it.
3. Re-run readiness reports until no blockers remain.
- Validation Steps:
1. Trigger visible execution controls under valid and invalid contexts.
2. Confirm enablement and handler behavior are accurate.
- Acceptance Criteria: no dead or misleading execution control remains visible on the release path.
- Dependencies: P04-T01; P05-T01
- Parallelization Notes: panel and command owners can split this by surface.
- Risks / Failure Modes: hidden menu entries remain reachable.
- Observability / Diagnostics Notes: emit action and panel blocker IDs in execution category reports.
- Rollback / Safety Notes: gate incomplete execution features rather than shipping misleading UI.
- References / Context: `docs/v24_docs/Phase_12__Build_Run_Terminal_Debug_And_Output_Workflows.md`

## Task P12-T02

- Phase ID: P12
- Task ID: P12-T02
- Task Title: Align terminal, output, and problems panels with real process and diagnostics state
- Priority: P1
- Category: Core Workflow Hardening
- Objective: make the bottom-panel workflow coherent from process launch through output and error surfacing.
- Why This Matters Now: execution surfaces only feel real when their panels agree with one another.
- Release Gap Statement: panel breadth still risks outrunning underlying lifecycle cohesion.
- User / Product Impact: inconsistent output or diagnostics handling makes run/build flows frustrating and untrustworthy.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp`
- Prior Plan References: `v21 Phase 07`; `v24 Phase 12`
- Scope: output channel updates, terminal run state, problems panel sync, process completion notifications.
- Out of Scope: advanced terminal multiplexing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ProgressService.h`; `/Users/ryanrentfro/code/markamp/src/core/DiagnosticsService.h`
- Related Features / Systems / Components: terminal; output; diagnostics; progress
- Current Behavior: individual panels exist, but end-to-end execution-state cohesion still needs tightening.
- Intended Release-Ready Behavior: users can follow process state through terminal, output, and problems surfaces without contradiction.
- Missing Pieces: shared execution-state model; panel sync checks; workflow smoke.
- Technical Approach: define canonical execution-state events and make all bottom panels consume them consistently.
- Implementation Steps:
1. Identify execution-state events and panel consumers.
2. Normalize process-start, output, error, and completion flows.
3. Add bottom-panel workflow smoke tests.
- Validation Steps:
1. Run success and failure scenarios.
2. Verify the correct output, notification, and diagnostics behavior.
- Acceptance Criteria: output, terminal, and problems flows agree on process state and user messaging.
- Dependencies: P12-T01; P06-T03
- Parallelization Notes: can proceed with service owners and panel owners in parallel.
- Risks / Failure Modes: duplicate output streams; stale problems state.
- Observability / Diagnostics Notes: add process and output correlation IDs where possible.
- Rollback / Safety Notes: preserve readable error output even if richer panel sync fails.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

## Task P12-T03

- Phase ID: P12
- Task ID: P12-T03
- Task Title: Add smoke coverage for build, run, and output workflows that matter to release credibility
- Priority: P1
- Category: Testing / Regression Protection
- Objective: make execution-surface reliability part of release evidence rather than informal testing.
- Why This Matters Now: IDE credibility depends on repeated execution workflow success.
- Release Gap Statement: execution workflows remain under-proven as integrated shell behavior.
- User / Product Impact: broken build or run workflows can block serious usage even if editing works.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p12_execution_surfaces.cpp`
- Prior Plan References: `v24 Phase 12`; `v24 Phase 19`
- Scope: build trigger, output visibility, failure visibility, context enablement, panel reopen continuity after run.
- Out of Scope: deep debugger protocol automation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p19_validation_gates.cpp`
- Related Features / Systems / Components: smoke runner; execution surfaces; release gates
- Current Behavior: helper tests exist, but end-to-end execution workflows are not yet strong release evidence.
- Intended Release-Ready Behavior: execution workflow smoke is required signoff input.
- Missing Pieces: real smoke registration; failure diagnostics; panel continuity scenarios.
- Technical Approach: define a minimal execution workflow smoke matrix and bind it into phase readiness and final signoff.
- Implementation Steps:
1. Register build/run/output smoke scenarios.
2. Ensure they hit real shell surfaces and panel transitions.
3. Make failures block signoff.
- Validation Steps:
1. Run the smoke suite on clean and failure scenarios.
2. Verify errors are actionable and correlated to the failed step.
- Acceptance Criteria: build/run/output smoke failures are visible and release-blocking.
- Dependencies: P01-T03; P12-T01; P12-T02
- Parallelization Notes: validation-focused and low-conflict with implementation work.
- Risks / Failure Modes: nondeterministic process setup; shallow smoke coverage.
- Observability / Diagnostics Notes: capture per-step output snapshots.
- Rollback / Safety Notes: isolate smoke environment to avoid machine-specific noise.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`
