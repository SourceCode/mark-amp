# Phase 01 - Program Spine, Backlog De-duplication, And Definition Of Finished

## Phase Goal

Create the single execution ledger that translates `v18` through `v23` into one current backlog, one owner map, one dependency map, and one set of subsystem completion criteria.

## Measurable Outcome

- One de-duplicated execution ledger exists.
- Every major subsystem has a definition of finished.
- Every earlier-plan item is either merged, deferred, retired, or preserved as an active `v24` task.

### Task P01-T01

- Phase ID: `P01`
- Task ID: `P01-T01`
- Task Title: Build the canonical completion ledger from `v18` through `v23`
- Priority: `P0`
- Category: `Cleanup / De-duplication`
- Objective: Consolidate earlier planning outputs into one implementation ledger used by all later phases.
- Why This Matters Now: The product has more planning volume than execution clarity.
- Execution Gap Statement: Earlier plans duplicate ownership across artifact, shell, UI, and completion workstreams.
- User / Product Impact: Without one ledger, implementation teams will keep fixing the same seams from different angles.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/docs/v18_docs`, `/Users/ryanrentfro/code/markamp/docs/v19_docs`, `/Users/ryanrentfro/code/markamp/docs/v20_docs`, `/Users/ryanrentfro/code/markamp/docs/v21_docs`, `/Users/ryanrentfro/code/markamp/docs/v22_docs`, `/Users/ryanrentfro/code/markamp/docs/v23_docs`
- Prior Plan References: `v18` master index, `v19` master index, `v20` assessment, `v21` assessment, `v22` assessment, `v23` master index
- Scope: Inventory, merge, status tagging, dependency tagging, obsolete-task retirement
- Out of Scope: Implementing product code
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v24_docs/README__MarkAmp_V24_Execution_Synthesis_Master_Index.md`
- Related Features / Systems / Components: Planning corpus, subsystem ownership, program tracking
- Current Behavior: Planning exists in multiple generations with overlap.
- Intended Completed Behavior: One authoritative execution ledger drives all subsequent implementation work.
- Missing Pieces: Merge policy, dedupe rules, subsystem mapping, blocker tagging
- Technical Approach: Build a normalized matrix keyed by subsystem, workflow, and dependency chain.
- Implementation Steps: Parse prior docs; merge duplicates; mark obsolete tasks; map blockers; publish ledger.
- Validation Steps: Spot-check every major subsystem against at least one retained task and one retired duplicate.
- Acceptance Criteria: No major completion theme remains duplicated across multiple active `v24` phases without rationale.
- Dependencies: None
- Parallelization Notes: Can run immediately and should finish before other phase execution begins.
- Risks / Failure Modes: False merges can hide real work; over-merging can remove necessary subsystem nuance.
- Cleanup / Migration Notes: Retire earlier task groupings that now belong to one `v24` workstream.
- Observability / Diagnostics Notes: Track counts for merged, retired, deferred, and active items.
- Rollback / Safety Notes: Preserve source-doc references for every merged item.
- References / Context: `v18` through `v23` planning packages
- Example scenarios where useful: A notebook save task and a generic artifact save task become one shared persistence task plus one notebook-specific follow-on task.

### Task P01-T02

