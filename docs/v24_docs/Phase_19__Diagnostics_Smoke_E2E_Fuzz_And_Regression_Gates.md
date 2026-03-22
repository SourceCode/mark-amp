# Phase 19 - Diagnostics, Smoke, E2E, Fuzz, And Regression Gates

## Phase Goal

Turn the completed workflows into defended workflows with enough diagnostics and regression protection to support confident releases.

## Measurable Outcome

- Core product workflows have deterministic smoke paths.
- High-risk integrations have targeted E2E and regression coverage.
- Diagnostics are strong enough to localize failures quickly.

### Task P19-T01

- Phase ID: `P19`
- Task ID: `P19-T01`
- Task Title: Build the release-critical smoke suite around the artifact and shell spine
- Priority: `P0`
- Category: `Testing / Regression Protection`
- Objective: Protect the primary end-to-end workflows that define whether MarkAmp feels complete.
- Why This Matters Now: The whole `v24` plan centers on a smaller set of high-value workflows that must not regress.
- Execution Gap Statement: Smoke coverage is still not strong enough for creation, save, restore, and shell continuity.
- User / Product Impact: Regressions in these paths instantly damage product trust.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`, `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Prior Plan References: `v19` Phase 09, `v20` Phase 10, `v23` Phase 19
- Scope: File/notebook/canvas create-open-save-restore, workspace open, panel restore, settings apply, SCM basic flow
- Out of Scope: Exhaustive UI visual diffing
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/e2e/appium/src/specs/`
- Related Features / Systems / Components: Smoke testing, shell workflows, regression gates
- Current Behavior: Workflow smoke coverage exists conceptually but is incomplete for the final critical path.
- Intended Completed Behavior: Release-critical workflows are covered by fast deterministic smoke runs.
- Missing Pieces: Scenario catalog, fixtures, artifact-aware assertions
- Technical Approach: Encode the critical-path workflows from `v24` into deterministic smoke scenarios with clear diagnostics.
- Implementation Steps: Define scenarios; create fixtures; add assertions; wire into CI and pre-release scripts.
- Validation Steps: Deliberately break critical-path flows and verify rapid smoke failure.
- Acceptance Criteria: The product’s critical path is defended by fast smoke suites that fail before release.
- Dependencies: Core completion phases 02-18
- Parallelization Notes: Can be scaffolded early but becomes mandatory late.
- Risks / Failure Modes: Slow or flaky smoke tests will be bypassed unless tightly curated.
- Cleanup / Migration Notes: Retire ad hoc manual-only signoff for these workflows.
- Observability / Diagnostics Notes: Capture workflow step traces and artifact state snapshots on failure.
- Rollback / Safety Notes: Keep smoke fixtures isolated and deterministic.
- References / Context: Smoke runner and Appium suite
- Example scenarios where useful: Create new file, save it, restart, reopen workspace, and confirm content and active tab restore.

### Task P19-T02

- Phase ID: `P19`
- Task ID: `P19-T02`
- Task Title: Add subsystem regression packs for action, panel, settings, and visual-system integrity
- Priority: `P1`
- Category: `Testing / Regression Protection`
- Objective: Protect the shell-governance systems that earlier phases made authoritative.
- Why This Matters Now: Once shell integrity is centralized, regressions can have wide blast radius.
- Execution Gap Statement: Action, panel, settings, and visual-system checks exist but need broader integration and enforcement.
- User / Product Impact: Regressions can reintroduce dead UI, placeholder panels, or broken settings behavior.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_control_audit.cpp`, `/Users/ryanrentfro/code/markamp/tests/unit/test_v23_action_readiness.cpp`
- Prior Plan References: `v21` Phase 10, `v22` Phase 20, `v23` Phase 19-20
- Scope: Action readiness, panel readiness, settings readiness, UI-system audits
- Out of Scope: End-user feature smoke paths already covered in `P19-T01`
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Related Features / Systems / Components: Shell governance, regression coverage
- Current Behavior: These checks exist but are not yet fully integrated as one regression pack.
- Intended Completed Behavior: Shell-governance regressions fail automatically with clear subsystem attribution.
- Missing Pieces: Unified regression pack and CI thresholds
- Technical Approach: Group related readiness tests and audits into mandatory regression packs by subsystem.
- Implementation Steps: Bundle tests; wire reports; set failure thresholds; add fixtures for known regression classes.
- Validation Steps: Seed dead action, placeholder panel, and settings drift regressions and confirm failures.
- Acceptance Criteria: Shell-governance regressions are caught before release candidates are built.
- Dependencies: `P01-T04`, `P04-T05`, `P05-T05`, `P13-T05`, `P07-T05`
- Parallelization Notes: Can harden progressively across the program.
- Risks / Failure Modes: Fragmented reporting can still obscure root causes without unified artifacts.
- Cleanup / Migration Notes: Replace scattered point checks with organized packs where practical.
- Observability / Diagnostics Notes: Publish regression-pack dashboards by subsystem.
- Rollback / Safety Notes: Phase in thresholds to avoid destabilizing CI abruptly.
- References / Context: Existing readiness tests and auditors
- Example scenarios where useful: A broken settings deep link and a missing toolbar handler both fail the same shell-integrity regression stage with separate diagnostics.

