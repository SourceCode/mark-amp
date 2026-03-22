# Phase 09 - Canvas Artifact, Workbench, And Interaction Completion

## Phase Goal

Promote the canvas from split shell/workbench behavior into one first-class artifact experience with real creation, save, restore, tooling, and panel integration.

## Measurable Outcome

- Canvas creation and save flows use the same shell artifact model as other primary surfaces.
- Canvas shell and workbench no longer fight for authority.
- Canvas controls and inspectors stop shipping as thin placeholder UI.

### Task P09-T01

- Phase ID: `P09`
- Task ID: `P09-T01`
- Task Title: Unify `CanvasWorkspacePanel` and `CanvasWorkbench` ownership
- Priority: `P0`
- Category: `Canvas Completion`
- Objective: Remove the split between placeholder shell behavior and richer workbench logic.
- Why This Matters Now: Canvas still has two competing product narratives in the repo.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp` and `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp` represent different levels of completeness.
- User / Product Impact: Canvas creation and interaction remain less trustworthy than editor workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Prior Plan References: `v20` Phase 04, `v23` Phase 07
- Scope: Board creation/open/save, host ownership, event routing, active board state
- Out of Scope: Advanced multi-user collaboration details
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CanvasEventBridge.h`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Canvas artifact lifecycle, workbench host, event bus
- Current Behavior: The visible canvas shell still contains placeholder behavior while the workbench contains deeper capabilities.
- Intended Completed Behavior: One canvas host delegates to one authoritative workbench and artifact layer.
- Missing Pieces: Ownership model, event bridge adoption, lifecycle routing
- Technical Approach: Make shell canvas panels thin views over a shared canvas artifact/workbench controller.
- Implementation Steps: Replace private event bus usage; bind to shared artifact lifecycle; move shell actions into canonical workbench controller.
- Validation Steps: Create/open/rename/save/restore canvases and verify identical behavior across entry points.
- Acceptance Criteria: Canvas no longer has competing creation or event-topology paths in production.
- Dependencies: `P02-T05`, `P04-T02`
- Parallelization Notes: Foundation for later canvas UI and import/export work.
- Risks / Failure Modes: Hidden private event paths can still desync canvas UI from the rest of the shell.
- Cleanup / Migration Notes: Remove placeholder `NewBoard()` logic and private event bus setup.
- Observability / Diagnostics Notes: Emit canvas artifact lifecycle and event-bridge diagnostics.
- Rollback / Safety Notes: Keep compatibility adapters while retiring old event assumptions.
- References / Context: Canvas event bridge and workbench files
- Example scenarios where useful: Opening a board from the command palette and from explorer lands in the same canvas host state and board model.

### Task P09-T02

- Phase ID: `P09`
- Task ID: `P09-T02`
- Task Title: Finish canvas artifact save, restore, autosave, and recovery flows
- Priority: `P0`
- Category: `Canvas Completion`
- Objective: Make canvas persistence trustworthy under the same product rules as files and notebooks.
- Why This Matters Now: Canvas cannot be considered primary until its state survives realistic workflows.
- Execution Gap Statement: Canvas persistence and shell restore still trail behind workbench capability.
- User / Product Impact: Users risk losing board state or reopening into incomplete shell context.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.h`, `/Users/ryanrentfro/code/markamp/src/canvas/AutosaveModel.h`
- Prior Plan References: `v20` Phase 05, `v23` Phase 07
- Scope: Save, save-as, autosave, restore, dirty state, crash recovery
- Out of Scope: Remote sync conflicts
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/Board.h`
- Related Features / Systems / Components: Board persistence, autosave, recovery
- Current Behavior: Canvas save and restore are not yet fully aligned with the shared artifact spine.
- Intended Completed Behavior: Canvas follows the same persistence guarantees and recovery semantics as other primary artifact families.
- Missing Pieces: Shared artifact persistence adapters and restore manifests for canvas boards
- Technical Approach: Adapt board serializers and autosave model into the shared artifact persistence contract.
- Implementation Steps: Wire save commands; persist board session state; restore active board and viewport; add failure UX.
- Validation Steps: Edit boards, autosave, crash, restart, and verify restored board state.
- Acceptance Criteria: Canvas persistence passes the same smoke and recovery gates as files and notebooks.
- Dependencies: `P03-T02`, `P03-T03`, `P09-T01`
- Parallelization Notes: Can proceed with board-tool completion but should stabilize before canvas UI polish.
- Risks / Failure Modes: Board data loss if save and autosave serialization diverge.
- Cleanup / Migration Notes: Remove any board-only persistence shortcuts outside shared orchestrators.
- Observability / Diagnostics Notes: Track board-save duration, autosave failures, restore mismatches.
- Rollback / Safety Notes: Preserve recoverable local snapshots during serializer migration.
- References / Context: Board serializer and autosave model
- Example scenarios where useful: A board with grouped objects and viewport zoom reopens exactly as last saved/restored.

