# Phase 01: Release Spine, Done Criteria, And Gate Enforcement

## Phase Intent

Convert the new `v24` governance helpers from passive scaffolding into active release law.

## Release-Ready Exit Criteria

- subsystem done criteria exist for every release-path track,
- release gates fail on dead actions, placeholder panels, missing evidence, or blocked smoke suites,
- the team has one authoritative execution ledger and one signoff vocabulary for the rest of `v25`.

## Task Count

3

## Task P01-T01

- Phase ID: P01
- Task ID: P01-T01
- Task Title: Wire execution ledger and done-criteria ownership into the active release program
- Priority: P0
- Category: Release Readiness
- Objective: Make the `ExecutionLedger`, `SubsystemDoneCriteria`, and `DependencyGraphMapper` the canonical release-planning and signoff data model.
- Why This Matters Now: `v24` introduced these helpers, but they are not yet the enforced operating model for the release wave.
- Release Gap Statement: The repository can describe readiness, but it cannot yet prove that release-path work has been sequenced, owned, and signed off through one authoritative mechanism.
- User / Product Impact: Without one active release spine, blocker resolution drifts and regression evidence stays fragmented.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.h`; `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.h`; `/Users/ryanrentfro/code/markamp/src/core/DependencyGraphMapper.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p20_release_signoff.cpp`
- Prior Plan References: `v24 Phase 01`; `v24 Phase 20`
- Scope: define subsystem owners, required evidence, gate metadata, and dependency relationships for all `v25` phases.
- Out of Scope: implementing subsystem business logic.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DependencyGraphMapper.cpp`
- Related Features / Systems / Components: governance; validation; release signoff; dependency tracking
- Current Behavior: helper classes exist and tests validate them structurally, but they are not yet treated as authoritative release machinery.
- Intended Release-Ready Behavior: every release-path stream has explicit tasks, dependencies, evidence links, required smoke suites, and signoff status.
- Missing Pieces: active population; subsystem ownership map; evidence conventions; gate consumers.
- Technical Approach: create a single release ledger bootstrap path and a subsystem signoff registry consumed by smoke and gate reporting.
- Implementation Steps:
1. Define the `v25` subsystem map and required criteria set.
2. Seed ledger entries for all release-path tasks and dependencies.
3. Add export paths for human-readable and CI-consumable signoff reports.
- Validation Steps:
1. Add tests for dependency closure and missing-evidence failure cases.
2. Produce a sample release report and ensure blocked subsystems are surfaced clearly.
- Acceptance Criteria: missing required evidence blocks signoff; dependency graphs are complete; every `v25` subsystem appears in the registry.
- Dependencies: none
- Parallelization Notes: can run in parallel with evidence collection, but must land before later gate-enforcement work.
- Risks / Failure Modes: over-modeling without adoption; incomplete subsystem map; stale reports.
- Observability / Diagnostics Notes: export ledger and signoff reports in Markdown and JSON.
- Rollback / Safety Notes: keep reports additive until all downstream consumers are wired.
- References / Context: `docs/v24_docs/Phase_01__Program_Spine_Backlog_Deduplication_And_Definition_Of_Finished.md`

## Task P01-T02

