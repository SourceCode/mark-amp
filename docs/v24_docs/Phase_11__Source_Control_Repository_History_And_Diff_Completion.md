# Phase 11 - Source Control, Repository, History, And Diff Completion

## Phase Goal

Replace stubbed source-control and repository behavior with real workflow depth so history, diff, and snapshot surfaces stop pretending to be complete.

## Measurable Outcome

- Git operations stop using synthetic production behavior.
- Repository snapshots and diffs are trustworthy.
- History and diff panels are either complete or gated correctly.

### Task P11-T01

- Phase ID: `P11`
- Task ID: `P11-T01`
- Task Title: Replace stubbed `GitService` production behavior with real repository operations
- Priority: `P0`
- Category: `Migration Completion`
- Objective: Eliminate deterministic fake hashes and purely synthetic repository behavior in production paths.
- Why This Matters Now: Source control is a core IDE expectation and remains explicitly stubbed.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp` is still "stubbed for testability."
- User / Product Impact: Source control UI cannot be trusted for real work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`
- Prior Plan References: `v23` Phase 09 and 17
- Scope: Repository open, status, diff, stage, commit, branch operations
- Out of Scope: Novel VCS features outside current Git scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/GitStashService.h`, `/Users/ryanrentfro/code/markamp/src/core/GitRemoteService.h`
- Related Features / Systems / Components: Git, source control panel, commit graph
- Current Behavior: Git operations are implemented as a controlled stub model.
- Intended Completed Behavior: Production Git behavior talks to a real repository backend while tests use explicit doubles.
- Missing Pieces: Real backend integration, error handling, test-double separation
- Technical Approach: Move synthetic behavior to test fakes and integrate a real Git backend behind the service interface.
- Implementation Steps: Implement real operations; preserve test harnesses separately; update panels and diagnostics for real errors.
- Validation Steps: Open a real repository, stage files, commit, branch, and inspect results.
- Acceptance Criteria: Source-control workflows no longer depend on synthetic production logic.
- Dependencies: `P10-T02`
- Parallelization Notes: Can progress with repository and history completion.
- Risks / Failure Modes: Real repository state introduces complex failure cases absent in stubs.
- Cleanup / Migration Notes: Remove fake hash generation from production paths.
- Observability / Diagnostics Notes: Emit Git operation diagnostics and repository-state summaries.
- Rollback / Safety Notes: Keep destructive Git actions confirmed and recoverable where possible.
- References / Context: Git and stash/remote service stack
- Example scenarios where useful: Committing from the source-control panel yields a real commit hash and real status updates.

### Task P11-T02

- Phase ID: `P11`
- Task ID: `P11-T02`
- Task Title: Complete repository snapshot, diff, and archive workflows
- Priority: `P1`
- Category: `Migration Completion`
- Objective: Finish repository-level history mechanisms that still stop at partial behaviors.
- Why This Matters Now: Snapshot and repository tooling is visible enough to imply product depth.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp` still leaves snapshot diffing incomplete.
- User / Product Impact: Users cannot trust repository snapshots or compare states meaningfully.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`
- Prior Plan References: `v23` Phase 04 and 09
- Scope: Snapshot creation, diffing, checkout, archive/extract, index persistence
- Out of Scope: Distributed sync semantics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CommitGraphEngine.cpp`
- Related Features / Systems / Components: Repository service, snapshot history, archive workflow
- Current Behavior: Snapshot features are only partially complete.
- Intended Completed Behavior: Repository snapshots can be created, diffed, restored, and archived with trustworthy results.
- Missing Pieces: Snapshot diff logic, lifecycle validation, archive fidelity
- Technical Approach: Complete repository data structures and compare/archive routines behind explicit service contracts.
- Implementation Steps: Implement diffing; harden index load/save; validate archive and extraction paths; update UI consumers.
- Validation Steps: Create snapshots, compare them, restore one, and archive/extract the repo state.
- Acceptance Criteria: Repository snapshots behave like real recoverable checkpoints, not placeholder metadata.
- Dependencies: `P11-T01`, `P03-T03`
- Parallelization Notes: Can proceed with history-panel work.
- Risks / Failure Modes: Snapshot metadata corruption can damage restore confidence.
- Cleanup / Migration Notes: Remove placeholder snapshot diff branches.
- Observability / Diagnostics Notes: Emit snapshot IDs, file counts, diff summaries, and index errors.
- Rollback / Safety Notes: Keep archive/restore operations non-destructive until verified.
- References / Context: Repository service and commit graph support
- Example scenarios where useful: Comparing two snapshots reveals actual changed files and content deltas.

### Task P11-T03