### Task P19-T03

- Phase ID: `P19`
- Task ID: `P19-T03`
- Task Title: Expand restart, recovery, and chaos validation for high-risk workflows
- Priority: `P1`
- Category: `Diagnostics / Recovery`
- Objective: Prove the product survives interruption, restart, and partial-failure cases on the completed workflow spine.
- Why This Matters Now: Completion without interruption testing still leaves reliability unproven.
- Execution Gap Statement: Recovery and chaos coverage remain thinner than the new lifecycle promises.
- User / Product Impact: Crash or interruption behavior can destroy trust even when normal-path workflows pass.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/scripts/run_chaos_tests.sh`, `/Users/ryanrentfro/code/markamp/tests/chaos/`
- Prior Plan References: `v19` Phase 09, `v23` Phase 19
- Scope: Restart during dirty state, autosave failure, sync interruption, panel restore corruption, kernel interruption
- Out of Scope: Large-scale fault-injection research outside current workflows
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Related Features / Systems / Components: Recovery, chaos testing, persistence
- Current Behavior: Some chaos and recovery infrastructure exists, but coverage is not yet aligned to the final workflow spine.
- Intended Completed Behavior: High-risk interruption paths are tested and diagnosable.
- Missing Pieces: Scenario mapping from critical workflows to fault modes
- Technical Approach: Add targeted interruption tests at the persistence, shell, sync, and execution boundaries.
- Implementation Steps: Define chaos scenarios; add restart fixtures; capture recovery diagnostics; wire into CI/nightly runs.
- Validation Steps: Force interruption during save, notebook execution, sync, and panel restore.
- Acceptance Criteria: High-risk interruption paths are covered by repeatable recovery or chaos tests.
- Dependencies: `P03-T05`, `P08-T02`, `P15-T03`
- Parallelization Notes: Can evolve as subsystem completion solidifies.
- Risks / Failure Modes: Chaotic scenarios can be flaky without controlled harnesses.
- Cleanup / Migration Notes: Replace informal "test this manually after crash" guidance with automated scenarios.
- Observability / Diagnostics Notes: Capture pre-failure, failure, and recovery state snapshots.
- Rollback / Safety Notes: Keep failure injection isolated from developer data.
- References / Context: Chaos scripts and recovery subsystems
- Example scenarios where useful: Interrupting a notebook execution and then restarting the app preserves notebook document state and surfaces kernel failure cleanly.

### Task P19-T04

- Phase ID: `P19`
- Task ID: `P19-T04`
- Task Title: Add fuzz and large-input validation to newly completed core services
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Prevent edge-case crashes in services that moved from stubbed to real implementations.
- Why This Matters Now: Search, parsing, import/export, and serializers become riskier after real completion work lands.
- Execution Gap Statement: Existing fuzz coverage is explicitly stub-like in places and does not yet target all newly completed services.
- User / Product Impact: Edge cases could still crash or corrupt the app under real-world input.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_markdown_parser.cpp`, `/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_board_serializer.cpp`
- Prior Plan References: `v23` Phase 19
- Scope: Search parsing, board serialization, notebook serialization, import/export parsers
- Out of Scope: Performance benchmarking under production-scale datasets
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/run_fuzz.sh`
- Related Features / Systems / Components: Fuzzing, parsers, serializers
- Current Behavior: Fuzz infrastructure exists but does not yet fully target newly completed product paths.
- Intended Completed Behavior: High-risk parsers and serializers are fuzzed or large-input tested after completion.
- Missing Pieces: Target selection and fixture corpora
- Technical Approach: Expand fuzz corpus and targets for services that lost placeholder behavior.
- Implementation Steps: Add targets; seed corpora; wire into nightly or specialized CI jobs.
- Validation Steps: Run fuzz jobs and verify reproducible crash artifacts when failures are found.
- Acceptance Criteria: Newly completed high-risk parsers and serializers are covered by fuzz or stress validation.
- Dependencies: Completion of search, notebook, canvas, import/export services
- Parallelization Notes: Can proceed incrementally as subsystems stabilize.
- Risks / Failure Modes: Overly broad fuzz scope can create noisy or low-value results.
- Cleanup / Migration Notes: Replace old fuzz stubs with real target integration.
- Observability / Diagnostics Notes: Persist minimized reproducer inputs and crash metadata.
- Rollback / Safety Notes: Keep fuzz targets deterministic and isolated.
- References / Context: Existing fuzz harnesses
- Example scenarios where useful: A malformed board export no longer crashes the app and becomes a captured fuzz regression instead.

### Task P19-T05

- Phase ID: `P19`
- Task ID: `P19-T05`
- Task Title: Publish a subsystem-by-subsystem validation matrix and pass/fail dashboard
- Priority: `P2`
- Category: `Release Readiness`
- Objective: Make the final validation state readable enough to support real go/no-go decisions.
- Why This Matters Now: A large product cannot rely on ad hoc test results scattered across many suites.
- Execution Gap Statement: Completion progress needs a single validation view tied to subsystem done criteria.
- User / Product Impact: Better release decisions mean fewer incomplete workflows reaching users.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/docs/v24_docs/README__MarkAmp_V24_Execution_Synthesis_Master_Index.md`
- Prior Plan References: `v23` acceptance criteria, `v24` Phase 01
- Scope: Dashboard/reporting for smoke, regression, readiness, recovery, fuzz, and signoff state
- Out of Scope: Full external release management tooling
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`
- Related Features / Systems / Components: Validation reporting, release governance
- Current Behavior: Validation evidence is fragmented.
- Intended Completed Behavior: One dashboard/report shows subsystem readiness and blocking failures.
- Missing Pieces: Matrix generation and artifact aggregation
- Technical Approach: Aggregate outputs from smoke, readiness, regression, chaos, and fuzz systems into a single report.
- Implementation Steps: Define matrix fields; collect results; publish dashboard; tie to release signoff.
- Validation Steps: Break one subsystem gate and confirm dashboard attribution and block signal.
- Acceptance Criteria: Release decisions can be made from one subsystem-by-subsystem validation view.
- Dependencies: `P01-T02`, `P19-T01` through `P19-T04`
- Parallelization Notes: Best finalized late, after most suites are stable.
- Risks / Failure Modes: Weak aggregation can hide blockers in green averages.
- Cleanup / Migration Notes: Retire fragmented manual status spreadsheets or notes.
- Observability / Diagnostics Notes: Dashboard should preserve drill-down links to raw artifacts.
- Rollback / Safety Notes: Keep raw test artifacts available independently of dashboard generation.
- References / Context: V24 subsystem done criteria
- Example scenarios where useful: The dashboard shows notebook runtime as blocked on kernel restart tests while editor/search is signed off.
