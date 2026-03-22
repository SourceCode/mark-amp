# Phase 19: Integrated Validation, Recovery, Observability, And Regression Gates

## Phase Intent

Make validation and recovery evidence capable of blocking release.

## Release-Ready Exit Criteria

- core user journeys have smoke or integration coverage,
- restart and recovery scenarios are validated,
- release gating consumes observability and regression outputs rather than ignoring them.

## Task Count

3

## Task P19-T01

- Phase ID: P19
- Task ID: P19-T01
- Task Title: Turn workflow smoke coverage into a required release gate
- Priority: P0
- Category: Testing / Regression Protection
- Objective: make smoke results a blocking input to subsystem readiness and final signoff.
- Why This Matters Now: the repository already contains smoke infrastructure but still needs enforcement.
- Release Gap Statement: smoke coverage remains too optional to serve as release proof.
- User / Product Impact: unblocked smoke failures would allow obvious workflow regressions into a release candidate.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.cpp`
- Prior Plan References: `v20 Phase 10`; `v24 Phase 19`; `v24 Phase 20`
- Scope: artifact lifecycle, notebook, canvas, workspace reopen, settings, search, source control, build/run smoke suites.
- Out of Scope: large-scale performance or chaos coverage beyond release needs.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p19_validation_gates.cpp`
- Related Features / Systems / Components: smoke runner; done criteria; release signoff
- Current Behavior: smoke helpers exist, but failures are not yet the decisive release law.
- Intended Release-Ready Behavior: missing or failing required smoke suites block subsystem signoff.
- Missing Pieces: required-smoke registry; gate wiring; live suite registration.
- Technical Approach: make required smoke IDs part of subsystem criteria and fail signoff automatically when they are absent or red.
- Implementation Steps:
1. Define the required smoke suite inventory.
2. Register suites for each release-path subsystem.
3. Connect smoke failures to the signoff report.
- Validation Steps:
1. Force a smoke failure and confirm signoff blocks.
2. Verify passing suites unblock the associated subsystem.
- Acceptance Criteria: smoke failures are visible, attributed, and release-blocking.
- Dependencies: P01-T03
- Parallelization Notes: can progress throughout the wave as subsystems mature.
- Risks / Failure Modes: shallow suites; unstable setup.
- Observability / Diagnostics Notes: produce per-suite pass/fail reports with blocking IDs.
- Rollback / Safety Notes: stabilize flaky suites before promoting them to hard gates.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`

## Task P19-T02

- Phase ID: P19
- Task ID: P19-T02
- Task Title: Add restart, recovery, and invalid-state regression coverage for the highest-risk workflows
- Priority: P0
- Category: Diagnostics / Recovery
- Objective: explicitly validate the recovery scenarios most likely to block release confidence.
- Why This Matters Now: restart and recovery are still among the weakest high-stakes trust areas.
- Release Gap Statement: recovery behavior exists, but its regression and restart proof remains insufficient.
- User / Product Impact: a crash or invalid state that loses work can negate confidence in the whole product.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/RecoveryUXService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v18 Phase 09`; `v24 Phase 03`; `v24 Phase 19`
- Scope: autosave recovery, failed save recovery, external-change recovery, crash/restart recovery, invalid panel restore fallback.
- Out of Scope: chaos coverage beyond high-signal release scenarios.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/chaos/test_chaos_engine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SafeMode.h`
- Related Features / Systems / Components: recovery UX; session restore; safe mode; restart tests
- Current Behavior: high-risk recovery cases are not yet strongly represented in release-gate evidence.
- Intended Release-Ready Behavior: the most damaging recovery regressions are caught before release.
- Missing Pieces: deterministic restart harness; scenario definitions; gate wiring.
- Technical Approach: define a small set of highest-value recovery regressions and make them mandatory.
- Implementation Steps:
1. Select the highest-risk recovery cases from release-path workflows.
2. Build deterministic regression cases for them.
3. Wire failures into subsystem signoff.
- Validation Steps:
1. Run crash/restart and failed-save scenarios.
2. Confirm correct recovery choices and restored content.
- Acceptance Criteria: recovery regressions in high-risk flows cannot pass unnoticed.
- Dependencies: P03-T02; P03-T03; P19-T01
- Parallelization Notes: validation-focused and can overlap with final subsystem work.
- Risks / Failure Modes: nondeterministic restart behavior; flaky recovery setup.
- Observability / Diagnostics Notes: capture recovery traces and restored artifact IDs.
- Rollback / Safety Notes: default to safe recovery prompts when uncertain.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/RecoveryUXService.cpp`

## Task P19-T03

- Phase ID: P19
- Task ID: P19-T03
- Task Title: Consolidate release-path observability and blocker reporting into one final validation dashboard
- Priority: P1
- Category: Release Readiness
- Objective: give the implementation team one authoritative view of blockers, failed smoke suites, unresolved placeholders, and blocked subsystems.
- Why This Matters Now: the repository contains many audit helpers, but release closure needs one integrated view.
- Release Gap Statement: evidence remains spread across helpers and tests.
- User / Product Impact: engineering velocity and release confidence improve when blocker evidence is centralized.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CompletionInventory.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ServiceStubCompletionAuditor.cpp`
- Prior Plan References: `v23 Phase 19`; `v24 Phase 19`; `v24 Phase 20`
- Scope: ledger state, smoke failures, done criteria, action/panel blockers, placeholder inventories.
- Out of Scope: external telemetry dashboards beyond release needs.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.cpp`
- Related Features / Systems / Components: ledger; signoff; inventories; gates
- Current Behavior: useful reports exist, but they remain fragmented.
- Intended Release-Ready Behavior: one generated dashboard or report shows exactly why release is blocked or clear.
- Missing Pieces: report aggregation; stable output format; release consumption path.
- Technical Approach: aggregate existing governance and audit outputs into a final release-readiness report generator.
- Implementation Steps:
1. Define the integrated report schema.
2. Aggregate outputs from the ledger, smoke runner, readiness gates, and inventories.
3. Publish the report in CI-friendly and human-readable forms.
- Validation Steps:
1. Trigger known blockers and verify they appear in the report.
2. Confirm a green state clearly indicates signoff readiness.
- Acceptance Criteria: there is one authoritative release validation report with stable blocker categories.
- Dependencies: P01-T01; P01-T02; P01-T03; P19-T01
- Parallelization Notes: aggregation work can start early, final wiring lands late.
- Risks / Failure Modes: duplicated blocker entries; report noise overwhelming signal.
- Observability / Diagnostics Notes: this task is itself observability consolidation.
- Rollback / Safety Notes: keep underlying reports available during integration.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.h`
