# Phase 14: Node Editor Domain Runtime And Advanced Workflow Completion

## Outcome

Finish the node-editor and advanced runtime domains that currently validate contracts but defer real computation, routing, or interaction depth.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P14-T01

- Phase ID: P14
- Task ID: P14-T01
- Task Title: Replace placeholder node-editor domain runtimes with real execution backends
- Priority: P1
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Turn domain runtime headers from contract validators into actual execution engines.
- Why This Matters Now: The node-editor subsystem advertises breadth, but its core domains still defer real computation.
- Completion Gap Statement: Audio, data generation, data transform, code flow, and graphics runtimes explicitly return placeholder results with real computation deferred.
- User / Product Impact: Advanced workflow features remain structural rather than complete.
- Repository Evidence: [AudioRuntime.h](/Users/ryanrentfro/code/markamp/src/node_editor/AudioRuntime.h), [DataGenRuntime.h](/Users/ryanrentfro/code/markamp/src/node_editor/DataGenRuntime.h), [DataTransformRuntime.h](/Users/ryanrentfro/code/markamp/src/node_editor/DataTransformRuntime.h), [CodeFlowRuntime.h](/Users/ryanrentfro/code/markamp/src/node_editor/CodeFlowRuntime.h), [GraphicsRuntime.h](/Users/ryanrentfro/code/markamp/src/node_editor/GraphicsRuntime.h)
- Scope: Domain runtime computation, result models, node execution, and error contracts.
- Out of Scope: Visual-node styling and editor chrome.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/node_editor/AudioRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/DataGenRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/DataTransformRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/CodeFlowRuntime.h`; `/Users/ryanrentfro/code/markamp/src/node_editor/GraphicsRuntime.h`
- Related Features / Systems / Components: Node editor, domain runtimes, advanced automation workflows.
- Current Behavior: Domain runtime types exist but explicitly defer their real computation.
- Intended Completed Behavior: Each supported node domain executes real logic or is explicitly gated from production.
- Missing Pieces: Domain implementations, capability metadata, runtime error handling, and persistence of results.
- Technical Approach: Implement domain runtimes behind the existing contracts and define unsupported-domain policy where needed.
- Implementation Steps: Prioritize production domains; implement execution backends; add validation and failure handling; integrate outputs with node-editor surfaces.
- Validation Steps: Execute representative graphs in each supported domain and verify real outputs.
- Acceptance Criteria: Supported node domains no longer return placeholder results in production.
- Dependencies: Phase 17.
- Risks / Failure Modes: Some domains may require external engines or libraries and need explicit feature gating if not ready.
- Cleanup / Migration Notes where relevant: Remove “real computation deferred” comments once runtimes are complete or gated.
- Observability / Diagnostics Notes where relevant: Emit per-domain execution success, error, and latency metrics.
- Rollback / Safety Notes: Gate unfinished domains rather than leaving them exposed with placeholder execution.
- References / Context: This phase turns aspirational advanced workflows into actual features.
- Example scenarios where useful: A data-transform node graph produces transformed output instead of a contract-validation placeholder.

### P14-T02

- Phase ID: P14
- Task ID: P14-T02
- Task Title: Finish node-editor interaction fidelity and graph hit-testing depth
- Priority: P2
- Category: UI Workflow Completion
- Atomic Completion Tasks Covered: 60
- Objective: Close deferred interaction seams around link routing, frame handling, and preview behavior in node workflows.
- Why This Matters Now: Interaction depth is the difference between a demo graph editor and a finished tool.
- Completion Gap Statement: Hit-testing and some graph-level interactions still describe deferred logic rather than complete behavior.
- User / Product Impact: Node editing feels shallower than the available domain list suggests.
- Repository Evidence: [HitTester.cpp](/Users/ryanrentfro/code/markamp/src/node_editor/HitTester.cpp), [PreviewSystem.h](/Users/ryanrentfro/code/markamp/src/node_editor/PreviewSystem.h), [SelectTool.cpp](/Users/ryanrentfro/code/markamp/src/canvas/SelectTool.cpp)
- Scope: Link hit-testing, frame hit-testing, preview cancellation, and graph interaction parity.
- Out of Scope: Visual-only improvements.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/node_editor/HitTester.cpp`; `/Users/ryanrentfro/code/markamp/src/node_editor/PreviewSystem.h`; `/Users/ryanrentfro/code/markamp/src/canvas/SelectTool.cpp`; `/Users/ryanrentfro/code/markamp/src/node_editor/LinkRouter.h`
- Related Features / Systems / Components: Node editor interactions, preview, routing, graph editing.
- Current Behavior: Some deeper graph interactions remain simplified or deferred.
- Intended Completed Behavior: Node-editor interactions are complete and consistent for supported graph features.
- Missing Pieces: Full path-aware hit testing, frame integration, and preview lifecycle integration.
- Technical Approach: Finish interaction math and route it through canonical graph-state services.
- Implementation Steps: Implement Bézier-aware link hit tests; complete frame interaction hooks; finish preview cancellation and refresh semantics.
- Validation Steps: Interact with dense graphs, overlapping links, frames, and rapid preview updates.
- Acceptance Criteria: Node-editor interaction paths no longer contain deferred “real logic later” branches for supported features.
- Dependencies: P14-T01.
- Risks / Failure Modes: More accurate hit testing can reveal latent performance issues on large graphs.
- Cleanup / Migration Notes where relevant: Remove deferred comments once real hit testing is in place.
- Observability / Diagnostics Notes where relevant: Add graph-interaction traces for missed-hit debugging.
- Rollback / Safety Notes: Use feature flags for new hit-testing algorithms if needed during rollout.
- References / Context: Advanced domains need equally complete interaction depth.
- Example scenarios where useful: Clicking a curved connector selects the intended link reliably, not a rough placeholder target.

