# Phase 10: Diagnostics Validation Harnesses And Consolidation

## Outcome

Lock the `v20` wave into a durable delivery path by instrumenting the critical workflows, adding regression harnesses, and retiring stale duplicate systems that would otherwise re-break the product.

## Improvement Count

210 atomic improvements across 6 execution tasks.

### P10-T01

- Phase ID: P10
- Task ID: P10-T01
- Task Title: Instrument full artifact lifecycle traces for file, notebook, and canvas workflows
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Make create/open/save/restore failures diagnosable instead of anecdotal.
- Why This Matters Now: Core product blockers currently hide behind thin logs and partial event chains.
- Problem Statement: The product lacks one end-to-end trace for artifact lifecycle execution.
- User Impact: Broken workflows are hard to reproduce and fix quickly.
- Scope: Create, mount, focus, dirty, save, autosave, rename, duplicate, restore, delete trace events.
- Out of Scope: External telemetry service rollout.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Related Systems / Components: Shell, persistence, restore, commands, diagnostics.
- Current Behavior: Important workflow transitions are distributed across logs with no unified trace shape.
- Intended Behavior: Engineers can inspect one lifecycle trace per artifact and per user action.
- Technical Approach: Add structured lifecycle events keyed by artifact ID and command source.
- Implementation Steps: Define trace schema; emit at each major transition; persist in debug logs; expose recent trace viewer if feasible.
- Validation Steps: Run create/save/restore workflows and inspect generated traces.
- Acceptance Criteria: Every primary artifact action can be reconstructed from diagnostic output.
- Dependencies: Phases 01 to 06.
- Risks / Failure Modes: Excessive logging in hot paths.
- UX Notes: Diagnostics should support supportability without leaking into normal UX.
- Styling / Highlighting Notes where relevant: Include style-mode and theme information in rendering-related traces when helpful.
- Observability / Diagnostics Notes: This task is the observability backbone for the rest of Phase 10.
- Rollback / Safety Notes: Gate verbose tracing behind debug or diagnostics settings.
- References / Context: Current broken creation flows prove that silent path divergence is too easy.
- Example scenarios where useful: A failed new notebook save can be traced from creation command to storage resolution to write failure.

### P10-T02

- Phase ID: P10
- Task ID: P10-T02
- Task Title: Build release-gating smoke paths for the top product workflows
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Turn the most important user journeys into mandatory pass/fail checks.
- Why This Matters Now: The product cannot regress on creation again.
- Problem Statement: Top workflows are broad, cross-surface, and easy to break with local changes.
- User Impact: Users lose trust fastest when the basics fail.
- Scope: New file, new notebook, new canvas, save, save as, reopen, restore, rename, duplicate, delete, command targeting, search and selection sanity.
- Out of Scope: Exhaustive deep feature coverage.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Smoke scripts, CI, shell, artifact services.
- Current Behavior: Core flows have evidently shipped broken.
- Intended Behavior: Release candidates fail fast if the top workflows break.
- Technical Approach: Encode realistic user journeys as smoke scripts plus artifact-state assertions.
- Implementation Steps: Define workflow matrix; add per-platform smoke scripts; integrate into CI presets; publish summary reports.
- Validation Steps: Run smoke suite on debug and release builds.
- Acceptance Criteria: Broken creation or restore workflows block release.
- Dependencies: All prior phases.
- Risks / Failure Modes: Smoke suite too slow or too shallow.
- UX Notes: Include keyboard-first paths, not only click paths.
- Styling / Highlighting Notes where relevant: Verify visible state changes during smoke runs where critical.
- Observability / Diagnostics Notes: Attach lifecycle traces to smoke failures automatically.
- Rollback / Safety Notes: Keep smoke scripts deterministic and temp-directory-scoped.
- References / Context: User explicitly identified file, notebook, and canvas creation as currently nonfunctional.
- Example scenarios where useful: Smoke run creates all three artifact types, edits them, saves them, restarts, and verifies restoration.

### P10-T03

