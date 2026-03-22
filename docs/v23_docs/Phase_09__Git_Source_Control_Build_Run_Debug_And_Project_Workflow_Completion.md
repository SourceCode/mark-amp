# Phase 09: Git Source Control Build Run Debug And Project Workflow Completion

## Outcome

Replace simulated Git behavior, partial repository tooling, stubbed build/debug surfaces, and shallow project workflows with finished development-environment capabilities.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P09-T01

- Phase ID: P09
- Task ID: P09-T01
- Task Title: Replace stubbed Git service behavior with real repository-backed operations
- Priority: P0
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make source-control features operate on real repositories instead of synthetic in-memory state.
- Why This Matters Now: Git behavior remains one of the clearest product features still simulated in production code.
- Completion Gap Statement: The Git service is still explicitly stubbed for testability and generates fake commit data.
- User / Product Impact: Source control features cannot yet be trusted as professional IDE capabilities.
- Repository Evidence: [GitService.cpp](/Users/ryanrentfro/code/markamp/src/core/GitService.cpp), [GitService.h](/Users/ryanrentfro/code/markamp/src/core/GitService.h), [SourceControlPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp)
- Scope: Repo open/detect, status, diff, stage, commit, branch operations, and shell integration.
- Out of Scope: Cloud remotes beyond core Git operations.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GitService.h`; `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp`
- Related Features / Systems / Components: Git, source control panel, menus, commands.
- Current Behavior: Git flows are broad but still synthetic.
- Intended Completed Behavior: Git operations reflect real repository state and filesystem changes.
- Missing Pieces: Repository integration, command execution layer, error handling, and shell synchronization.
- Technical Approach: Introduce a real Git backend and keep the current service API as the product-facing contract.
- Implementation Steps: Implement repository detection; replace synthetic state; wire stage/commit/branch operations; update panels and menu commands.
- Validation Steps: Open a real repo, stage changes, commit, switch branches, and inspect diffs.
- Acceptance Criteria: Git features no longer depend on fake hashes or synthetic working-tree state.
- Dependencies: Phase 03, Phase 04.
- Risks / Failure Modes: Real Git integration adds subprocess, auth, and repo-state complexity that needs explicit handling.
- Cleanup / Migration Notes where relevant: Remove deterministic fake-hash generation and synthetic branch bootstrap logic.
- Observability / Diagnostics Notes where relevant: Emit Git command timing, failure, and repo-state diagnostics.
- Rollback / Safety Notes: Keep read-only detection mode available during backend rollout.
- References / Context: This phase turns source control from a demo-level subsystem into a real IDE capability.
- Example scenarios where useful: Committing from the source-control panel produces a real Git commit in the opened repository.

### P09-T02

- Phase ID: P09
- Task ID: P09-T02
- Task Title: Finish commit graph, blame, stash, and repository history services that still use synthetic data
- Priority: P1
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Complete the secondary Git and repository tools that remain synthetic behind the shell.
- Why This Matters Now: Advanced source-control tooling still depends on stubbed history assumptions.
- Completion Gap Statement: Commit graph, blame, and related services still describe or use synthetic history instead of real DAG inspection.
- User / Product Impact: Advanced SCM tooling looks broader than it is.
- Repository Evidence: [CommitGraphEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/CommitGraphEngine.cpp), [GitBlameEngine.cpp](/Users/ryanrentfro/code/markamp/src/core/GitBlameEngine.cpp), [GitStashService.cpp](/Users/ryanrentfro/code/markamp/src/core/GitStashService.cpp)
- Scope: Commit DAG traversal, blame, stash operations, file history, and log panel support.
- Out of Scope: Visual polish of SCM panels.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CommitGraphEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GitBlameEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/GitStashService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/HistoryService.cpp`
- Related Features / Systems / Components: Commit graph, blame, stash, file history, log views.
- Current Behavior: Several advanced SCM services still operate on synthetic or simplified data.
- Intended Completed Behavior: Advanced SCM tools operate on real repository history and file metadata.
- Missing Pieces: Real history traversal, file diff lineage, stash persistence, and panel integration.
- Technical Approach: Build real repository-history adapters beneath the existing service interfaces.
- Implementation Steps: Implement DAG traversal; finish blame and stash paths; update history consumers; add repository fixture coverage.
- Validation Steps: Inspect blame, file history, commit graph, and stash workflows on real repositories.
- Acceptance Criteria: Advanced SCM features no longer depend on synthetic history data.
- Dependencies: P09-T01.
- Risks / Failure Modes: Large repositories can expose performance and caching issues if history services are naive.
- Cleanup / Migration Notes where relevant: Remove comments that explicitly describe stubbed DAG or blame behavior.
- Observability / Diagnostics Notes where relevant: Log repo-history query durations and cache hit rates.
- Rollback / Safety Notes: Use read-only fallback if advanced graph features fail while core Git remains operational.
- References / Context: This phase finishes the deeper tooling implied by the Git subsystem.
- Example scenarios where useful: A blame gutter reflects real commit lineage instead of generated placeholder entries.