- Phase ID: `P11`
- Task ID: `P11-T03`
- Task Title: Finish history and diff panels on top of real repository data
- Priority: `P1`
- Category: `Panel Completion`
- Objective: Replace stubbed specialized history surfaces with real data-backed panel behavior.
- Why This Matters Now: History surfaces are still visibly incomplete and undermine the perception of source-control depth.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp` is explicitly stubbed.
- User / Product Impact: History affordances open incomplete panels instead of usable workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`
- Prior Plan References: `v21` Phase 06-07, `v23` Phase 08 and 09
- Scope: History timeline, diff preview, rollback affordances, panel activation
- Out of Scope: Advanced blame-graph visualization beyond current panel scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/HistoryService.h`
- Related Features / Systems / Components: History panel, Git log, diff engine
- Current Behavior: Specialized history panel behavior is thin and largely illustrative.
- Intended Completed Behavior: History and diff panels show real repository history and actionable state.
- Missing Pieces: Real data binding, rollback behavior, diff rendering contract
- Technical Approach: Bind history panels to completed Git and repository services and gate unavailable capabilities explicitly.
- Implementation Steps: Replace stub timeline; add real diff data; wire rollback and navigation; integrate panel lifecycle.
- Validation Steps: Browse file history, view diffs, and invoke rollback or restore safely.
- Acceptance Criteria: History and diff surfaces are data-backed, navigable, and either fully functional or clearly gated.
- Dependencies: `P05-T05`, `P11-T01`, `P11-T02`
- Parallelization Notes: Can progress with PDF/history specialized surfaces later.
- Risks / Failure Modes: History-panel UX can still mislead if repository state is unavailable and not surfaced clearly.
- Cleanup / Migration Notes: Remove stub painting and fake timeline content.
- Observability / Diagnostics Notes: Emit panel load, history query, and rollback action diagnostics.
- Rollback / Safety Notes: Keep restore/rollback actions confirmable and reversible where possible.
- References / Context: History and Git log panels
- Example scenarios where useful: Opening file history from the explorer shows actual commits and diffs for the selected file.

### Task P11-T04

- Phase ID: `P11`
- Task ID: `P11-T04`
- Task Title: Align source-control shell controls with real repository state
- Priority: `P1`
- Category: `Menu / Command Completion`
- Objective: Make source-control buttons, menus, status items, and context actions obey real repository readiness.
- Why This Matters Now: Real backend completion is not enough unless the shell reflects it accurately.
- Execution Gap Statement: Source-control UI can still expose actions without fully trustworthy state evaluation.
- User / Product Impact: Users can trigger actions in invalid repo states or miss available actions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Prior Plan References: `v21` Phase 03-04, `v23` Phase 09
- Scope: Enablement, visibility, status messaging, context actions, shortcut parity
- Out of Scope: New SCM features beyond current product scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`, `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`
- Related Features / Systems / Components: SCM shell controls, status bar, menus
- Current Behavior: Some source-control UI still sits ahead of real repository readiness or relies on older assumptions.
- Intended Completed Behavior: SCM controls accurately reflect repo state, selected files, and operation availability.
- Missing Pieces: SCM context keys and readiness gating
- Technical Approach: Expose repository state through canonical action-state evaluation and bind all SCM controls to it.
- Implementation Steps: Define SCM context keys; bind panel/status/menu actions; remove stale enablement logic.
- Validation Steps: Exercise SCM controls across clean, dirty, staged, detached, and no-repo states.
- Acceptance Criteria: SCM shell controls never imply unavailable or fake behavior.
- Dependencies: `P04-T03`, `P11-T01`
- Parallelization Notes: Can proceed while panels are being completed.
- Risks / Failure Modes: SCM state changes quickly and can expose race conditions in action enablement.
- Cleanup / Migration Notes: Remove direct status polling branches once canonical state is wired.
- Observability / Diagnostics Notes: Log SCM state transitions feeding control enablement.
- Rollback / Safety Notes: Fail closed by disabling risky actions when repo state is uncertain.
- References / Context: SCM panel and status shell
- Example scenarios where useful: `Commit` stays disabled until staged content and a valid message are present, from every surface.

### Task P11-T05

- Phase ID: `P11`
- Task ID: `P11-T05`
- Task Title: Add repository and source-control smoke suites for real repos
- Priority: `P2`
- Category: `Testing / Regression Protection`
- Objective: Protect the shift from synthetic Git behavior to real repository integration.
- Why This Matters Now: Real SCM integration introduces failure modes the current stubbed model never faced.
- Execution Gap Statement: Existing tests focus heavily on stub behavior and readiness audits.
- User / Product Impact: SCM regressions could quietly re-break one of the most trust-sensitive IDE areas.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/tests/unit/test_v23_service_stub_completion.cpp`
- Prior Plan References: `v23` Phase 19
- Scope: Real-repo fixtures, smoke tests, panel command flows, repository snapshots
- Out of Scope: Large-scale Git performance benchmarking
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Features / Systems / Components: SCM validation, smoke coverage
- Current Behavior: Coverage exists, but much of it normalizes stub behavior.
- Intended Completed Behavior: Real repository workflows are covered by repeatable smoke tests and targeted integration tests.
- Missing Pieces: Real fixture repos and end-to-end scenario coverage
- Technical Approach: Use deterministic temporary repositories in smoke and integration suites.
- Implementation Steps: Create fixture repos; add stage/commit/history/snapshot scenarios; wire into release readiness.
- Validation Steps: Break repository integration intentionally and verify smoke failure.
- Acceptance Criteria: Real repository workflows are protected by non-stub regression suites.
- Dependencies: `P11-T01` through `P11-T04`
- Parallelization Notes: Can begin with scaffolding early, but full value arrives after backend completion.
- Risks / Failure Modes: Test fixtures can become flaky across platforms if shelling out is inconsistent.
- Cleanup / Migration Notes: Reclassify stub-only tests as unit coverage for fakes rather than product readiness.
- Observability / Diagnostics Notes: Emit fixture repo paths, operation traces, and captured Git stderr on failure.
- Rollback / Safety Notes: Keep fixtures sandboxed and disposable.
- References / Context: V23 service-stub completion tests
- Example scenarios where useful: A smoke run initializes a temp repo, edits a file, stages it, commits it, and verifies history UI visibility.
