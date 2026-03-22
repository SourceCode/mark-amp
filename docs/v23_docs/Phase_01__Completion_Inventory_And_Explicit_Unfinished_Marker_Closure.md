# Phase 01: Completion Inventory And Explicit Unfinished Marker Closure

## Outcome

Create one authoritative inventory of explicit unfinished work and turn marker-driven completion debt into a governed execution backlog instead of scattered comments, test notes, and ad hoc audits.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P01-T01

- Phase ID: P01
- Task ID: P01-T01
- Task Title: Build the canonical unfinished-work inventory from first-party source and test evidence
- Priority: P0
- Category: TODO / FIXME Audit
- Atomic Completion Tasks Covered: 60
- Objective: Create one repository-backed source of truth for unfinished work markers and known stub states.
- Why This Matters Now: The repo already contains enough explicit signals to drive a full completion program without guessing.
- Completion Gap Statement: Unfinished work is currently distributed across comments, tests, and phase-labeled stubs with no canonical closure inventory.
- User / Product Impact: Product completion remains ambiguous and easy to mis-sequence.
- Repository Evidence: Marker-rich clusters appear in [SearchService.cpp](/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp), [KernelManager.cpp](/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp), [GitService.cpp](/Users/ryanrentfro/code/markamp/src/core/GitService.cpp), [CloudSyncService.cpp](/Users/ryanrentfro/code/markamp/src/core/CloudSyncService.cpp), and [tests/CMakeLists.txt](/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt).
- Scope: First-party `src/`, `tests/`, `resources/`, build files, and prior planning docs.
- Out of Scope: Third-party `node_modules` or external vendor code.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src`; `/Users/ryanrentfro/code/markamp/tests`; `/Users/ryanrentfro/code/markamp/docs/v18_docs`; `/Users/ryanrentfro/code/markamp/docs/v22_docs`
- Related Features / Systems / Components: Entire repository.
- Current Behavior: Marker discovery is ad hoc and noisy.
- Intended Completed Behavior: One normalized unfinished-work inventory drives sequencing and closure tracking.
- Missing Pieces: Marker taxonomy, evidence normalization, ownership tags, and closure criteria.
- Technical Approach: Build a classifier for explicit markers plus curated exclusions for benign “disabled” or “pending” terminology.
- Implementation Steps: Scan first-party files; classify findings; de-duplicate repeated comments; tag by subsystem; link to corresponding workflows and tests.
- Validation Steps: Re-run the scan after curation and confirm known high-risk stubs are retained while third-party noise is excluded.
- Acceptance Criteria: The team can sort unfinished work by subsystem, severity, and production impact from one inventory.
- Dependencies: None.
- Risks / Failure Modes: Overly broad scanning can drown the inventory in irrelevant terminology.
- Cleanup / Migration Notes where relevant: Retire one-off unfinished-work spreadsheets once the canonical inventory exists.
- Observability / Diagnostics Notes where relevant: Emit machine-readable inventory snapshots for CI and release review.
- Rollback / Safety Notes: Keep inventory generation read-only until remediation begins.
- References / Context: This phase is the evidence spine for all later `v23` work.
- Example scenarios where useful: A stub in cloud sync and a placeholder in panels appear in one ranked backlog instead of two unrelated note systems.

### P01-T02

- Phase ID: P01
- Task ID: P01-T02
- Task Title: Classify explicit comments and phase-labeled stubs into actionable completion families
- Priority: P0
- Category: TODO / FIXME Audit
- Atomic Completion Tasks Covered: 60
- Objective: Convert raw marker hits into categories that map directly to implementation waves.
- Why This Matters Now: A raw grep dump is not executable.
- Completion Gap Statement: The repo mixes TODO-style comments, “for now” fallbacks, phase-labeled stub files, and audit-model placeholder states without a shared completion vocabulary.
- User / Product Impact: High-severity stubs compete with cosmetic placeholders for attention.
- Repository Evidence: [HistoryPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp), [PDFViewerPanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp), [ThemeMarketplace.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp), [BuildLogManager.cpp](/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp)
- Scope: Classification into service stubs, workflow partials, panel shells, placeholder rendering, migration seams, and test-gap items.
- Out of Scope: Designing final implementations inside this classification task.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/HistoryPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/PDFViewerPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp`; `/Users/ryanrentfro/code/markamp/src/core/BuildLogManager.cpp`
- Related Features / Systems / Components: Panels, services, infrastructure, release blockers.
- Current Behavior: Similar unfinished patterns are described differently across subsystems.
- Intended Completed Behavior: Every unfinished seam is tagged into a category that implies next action and validation strategy.
- Missing Pieces: Shared taxonomy and severity model.
- Technical Approach: Define completion families and assign every explicit marker to one primary family plus optional secondary tags.
- Implementation Steps: Create category rules; map known files; tag production-path versus test-only debt; record migration and cleanup status.
- Validation Steps: Sample files from each category and verify that their assigned remediation path is plausible.
- Acceptance Criteria: Each explicit unfinished item lands in a category that suggests a concrete implementation lane.
- Dependencies: P01-T01.
- Risks / Failure Modes: Over-classification can hide relationships between cross-cutting gaps.
- Cleanup / Migration Notes where relevant: Replace legacy “phase” naming in comments with completion-category tags when files are touched.
- Observability / Diagnostics Notes where relevant: Include category counts and production-path counts in inventory output.
- Rollback / Safety Notes: Preserve raw marker evidence alongside the normalized category record.
- References / Context: Prior plans identified broad gaps; this task turns explicit code evidence into a closure map.
- Example scenarios where useful: A panel stub and a transport stub do not end up in the same vague “unfinished” bucket.

