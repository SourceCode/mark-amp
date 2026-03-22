# Phase 17 - Advanced Domain Workflows: AV, Graph, Tasks, Presentation, And Node Editor

## Phase Goal

Finish or decisively gate advanced subsystems that are structurally present but still incomplete, so the product stops advertising breadth without finished depth.

## Measurable Outcome

- Structured-data and advanced-domain features have clear completion or gating status.
- Placeholder engines and panel shells are either implemented, hidden, or explicitly labeled outside release paths.
- Cross-subsystem dependencies are made explicit rather than implied.

### Task P17-T01

- Phase ID: `P17`
- Task ID: `P17-T01`
- Task Title: Finish AV table/editor/rendering seams or gate AV views to their real maturity
- Priority: `P1`
- Category: `Migration Completion`
- Objective: Make attribute-view functionality reflect real product depth rather than thin UI coverage.
- Why This Matters Now: AV surfaces are broad but still contain explicit thin or stubbed areas.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.h`, and `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.h` still indicate deferred or stubbed work.
- User / Product Impact: Database-like workflows can feel unfinished despite visible UI.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.h`, `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.h`
- Prior Plan References: `v23` Phase 15
- Scope: Table view, cell editor, renderer fidelity, command support
- Out of Scope: Entirely new AV view types
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/av/AttributeViewStore.h`
- Related Features / Systems / Components: AV views, cell editing, structured data
- Current Behavior: AV support is real in the model layer but still thin in key UI areas.
- Intended Completed Behavior: AV surfaces are either truly usable or clearly gated from default production navigation.
- Missing Pieces: Full editor/renderer implementation and readiness policy
- Technical Approach: Finish core AV table editing and rendering loops before exposing broader AV claims.
- Implementation Steps: Complete renderer/editor; bind commands; validate persistence and undo/redo; gate unfinished subviews.
- Validation Steps: Create and edit structured rows, relations, filters, and sorts through the UI.
- Acceptance Criteria: Core AV workflows are either complete and testable or hidden/gated appropriately.
- Dependencies: `P05-T05`, `P13-T04`
- Parallelization Notes: Can proceed with graph/task/presentation work independently.
- Risks / Failure Modes: AV surface complexity can spread into many incomplete edge cases quickly.
- Cleanup / Migration Notes: Remove thin placeholder subviews or label them explicitly outside release paths.
- Observability / Diagnostics Notes: Emit AV view readiness and cell-edit action diagnostics.
- Rollback / Safety Notes: Keep data persistence safe while upgrading editors/renderers.
- References / Context: AV model and UI stack
- Example scenarios where useful: Editing an AV cell updates the model, undo stack, and visible table immediately.

### Task P17-T02

- Phase ID: `P17`
- Task ID: `P17-T02`
- Task Title: Complete graph, backlink, and secondary-sidebar specialized panels or gate them
- Priority: `P1`
- Category: `Panel Completion`
- Objective: Resolve long-standing placeholder graph-related shells in sidebars.
- Why This Matters Now: Several sidebars still imply graph and backlink depth that is not fully realized in visible surfaces.
- Execution Gap Statement: `LayoutManager` still registers lightweight placeholder graph/backlink surfaces inline.
- User / Product Impact: Secondary sidebars can feel like unfinished demos.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/GraphSidebarPanel.cpp`
- Prior Plan References: `v21` panel audit, `v23` Phase 08 and 15
- Scope: Backlinks, graph mini-map, outline, related metadata panels
- Out of Scope: New graph analytics features beyond current product direction
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/GraphService.h`, `/Users/ryanrentfro/code/markamp/src/core/BacklinkService.h`
- Related Features / Systems / Components: Graph engine, backlinks, sidebar panels
- Current Behavior: Some graph-related sidebars remain light or placeholder-backed.
- Intended Completed Behavior: Graph-related panels are real, data-backed, and lifecycle-complete or hidden.
- Missing Pieces: Data binding, panel readiness, shell integration
- Technical Approach: Bind panels to completed graph/backlink services and gate unfinished variants.
- Implementation Steps: Replace placeholder panel factories; wire real data; add panel readiness checks and restore behavior.
- Validation Steps: Open graph/backlink panels on linked documents and verify data, navigation, and restore behavior.
- Acceptance Criteria: No graph/backlink panel remains a visible placeholder shell.
- Dependencies: `P05-T01`, `P05-T05`
- Parallelization Notes: Can proceed with AV and presentation tracks.
- Risks / Failure Modes: Graph data computation can be expensive or stale without proper caching.
- Cleanup / Migration Notes: Remove placeholder inline factories from `LayoutManager`.
- Observability / Diagnostics Notes: Emit graph/backlink panel load and data freshness diagnostics.
- Rollback / Safety Notes: Hide panels if graph indexes are unavailable rather than showing empty placeholder shells.
- References / Context: Graph services and sidebar panel registration
- Example scenarios where useful: Opening backlinks in the secondary sidebar shows real references and navigates to them accurately.

### Task P17-T03

- Phase ID: `P17`
- Task ID: `P17-T03`
- Task Title: Finish task/calendar/presentation workflows or narrow their visible production scope
- Priority: `P2`
- Category: `Migration Completion`
- Objective: Resolve advanced productivity surfaces that are structurally broad but may still be incomplete in workflow depth.
- Why This Matters Now: These features add product breadth, but incomplete visible breadth still damages trust.
- Execution Gap Statement: Task, calendar, and presentation systems are present in the architecture but need explicit completion or gating decisions.
- User / Product Impact: Users can encounter unfinished advanced workflows that dilute the core IDE experience.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/TaskService.h`, `/Users/ryanrentfro/code/markamp/src/core/PresentationManager.h`
- Prior Plan References: `v23` Phase 15
- Scope: Task board/Gantt/calendar flows, presentation lifecycle, command exposure
- Out of Scope: Major new feature expansion
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CalendarEngine.h`, `/Users/ryanrentfro/code/markamp/src/core/SlideCommandProvider.h`
- Related Features / Systems / Components: Tasks, calendar, presentation
- Current Behavior: Architecture is broad, but visible completion depth varies.
- Intended Completed Behavior: Advanced productivity features are either credibly complete or no longer over-represented in release UI.
- Missing Pieces: Product-depth decision, completion criteria, gating policy
- Technical Approach: Audit visible affordances, finish the smallest credible core, and gate the rest.
- Implementation Steps: Inventory visible entry points; decide must-complete core; implement missing workflow links; hide unfinished branches.
- Validation Steps: Run end-to-end task and presentation core workflows from creation to persistence.
- Acceptance Criteria: Advanced productivity surfaces do not over-claim unsupported depth in production.
- Dependencies: `P04-T01`, `P05-T05`
- Parallelization Notes: Can proceed largely in parallel with other advanced-domain tracks.
- Risks / Failure Modes: Scope creep can distract from the core IDE completion path.
- Cleanup / Migration Notes: Remove or gate menu items and panels for unfinished advanced features.
- Observability / Diagnostics Notes: Emit usage and readiness diagnostics to confirm real capability depth.
- Rollback / Safety Notes: Prefer narrowing visible scope over shipping partial workflows.
- References / Context: Task and presentation subsystem inventory
- Example scenarios where useful: Presentation mode appears only if slide lifecycle and export/open flows are actually complete.

### Task P17-T04

- Phase ID: `P17`
- Task ID: `P17-T04`
- Task Title: Resolve node-editor runtime placeholders with a clear completion or gating strategy
- Priority: `P2`
- Category: `Placeholder / Stub Removal`
- Objective: Stop domain runtimes from sitting in an indefinite "contract validation only" state inside production paths.
- Why This Matters Now: Node-editor runtimes are one of the clearest examples of advanced breadth outrunning completion.
- Execution Gap Statement: Audio, graphics, data-generation, data-transform, and code-flow runtimes still state that they return placeholder results.
- User / Product Impact: Users can infer serious advanced workflows that are not actually real.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/node_editor/AudioRuntime.h`, `/Users/ryanrentfro/code/markamp/src/node_editor/GraphicsRuntime.h`, `/Users/ryanrentfro/code/markamp/src/node_editor/DataGenRuntime.h`, `/Users/ryanrentfro/code/markamp/src/node_editor/DataTransformRuntime.h`, `/Users/ryanrentfro/code/markamp/src/node_editor/CodeFlowRuntime.h`
- Prior Plan References: `v23` Phase 14
- Scope: Node runtime readiness, visible entry points, panel/menu gating
- Out of Scope: Building an entire new node-execution platform in this wave
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ExpressionEvaluator.cpp`
- Related Features / Systems / Components: Node editor, runtimes, advanced workflows
- Current Behavior: Runtimes validate contracts but intentionally do not deliver full computation.
- Intended Completed Behavior: Either a narrowed, credible node-editor core ships or the unfinished runtimes are removed from production-facing paths.
- Missing Pieces: Completion policy, minimal viable runtime behavior, gating rules
- Technical Approach: Define the smallest defensible runtime slice and explicitly gate all other unfinished domains.
- Implementation Steps: Inventory visible node-editor affordances; implement core runtime if justified; gate unresolved domains; update docs/tests.
- Validation Steps: Attempt to access node-editor workflows and verify only real capabilities are reachable.
- Acceptance Criteria: No placeholder runtime domain remains represented as standard production-ready capability.
- Dependencies: `P01-T02`, `P05-T05`
- Parallelization Notes: Can proceed independently, but should remain lower priority than core IDE blockers.
- Risks / Failure Modes: Attempting to finish all node domains at once can consume disproportionate effort.
- Cleanup / Migration Notes: Remove placeholder runtime outputs and menu surfaces when gating unresolved domains.
- Observability / Diagnostics Notes: Log attempts to access gated node-editor domains.
- Rollback / Safety Notes: Hide unresolved domains by default.
- References / Context: Node-editor runtime headers and expression evaluator
- Example scenarios where useful: A graphics-runtime node palette is absent from release builds until real compute/render behavior exists.

### Task P17-T05

- Phase ID: `P17`
- Task ID: `P17-T05`
- Task Title: Add advanced-domain readiness matrices and signoff gates
- Priority: `P2`
- Category: `Release Readiness`
- Objective: Prevent advanced subsystems from remaining in a permanent ambiguous state.
- Why This Matters Now: Breadth-heavy features are the easiest place for vague "almost done" claims to persist.
- Execution Gap Statement: Advanced domains need explicit visible-scope decisions and release criteria.
- User / Product Impact: The product can otherwise feel sprawling but unfinished.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/SpecializedSurfaceCompletionAuditor.h`
- Prior Plan References: `v23` Phase 20
- Scope: Readiness matrices, gating policy, signoff requirements, release visibility decisions
- Out of Scope: New advanced feature ideation
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/docs/v24_docs/ASSESSMENT__MarkAmp_V24_Execution_Synthesis_And_Completion_Assessment.md`
- Related Features / Systems / Components: Advanced subsystem governance, release scope
- Current Behavior: Advanced areas can remain structurally broad without clear release criteria.
- Intended Completed Behavior: Each advanced subsystem is explicitly complete, gated, or deferred from production scope.
- Missing Pieces: Matrix definitions, signoff rules, release policy wiring
- Technical Approach: Use the Phase 01 definition-of-finished framework to classify every advanced domain.
- Implementation Steps: Build readiness matrices; assign policy; wire panel/menu gating and release notes to the results.
- Validation Steps: Review all advanced-domain entry points against the final readiness matrix.
- Acceptance Criteria: No advanced subsystem remains ambiguously visible in release scope.
- Dependencies: `P01-T02`, `P17-T01` through `P17-T04`
- Parallelization Notes: Best finalized late in the phase, after implementation/gating decisions.
- Risks / Failure Modes: Teams may resist narrowing scope without explicit governance support.
- Cleanup / Migration Notes: Retire outdated docs and menu entries for deferred domains.
- Observability / Diagnostics Notes: Publish readiness summary by advanced domain.
- Rollback / Safety Notes: Allow developer-mode exposure only with explicit flags.
- References / Context: Completion auditor and `v24` release-readiness framework
- Example scenarios where useful: AV core table editing is release-ready, while timeline view remains clearly gated if still incomplete.
