# Phase 20: Final Placeholder Eradication Dead Path Removal And Release Gates

## Outcome

Finish the completion wave by removing remaining placeholder production paths, deleting obsolete dead code and transitional seams, and enforcing a release bar that treats unresolved unfinished work as a blocker.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P20-T01

- Phase ID: P20
- Task ID: P20-T01
- Task Title: Remove remaining production placeholder strings shell text and fake stand-ins
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Completion Tasks Covered: 60
- Objective: Eliminate the last user-visible and production-path placeholder residue after subsystem completion work lands.
- Why This Matters Now: A finished product cannot still expose “placeholder,” “stub,” or “for now” behavior in production pathways.
- Completion Gap Statement: The repo still contains visible placeholder text, phase-labeled stubs, and fake stand-ins in production-relevant files.
- User / Product Impact: Placeholder residue weakens product trust even after core behavior improves.
- Repository Evidence: [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp), [PDFViewerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp), [IconManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp)
- Scope: Placeholder strings, visible fake IDs, shell messages, question-mark fallback icons, phase-labeled stub headers, and UI stand-ins.
- Out of Scope: Legitimate loading or empty-state copy that is not placeholder debt.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/IconManager.cpp`
- Related Features / Systems / Components: Entire visible product and production-path strings.
- Current Behavior: Placeholder residue remains in several visible or production-relevant pathways.
- Intended Completed Behavior: Production surfaces and logs no longer reveal unfinished placeholder intent except in explicitly hidden experimental areas.
- Missing Pieces: Final audit sweep, replacement content, and removal policy.
- Technical Approach: Run a final first-party placeholder audit after implementation work and clear every production-facing hit.
- Implementation Steps: Inventory remaining placeholder text; replace or remove each item; confirm hidden/internal-only exceptions explicitly.
- Validation Steps: Search the final production source set and verify unresolved placeholder strings are zero or explicitly waived.
- Acceptance Criteria: No prominent production path still carries obvious unfinished placeholder text or stand-ins.
- Dependencies: All earlier subsystem phases.
- Risks / Failure Modes: Blind string cleanup can remove useful explicit unsupported-state messaging if not reviewed carefully.
- Cleanup / Migration Notes where relevant: Keep waivers only for truly internal or experimental paths.
- Observability / Diagnostics Notes where relevant: Publish the final placeholder audit with release artifacts.
- Rollback / Safety Notes: Preserve internal debug messaging separately from user-facing strings where needed.
- References / Context: This is the final honest-surface cleanup after substantive completion work.
- Example scenarios where useful: A production panel no longer includes comments or labels that describe it as a stub.

### P20-T02

- Phase ID: P20
- Task ID: P20-T02
- Task Title: Delete obsolete legacy paths duplicate implementations and transitional adapters
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Completion Tasks Covered: 60
- Objective: Remove dead and duplicate code that would otherwise reintroduce completion drift.
- Why This Matters Now: Keeping both legacy and new paths after migration preserves long-term instability.
- Completion Gap Statement: Many subsystem migrations leave behind obsolete fallback paths that continue to compile and confuse ownership.
- User / Product Impact: Indirect but important; maintenance cost and regression risk stay high.
- Repository Evidence: [Toolbar.cpp](/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp), [ToolbarModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp), [SettingsPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp), [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp)
- Scope: Duplicate service paths, legacy panel factories, direct widget callbacks superseded by manifests, old lifecycle adapters, temporary compatibility shims.
- Out of Scope: Intentionally retained compatibility shims with active rollout windows and explicit sunset dates.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`
- Related Features / Systems / Components: Shell, settings, toolbar, panels, artifacts, services.
- Current Behavior: Transitional duplicates still exist in several major subsystems.
- Intended Completed Behavior: Each completed subsystem has one authoritative implementation path.
- Missing Pieces: Deletion pass, dependency cleanup, and dead-code verification.
- Technical Approach: Use the completion inventory and migration reports to drive a focused legacy-removal wave.
- Implementation Steps: Identify obsolete paths; prove no live callers remain; delete code; simplify interfaces; rerun dead-code and smoke checks.
- Validation Steps: Run search, build, and test passes to confirm removed paths are truly dead.
- Acceptance Criteria: Major completed subsystems do not retain redundant legacy execution paths.
- Dependencies: Phase 18.
- Risks / Failure Modes: Premature deletion can strand low-traffic edge cases if caller audits were incomplete.
- Cleanup / Migration Notes where relevant: Record each removal in the final completion log.
- Observability / Diagnostics Notes where relevant: Pair dead-code reports with migration closures.
- Rollback / Safety Notes: Delete only after smoke coverage and usage logging show no live dependency.
- References / Context: Completion includes subtraction, not only addition.
- Example scenarios where useful: After settings migration, direct panel config writes are deleted rather than left dormant.

