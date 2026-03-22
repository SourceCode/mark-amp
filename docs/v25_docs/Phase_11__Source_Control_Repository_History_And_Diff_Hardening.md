# Phase 11: Source Control, Repository, History, And Diff Hardening

## Phase Intent

Replace synthetic repository behavior in the release path with trustworthy source-control flows.

## Release-Ready Exit Criteria

- release-path Git and repository workflows are not backed by fake hashes or stubbed diffs,
- history and diff surfaces reflect real data sources,
- repository-related UI does not imply more than the implementation can safely deliver.

## Task Count

3

## Task P11-T01

- Phase ID: P11
- Task ID: P11-T01
- Task Title: Replace fake Git status and hash behavior in release-path flows
- Priority: P0
- Category: Release Blocker
- Objective: remove synthetic Git behavior from the user-visible source-control path.
- Why This Matters Now: source-control credibility is impossible if the repository model is fake-by-design.
- Release Gap Statement: `GitService` is still explicitly stubbed for testability and generates fake commit hashes.
- User / Product Impact: developers cannot trust status, staging, or commit context if the service is synthetic.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`
- Prior Plan References: `v23 Phase 09`; `v24 Phase 11`
- Scope: repository detection, branch identity, status, staging, real diff source, commit identity.
- Out of Scope: advanced enterprise Git features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/GitService.h`; `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`
- Related Features / Systems / Components: Git service; source-control panel; diff rendering
- Current Behavior: service comments still describe stubbed behavior and fake commit generation.
- Intended Release-Ready Behavior: release-path source control reflects real repository state or is clearly gated where unsupported.
- Missing Pieces: real repo integration; capability/error reporting; gated fallback behavior.
- Technical Approach: implement real status and diff sourcing for supported repository states and gate unsupported operations explicitly.
- Implementation Steps:
1. Define the minimum release-path Git support matrix.
2. Replace stubbed status/hash paths with real repository queries.
3. Update source-control UI to surface unsupported states honestly.
- Validation Steps:
1. Run status, stage, unstage, and diff flows against a real test repository.
2. Verify UI reflects actual branch and file state.
- Acceptance Criteria: release-path source control is not backed by fake hashes or synthetic working-tree state.
- Dependencies: P05-T01
- Parallelization Notes: repository snapshot work can proceed once service contracts are stable.
- Risks / Failure Modes: platform-specific Git handling; partial repository states.
- Observability / Diagnostics Notes: emit repository detection and command-failure diagnostics.
- Rollback / Safety Notes: gate unsupported repo states rather than falling back to fake data.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`

## Task P11-T02

- Phase ID: P11
- Task ID: P11-T02
- Task Title: Complete repository snapshot diff and history behavior needed on the release path
- Priority: P1
- Category: Source Control Hardening
- Objective: replace stubbed snapshot diff/history behavior with trustworthy repository-level comparison and restore semantics.
- Why This Matters Now: repository history and recovery are part of the product’s dependability story.
- Release Gap Statement: `RepositoryService` still leaves snapshot diffing and archive handling stubbed.
- User / Product Impact: history and restore tools cannot be trusted until comparisons are real.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`
- Prior Plan References: `v23 Phase 04`; `v24 Phase 11`
- Scope: snapshot diff, snapshot restore safety, history panel backing data, repository integrity messaging.
- Out of Scope: long-term archival optimization.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.h`; `/Users/ryanrentfro/code/markamp/src/core/AtomicWriter.cpp`
- Related Features / Systems / Components: repository snapshots; history panel; safety snapshots
- Current Behavior: stub comments still describe real work deferred in snapshot comparison and archive handling.
- Intended Release-Ready Behavior: snapshot history reflects real manifest and diff information, and restore paths are safe and understandable.
- Missing Pieces: manifest comparison; archive extraction correctness; panel integration.
- Technical Approach: implement manifest-based snapshot comparison first, then wire history views to it.
- Implementation Steps:
1. Build real snapshot manifests and diff them.
2. Replace empty or stub history data sources.
3. Add restore safety and history regression tests.
- Validation Steps:
1. Create multiple snapshots with changed files and compare them.
2. Verify history UI and restore behavior reflect real differences.
- Acceptance Criteria: snapshot diff and history flows no longer depend on stubbed comparison logic.
- Dependencies: P11-T01
- Parallelization Notes: history UI integration can be separate once service output is stable.
- Risks / Failure Modes: archive corruption; incorrect diff metadata; expensive comparisons.
- Observability / Diagnostics Notes: log snapshot IDs, manifest counts, and restore safety actions.
- Rollback / Safety Notes: keep pre-checkout safety snapshots mandatory.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`

## Task P11-T03

- Phase ID: P11
- Task ID: P11-T03
- Task Title: Gate or finish history and diff panels so they only expose trustworthy release behavior
- Priority: P1
- Category: Panel Hardening
- Objective: ensure source-control-adjacent panels do not over-promise beyond the services that back them.
- Why This Matters Now: thin or stubbed history surfaces should not remain visible on the release path.
- Release Gap Statement: panels can still exist ahead of real backing depth.
- User / Product Impact: users read visible history/diff surfaces as product commitments.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/GitLogPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SourceControlPanel.cpp`
- Prior Plan References: `v23 Phase 08`; `v24 Phase 11`
- Scope: history panel, Git log panel, diff panel visibility and messaging.
- Out of Scope: advanced Git analytics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.h`; `/Users/ryanrentfro/code/markamp/src/core/PanelLifecycleAuditor.cpp`
- Related Features / Systems / Components: source-control panels; panel readiness; diff rendering
- Current Behavior: history panel remains explicitly stubbed and diff/history trust varies.
- Intended Release-Ready Behavior: only finished and honest source-control surfaces remain visible.
- Missing Pieces: panel gating rules; service-backed panel completion; release-path inventory update.
- Technical Approach: either complete the release-path panel behavior or remove the affordance from release scope.
- Implementation Steps:
1. Classify each source-control-adjacent panel by release necessity.
2. Complete must-have panels and gate the rest.
3. Update action and panel readiness reports.
- Validation Steps:
1. Open all visible source-control panels.
2. Confirm each shows real data or is absent from the release path.
- Acceptance Criteria: no source-control panel remains visible while still stubbed.
- Dependencies: P05-T01; P11-T01; P11-T02
- Parallelization Notes: can proceed after service contracts are decided.
- Risks / Failure Modes: hidden panel entry points remain enabled.
- Release Notes / Cleanup Notes: explicitly record gated panels in release scope notes.
- Observability / Diagnostics Notes: include panel readiness blockers in release reports.
- Rollback / Safety Notes: prefer gating over shipping misleading history UI.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`
