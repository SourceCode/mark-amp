# Phase 18: Migration, Stub, And Dead Path Retirement

## Phase Intent

Retire transitional logic, duplicate pathways, and known placeholder code that still sits on the release path.

## Release-Ready Exit Criteria

- duplicate release-path ownership is removed,
- placeholder and stub residue on the release path is either finished or gated,
- migration seams no longer create ambiguity about which subsystem is authoritative.

## Task Count

3

## Task P18-T01

- Phase ID: P18
- Task ID: P18-T01
- Task Title: Remove duplicate release-path ownership between new governance models and legacy shell paths
- Priority: P0
- Category: Cleanup / De-duplication
- Objective: eliminate situations where both old direct behavior and new governance infrastructure remain active.
- Why This Matters Now: duplicated ownership is the main reason earlier completion work remains only partially effective.
- Release Gap Statement: new models exist, but legacy direct paths still survive in key shell workflows.
- User / Product Impact: duplicate ownership produces inconsistent behavior and late regressions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ActionManifest.cpp`
- Prior Plan References: `v20 Phase 01`; `v23 Phase 18`; `v24 Phase 18`
- Scope: artifact lifecycle, shell actions, panel ownership, settings host ownership.
- Out of Scope: hidden compatibility readers for legacy persisted data.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.cpp`
- Related Features / Systems / Components: migration seams; shell ownership; de-duplication
- Current Behavior: helper and audit layers exist in parallel with legacy direct logic.
- Intended Release-Ready Behavior: one authoritative release-path owner exists for each major workflow.
- Missing Pieces: final deletion of legacy paths; ownership audit; regression coverage.
- Technical Approach: identify release-path duplicates by workflow and remove the non-authoritative path once validated.
- Implementation Steps:
1. Build a duplicate-ownership ledger for release-path workflows.
2. Retire old paths after new-path validation.
3. Add tests ensuring only the authoritative path is used.
- Validation Steps:
1. Instrument and verify action and lifecycle routing.
2. Confirm no release-path workflow triggers both old and new code.
- Acceptance Criteria: duplicate ownership on release-path workflows is eliminated.
- Dependencies: P02-T01; P04-T03; P05-T03
- Parallelization Notes: can be split by workflow family.
- Risks / Failure Modes: deleting compatibility too early; hidden entry points still using old code.
- Release Notes / Cleanup Notes: record retired pathways and their replacements.
- Observability / Diagnostics Notes: use `WorkflowSmokeRunner` legacy-path tracking where possible.
- Rollback / Safety Notes: retire old writers first; keep read-only compatibility when needed.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/WorkflowSmokeRunner.cpp`

## Task P18-T02

- Phase ID: P18
- Task ID: P18-T02
- Task Title: Use completion-inventory and stub-auditor data to finish or gate remaining release-path placeholders
- Priority: P1
- Category: Placeholder / Stub Removal
- Objective: convert marker inventories into final release-path closure actions instead of leaving them as diagnostics.
- Why This Matters Now: the codebase already knows where many placeholders live.
- Release Gap Statement: inventory and auditor systems still identify work that has not been decisively finished or gated.
- User / Product Impact: users only care whether placeholder behavior still reaches them.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/CompletionInventory.h`; `/Users/ryanrentfro/code/markamp/src/core/ServiceStubCompletionAuditor.h`; `/Users/ryanrentfro/code/markamp/src/core/ActionReadinessGate.h`
- Prior Plan References: `v23 Phase 01`; `v23 Phase 17`; `v24 Phase 18`
- Scope: release-path markers in `src/core`, `src/ui`, `src/canvas`, `src/rendering`, and visible advanced domains.
- Out of Scope: non-release internal markers and low-value cosmetic comments.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CompletionInventory.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ServiceStubCompletionAuditor.cpp`
- Related Features / Systems / Components: inventories; stub auditors; release gates
- Current Behavior: inventories and auditors exist, but they are not yet the final closure mechanism for the release path.
- Intended Release-Ready Behavior: release-path placeholders are either eliminated or explicitly gated with no ambiguity.
- Missing Pieces: release-path filter; final blocker list; closure loop.
- Technical Approach: classify inventory items by release-path exposure and drive explicit close/gate decisions off that filtered set.
- Implementation Steps:
1. Re-scan release-path markers and classify by blocker severity.
2. Map each blocker to a finish, gate, or de-scope action.
3. Fail signoff on unresolved release-path blockers.
- Validation Steps:
1. Compare filtered inventory before and after closure work.
2. Confirm remaining markers are either non-release or explicitly gated.
- Acceptance Criteria: no critical release-path marker remains unresolved and ungated.
- Dependencies: P17-T01; P19-T01
- Parallelization Notes: can be run as a sidecar validation track throughout the wave.
- Risks / Failure Modes: noisy scans; false positives; stale classification.
- Observability / Diagnostics Notes: store filtered inventory reports with blocker counts.
- Rollback / Safety Notes: gating is acceptable where completion is not release-critical.
- References / Context: `docs/v23_docs/ASSESSMENT__MarkAmp_V23_Unfinished_Feature_And_Completion_Gap_Assessment.md`

## Task P18-T03

- Phase ID: P18
- Task ID: P18-T03
- Task Title: Finalize release-path labels, tooltips, and user-facing copy that still signals incompleteness
- Priority: P2
- Category: Release Readiness
- Objective: make remaining visible copy consistent with the actual supported product scope.
- Why This Matters Now: even after functional closure, stray copy can still signal unfinished software.
- Release Gap Statement: labels and placeholder text remain part of the release-path roughness problem.
- User / Product Impact: misleading or provisional copy makes the product feel less intentional.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarSkeletonPlaceholder.cpp`
- Prior Plan References: `v22 Phase 20`; `v24 Phase 18`
- Scope: tooltips, empty-state text, panel labels, disabled-state messaging for release-path surfaces.
- Out of Scope: marketing copy outside the app.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/NotificationManager.cpp`
- Related Features / Systems / Components: UI copy; release scope messaging; tooltips
- Current Behavior: some visible copy still reads as placeholder or temporary.
- Intended Release-Ready Behavior: user-facing text is clear, consistent, and aligned to supported behavior.
- Missing Pieces: copy sweep; gated-feature messaging; consistency checks.
- Technical Approach: perform a release-path copy audit after functional scope is fixed.
- Implementation Steps:
1. Inventory user-facing copy on release-critical surfaces.
2. Replace placeholder or ambiguous copy.
3. Align gated-feature messaging with actual support matrix.
- Validation Steps:
1. Manual shell sweep.
2. Confirm visible copy does not contradict release scope.
- Acceptance Criteria: release-path copy no longer signals accidental incompleteness.
- Dependencies: P07-T03; P17-T01
- Parallelization Notes: best done near the end after feature scope stabilizes.
- Risks / Failure Modes: stale copy after late scope changes.
- Release Notes / Cleanup Notes: include final wording changes with release candidate polish.
- Observability / Diagnostics Notes: not generally required beyond review notes.
- Rollback / Safety Notes: copy-only unless tied to gating.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