### Task P09-T03

- Phase ID: `P09`
- Task ID: `P09-T03`
- Task Title: Complete canvas tools, selection, and inspector command integrity
- Priority: `P1`
- Category: `Canvas Completion`
- Objective: Make canvas tool actions and property editing route through canonical commands and shared shell context.
- Why This Matters Now: Canvas still presents controls that can outpace behavior or state synchronization.
- Execution Gap Statement: Selection, tool, and inspector flows still contain placeholder or partial interaction seams.
- User / Product Impact: Canvas feels less dependable than editor interactions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas/SelectTool.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp`
- Prior Plan References: `v21` controls audit, `v22` canvas UI, `v23` canvas completion
- Scope: Tool switching, selection visuals/state, inspector editing, layering/grouping commands
- Out of Scope: Entirely new canvas-object types
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h`, `/Users/ryanrentfro/code/markamp/src/canvas/LayeringService.h`
- Related Features / Systems / Components: Canvas tools, inspector, command routing
- Current Behavior: Some tool and inspector behaviors remain local, partial, or weakly integrated.
- Intended Completed Behavior: Canvas interactions route through stable tool and command contracts with synchronized shell state.
- Missing Pieces: Shared command mapping, state propagation, inspector completion
- Technical Approach: Bind canvas actions to the shell action model and expose canvas context keys for enablement.
- Implementation Steps: Inventory actions; route commands; complete selection/inspector update chain; add proper enablement.
- Validation Steps: Switch tools, edit object properties, group/ungroup, and undo/redo through all entry points.
- Acceptance Criteria: Canvas actions are real, context-aware, and reflected correctly in shell controls.
- Dependencies: `P04-T03`, `P09-T01`
- Parallelization Notes: Can progress with canvas import/export completion.
- Risks / Failure Modes: Canvas-specific command state can drift from global shell state without robust context propagation.
- Cleanup / Migration Notes: Remove placeholder inspector actions and stale event shortcuts.
- Observability / Diagnostics Notes: Log canvas action dispatch, active tool changes, and selection/inspector sync mismatches.
- Rollback / Safety Notes: Keep read-only inspector fallbacks for unsupported object types during migration.
- References / Context: Canvas tool host and inspector stack
- Example scenarios where useful: Clicking align-left in a canvas toolbar and triggering it from the command palette execute the same command path.

### Task P09-T04

