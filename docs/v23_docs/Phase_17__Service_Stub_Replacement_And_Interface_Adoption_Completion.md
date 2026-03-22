# Phase 17: Service Stub Replacement And Interface Adoption Completion

## Outcome

Finish the common service and interface layer so APIs, registries, and abstraction seams stop returning defaults, mocks, or partial results when production callers expect real behavior.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P17-T01

- Phase ID: P17
- Task ID: P17-T01
- Task Title: Inventory and replace first-party services that still return default or synthetic production results
- Priority: P0
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Remove broad classes of service-layer stubs that cut across many product workflows.
- Why This Matters Now: Many incomplete features are symptoms of shared service stubs, not only local UI debt.
- Completion Gap Statement: Multiple services still return empty lists, synthetic objects, or count-only success even when their APIs imply finished runtime behavior.
- User / Product Impact: Feature breadth remains higher than feature depth across the product.
- Repository Evidence: [SearchService.cpp](/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp), [ThemeMarketplace.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp), [OtlpExporter.cpp](/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp), [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp)
- Scope: Shared services that power multiple product areas and still contain first-party placeholder behavior.
- Out of Scope: UI-only placeholder surfaces.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp`; `/Users/ryanrentfro/code/markamp/src/core/OtlpExporter.cpp`; `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`
- Related Features / Systems / Components: Search, marketplace, telemetry, repository, settings, notebooks, cloud.
- Current Behavior: Services expose broad contracts but still return synthetic or incomplete results.
- Intended Completed Behavior: Shared services either do real work or clearly report unsupported capability.
- Missing Pieces: Real implementations, capability metadata, and production gating for incomplete services.
- Technical Approach: Maintain the existing API surfaces but replace placeholder internals with real backends or explicit capability states.
- Implementation Steps: Rank stubs by cross-feature impact; replace internals; align callers to explicit capability outcomes; remove silent synthetic paths.
- Validation Steps: Exercise each service through at least one real product workflow and one failure workflow.
- Acceptance Criteria: High-impact shared services no longer rely on default or synthetic production returns.
- Dependencies: Phase 01.
- Risks / Failure Modes: Shared service replacement can create broad regressions if callers relied on synthetic behavior.
- Cleanup / Migration Notes where relevant: Remove transitional adapters once all callers consume the real service contract.
- Observability / Diagnostics Notes where relevant: Add service-level health reports and fallback-usage counters.
- Rollback / Safety Notes: Prefer capability gating over quietly falling back to fake behavior.
- References / Context: This phase is the repository-wide multiplier for many earlier subsystem plans.
- Example scenarios where useful: A feature stops receiving an empty “success” result from a shared service and instead gets real data or a clear unsupported error.

### P17-T02

- Phase ID: P17
- Task ID: P17-T02
- Task Title: Finish interface-to-implementation adoption where newer abstractions are only partially consumed
- Priority: P0
- Category: Migration Completion
- Atomic Completion Tasks Covered: 60
- Objective: Close the gap between abstraction layers that exist and the production callers that still bypass them.
- Why This Matters Now: Many incomplete workflows are really incomplete migrations.
- Completion Gap Statement: The repo contains newer controllers, registries, and service owners that are not yet fully adopted by visible product pathways.
- User / Product Impact: Product behavior stays inconsistent and harder to finish because old and new pathways coexist.
- Repository Evidence: [SettingsStateOwner.cpp](/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp), [WorkbenchShellController.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp), [ToolbarModel.cpp](/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp), [SidebarPanelRegistry.cpp](/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp)
- Scope: Settings ownership, shell controller adoption, toolbar/panel models, workspace and artifact contracts, provider registries.
- Out of Scope: New abstractions not already in the repo.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SettingsStateOwner.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SidebarPanelRegistry.cpp`
- Related Features / Systems / Components: Shell, settings, panels, toolbar, lifecycle.
- Current Behavior: New abstractions exist, but old direct pathways still bypass them.
- Intended Completed Behavior: Visible product pathways consume the canonical abstraction layer consistently.
- Missing Pieces: Caller migration, duplicate-path removal, and invariant checks.
- Technical Approach: Identify each newer abstraction that already encodes the intended final ownership model and migrate callers onto it fully.
- Implementation Steps: Map bypass callers; migrate in dependency order; add invariants and audits; remove obsolete direct paths.
- Validation Steps: Compare behavior before and after migration on the highest-traffic workflows.
- Acceptance Criteria: Production callers stop bypassing the newer canonical abstractions.
- Dependencies: Phase 02 through Phase 10.
- Risks / Failure Modes: Abstraction adoption can break edge cases if the new layer still lacks parity.
- Cleanup / Migration Notes where relevant: Delete obsolete direct wiring after each adoption wave is validated.
- Observability / Diagnostics Notes where relevant: Add logs when callers still hit legacy paths during migration.
- Rollback / Safety Notes: Keep narrow compatibility shims only until parity is proven.
- References / Context: Many previous planning passes already pointed at these partially migrated seams.
- Example scenarios where useful: The visible toolbar is actually driven by `ToolbarModel` rather than its own hardcoded button contract.

