# Phase 18: Feature Flag Gated Capability And Migration Seam Completion

## Outcome

Finish or retire feature-flagged, gated, shadow, and partially migrated systems so the repository stops carrying replacement paths that never fully become authoritative.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P18-T01

- Phase ID: P18
- Task ID: P18-T01
- Task Title: Inventory feature flags gated capabilities and intentionally hidden unfinished paths
- Priority: P0
- Category: Disabled Feature Enablement
- Atomic Completion Tasks Covered: 60
- Objective: Make every gated or hidden unfinished capability visible as a completion decision.
- Why This Matters Now: Hidden incomplete work is still product debt even when it is not visible by default.
- Completion Gap Statement: Feature flags and gating systems can hide unfinished subsystems from planning unless they are inventoried explicitly.
- User / Product Impact: Hidden debt delays true feature completeness and complicates release decisions.
- Repository Evidence: [FeatureRegistry.cpp](/Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp), [SafeMode.cpp](/Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp), [PluginManager.cpp](/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp)
- Scope: Feature flags, gated panels, gated actions, disabled-by-default capabilities, and shadow migrations.
- Out of Scope: Purely user-configurable preferences that are intentionally optional long-term.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SafeMode.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`
- Related Features / Systems / Components: Feature registry, safe mode, panels, actions, migrations.
- Current Behavior: Gated capabilities are spread across subsystems and not yet treated as one completion inventory.
- Intended Completed Behavior: Every gated or hidden unfinished path is either scheduled for completion, retirement, or permanent optional status.
- Missing Pieces: Central inventory, rationale, and exit criteria for each gated capability.
- Technical Approach: Extend the `v23` inventory with a dedicated gated-capability dimension.
- Implementation Steps: Scan feature flags and gated surfaces; map them to product areas; classify finish/retire/keep-gated decisions; attach evidence.
- Validation Steps: Confirm known gated or stubbed capabilities appear in the inventory with explicit status.
- Acceptance Criteria: No significant hidden unfinished capability remains unclassified.
- Dependencies: Phase 01.
- Risks / Failure Modes: Some flags represent experiments rather than completion debt and need careful separation.
- Cleanup / Migration Notes where relevant: Mark permanent experimental flags distinctly from completion flags.
- Observability / Diagnostics Notes where relevant: Publish a gated-capability report during startup and CI.
- Rollback / Safety Notes: Do not enable hidden capabilities by default until completion or retirement decisions are made.
- References / Context: `v23` must account for unfinished work even when it is not user-visible yet.
- Example scenarios where useful: A hidden panel or command no longer sits in limbo without a documented completion decision.

### P18-T02

- Phase ID: P18
- Task ID: P18-T02
- Task Title: Finish or retire dual-path migrations where old and new systems still coexist
- Priority: P0
- Category: Migration Completion
- Atomic Completion Tasks Covered: 60
- Objective: End long-lived migration limbo that keeps completion debt alive.
- Why This Matters Now: Coexisting old and new systems are a major source of inconsistent behavior.
- Completion Gap Statement: Multiple areas still have new abstractions or services that only partially replaced older direct pathways.
- User / Product Impact: Users see inconsistent behavior and maintainers must reason about both systems.
- Repository Evidence: [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp), [ToolbarModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp), [PanelService.cpp](/Users/ryanrentfro/code/markamp/src/core/PanelService.cpp), [UnsavedDocumentBuffer.h](/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h)
- Scope: Old/new lifecycle paths, old/new control paths, old/new settings paths, old/new panel ownership, old/new artifact models.
- Out of Scope: Entirely additive subsystems with no overlap.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/PanelService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`
- Related Features / Systems / Components: Settings, toolbar, panels, artifacts, shell.
- Current Behavior: Some newer systems coexist with older wiring instead of replacing it fully.
- Intended Completed Behavior: Each migration seam ends with one authoritative implementation path.
- Missing Pieces: Parity proof, caller migration, and legacy-path retirement.
- Technical Approach: Treat each dual-path seam as a migration project with explicit finish criteria and deletion work.
- Implementation Steps: Inventory coexistence seams; prove parity; switch defaults; remove legacy callers; add invariants to prevent reintroduction.
- Validation Steps: Run before/after workflow comparisons for each migrated seam.
- Acceptance Criteria: No major product workflow still has two competing implementation paths.
- Dependencies: Phase 17.
- Risks / Failure Modes: Removing legacy paths too early can strand un-migrated edge cases.
- Cleanup / Migration Notes where relevant: Delete dead migration code promptly once parity is proven.
- Observability / Diagnostics Notes where relevant: Log remaining legacy-path hits during the migration window.
- Rollback / Safety Notes: Keep narrow fallback shims until the new path survives real smoke coverage.
- References / Context: This phase is essential for turning broad progress into stable completion.
- Example scenarios where useful: There is one settings mutation path, not staged owner plus direct panel write logic in parallel.