- Phase ID: P10
- Task ID: P10-T03
- Task Title: Retire duplicate and stale workflow pathways after migration
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 35
- Objective: Remove the legacy branches that caused divergence in the first place.
- Why This Matters Now: Keeping old paths alive will reintroduce the same class of defects.
- Problem Statement: The repository contains duplicate create/open/save behaviors in widget-level code and thin adapters.
- User Impact: Dead or shadow paths increase unpredictability and maintenance burden.
- Scope: Remove fake untitled-path creation code, local canvas event-bus creation, notification-only notebook create paths, stale command aliases, obsolete icon residue.
- Out of Scope: Removal of unrelated legacy features not tied to workflow correctness.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Related Systems / Components: Shell, commands, canvas, notebooks, icons.
- Current Behavior: Duplicate paths remain active or easy to reactivate.
- Intended Behavior: One canonical implementation remains for each core workflow.
- Technical Approach: Add deprecation inventory, migrate callers, then remove dead code under release gating.
- Implementation Steps: Build residue list; remove direct widget creation logic; remove private bus creation; collapse notebook create shims; delete icon placeholders.
- Validation Steps: Grep/code-search and runtime diagnostics confirm no shadow paths remain.
- Acceptance Criteria: Core workflow duplication is materially reduced and traceably retired.
- Dependencies: Migration completion from Phases 01 to 09.
- Risks / Failure Modes: Deleting adapters too early breaks edge cases.
- UX Notes: Users should not see any behavioral difference except improved reliability.
- Styling / Highlighting Notes where relevant: Remove legacy icon and style residue along with code residue.
- Observability / Diagnostics Notes: Keep residue detectors in CI for one additional cycle.
- Rollback / Safety Notes: Remove in slices with easy revert points.
- References / Context: `MainFrame.cpp`, `TabBar.cpp`, and `CanvasWorkspacePanel.cpp` are confirmed duplicate-path hotspots.
- Example scenarios where useful: Search for `Untitled-` creation logic after migration returns no active production path.

### P10-T04

- Phase ID: P10
- Task ID: P10-T04
- Task Title: Build workflow-specific performance and responsiveness checks
- Priority: P1
- Category: Performance
- Atomic Improvements Covered: 35
- Objective: Ensure the new cohesive workflows remain fast enough to feel premium.
- Why This Matters Now: More correctness layers can accidentally add latency and repaint churn.
- Problem Statement: Creation, save, highlight, and restore work may become slower as orchestration improves.
- User Impact: Users will perceive sluggishness as unreliability.
- Scope: Create latency, open latency, save latency, restore latency, highlight latency, canvas frame cost, notebook mount cost.
- Out of Scope: Full benchmark lab coverage for every feature.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/benchmarks`; `/Users/ryanrentfro/code/markamp/src/core/AsyncHighlighter.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Benchmarks, highlighter, persistence, shell, canvas.
- Current Behavior: Responsiveness cost of integration work is not yet well measured.
- Intended Behavior: Core workflows meet explicit latency budgets and degrade gracefully.
- Technical Approach: Add timing probes and targeted benchmark scenarios for high-value workflows.
- Implementation Steps: Define budgets; benchmark create/open/save/restore; benchmark large-file highlighting and board mount; track regressions over time.
- Validation Steps: Run benchmark comparisons before and after major integration slices.
- Acceptance Criteria: Core workflows stay within acceptable latency budgets or surface explicit follow-up actions.
- Dependencies: Phases 01 to 09.
- Risks / Failure Modes: Benchmarks may ignore real UI-thread contention if scoped too narrowly.
- UX Notes: Responsiveness matters most on create/save/open and active editing.
- Styling / Highlighting Notes where relevant: Premium visuals should not cost disproportionate frame time.
- Observability / Diagnostics Notes: Include per-surface latency in diagnostics overlays where useful.
- Rollback / Safety Notes: Feature-flag expensive visual treatments if needed.
- References / Context: Large-file highlighting and canvas shell polish both carry responsiveness risk.
- Example scenarios where useful: Creating a new board and opening a large source file both remain visibly snappy.

### P10-T05