### P17-T03

- Phase ID: P17
- Task ID: P17-T03
- Task Title: Normalize unsupported-state handling instead of using silent stubs or fake success
- Priority: P1
- Category: Cleanup / Consolidation
- Atomic Completion Tasks Covered: 60
- Objective: Make incomplete or unavailable capabilities explicit to callers and users.
- Why This Matters Now: Silent fake success is one of the most damaging completion anti-patterns in the repo.
- Completion Gap Statement: Several services currently succeed shallowly or return empty data where they should surface unsupported state or failure.
- User / Product Impact: Users and upstream code cannot reliably distinguish unsupported features from empty-but-valid results.
- Repository Evidence: [SearchService.cpp](/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp), [ThemeMarketplace.cpp](/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp), [S3Client.cpp](/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp), [WebDavClient.cpp](/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp)
- Scope: Unsupported-state semantics, result typing, diagnostics, and user-facing messaging.
- Out of Scope: Features that are genuinely intended to return empty or disabled state in normal operation.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/SearchService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ThemeMarketplace.cpp`; `/Users/ryanrentfro/code/markamp/src/core/S3Client.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WebDavClient.cpp`
- Related Features / Systems / Components: Shared services, transport, search, marketplace, cloud.
- Current Behavior: Empty or shallow results can mean either “not implemented” or “nothing found.”
- Intended Completed Behavior: Unsupported, unavailable, and empty-success states are distinct and consistently handled.
- Missing Pieces: Result-type cleanup, call-site adaptation, and diagnostics standards.
- Technical Approach: Introduce or standardize explicit capability/unsupported result paths and update callers to respect them.
- Implementation Steps: Audit silent-success stubs; revise result types or status fields; update callers and UI surfaces; add unsupported-state tests.
- Validation Steps: Trigger unsupported and empty-success scenarios separately and confirm they are differentiated.
- Acceptance Criteria: Unsupported behavior is no longer masked by generic empty or success returns.
- Dependencies: P17-T01.
- Risks / Failure Modes: Tightening result semantics can surface many hidden caller assumptions at once.
- Cleanup / Migration Notes where relevant: Remove comments that imply callers should “just get an empty result for now.”
- Observability / Diagnostics Notes where relevant: Count unsupported-state hits by subsystem.
- Rollback / Safety Notes: Introduce compatibility adapters only temporarily for legacy callers.
- References / Context: This phase makes the product honest while full implementation catches up.
- Example scenarios where useful: A marketplace fetch returns “service unavailable” instead of an empty list that looks like “no themes found.”

### P17-T04

- Phase ID: P17
- Task ID: P17-T04
- Task Title: Finish service-level diagnostics health reporting and readiness metadata
- Priority: P2
- Category: Diagnostics / Recovery Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make service completion visible and queryable at runtime.
- Why This Matters Now: A large completion program needs direct evidence of what is still real, gated, or degraded.
- Completion Gap Statement: The repo contains many service seams but not one standardized readiness report for them.
- User / Product Impact: Indirect but important; unresolved service debt stays hidden longer.
- Repository Evidence: [ReleaseGateChecker.cpp](/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp), [VisualCleanupCoordinator.cpp](/Users/ryanrentfro/code/markamp/src/core/VisualCleanupCoordinator.cpp), [ControlCompletenessMatrix.cpp](/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp)
- Scope: Readiness flags, health endpoints, startup reports, unsupported-state inventory, service gate feeds.
- Out of Scope: New telemetry infrastructure beyond what is needed for completion visibility.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/ReleaseGateChecker.cpp`; `/Users/ryanrentfro/code/markamp/src/core/VisualCleanupCoordinator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ControlCompletenessMatrix.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ObservabilityService.cpp`
- Related Features / Systems / Components: Diagnostics, release gates, service health, readiness reporting.
- Current Behavior: Service completion state is spread across comments, tests, and a few subsystem-specific audits.
- Intended Completed Behavior: The product can report which services are complete, degraded, gated, or unsupported.
- Missing Pieces: Shared readiness schema, publishers, and release integration.
- Technical Approach: Extend existing audit/report models to include service readiness families.
- Implementation Steps: Define readiness schema; publish per-service status; connect into release gate and startup diagnostics; document consumption rules.
- Validation Steps: Mark a known service stub and verify readiness reporting exposes it clearly.
- Acceptance Criteria: Service completion status is visible without reading source comments.
- Dependencies: P17-T01 through P17-T03.
- Risks / Failure Modes: Readiness reports can become stale if not generated directly from runtime state or code-derived inventories.
- Cleanup / Migration Notes where relevant: Consolidate overlapping readiness signals into one schema.
- Observability / Diagnostics Notes where relevant: Include readiness snapshots in CI and startup artifacts.
- Rollback / Safety Notes: Keep readiness reporting additive until consumers adopt it.
- References / Context: This phase makes the completion audit operational inside the product itself.
- Example scenarios where useful: A release report shows search backend complete, cloud transport degraded, and theme marketplace gated.