### P18-T03

- Phase ID: P18
- Task ID: P18-T03
- Task Title: Resolve hidden incomplete panels actions and services currently protected only by gating
- Priority: P1
- Category: Disabled Feature Enablement
- Atomic Completion Tasks Covered: 60
- Objective: Decide whether gated incomplete work should be completed or removed rather than left indefinitely hidden.
- Why This Matters Now: Gating is a useful safety tool, but not a substitute for closure.
- Completion Gap Statement: Some panels, actions, and services are effectively hidden unfinished work rather than finished optional features.
- User / Product Impact: Product scope remains fuzzy and maintenance cost remains high.
- Repository Evidence: [PanelCapabilityModel.cpp](/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp), [SurfaceActionAuditor.cpp](/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp), [ControlActionManifest.h](/Users/ryanrentfro/code/markamp/src/core/ControlActionManifest.h)
- Scope: Gated panels, gated actions, stub actions, placeholder surfaces, and hidden services.
- Out of Scope: User-disabled but otherwise complete features.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/PanelCapabilityModel.cpp`; `/Users/ryanrentfro/code/markamp/src/core/SurfaceActionAuditor.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ControlActionManifest.h`; `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`
- Related Features / Systems / Components: Gated surfaces, control audits, release gates.
- Current Behavior: Gated incomplete work can remain indefinitely unresolved.
- Intended Completed Behavior: Gated incomplete work is either completed, permanently removed, or documented as long-term experimental.
- Missing Pieces: Decision matrix, completion plans, and removal tasks.
- Technical Approach: Use readiness audits plus the `v23` blocker policy to classify each gated item.
- Implementation Steps: Enumerate gated items; decide complete/retire/experimental; route each item to an implementation or deletion task; update gates.
- Validation Steps: Verify every gated incomplete item has a tracked end state.
- Acceptance Criteria: No hidden incomplete item remains indefinitely “just gated.”
- Dependencies: Phase 02, Phase 17.
- Risks / Failure Modes: Some teams may resist retiring breadth that is not realistically finishable soon.
- Cleanup / Migration Notes where relevant: Remove obsolete gating after completion or deletion.
- Observability / Diagnostics Notes where relevant: Count gated items by disposition in CI.
- Rollback / Safety Notes: Keep retired items recoverable from history if future priorities change.
- References / Context: Feature completeness requires closure, not permanent limbo.
- Example scenarios where useful: A stub debug action is either finished or deleted, not simply hidden from one menu.

### P18-T04

- Phase ID: P18
- Task ID: P18-T04
- Task Title: Add migration-readiness and flag-resolution reporting to startup and release tooling
- Priority: P2
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make migration and gating debt visible every time the product starts or releases.
- Why This Matters Now: Unfinished migrations are easy to forget without continual visibility.
- Completion Gap Statement: Migration seams are currently visible mostly through source inspection and specialized tests.
- User / Product Impact: Indirect but important; hidden migration debt slows product completion.
- Repository Evidence: [ReleaseGateChecker.cpp](/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp), [FeatureRegistry.cpp](/Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp), [ControlCompletenessMatrix.cpp](/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp)
- Scope: Startup reports, release summaries, migration debt counters, flag-resolution status.
- Out of Scope: Telemetry dashboards external to the repository.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`; `/Users/ryanrentfro/code/markamp/src/core/FeatureRegistry.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp`; `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Related Features / Systems / Components: Startup diagnostics, release gates, feature flags, migrations.
- Current Behavior: Migration and flag debt is not summarized centrally at runtime.
- Intended Completed Behavior: Startup and release tooling report unresolved migrations and gated completion debt explicitly.
- Missing Pieces: Shared schema, publishers, and integration with release reports.
- Technical Approach: Extend existing release-check and diagnostics systems with migration-specific summaries.
- Implementation Steps: Define report schema; feed inventory and readiness data; surface during startup and release jobs; add severity thresholds.
- Validation Steps: Leave a known migration seam unresolved and verify the report surfaces it correctly.
- Acceptance Criteria: Migration debt can no longer remain invisible at runtime and release time.
- Dependencies: P18-T01 through P18-T03.
- Risks / Failure Modes: Reports can become ignored if too verbose or not tied to blocker policy.
- Cleanup / Migration Notes where relevant: Retire older one-off migration notes once runtime reports exist.
- Observability / Diagnostics Notes where relevant: Archive migration debt reports with build artifacts.
- Rollback / Safety Notes: Start in advisory mode, then bind to release blocking once the baseline is stable.
- References / Context: This phase operationalizes hidden-completion debt management.
- Example scenarios where useful: Startup warns that one panel migration and one settings migration seam are still unresolved.

### P18-T05

- Phase ID: P18
- Task ID: P18-T05
- Task Title: Add migration and feature-flag closure tests
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Prevent migration seams and gated incomplete work from silently reappearing.
- Why This Matters Now: Migration cleanup regresses easily when later work reintroduces legacy shortcuts.
- Completion Gap Statement: The repo lacks enough tests that assert one authoritative path and one explicit gate policy per migrated subsystem.
- User / Product Impact: Inconsistent behavior can return even after migration work lands.
- Repository Evidence: [test_feature_registry.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_feature_registry.cpp), [test_feature_guard_integration.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_feature_guard_integration.cpp), [test_v21_control_audit.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v21_control_audit.cpp)
- Scope: Feature flags, migration seams, gated panels/actions, legacy-path elimination, startup reports.
- Out of Scope: UI style regression checks.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_feature_registry.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_feature_guard_integration.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_control_audit.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Feature flags, migrations, readiness, tests.
- Current Behavior: Existing tests validate some gating semantics but not enough end-state closure behavior.
- Intended Completed Behavior: Tests assert that migrated systems have one live path and that gated incomplete items are surfaced deliberately.
- Missing Pieces: Legacy-path assertions, migration fixture scenarios, and startup/report verification.
- Technical Approach: Write tests against the completion policy, not just the presence of flags.
- Implementation Steps: Add one-path assertions; add gated-item disposition tests; verify report output; fail on resurrected legacy code usage.
- Validation Steps: Reintroduce a direct-write settings path or an old panel factory and confirm tests fail.
- Acceptance Criteria: Migration completion and gate policy are regression-protected.
- Dependencies: P18-T01 through P18-T04.
- Risks / Failure Modes: Overly synthetic tests may miss real integration regressions if not backed by workflows.
- Cleanup / Migration Notes where relevant: Remove tests that only prove feature flags toggle values without checking completion semantics.
- Observability / Diagnostics Notes where relevant: Publish migration-test summaries alongside release reports.
- Rollback / Safety Notes: Keep tests focused on intended authoritative paths to avoid freezing deliberate experiments.
- References / Context: This phase locks in migration progress and prevents the codebase from drifting back toward dual-path limbo.
- Example scenarios where useful: A test fails if a panel can still be created via a deprecated placeholder factory after migration completion.
