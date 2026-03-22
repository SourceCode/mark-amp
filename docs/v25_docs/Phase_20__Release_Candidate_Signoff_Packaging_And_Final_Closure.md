# Phase 20: Release Candidate Signoff, Packaging, And Final Closure

## Phase Intent

Execute the final release-candidate closure loop using the evidence produced by the earlier phases.

## Release-Ready Exit Criteria

- each in-scope subsystem is explicitly signed off or explicitly gated,
- packaging and release-facing metadata match the real supported scope,
- final blocker review leaves no unresolved release-path gaps.

## Task Count

3

## Task P20-T01

- Phase ID: P20
- Task ID: P20-T01
- Task Title: Run subsystem signoff using done criteria, smoke evidence, and blocker reports
- Priority: P0
- Category: Release Readiness
- Objective: produce the final per-subsystem release verdict from real evidence, not aspiration.
- Why This Matters Now: this is the point where `v25` converts into an actual release-candidate decision.
- Release Gap Statement: earlier planning waves described what to do; this phase proves whether it is done.
- User / Product Impact: a credible release candidate requires explicit subsystem signoff and transparent blockers.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v24_p20_release_signoff.cpp`
- Prior Plan References: `v24 Phase 20`
- Scope: all `v25` release-path subsystems and their required evidence.
- Out of Scope: future backlog generation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.cpp`
- Related Features / Systems / Components: signoff; release gates; subsystem criteria
- Current Behavior: signoff structures exist but still need full evidence integration.
- Intended Release-Ready Behavior: every subsystem receives a green, gated, or blocked verdict with explicit reasons.
- Missing Pieces: final evidence hookup; signoff runbook; blocker resolution loop.
- Technical Approach: execute signoff from the integrated validation dashboard and subsystem criteria registry.
- Implementation Steps:
1. Run all required smoke and validation suites.
2. Aggregate blocker and readiness reports.
3. Assign explicit verdicts to each subsystem.
- Validation Steps:
1. Verify a known blocker prevents signoff.
2. Verify a green subsystem reports full required evidence.
- Acceptance Criteria: there is a final subsystem signoff report with no ambiguous release-path status.
- Dependencies: P19-T01; P19-T02; P19-T03
- Parallelization Notes: final aggregation task with low implementation conflict.
- Risks / Failure Modes: missing evidence links; hidden blockers outside the report.
- Observability / Diagnostics Notes: preserve the final signoff report as a release artifact.
- Rollback / Safety Notes: do not issue release-candidate approval on partial evidence.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/SubsystemDoneCriteria.h`

## Task P20-T02

- Phase ID: P20
- Task ID: P20-T02
- Task Title: Align packaging, versioning, defaults, and release-facing scope with actual product readiness
- Priority: P1
- Category: Release Readiness
- Objective: ensure release metadata and packaging reflect the final supported scope and defaults.
- Why This Matters Now: final packaging cannot overstate feature support or preserve temporary defaults.
- Release Gap Statement: a release candidate can still be undermined by mismatched defaults, scope, or packaging assumptions.
- User / Product Impact: users judge the product partly by the stability and coherence of packaged defaults and supported features.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`; `/Users/ryanrentfro/code/markamp/vcpkg.json`; `/Users/ryanrentfro/code/markamp/packaging/macos/Info.plist`
- Prior Plan References: `v24 Phase 20`
- Scope: versioning, packaging metadata, default settings, gated feature visibility, release notes scope.
- Out of Scope: post-release roadmap planning.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/packaging/windows/AppxManifest.xml`; `/Users/ryanrentfro/code/markamp/packaging/linux/markamp.desktop`; `/Users/ryanrentfro/code/markamp/scripts/package_macos.sh`
- Related Features / Systems / Components: packaging; defaults; versioning; gated scope
- Current Behavior: broad product ambition and visible surfaces still require final scope alignment.
- Intended Release-Ready Behavior: packaged builds and defaults represent the real release candidate honestly.
- Missing Pieces: final default audit; gated-feature packaging pass; version metadata review.
- Technical Approach: review packaging and default settings only after final supported scope is fixed by signoff.
- Implementation Steps:
1. Audit defaults and packaged metadata against final scope.
2. Remove or hide gated features from release packaging where needed.
3. Verify version metadata and release notes inputs.
- Validation Steps:
1. Inspect packaged configuration and visible startup defaults.
2. Confirm gated features do not reappear through packaging or defaults.
- Acceptance Criteria: packaged release artifacts match the real supported product scope.
- Dependencies: P17-T01; P18-T03; P20-T01
- Parallelization Notes: can run late and mostly independently.
- Risks / Failure Modes: packaging re-exposes gated UI; stale defaults.
- Release Notes / Cleanup Notes: include explicit supported-scope notes in release artifacts.
- Observability / Diagnostics Notes: record package validation checklist output.
- Rollback / Safety Notes: prefer conservative defaults and hidden features.
- References / Context: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

## Task P20-T03

- Phase ID: P20
- Task ID: P20-T03
- Task Title: Execute final blocker review and close the v25 release-hardening wave
- Priority: P0
- Category: Release Readiness
- Objective: produce the final authoritative list of what is done, what is gated, and what still blocks release.
- Why This Matters Now: `v25` must end with closure logic, not another backlog explosion.
- Release Gap Statement: without a final blocker review, the wave would still end in ambiguity.
- User / Product Impact: a serious release candidate requires an explicit closure decision and residual-risk statement.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/docs/v25_docs/ASSESSMENT__MarkAmp_V25_Release_Readiness_Hardening_And_Closure_Assessment.md`; `/Users/ryanrentfro/code/markamp/src/core/ExecutionLedger.h`
- Prior Plan References: `v24 Phase 20`
- Scope: residual blockers, gated scope, deferred scope, release-candidate recommendation.
- Out of Scope: new implementation wave design beyond recording residual deferred items.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v25_docs/README__MarkAmp_V25_Release_Readiness_Master_Index.md`
- Related Features / Systems / Components: signoff; release closure; blocker review
- Current Behavior: prior waves ended in planning language; `v25` must end in explicit release-hardening closure.
- Intended Release-Ready Behavior: the team can answer whether MarkAmp is ready for a release candidate and why.
- Missing Pieces: final blocker review artifact; residual risk statement; gated/deferred inventory.
- Technical Approach: generate a closure report from the final signoff run and classify remaining items only as gated, deferred, or release blockers.
- Implementation Steps:
1. Review final signoff output and unresolved blockers.
2. Classify residual items as gate, defer, or must-fix.
3. Publish the final closure report for the wave.
- Validation Steps:
1. Confirm every unresolved item has one final classification.
2. Confirm no hidden release-path blocker remains outside the closure report.
- Acceptance Criteria: `v25` ends with an authoritative closure artifact and explicit release recommendation.
- Dependencies: P20-T01; P20-T02
- Parallelization Notes: final synthesis step.
- Risks / Failure Modes: silent residual blockers; disguised future backlog expansion.
- Release Notes / Cleanup Notes: closure report should separate must-fix blockers from gated or deferred residual scope.
- Observability / Diagnostics Notes: preserve the final closure report with stable blocker IDs.
- Rollback / Safety Notes: if blockers remain, do not label the build release-candidate ready.
- References / Context: `docs/v24_docs/README__MarkAmp_V24_Execution_Synthesis_Master_Index.md`