### P09-T03

- Phase ID: P09
- Task ID: P09-T03
- Task Title: Finish build, run, terminal, and debug workflows that still stop at deferred or stubbed surfaces
- Priority: P1
- Category: UI Workflow Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make project execution tooling behave like a real development environment rather than a partial shell.
- Why This Matters Now: Build/run/debug surfaces are visible enough to be product expectations, not optional demos.
- Completion Gap Statement: Build and debug panel registration, log persistence, and some terminal/debug wiring still remain deferred or shallow.
- User / Product Impact: Users cannot fully trust project execution workflows inside the IDE.
- Repository Evidence: [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp), [BuildLogManager.cpp](/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp), [TerminalPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp)
- Scope: Build panel, debug console, run/build commands, build logs, terminal integration, panel activation.
- Out of Scope: Full debugger protocol support if not already in scope.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/DebugConsolePanel.h`
- Related Features / Systems / Components: Build, run, debug, terminal, output, project workflows.
- Current Behavior: Some build/debug surfaces remain deferred in registration or backed by incomplete storage and service behavior.
- Intended Completed Behavior: Build/run/debug flows are panel-complete, stateful, and persist relevant execution history.
- Missing Pieces: Build-panel service wiring, debug-console integration, terminal/service parity, and persistent logs.
- Technical Approach: Finish the panel/service contracts for execution tooling and align them with project context.
- Implementation Steps: Replace deferred registrations; implement log persistence; finish terminal resize/session ownership; wire build and debug commands.
- Validation Steps: Run builds, inspect logs, reopen terminal sessions, and use debug-console flows where exposed.
- Acceptance Criteria: Build and debug tooling no longer relies on deferred shell registration or stubbed storage.
- Dependencies: Phase 08, Phase 10.
- Risks / Failure Modes: Build/debug tools can become brittle across platforms and toolchains if capability detection is weak.
- Cleanup / Migration Notes where relevant: Remove deferred comments from `LayoutManager` after real panel registration lands.
- Observability / Diagnostics Notes where relevant: Emit build-start, build-fail, terminal-resize, and debug-console diagnostics.
- Rollback / Safety Notes: Keep unavailable execution tools clearly gated when a project lacks needed configuration.
- References / Context: This completes the “IDE” side of MarkAmp, not just the editing side.
- Example scenarios where useful: A build started from the toolbar produces persisted logs in a real build panel and reopens after restart.

### P09-T04

- Phase ID: P09
- Task ID: P09-T04
- Task Title: Complete launch configuration, repository project metadata, and project-command surfaces
- Priority: P2
- Category: File / Workspace Completion
- Atomic Completion Tasks Covered: 60
- Objective: Finish the project metadata and launch plumbing needed to make run/debug commands coherent.
- Why This Matters Now: Execution tooling cannot feel complete if project metadata remains shallow.
- Completion Gap Statement: Launch and project helpers exist, but some underlying metadata and repository services remain incomplete.
- User / Product Impact: Project-aware commands can misbehave or feel underpowered.
- Repository Evidence: [LaunchConfig.cpp](/Users/ryanrentfro/code/markamp/src/core/LaunchConfig.cpp), [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp), [BuildService.cpp](/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp)
- Scope: Launch placeholders, project target detection, repository metadata, build target population, run/debug context.
- Out of Scope: Full DAP adapter development.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/LaunchConfig.cpp`; `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuildService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: Launch configs, build targets, run/debug commands, workspace metadata.
- Current Behavior: Project command surfaces depend on partially completed launch and metadata helpers.
- Intended Completed Behavior: Project-aware commands resolve paths, targets, and context deterministically.
- Missing Pieces: Real target discovery, metadata persistence, and launch-config validation.
- Technical Approach: Finish project metadata services and align run/debug commands to consume them.
- Implementation Steps: Validate and expand launch placeholders; finish build-target discovery; persist project metadata; connect commands to real resolution results.
- Validation Steps: Open configured projects and run build/debug commands with varying active files and roots.
- Acceptance Criteria: Project commands no longer depend on shallow defaults or placeholder metadata.
- Dependencies: P09-T03.
- Risks / Failure Modes: Project metadata can become toolchain-specific if abstraction is too narrow.
- Cleanup / Migration Notes where relevant: Remove “populate with default targets for now” behavior after real discovery is in place.
- Observability / Diagnostics Notes where relevant: Record launch-resolution failures and fallback target usage.
- Rollback / Safety Notes: Keep safe read-only project detection when full launch execution is unavailable.
- References / Context: This closes the metadata side of run/debug completion.
- Example scenarios where useful: Running the active file resolves the correct workspace root and target instead of using defaults.

### P09-T05

- Phase ID: P09
- Task ID: P09-T05
- Task Title: Add real-repository and project-execution integration coverage
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Prove source-control and project-execution features against real repositories and project fixtures.
- Why This Matters Now: Stubbed services can pass unit tests too easily unless real fixtures are used.
- Completion Gap Statement: Existing SCM and build coverage is not yet strong enough to guarantee that synthetic implementations have actually been replaced.
- User / Product Impact: Source-control and project regressions can remain hidden until manual testing.
- Repository Evidence: [test_extension_scanner.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_extension_scanner.cpp), [test_phase20_file_management.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp), [scripts/smoke_test.sh](/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh)
- Scope: Real Git fixtures, build-log persistence, launch config resolution, terminal/session behavior, panel activation.
- Out of Scope: Full debugger end-to-end if external adapters are unavailable.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`
- Related Features / Systems / Components: Git, build, run, debug, tests.
- Current Behavior: Tests still allow too much simulated SCM and project-execution behavior.
- Intended Completed Behavior: Fixtures and smoke paths prove the real integrations work.
- Missing Pieces: Real repo fixtures, execution fixtures, panel-state assertions, and persisted-log checks.
- Technical Approach: Add hermetic fixture repositories and project workspaces to integration coverage.
- Implementation Steps: Create repo fixtures; add commit/stage/diff scenarios; add build/run log checks; verify launch resolution and terminal persistence.
- Validation Steps: Swap back to fake Git state or stubbed log save behavior and confirm tests fail.
- Acceptance Criteria: SCM and project-execution completion work is protected by integration coverage.
- Dependencies: P09-T01 through P09-T04.
- Risks / Failure Modes: Real-repo fixtures can be brittle if platform-specific Git configuration leaks into tests.
- Cleanup / Migration Notes where relevant: Replace synthetic-history tests once real fixture coverage is stable.
- Observability / Diagnostics Notes where relevant: Archive repo fixture state and build logs for failed runs.
- Rollback / Safety Notes: Keep fixture repositories isolated from user global Git config wherever possible.
- References / Context: This prevents the source-control subsystem from regressing into a “broad but fake” state.
- Example scenarios where useful: A test repo is opened, modified, staged, committed, and reopened with history intact.