### P14-T03

- Phase ID: P14
- Task ID: P14-T03
- Task Title: Complete advanced transform and evaluation helpers that still echo or simplify results
- Priority: P1
- Category: Stub Service Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace shallow helper engines with real evaluation semantics for advanced workflows.
- Why This Matters Now: Helper engines cap what the advanced domains can express even if the shells are complete.
- Completion Gap Statement: Expression and transform helpers still rely on simplified or placeholder behavior.
- User / Product Impact: Advanced automation and data workflows are less useful than their surface area suggests.
- Repository Evidence: [ExpressionEvaluator.cpp](/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp), [DataTransformPipeline.cpp](/Users/ryanrentfro/code/markamp/src/core/DataTransformPipeline.cpp), [StringFormatter.cpp](/Users/ryanrentfro/code/markamp/src/core/StringFormatter.cpp)
- Scope: Expression evaluation, transform semantics, formatter logic, and downstream graph/runtime consumers.
- Out of Scope: General-purpose language runtimes beyond intended product scope.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DataTransformPipeline.cpp`; `/Users/ryanrentfro/code/markamp/src/core/StringFormatter.cpp`; `/Users/ryanrentfro/code/markamp/src/node_editor/DataTransformRuntime.h`
- Related Features / Systems / Components: Transform pipelines, node editor, advanced automation.
- Current Behavior: Some helpers still echo inputs or provide simple placeholder behavior.
- Intended Completed Behavior: Transform and evaluation helpers produce real computed outputs and errors.
- Missing Pieces: Real evaluators, richer transform primitives, and explicit error models.
- Technical Approach: Build deterministic evaluation layers with strong validation and bounded capability.
- Implementation Steps: Replace echo logic; implement supported expressions; add transform primitives; update runtime consumers and tests.
- Validation Steps: Run expression and transform fixtures through node and non-node consumers.
- Acceptance Criteria: Helper engines no longer rely on shallow placeholder semantics for supported workflows.
- Dependencies: P14-T01.
- Risks / Failure Modes: Expanding evaluation depth can introduce security and sandbox concerns if arbitrary execution is allowed.
- Cleanup / Migration Notes where relevant: Remove comments labeling transforms as placeholders once real semantics land.
- Observability / Diagnostics Notes where relevant: Track evaluation errors and unsupported-expression reasons.
- Rollback / Safety Notes: Keep evaluation capabilities explicitly bounded and schema-driven.
- References / Context: This phase closes hidden capability gaps beneath advanced-feature shells.
- Example scenarios where useful: A transform graph calculates derived values instead of returning the original expression text.

### P14-T04

- Phase ID: P14
- Task ID: P14-T04
- Task Title: Finish advanced-domain persistence import export and interoperability seams
- Priority: P2
- Category: Persistence Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make advanced node workflows durable and portable once their runtimes are real.
- Why This Matters Now: Advanced features are not complete if they only execute transiently.
- Completion Gap Statement: Advanced workflow runtimes and graphs still lack enough completion around persistence and interchange.
- User / Product Impact: Users cannot fully trust advanced workflows as durable project artifacts.
- Repository Evidence: [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp), [CanvasIntegrationService.cpp](/Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp), [FileTemplateEngine.h](/Users/ryanrentfro/code/markamp/src/core/FileTemplateEngine.h)
- Scope: Graph serialization, snapshot fidelity, import/export, repository integration, and external references.
- Out of Scope: Marketplace packaging.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/FileTemplateEngine.h`; `/Users/ryanrentfro/code/markamp/src/node_editor`
- Related Features / Systems / Components: Advanced graphs, persistence, repository, import/export.
- Current Behavior: Advanced workflow depth is limited partly because persistence and interchange are not fully finished.
- Intended Completed Behavior: Advanced node workflows can be saved, restored, versioned, and exchanged reliably.
- Missing Pieces: Serialization contracts, migration/versioning, and repository/test integration.
- Technical Approach: Extend the finished artifact and repository contracts into the node-editor domain.
- Implementation Steps: Define graph persistence schema; add versioning; integrate with repository snapshots; add import/export support.
- Validation Steps: Save, reopen, snapshot, diff, and import/export advanced graph artifacts.
- Acceptance Criteria: Supported advanced workflows are durable artifacts, not transient demos.
- Dependencies: Phase 03, Phase 04, P14-T01.
- Risks / Failure Modes: Schema churn can make advanced artifacts fragile unless migration is handled early.
- Cleanup / Migration Notes where relevant: Version all advanced graph schemas from the first real persistence release.
- Observability / Diagnostics Notes where relevant: Emit graph migration and deserialize-failure diagnostics.
- Rollback / Safety Notes: Keep converters for early graph schema versions.
- References / Context: Completion requires more than runtime execution; it requires durability.
- Example scenarios where useful: A node graph can be saved, versioned, and restored after restart without losing runtime semantics.