- Phase ID: P01
- Task ID: P01-T02
- Task Title: Enforce release gates for dead actions, placeholder panels, and missing shell adoption
- Priority: P0
- Category: Release Blocker
- Objective: Turn action and panel audit helpers into blocking gates for release-path surfaces.
- Why This Matters Now: the repository already diagnoses dead or placeholder UI, but that diagnosis is still optional.
- Release Gap Statement: visible controls and panels can remain misleading even while audit infrastructure exists.
- User / Product Impact: users encounter no-op commands, thin panels, and inconsistent enablement in visible shell flows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.h`; `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PanelLifecycleAuditor.cpp`
- Prior Plan References: `v21 Phase 01`; `v23 Phase 02`; `v24 Phase 04`; `v24 Phase 05`
- Scope: release-path menus, toolbar, context menus, primary panels, bottom panels, settings host, notebook actions, canvas actions.
- Out of Scope: hidden or explicitly gated future features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.cpp`; `/Users/ryanrentfro/code/markamp/src/core/MenuCommandBinder.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: command routing; panel registry; shell controls; release gate
- Current Behavior: auditors can report dead or placeholder states, but release-path surfaces can still ship that way.
- Intended Release-Ready Behavior: release reports fail until every visible release-path action and panel is either real or explicitly gated out of release scope.
- Missing Pieces: gate wiring; release-path allowlist; CI or smoke integration; panel placeholder detection on live shells.
- Technical Approach: centralize manifest/audit aggregation and fail on dead, stub, orphaned, or placeholder records outside an explicit gated-scope list.
- Implementation Steps:
1. Define the release-path action and panel inventory.
2. Feed live shell registrations into a unified readiness gate.
3. Fail the release report on ungated blockers.
- Validation Steps:
1. Add tests covering dead action, orphaned action, and placeholder panel failure.
2. Run the gate against the real shell registration path.
- Acceptance Criteria: gate produces blocking output for live issues and passes only when release-path controls are real or gated.
- Dependencies: P01-T01
- Parallelization Notes: can proceed alongside panel and command cleanup work.
- Risks / Failure Modes: false positives on intentionally hidden features; incomplete inventory coverage.
- Release Notes / Cleanup Notes: create a documented gated-scope list rather than relying on silent omission.
- Observability / Diagnostics Notes: emit actionable blocker IDs and source file references.
- Rollback / Safety Notes: introduce warning mode briefly, then promote to hard fail.
- References / Context: `docs/v23_docs/Phase_02__Action_Control_Menu_And_Panel_Readiness_Closure.md`

## Task P01-T03

- Phase ID: P01
- Task ID: P01-T03
- Task Title: Define release-ready subsystem done criteria and evidence templates
- Priority: P0
- Category: Release Readiness
- Objective: make each major subsystem’s definition of done explicit, measurable, and reusable by AI coding agents.
- Why This Matters Now: `v25` must reduce ambiguity, not create another planning layer.
- Release Gap Statement: prior waves describe the problem well, but the exact signoff proof for each subsystem still needs tighter operational definition.
- User / Product Impact: without crisp done criteria, work will continue to optimize for local fixes instead of trustworthy completion.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p20_release_signoff.cpp`
- Prior Plan References: `v24 Phase 20`
- Scope: artifact lifecycle; shell actions; panels; editor; notebook; canvas; settings; source control; cloud/security; validation.
- Out of Scope: deep implementation detail for non-release-critical future features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.h`
- Related Features / Systems / Components: signoff; smoke tests; validation; documentation
- Current Behavior: subsystem readiness concepts exist, but evidence requirements are not yet standardized.
- Intended Release-Ready Behavior: each subsystem has required smoke paths, required tests, required recovery checks, and mandatory evidence outputs.
- Missing Pieces: template format; required evidence taxonomy; owner mapping.
- Technical Approach: define per-subsystem criteria documents and machine-readable identifiers consumed by the ledger and release gate.
- Implementation Steps:
1. Define the evidence schema.
2. Add subsystem criteria bootstrap data.
3. Publish signoff templates for implementation agents.
- Validation Steps:
1. Verify every `v25` phase maps to at least one subsystem criterion.
2. Verify blocked subsystems report unmet criteria precisely.
- Acceptance Criteria: no release-path subsystem lacks explicit required criteria or evidence expectations.
- Dependencies: P01-T01
- Parallelization Notes: can proceed while later subsystem work starts.
- Risks / Failure Modes: criteria too vague; criteria too broad; evidence sprawl.
- Observability / Diagnostics Notes: output unmet criteria as stable IDs.
- Rollback / Safety Notes: keep criteria data additive and versioned.
- References / Context: `docs/v24_docs/README__MarkAmp_V24_Execution_Synthesis_Master_Index.md`