- Phase ID: `P01`
- Task ID: `P01-T02`
- Task Title: Define subsystem-level "done" criteria before more implementation starts
- Priority: `P0`
- Category: `Release Readiness`
- Objective: Turn vague completion language into subsystem-specific signoff criteria.
- Why This Matters Now: The codebase has many features that look broad but still are not finishable because "done" is undefined.
- Execution Gap Statement: Audit helpers and prior plans describe problems, but most subsystems still lack explicit completion gates.
- User / Product Impact: Teams can ship surfaces that look present while still being incomplete.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`
- Prior Plan References: `v21` assessment, `v23` assessment
- Scope: Artifact, shell, editor, notebook, canvas, search, Git, settings, sync, panel, rendering, validation subsystems
- Out of Scope: Feature ideation outside current product scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v24_docs/ASSESSMENT__MarkAmp_V24_Execution_Synthesis_And_Completion_Assessment.md`
- Related Features / Systems / Components: Release criteria, subsystem signoff
- Current Behavior: Subsystems compile and render but often stop short of trustworthy workflows.
- Intended Completed Behavior: Each subsystem has a checklist for workflow integrity, persistence, errors, and regression gates.
- Missing Pieces: Criteria templates, signoff requirements, validation mapping
- Technical Approach: Define subsystem done criteria as a matrix of workflow, correctness, UX integrity, and regression gates.
- Implementation Steps: Enumerate subsystems; define entry points; define must-pass workflows; attach required diagnostics/tests.
- Validation Steps: Review criteria against current code evidence and active `v24` tasks.
- Acceptance Criteria: Every phase references at least one definition-of-finished rule for the subsystem it advances.
- Dependencies: `P01-T01`
- Parallelization Notes: Can overlap lightly with phase planning, but should finalize before implementation execution.
- Risks / Failure Modes: Criteria that are too weak will permit pseudo-completion; too strict without sequencing can stall delivery.
- Cleanup / Migration Notes: Replace vague "finish feature" language from earlier plans with explicit criteria.
- Observability / Diagnostics Notes: Record pass/fail status by subsystem.
- Rollback / Safety Notes: Keep criteria versioned and reviewable.
- References / Context: `v20` creation blockers, `v21` control trust blockers, `v23` unfinished-work audit
- Example scenarios where useful: Search is not done until keyword, phrase, regex, navigation, and search panel flows all pass smoke and restart tests.

### Task P01-T03

- Phase ID: `P01`
- Task ID: `P01-T03`
- Task Title: Publish a dependency map for all remaining blockers
- Priority: `P0`
- Category: `Architecture Consolidation`
- Objective: Make prerequisite chains explicit so teams stop implementing polish before ownership is stable.
- Why This Matters Now: Many earlier tasks were correct but sequenced too loosely.
- Execution Gap Statement: Workstreams like UI polish, notebook shell work, and panel completion depend on unresolved lifecycle and command authority.
- User / Product Impact: Poor sequencing creates churn, rewrites, and user-visible regressions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`
- Prior Plan References: `v19` execution spine, `v20` artifact creation, `v22` UI quality
- Scope: Dependency graph for lifecycle, commands, panels, settings, notebook, canvas, search, sync, validation
- Out of Scope: Detailed sprint scheduling
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v24_docs/README__MarkAmp_V24_Execution_Synthesis_Master_Index.md`
- Related Features / Systems / Components: Program sequencing, subsystem dependencies
- Current Behavior: Dependencies are implied but not formalized.
- Intended Completed Behavior: Each active task identifies prerequisites and safe parallel tracks.
- Missing Pieces: Dependency graph, blocker labels, parallelization rules
- Technical Approach: Model dependencies by required authority layers: artifact, command, persistence, panel, service, UI, validation.
- Implementation Steps: Map task prerequisites; mark critical path; mark parallelizable streams; publish ordering rules.
- Validation Steps: Confirm later-phase tasks no longer depend on unresolved earlier-phase foundations.
- Acceptance Criteria: Every phase has clear critical-path and parallelization notes.
- Dependencies: `P01-T01`, `P01-T02`
- Parallelization Notes: Must precede large-scale parallel implementation.
- Risks / Failure Modes: Hidden dependencies can still surface if repository seams are missed.
- Cleanup / Migration Notes: Retire plan items whose only value was sequencing explanation.
- Observability / Diagnostics Notes: Maintain a blocker dashboard by phase and subsystem.
- Rollback / Safety Notes: Recompute dependency map when foundational ownership changes.
- References / Context: All prior planning generations
- Example scenarios where useful: Notebook toolbar polish cannot precede notebook artifact save/restore ownership.

### Task P01-T04