### P14-T05

- Phase ID: P14
- Task ID: P14-T05
- Task Title: Add advanced-domain completion gates and real-workflow fixtures
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Make advanced feature completion measurable and durable.
- Why This Matters Now: Advanced domains are especially prone to remaining half-built without explicit completion pressure.
- Completion Gap Statement: The repo lacks enough fixtures that prove advanced workflows do real work rather than returning placeholders.
- User / Product Impact: Advanced features can remain demo-quality while appearing present.
- Repository Evidence: [tests/unit/test_phase14_canvas_extensibility.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase14_canvas_extensibility.cpp), [tests/unit/test_phase40_automation_hooks.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase40_automation_hooks.cpp)
- Scope: Domain runtimes, graph persistence, transform/evaluation correctness, interaction fidelity.
- Out of Scope: UI screenshot suites.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_phase14_canvas_extensibility.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase40_automation_hooks.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/src/node_editor`
- Related Features / Systems / Components: Node editor, automation, advanced domains, tests.
- Current Behavior: Tests still lean toward model validity instead of proving final workflow depth.
- Intended Completed Behavior: Advanced workflows are protected by fixture-driven completion tests.
- Missing Pieces: Real graph fixtures, output assertions, persistence scenarios, and interaction cases.
- Technical Approach: Build representative advanced workflow fixtures and assert real outputs end to end.
- Implementation Steps: Add graph fixtures; add runtime-output tests; add save/reopen scenarios; wire gates into CI.
- Validation Steps: Reintroduce placeholder runtime behavior and confirm the suite fails.
- Acceptance Criteria: Advanced-domain regressions are observable and blocking.
- Dependencies: P14-T01 through P14-T04.
- Risks / Failure Modes: Fixture maintenance can become expensive if domain semantics are unstable.
- Cleanup / Migration Notes where relevant: Retire tests that only confirm placeholders compile.
- Observability / Diagnostics Notes where relevant: Store graph execution traces and persisted fixture diffs on failure.
- Rollback / Safety Notes: Keep fixtures small and deterministic.
- References / Context: This phase keeps advanced-feature completion from remaining aspirational.
- Example scenarios where useful: A data-gen graph test asserts real generated output and persistence round trips.