- Phase ID: P10
- Task ID: P10-T05
- Task Title: Publish an execution scoreboard and acceptance dashboard for the v20 wave
- Priority: P1
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Keep the implementation wave accountable to measurable outcomes.
- Why This Matters Now: The scope is large and can easily drift into local wins without product completion.
- Problem Statement: Without an execution scoreboard, the team can ship lots of work without actually finishing the IDE.
- User Impact: Product blockers may survive behind a large quantity of changes.
- Scope: Phase progress, blocker status, smoke status, restore status, syntax quality status, visual debt status, residue counts.
- Out of Scope: Public-facing roadmap tooling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v20_docs`; `/Users/ryanrentfro/code/markamp/scripts`; `/Users/ryanrentfro/code/markamp/tests/unit`
- Related Systems / Components: Docs, CI, smoke tests, diagnostics, design debt inventory.
- Current Behavior: Prior planning exists, but execution status can still become diffuse.
- Intended Behavior: The team can see whether MarkAmp is actually getting closer to finished-IDE quality.
- Technical Approach: Define scorecard metrics tied directly to v20 acceptance criteria and release gates.
- Implementation Steps: Create dashboard schema; wire CI outputs; track blocker burn-down; publish phase completion criteria and counts.
- Validation Steps: Populate dashboard from real smoke/test/diagnostic outputs.
- Acceptance Criteria: Product-completion status is visible in one place and tied to real evidence.
- Dependencies: P10-T01 through P10-T04.
- Risks / Failure Modes: Vanity metrics instead of product metrics.
- UX Notes: The dashboard should privilege blocker resolution over raw task completion.
- Styling / Highlighting Notes where relevant: Include syntax and visual quality gates as first-class scoreboard items.
- Observability / Diagnostics Notes: Dashboard consumes the traces and smoke results defined earlier in Phase 10.
- Rollback / Safety Notes: Keep the dashboard lightweight and automation-driven.
- References / Context: User requested an authoritative operating plan, not another vague strategy memo.
- Example scenarios where useful: The dashboard shows file creation green, notebook creation yellow, and canvas restore red with linked failing smoke traces.

### P10-T06

- Phase ID: P10
- Task ID: P10-T06
- Task Title: Finalize v20 exit criteria and handoff rules for post-v20 work
- Priority: P1
- Category: Cleanup / Consolidation
- Atomic Improvements Covered: 35
- Objective: Define what “finished enough to move on” actually means for the v20 wave.
- Why This Matters Now: The project has enough surface area that “mostly done” is too vague.
- Problem Statement: Without hard exit rules, the team may leave blockers behind and shift focus prematurely.
- User Impact: Core IDE expectations remain unmet even after a large implementation cycle.
- Scope: Exit criteria, unresolved-risk ledger, follow-on queue rules, blocker downgrade rules, release readiness.
- Out of Scope: Long-term future feature roadmap.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v20_docs/README__MarkAmp_V20_Execution_Master_Index.md`; `/Users/ryanrentfro/code/markamp/docs/v20_docs/ASSESSMENT__MarkAmp_V20_Master_Assessment.md`
- Related Systems / Components: Planning docs, CI, release process, risk management.
- Current Behavior: Prior waves improved planning quality, but the product still has unacceptable broken basics.
- Intended Behavior: v20 ends only when the product is materially closer to finished-IDE status by objective measures.
- Technical Approach: Convert acceptance criteria into release gates and unresolved-risk handling rules.
- Implementation Steps: Define must-pass criteria; define blocker downgrade bar; define follow-on-only categories; tie to smoke and diagnostics outputs.
- Validation Steps: Review release candidate against the exit grid before calling v20 complete.
- Acceptance Criteria: v20 cannot close while primary artifact creation, restore, or IDE-grade styling/highlighting remain broken.
- Dependencies: All of v20.
- Risks / Failure Modes: Exit criteria are written too softly to matter.
- UX Notes: Product completion should be judged by trustworthy daily workflows, not by raw subsystem count.
- Styling / Highlighting Notes where relevant: Visual and syntax quality remain release criteria, not optional polish.
- Observability / Diagnostics Notes: Exit review must consume evidence produced by the test and diagnostics systems.
- Rollback / Safety Notes: Keep unresolved issues visible rather than burying them in a future backlog.
- References / Context: The entire purpose of v20 is to finish core IDE workflows in a serious way.
- Example scenarios where useful: v20 cannot be marked complete if `Cmd+N` still fails for notebooks or if code cells still fall back to flat styling.