- Phase ID: `P01`
- Task ID: `P01-T04`
- Task Title: Convert audit helpers into explicit implementation and release gates
- Priority: `P0`
- Category: `Diagnostics / Recovery`
- Objective: Make existing audit models actionable and enforceable.
- Why This Matters Now: The repository already knows where some incomplete controls and panels are.
- Execution Gap Statement: Audit helpers diagnose missing work, but product code is still allowed to ship around them.
- User / Product Impact: Dead controls and placeholder panels remain possible in production builds.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`, `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`, `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Prior Plan References: `v21` control audit, `v23` completion audit
- Scope: Audit outputs, blocking rules, CI integration, release signoff usage
- Out of Scope: Building new unrelated audit frameworks
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_control_audit.cpp`, `/Users/ryanrentfro/code/markamp/tests/unit/test_v23_action_readiness.cpp`
- Related Features / Systems / Components: Action readiness, panel readiness, release gates
- Current Behavior: Audits exist, but failures do not yet clearly block release.
- Intended Completed Behavior: Missing handlers, placeholder panels, and stub actions fail readiness gates.
- Missing Pieces: Severity rules, allowlist policy, CI wiring
- Technical Approach: Promote audit outputs to structured gate results with subsystem exemptions only by explicit policy.
- Implementation Steps: Define blocker rules; wire tests; add reports; require gate pass for release.
- Validation Steps: Seed known broken control/panel states and confirm gate failure.
- Acceptance Criteria: Placeholder or stub production surfaces cannot pass release readiness.
- Dependencies: `P01-T02`
- Parallelization Notes: Can begin early and mature across later phases.
- Risks / Failure Modes: Overly strict gates can block legitimate staged development unless exemptions are well-defined.
- Cleanup / Migration Notes: Remove soft-fail-only audit pathways.
- Observability / Diagnostics Notes: Emit per-surface readiness summaries.
- Rollback / Safety Notes: Support temporary documented waivers with expiration.
- References / Context: Existing V21/V23 audit tests
- Example scenarios where useful: A menu item without handler blocks the control-integrity gate immediately.

### Task P01-T05

- Phase ID: `P01`
- Task ID: `P01-T05`
- Task Title: Establish phase-level execution bundles and ownership boundaries
- Priority: `P1`
- Category: `Core Workflow Completion`
- Objective: Translate the `v24` phases into clear implementation bundles with low cross-conflict risk.
- Why This Matters Now: AI coding agents need bounded slices with stable ownership.
- Execution Gap Statement: Earlier plans contained valid work, but not all tasks were grouped for low-conflict parallel execution.
- User / Product Impact: Better bundling reduces churn and shortens time to a stable product.
- Repository Evidence: Cross-cutting ownership in `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/core/Config.h`
- Prior Plan References: `v19` execution phases, `v23` completion phases
- Scope: Phase ownership map, write-scope guidance, parallel execution constraints
- Out of Scope: Team staffing or personnel assignment
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v24_docs/README__MarkAmp_V24_Execution_Synthesis_Master_Index.md`
- Related Features / Systems / Components: Execution model, task ownership
- Current Behavior: Some workstreams overlap heavily in files and responsibilities.
- Intended Completed Behavior: Each phase exposes a practical write scope and handoff boundary.
- Missing Pieces: Ownership rules, conflict notes, bundle structure
- Technical Approach: Bundle by authority layer and shared file surfaces rather than by vague product theme.
- Implementation Steps: Map write scopes; cluster tasks; identify shared files; publish conflict warnings.
- Validation Steps: Review each phase for isolated ownership where possible and explicit conflict notes where not.
- Acceptance Criteria: Each `v24` phase can be assigned independently without hidden ownership ambiguity.
- Dependencies: `P01-T01`, `P01-T03`
- Parallelization Notes: Complements the dependency map and should finalize before heavy parallel implementation.
- Risks / Failure Modes: Over-isolation can create artificial boundaries; under-isolation causes merge contention.
- Cleanup / Migration Notes: Retire prior phase groupings that no longer reflect execution reality.
- Observability / Diagnostics Notes: Track cross-phase file contention hot spots.
- Rollback / Safety Notes: Re-cluster ownership bundles if foundational files are refactored.
- References / Context: Prior phase packages and current repository hotspots
- Example scenarios where useful: Canvas runtime work owns `src/canvas/*`, while shell integration owns `src/ui/CanvasWorkspacePanel.cpp` and related bridge code.