- Phase ID: `P09`
- Task ID: `P09-T04`
- Task Title: Finish canvas import, export, template, and embedded-asset seams
- Priority: `P1`
- Category: `Canvas Completion`
- Objective: Replace remaining placeholder logic in board import/export and template flows.
- Why This Matters Now: Canvas appears broad, but several production paths still contain placeholders.
- Execution Gap Statement: PDF import, template parsing, metadata scraping, and some export behaviors remain incomplete.
- User / Product Impact: Canvas workflows fail at high-value interoperability points.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp`
- Prior Plan References: `v23` Phase 07 and 13
- Scope: Template import, PDF import, asset metadata, export fidelity
- Out of Scope: Marketplace-scale template browsing
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp`
- Related Features / Systems / Components: Import/export, templates, metadata, embedded assets
- Current Behavior: Several interoperability paths still rely on placeholder or deferred implementations.
- Intended Completed Behavior: Canvas import/export and template workflows are product-grade and testable.
- Missing Pieces: Real template parsing, page rendering, metadata fetch, export object fidelity
- Technical Approach: Replace placeholder branches with real parsers/services or explicit gating.
- Implementation Steps: Implement template parsing; complete PDF page rendering path; finish export object coverage; add metadata service contracts.
- Validation Steps: Import templates/PDFs, export boards, and verify content fidelity against fixtures.
- Acceptance Criteria: Canvas interoperability workflows no longer depend on placeholder production logic.
- Dependencies: `P09-T01`, `P16-T01`
- Parallelization Notes: Can progress partly in parallel with rendering and PDF work.
- Risks / Failure Modes: Asset-heavy boards can expose performance and memory issues.
- Cleanup / Migration Notes: Remove placeholder HTML and stub object export branches.
- Observability / Diagnostics Notes: Emit import/export stage diagnostics and unsupported-object reports.
- Rollback / Safety Notes: Provide graceful unsupported-object reporting rather than silent omission.
- References / Context: Canvas import/export stack
- Example scenarios where useful: Importing a PDF creates real page objects instead of placeholder images or empty shells.

### Task P09-T05

- Phase ID: `P09`
- Task ID: `P09-T05`
- Task Title: Attach canvas visual-system work to the completed shell and workbench model
- Priority: `P2`
- Category: `UI Quality / Styling`
- Objective: Rebuild visible canvas chrome and tool surfaces only after the correct host is authoritative.
- Why This Matters Now: `v22` correctly flagged canvas UI thinness, but polish should now follow host completion.
- Execution Gap Statement: Canvas shell still contains generic controls and placeholder chrome that reflect the old host model.
- User / Product Impact: Canvas still looks less premium and less intentional than the rest of the app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Prior Plan References: `v22` Phase 10, `v24` Phase 07
- Scope: Toolbars, inspector chrome, minimap, empty states, object chrome, interaction visuals
- Out of Scope: New canvas feature scope
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp`
- Related Features / Systems / Components: Canvas UI, design system, interaction feedback
- Current Behavior: Canvas visuals still reflect a thin placeholder shell.
- Intended Completed Behavior: Canvas presentation matches the product’s premium shell and interaction quality.
- Missing Pieces: Host-driven visual state, token adoption, icon normalization
- Technical Approach: Bind canvas UI states to the authoritative workbench and apply shared shell design tokens.
- Implementation Steps: Replace generic controls; align tool/inspector chrome; apply icon/state rules; polish minimap and selection visuals.
- Validation Steps: Review canvas UI across create/edit/select/export/restore flows.
- Acceptance Criteria: Canvas UI polish reflects real canvas state and no longer depends on placeholder shell assumptions.
- Dependencies: `P09-T01` through `P09-T04`, `P07-T02`, `P07-T03`
- Parallelization Notes: Follows behavior completion rather than leading it.
- Risks / Failure Modes: Visual work can regress if the underlying host model is still shifting.
- Cleanup / Migration Notes: Remove placeholder control treatments and Unicode icon residue.
- Observability / Diagnostics Notes: Capture canvas UI state mismatches in debug/audit tooling.
- Rollback / Safety Notes: Layer visual changes over stable state contracts only.
- References / Context: `v22` canvas UI findings
- Example scenarios where useful: The canvas inspector visually reflects locked, grouped, and selected states with the same clarity as editor-side shell surfaces.
