# Phase 10: Workspace, Project Navigation, And Reopen Continuity

## Phase Intent

Make workspace continuity, navigation, and reopen behavior dependable enough for serious daily use.

## Release-Ready Exit Criteria

- project and workspace open/reopen flows are stable,
- recent items and navigation history restore meaningful continuity,
- no release-path navigation entry point depends on stale or placeholder assumptions.

## Task Count

3

## Task P10-T01

- Phase ID: P10
- Task ID: P10-T01
- Task Title: Harden workspace open, recent items, and reopen continuity across artifact kinds
- Priority: P0
- Category: Core Workflow Hardening
- Objective: ensure workspace-level continuity works across text, notebook, and canvas artifacts.
- Why This Matters Now: release candidate trust depends on reliable project reopen behavior.
- Release Gap Statement: reopen continuity still depends too much on partial session and panel assumptions.
- User / Product Impact: users need confidence that their project state survives normal reopen paths.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Prior Plan References: `v19 Phase 05`; `v24 Phase 10`
- Scope: open workspace, recent workspaces, recent artifacts, active artifact reopen, layout continuity.
- Out of Scope: cloud-synced cross-device history.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.cpp`
- Related Features / Systems / Components: workspace restore; recent items; navigation history
- Current Behavior: continuity scaffolding exists, but proof of full artifact-kind consistency is still weak.
- Intended Release-Ready Behavior: users reopen the workspace and return to a valid, artifact-aware shell state.
- Missing Pieces: cross-artifact continuity coverage; stale-entry validation; recent-item cleanup rules.
- Technical Approach: unify recent-item and restore metadata around canonical artifact descriptors and validated workspace state.
- Implementation Steps:
1. Define continuity rules for mixed artifact sessions.
2. Validate recent-item and reopen entries before use.
3. Add restart continuity smoke tests.
- Validation Steps:
1. Reopen recent text, notebook, and canvas artifacts from workspace state.
2. Confirm no invalid or duplicate reopen behavior.
- Acceptance Criteria: workspace reopen is deterministic and artifact-aware for release-path workflows.
- Dependencies: P03-T03; P09-T02; P08-T01
- Parallelization Notes: can run alongside navigation UX cleanup.
- Risks / Failure Modes: stale recent entries; wrong active artifact restoration.
- Observability / Diagnostics Notes: emit continuity diagnostics for restored versus skipped items.
- Rollback / Safety Notes: invalid recent entries should be skipped safely and surfaced.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.h`

## Task P10-T02

- Phase ID: P10
- Task ID: P10-T02
- Task Title: Make explorer, breadcrumbs, and navigation history context-correct
- Priority: P1
- Category: Core Workflow Hardening
- Objective: align navigation surfaces with the true active artifact and workspace context.
- Why This Matters Now: navigation coherence is part of shell trust, not polish-only work.
- Release Gap Statement: users can still encounter navigation surfaces that lag behind active artifact truth.
- User / Product Impact: wrong navigation state makes the IDE feel fragmented and unpredictable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.cpp`
- Prior Plan References: `v18 Phase 05`; `v22 Phase 05`; `v24 Phase 10`
- Scope: breadcrumbs, back/forward navigation, explorer selection sync, active artifact sync.
- Out of Scope: advanced graph-navigation features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.h`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Features / Systems / Components: breadcrumbs; tab activation; explorer sync; history
- Current Behavior: navigation pieces exist but still need stronger active-artifact coupling.
- Intended Release-Ready Behavior: navigation surfaces accurately reflect and update the user’s current context.
- Missing Pieces: active artifact sync rules; cross-surface selection coverage.
- Technical Approach: treat `ArtifactRegistry` active artifact as the navigation truth and update explorer/breadcrumb/history consumers accordingly.
- Implementation Steps:
1. Audit active-artifact updates across navigation surfaces.
2. Remove stale per-surface navigation assumptions.
3. Add back/forward and explorer-sync tests.
- Validation Steps:
1. Navigate across mixed artifact types.
2. Confirm explorer, tabs, and breadcrumbs stay aligned.
- Acceptance Criteria: release-path navigation surfaces do not drift from active artifact truth.
- Dependencies: P02-T03; P04-T02
- Parallelization Notes: good candidate for incremental surface-specific work.
- Risks / Failure Modes: stale selection loops; accidental recursive updates.
- Observability / Diagnostics Notes: trace active-artifact change propagation through navigation surfaces.
- Rollback / Safety Notes: prefer safe lagging updates over corrupting active context.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/NavigationHistory.cpp`

## Task P10-T03

- Phase ID: P10
- Task ID: P10-T03
- Task Title: Add workspace continuity smoke and restart validation for daily-use project flows
- Priority: P1
- Category: Testing / Regression Protection
- Objective: prove that normal project open/close/reopen behavior works repeatedly.
- Why This Matters Now: project continuity bugs often hide until late unless explicitly tested.
- Release Gap Statement: workspace continuity still lacks the kind of smoke coverage needed for release trust.
- User / Product Impact: broken reopen behavior can block serious adoption even when individual features work.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Prior Plan References: `v24 Phase 10`; `v24 Phase 19`
- Scope: open workspace, open mixed artifacts, save, close, restart, reopen, restore panel context.
- Out of Scope: multi-workspace concurrency.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p10_workspace_navigation.cpp`
- Related Features / Systems / Components: smoke runner; session restore; recent items
- Current Behavior: helpers exist, but workspace continuity is not yet strongly enforced by smoke gates.
- Intended Release-Ready Behavior: workspace continuity scenarios are required release evidence.
- Missing Pieces: real smoke registration; mixed-artifact scenarios; restart validation.
- Technical Approach: add workspace-continuity smoke suites to the phase-readiness and release-signoff flow.
- Implementation Steps:
1. Register the core workspace continuity smoke scenarios.
2. Ensure they exercise mixed artifact types and panel restore.
3. Make failures block later signoff.
- Validation Steps:
1. Run the smoke suite under clean and dirty workspace states.
2. Confirm failure diagnostics identify the broken continuity step.
- Acceptance Criteria: workspace continuity smoke failures block release readiness.
- Dependencies: P01-T03; P10-T01; P10-T02
- Parallelization Notes: validation-focused and can run while later subsystem work continues.
- Risks / Failure Modes: smoke suite too shallow; false positives due to unstable setup.
- Observability / Diagnostics Notes: produce per-step continuity traces and failure snapshots.
- Rollback / Safety Notes: keep the suite deterministic and isolated from local machine noise.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`