### P20-T03

- Phase ID: P20
- Task ID: P20-T03
- Task Title: Enforce final completion gates for controls panels services transports and tests
- Priority: P0
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Turn the `v23` audit into an enforceable release standard.
- Why This Matters Now: Without a final gate, broad progress can still end in a partly unfinished release.
- Completion Gap Statement: The repository has many audits and reports, but no single final completion gate spanning unfinished-code debt categories.
- User / Product Impact: Users can still receive a product that is broad but not finished.
- Repository Evidence: [ReleaseGateChecker.cpp](/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp), [ControlCompletenessMatrix.cpp](/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp), [PanelCapabilityModel.cpp](/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp)
- Scope: Control and panel readiness, service readiness, placeholder audits, migration closure, test scoreboards, unsupported-state policy.
- Out of Scope: Long-term product roadmap quality bars unrelated to current unfinished work.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`; `/Users/ryanrentfro/code/markamp/docs/v23_docs`
- Related Features / Systems / Components: Release readiness, audits, CI, completion program.
- Current Behavior: Gate fragments exist, but no final cross-category completion bar exists yet.
- Intended Completed Behavior: Release candidates fail if unfinished-code blocker categories remain unresolved.
- Missing Pieces: Unified gate spec, severity thresholds, waiver process, and artifact publishing.
- Technical Approach: Compose the audits, service readiness reports, and test scoreboards into one final completion gate.
- Implementation Steps: Define blocker thresholds; combine reports; wire into CI and release scripts; require explicit waivers and evidence for exceptions.
- Validation Steps: Leave a known blocker unresolved and verify the release gate fails.
- Acceptance Criteria: `v23` exit is governed by evidence, not optimism.
- Dependencies: All previous phases.
- Risks / Failure Modes: A vague or overly permissive gate will fail to finish the product; an overly broad gate may block harmless residue.
- Cleanup / Migration Notes where relevant: Replace older UI-only or control-only gates with the broader completion gate where appropriate.
- Observability / Diagnostics Notes where relevant: Archive gate inputs and blocker reports per release candidate.
- Rollback / Safety Notes: Allow only time-bounded, documented waivers for non-user-facing experimental debt.
- References / Context: This task turns the planning package into a release bar.
- Example scenarios where useful: Release is blocked because one transport stub and one visible panel placeholder remain unresolved.

### P20-T04

- Phase ID: P20
- Task ID: P20-T04
- Task Title: Publish the final completion scoreboard and unresolved-blocker register
- Priority: P1
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Give the team one concise operational view of what still prevents feature completeness.
- Why This Matters Now: Large completion programs fail when unresolved blockers are scattered across docs and logs.
- Completion Gap Statement: The repo still needs a final machine-readable and human-readable completion register tied to release candidates.
- User / Product Impact: Indirect but important; unresolved debt can otherwise be forgotten or misprioritized.
- Repository Evidence: [docs/v23_docs](/Users/ryanrentfro/code/markamp/docs/v23_docs), [scripts/run_tests.sh](/Users/ryanrentfro/code/markamp/scripts/run_tests.sh), [scripts/smoke_test.sh](/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh)
- Scope: Phase progress, blocker counts, unresolved waivers, service readiness, panel/control status, test coverage status.
- Out of Scope: External PM tooling.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v23_docs`; `/Users/ryanrentfro/code/markamp/scripts/run_tests.sh`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`
- Related Features / Systems / Components: Completion management, CI, release operations.
- Current Behavior: Completion evidence exists, but not yet as one final scoreboard/register.
- Intended Completed Behavior: Teams can inspect one register to see remaining blockers, waivers, and readiness by family.
- Missing Pieces: Scoreboard schema, data sources, and publishing path.
- Technical Approach: Aggregate outputs from audits, tests, service readiness, and placeholder scans into one final register.
- Implementation Steps: Define the register schema; collect inputs; publish with release artifacts; link each blocker to evidence and owning phase.
- Validation Steps: Add a known blocker and verify it appears in the final register with correct severity and ownership.
- Acceptance Criteria: There is one authoritative unresolved-blocker register for the completion wave.
- Dependencies: P20-T03.
- Risks / Failure Modes: If the register is stale or manual, it will quickly lose credibility.
- Cleanup / Migration Notes where relevant: Retire ad hoc completion spreadsheets once the register is live.
- Observability / Diagnostics Notes where relevant: Store the register as both human-readable markdown and machine-readable JSON.
- Rollback / Safety Notes: Keep register generation deterministic and read-only.
- References / Context: This is the operating console for the completion program.
- Example scenarios where useful: The register shows two blockers left: PDF annotations backend and real WebDAV list parsing.

### P20-T05

- Phase ID: P20
- Task ID: P20-T05
- Task Title: Run the final repository-wide unfinished-work sweep and certify feature-complete readiness
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Completion Tasks Covered: 60
- Objective: Ensure no significant unfinished-work seam has escaped the `v23` closure program.
- Why This Matters Now: Final certification requires a fresh sweep after all targeted completion work is done.
- Completion Gap Statement: The repo must be re-audited after implementation to prove that placeholder, stub, and unfinished-work debt is materially gone.
- User / Product Impact: Final trust depends on evidence that the product is truly finished, not only broadly improved.
- Repository Evidence: Entire first-party repo, especially [src](/Users/ryanrentfro/code/markamp/src), [tests](/Users/ryanrentfro/code/markamp/tests), and [docs/v23_docs](/Users/ryanrentfro/code/markamp/docs/v23_docs)
- Scope: Final marker scan, production-path review, gate verification, unresolved waiver review, and release recommendation.
- Out of Scope: New feature ideation beyond the audited scope.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src`; `/Users/ryanrentfro/code/markamp/tests`; `/Users/ryanrentfro/code/markamp/docs/v23_docs`; `/Users/ryanrentfro/code/markamp/scripts`
- Related Features / Systems / Components: Entire product and release process.
- Current Behavior: `v23` begins with an unfinished-work audit; it must end with a final certification sweep.
- Intended Completed Behavior: The team can state with evidence what unfinished work remains and whether it blocks feature-complete release.
- Missing Pieces: Final scan policy, evidence collation, and certification rubric.
- Technical Approach: Re-run the canonical unfinished-work inventory with the final blocker rules and compare against the opening baseline.
- Implementation Steps: Re-scan first-party code; diff against the original inventory; review waivers; rerun gates and smoke tests; publish certification results.
- Validation Steps: Ensure no previously known blocker is still unresolved without an explicit waiver.
- Acceptance Criteria: The team has a final evidence-backed statement of feature-complete readiness.
- Dependencies: P20-T01 through P20-T04 and all earlier phases.
- Risks / Failure Modes: Last-minute feature additions can reintroduce unfinished debt after the sweep if change control is weak.
- Cleanup / Migration Notes where relevant: Freeze new broad-scope additions until the sweep is complete.
- Observability / Diagnostics Notes where relevant: Store before/after audit deltas and final gate results as release artifacts.
- Rollback / Safety Notes: If blockers remain, fail release and keep the certification report as the new baseline.
- References / Context: This is the final closure step for the `v23` completion audit.
- Example scenarios where useful: The final scan shows no remaining production-path XOR crypto, stub Git hashes, or visible placeholder panels.