### P01-T03

- Phase ID: P01
- Task ID: P01-T03
- Task Title: Link explicit unfinished markers to the actual feature and workflow chains they block
- Priority: P1
- Category: Migration Completion
- Atomic Completion Tasks Covered: 60
- Objective: Prevent comment-level cleanup from being mistaken for real completion.
- Why This Matters Now: Many markers point to deeper workflow breaks rather than isolated code cleanup.
- Completion Gap Statement: The repo contains evidence markers, but many of them sit above broken command, persistence, or transport chains.
- User / Product Impact: Superficial cleanup can leave the underlying feature still incomplete.
- Repository Evidence: [SearchService.cpp](/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp), [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp), [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp), [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp)
- Scope: Map marker -> affected feature -> command/UI chain -> persistence/transport/test impact.
- Out of Scope: Full implementation of each chain.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Features / Systems / Components: Search, settings, repository, canvas, lifecycle.
- Current Behavior: Markers are visible, but their product impact is not normalized.
- Intended Completed Behavior: Each marker is attached to a specific blocked feature chain.
- Missing Pieces: Feature-chain mapping and blocker severity.
- Technical Approach: Trace each high-risk marker through command, model, service, persistence, render, and test pathways.
- Implementation Steps: Prioritize markers in product paths; map their downstream dependencies; note whether they are blockers, follow-on work, or cleanup.
- Validation Steps: Pick representative high-risk markers and confirm the mapped chain matches actual repository behavior.
- Acceptance Criteria: The inventory shows not just where unfinished code lives, but what it blocks.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Treating marker location as equivalent to implementation ownership can mislead sequencing.
- Cleanup / Migration Notes where relevant: Track whether removal requires replacement work, consolidation, or gating.
- Observability / Diagnostics Notes where relevant: Export blocker graphs for later phase planning.
- Rollback / Safety Notes: Keep mappings advisory until subsystem owners validate them.
- References / Context: This is the bridge between raw audit output and the 20-phase execution package.
- Example scenarios where useful: “Search returns empty results” is traced to search panel behavior, saved searches, and navigation expectations.

### P01-T04