### P17-T05

- Phase ID: P17
- Task ID: P17-T05
- Task Title: Add shared-service completion regression coverage and capability contracts
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Prevent shared services from regressing back into silent stubs after individual subsystem work lands.
- Why This Matters Now: Shared services affect many features at once and need explicit regression protection.
- Completion Gap Statement: Many service stubs are exercised only indirectly, making regressions easy to miss.
- User / Product Impact: Broad product behavior can degrade if a shared service slips back into placeholder mode.
- Repository Evidence: [tests/unit/test_service_registry.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_service_registry.cpp), [tests/unit/test_p1_p4_services.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_p1_p4_services.cpp), [tests/unit/test_v21_validation_harness.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_v21_validation_harness.cpp)
- Scope: Capability contracts, unsupported-state behavior, real-service expectations, and regression gates for shared APIs.
- Out of Scope: Subsystem-specific UI tests.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_service_registry.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_p1_p4_services.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_v21_validation_harness.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`
- Related Features / Systems / Components: Services, registries, capabilities, tests.
- Current Behavior: Existing tests do not yet force many shared services to stay complete once replaced.
- Intended Completed Behavior: Shared service contracts are tested directly and block regression into stub/default behavior.
- Missing Pieces: Capability fixtures, direct service tests, and unsupported-state assertions.
- Technical Approach: Define a small set of shared capability contracts and assert them consistently in tests.
- Implementation Steps: Add direct service regression tests; define expected capability matrices; update integration tests to consume readiness metadata.
- Validation Steps: Reintroduce a default-return stub in a shared service and confirm tests fail broadly.
- Acceptance Criteria: Shared service completion is durable and monitored.
- Dependencies: P17-T01 through P17-T04.
- Risks / Failure Modes: Overly abstract capability tests can become decoupled from real product behavior if not tied to workflows.
- Cleanup / Migration Notes where relevant: Remove placeholder service tests after stronger contract tests exist.
- Observability / Diagnostics Notes where relevant: Publish service capability test matrices and deltas.
- Rollback / Safety Notes: Keep tests focused on intended supported capabilities, not aspirational ones.
- References / Context: Shared services are the fastest way for completion debt to spread back into the product.
- Example scenarios where useful: A core service no longer can quietly return an empty vector in a supported workflow without failing tests.