- Phase ID: P01
- Task ID: P01-T04
- Task Title: Audit test placeholders, stub expectations, and skipped completion coverage
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Make tests reveal unfinished product work instead of normalizing it.
- Why This Matters Now: Tests currently encode many stub states as accepted behavior.
- Completion Gap Statement: Placeholder tests and stub-oriented assertions allow unfinished behavior to persist without release pressure.
- User / Product Impact: Product breadth appears more complete than runtime behavior actually is.
- Repository Evidence: [test_mark_amp_app.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_mark_amp_app.cpp), [test_outline.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_outline.cpp), [test_phase20_file_management.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp), [tests/fuzz/fuzz_markdown_parser.cpp](/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_markdown_parser.cpp)
- Scope: Placeholder tests, fuzz stubs, tests that explicitly mention stubs, and assertions that codify fake production behavior.
- Out of Scope: Full end-to-end test implementation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_mark_amp_app.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_outline.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp`; `/Users/ryanrentfro/code/markamp/tests/fuzz/fuzz_markdown_parser.cpp`
- Related Features / Systems / Components: Test harnesses, coverage, release confidence.
- Current Behavior: Some tests explicitly accept placeholder, stub, or empty behavior.
- Intended Completed Behavior: Tests distinguish temporary scaffolding from release-ready behavior and force the latter over time.
- Missing Pieces: Coverage inventory, promotion rules, and replacement test plans.
- Technical Approach: Flag tests that assert placeholder behavior; classify which need replacement versus gating.
- Implementation Steps: Scan tests; tag placeholder expectations; map each to the feature it should eventually validate; define conversion or deletion plans.
- Validation Steps: Confirm known stub-heavy areas such as file management, outline, fuzz, and app startup are captured.
- Acceptance Criteria: No major stub subsystem lacks a corresponding test-gap item in the completion inventory.
- Dependencies: P01-T01.
- Risks / Failure Modes: Some tests may intentionally isolate models and not indicate product incompletion; they need careful tagging.
- Cleanup / Migration Notes where relevant: Keep model tests that are valid while replacing product-placeholder assertions.
- Observability / Diagnostics Notes where relevant: Produce a “stub-normalizing test” report for CI.
- Rollback / Safety Notes: Do not delete placeholder tests until stronger replacement coverage exists.
- References / Context: `v23` is only credible if the test story matches the completion story.
- Example scenarios where useful: A placeholder app-start test is promoted into a real runtime smoke path.

### P01-T05

- Phase ID: P01
- Task ID: P01-T05
- Task Title: Establish completion severity scoring and release-blocker policy for unfinished code
- Priority: P0
- Category: Cleanup / Consolidation
- Atomic Completion Tasks Covered: 60
- Objective: Decide which unfinished states are release blockers versus follow-on cleanup.
- Why This Matters Now: Not every placeholder string deserves the same urgency as a fake sync implementation.
- Completion Gap Statement: The repo has many unfinished states but no one severity policy for how they affect release readiness.
- User / Product Impact: The team can underreact to high-risk stubs or overreact to low-risk residue.
- Repository Evidence: [ControlCompletenessMatrix.h](/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.h), [PanelCapabilityModel.h](/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.h), [VisualCleanupCoordinator.cpp](/Users/ryanrentfro/code/markamp/src/core/VisualCleanupCoordinator.cpp), [ReleaseGateChecker.cpp](/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp)
- Scope: Severity levels, production-path rules, allowed temporary gates, blocker definitions, and exit criteria.
- Out of Scope: Implementing all gates in CI in this phase.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.h`; `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.h`; `/Users/ryanrentfro/code/markamp/src/core/VisualCleanupCoordinator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`
- Related Features / Systems / Components: Release management, audits, validation harnesses.
- Current Behavior: Several audit and release-check classes exist, but there is no unified unfinished-work blocker policy.
- Intended Completed Behavior: High-risk stubs, fake transports, dead controls, and visible placeholder panels automatically count as blockers.
- Missing Pieces: Severity rubric, subsystem thresholds, and waiver rules.
- Technical Approach: Define a completion severity model and map it onto existing audit/report infrastructure.
- Implementation Steps: Draft severity levels; classify inventory items; align them with release gates; define evidence required to clear each severity.
- Validation Steps: Run the policy against representative high-risk and low-risk unfinished items and confirm the scoring is sensible.
- Acceptance Criteria: The team can tell which unfinished seams must be fixed before calling the product feature-complete.
- Dependencies: P01-T01 through P01-T04.
- Risks / Failure Modes: An overly soft policy preserves fake production paths; an overly hard policy can block harmless cleanup.
- Cleanup / Migration Notes where relevant: Replace informal “phase debt” language with blocker language tied to this scoring model.
- Observability / Diagnostics Notes where relevant: Feed severity counts into the `v23` scoreboard and release reports.
- Rollback / Safety Notes: Allow tightly scoped waivers only for genuinely hidden or experimental surfaces.
- References / Context: This policy is the operational glue between the audit and the remaining 19 phases.
- Example scenarios where useful: XOR encryption in cloud sync is a blocker; a placeholder icon in an internal audit tool is not.
