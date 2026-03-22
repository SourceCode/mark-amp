# Phase 07: Canvas Board Tool Import Export And Collaboration Completion

## Outcome

Finish the canvas subsystem where board lifecycle, tools, import/export, metadata, PDF, widget, and collaboration paths still rely on placeholders, deferred integrations, or low-fidelity implementations.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P07-T01

- Phase ID: P07
- Task ID: P07-T01
- Task Title: Replace placeholder canvas shell behavior with real board/workbench integration
- Priority: P0
- Category: Canvas Completion
- Atomic Completion Tasks Covered: 60
- Objective: Remove shell-level placeholder behavior that still stands between users and the richer canvas workbench.
- Why This Matters Now: Canvas is one of the clearest examples of split intent versus finished behavior.
- Completion Gap Statement: The visible canvas shell still exposes placeholder controls and panel-local behavior instead of fully adopting workbench services.
- User / Product Impact: Canvas workflows feel less trustworthy and less complete than editor workflows.
- Repository Evidence: [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), [CanvasWorkbench.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp), [LayoutManager.cpp](/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp)
- Scope: Shell actions, board open/create wiring, inspector mounting, workbench events, title and session sync.
- Out of Scope: Pure UI polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp`
- Related Features / Systems / Components: Canvas shell, boards, workbench, inspector.
- Current Behavior: Placeholder shell constructs still mediate core canvas behavior.
- Intended Completed Behavior: The shell delegates canvas behavior to finished board/workbench services with no placeholder shortcuts.
- Missing Pieces: Event topology cleanup, command routing, workbench ownership, and panel integration.
- Technical Approach: Make the workbench authoritative and demote `CanvasWorkspacePanel` to a surface host rather than a placeholder logic owner.
- Implementation Steps: Remove panel-local placeholders; route shell commands to workbench; align inspector and session state; finish event-bus integration.
- Validation Steps: Create/open/rename/save boards from all shell entry points and confirm workbench state stays authoritative.
- Acceptance Criteria: Canvas shell no longer owns placeholder board behavior.
- Dependencies: Phase 03.
- Risks / Failure Modes: Canvas tool state may still assume local panel ownership if migration is partial.
- Cleanup / Migration Notes where relevant: Remove Unicode placeholder icon and dummy control patterns during the migration.
- Observability / Diagnostics Notes where relevant: Log shell-to-workbench event transitions and board-state mismatches.
- Rollback / Safety Notes: Keep adapter shims only until all shell entry points route through the workbench.
- References / Context: This phase finishes the feature depth that the canvas subsystem already structurally suggests.
- Example scenarios where useful: Creating a canvas from the shell produces a real board object with workbench-backed lifecycle and restore support.

### P07-T02

- Phase ID: P07
- Task ID: P07-T02
- Task Title: Finish canvas import and export paths that still rely on placeholder rendering or metadata extraction
- Priority: P1
- Category: Canvas Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make canvas import/export features real rather than heuristic or placeholder-backed.
- Why This Matters Now: Import/export seams are a common place for “mostly there” feature claims to hide.
- Completion Gap Statement: PDF import, widget rendering, metadata scraping, and export paths still rely on placeholders or low-fidelity substitutes.
- User / Product Impact: Canvas portability and rich-media workflows remain incomplete.
- Repository Evidence: [PDFImporter.cpp](/Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp), [CanvasExportService.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp), [MetadataScraper.cpp](/Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp), [WidgetRenderer.cpp](/Users/ryanrentfro/code/markamp/src/canvas/WidgetRenderer.cpp)
- Scope: PDF page rendering, widget previews, metadata fetch, export fidelity, icon object export, loading/error states.
- Out of Scope: Canvas visual polish already handled in `v22`.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/PDFImporter.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/MetadataScraper.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/WidgetRenderer.cpp`
- Related Features / Systems / Components: Import, export, widgets, media, metadata.
- Current Behavior: Several rich-media flows still use placeholder rendering or heuristic metadata parsing.
- Intended Completed Behavior: Canvas import/export features use real renderers and robust extraction flows.
- Missing Pieces: PDF rasterization backend, widget runtime rendering, network metadata retrieval, export fidelity rules.
- Technical Approach: Finish the external integration points and normalize failures as explicit unsupported/error states instead of placeholders.
- Implementation Steps: Integrate PDF rendering; replace heuristic metadata fetch; implement widget preview loading; upgrade export for icon and media objects.
- Validation Steps: Import PDFs, scrape links, export rich boards, and compare outputs against expected fixtures.
- Acceptance Criteria: Canvas import/export no longer depends on placeholder or heuristic stand-ins for supported workflows.
- Dependencies: Phase 12, Phase 13.
- Risks / Failure Modes: External library integration can introduce performance and licensing concerns if not bounded.
- Cleanup / Migration Notes where relevant: Retire placeholder export and metadata comments after replacing them.
- Observability / Diagnostics Notes where relevant: Track import/export failures by object type and external backend.
- Rollback / Safety Notes: Keep unsupported importers and renderers explicitly unavailable if dependencies are missing.
- References / Context: This closes the gap between canvas object breadth and actual portability depth.
- Example scenarios where useful: Importing a PDF yields real page renders instead of one heuristic page shell per document.

### P07-T03

- Phase ID: P07
- Task ID: P07-T03
- Task Title: Complete canvas tool, selection, and object interaction seams that are still deferred
- Priority: P1
- Category: Canvas Completion
- Atomic Completion Tasks Covered: 60
- Objective: Finish the tool and interaction paths that still include deferred comments or placeholder logic.
- Why This Matters Now: Canvas interaction credibility depends on details that are currently deferred.
- Completion Gap Statement: Selection, hit-testing, key handling, and some object interaction paths still explicitly defer deeper behavior.
- User / Product Impact: Canvas interaction can feel broad but not fully reliable.
- Repository Evidence: [SelectTool.cpp](/Users/ryanrentfro/code/markamp/src/canvas/SelectTool.cpp), [HitTester.cpp](/Users/ryanrentfro/code/markamp/src/node_editor/HitTester.cpp), [SelectionManager.cpp](/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp)
- Scope: Key handling, selection integration, hit-testing fidelity, object-specific edit actions, separators/placeholder logic.
- Out of Scope: Pure animation and visual feedback refinements.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/SelectTool.cpp`; `/Users/ryanrentfro/code/markamp/src/node_editor/HitTester.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.h`
- Related Features / Systems / Components: Canvas tools, selection, object interactions, keybinding behavior.
- Current Behavior: Some interaction branches still rely on deferred or simplified behavior.
- Intended Completed Behavior: Canvas tools behave deterministically and completely across pointer and keyboard input.
- Missing Pieces: Full hit-testing, keyboard interaction coverage, and object-action completion.
- Technical Approach: Close deferred paths in tool and selection logic while aligning them with the canonical input/state models.
- Implementation Steps: Finish deferred key handling; improve link and frame hit testing; remove placeholder separators; complete object-specific actions.
- Validation Steps: Exercise selection, keyboard shortcuts, object edits, and edge-case hit targets across dense boards.
- Acceptance Criteria: Canvas interaction paths no longer contain deferred “real logic later” seams for supported tools.
- Dependencies: P07-T01.
- Risks / Failure Modes: Fixing interaction fidelity can expose latent assumptions in undo/redo and collaboration.
- Cleanup / Migration Notes where relevant: Delete deferred comments once behavior is finished and covered.
- Observability / Diagnostics Notes where relevant: Capture tool-state transitions and hit-test fallbacks for debugging.
- Rollback / Safety Notes: Keep behavior flags for new hit-testing logic until parity is confirmed.
- References / Context: Interaction depth is where canvas breadth becomes a real professional tool.
- Example scenarios where useful: Selecting connectors and frame-linked objects no longer depends on simplified placeholder distance checks.

### P07-T04

- Phase ID: P07
- Task ID: P07-T04
- Task Title: Finish collaborative, template, and board-automation paths that still stop at structural placeholders
- Priority: P2
- Category: Canvas Completion
- Atomic Completion Tasks Covered: 60
- Objective: Move secondary canvas workflows from structural shells to finished behavior.
- Why This Matters Now: Collaboration and templates are exposed parts of the canvas value proposition.
- Completion Gap Statement: Several board-level flows still describe what a real implementation would do instead of doing it.
- User / Product Impact: Advanced canvas workflows remain shallow or inconsistent.
- Repository Evidence: [CanvasTemplateEngine.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp), [CanvasIntegrationService.cpp](/Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp), [CollabUndoRedoManager.h](/Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h)
- Scope: Board templates, collaboration sync hooks, widget lifecycle integration, automation services.
- Out of Scope: Realtime protocol redesign beyond completing intended behavior.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasIntegrationService.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CollabUndoRedoManager.h`; `/Users/ryanrentfro/code/markamp/src/canvas/WidgetLifecycleManager.h`
- Related Features / Systems / Components: Templates, collaboration, integrations, automation.
- Current Behavior: Some advanced paths remain descriptive shells around incomplete implementations.
- Intended Completed Behavior: Advanced board workflows operate through real services, not comments or structural placeholders.
- Missing Pieces: Template parsing, integration job execution, collaboration replay semantics, widget lifecycle completion.
- Technical Approach: Finish the intended backend work for already-exposed advanced board features and add explicit gates where not yet ready.
- Implementation Steps: Implement template parsing and object creation; finish integration service job loops; verify collaboration undo semantics; complete widget lifecycle hooks.
- Validation Steps: Apply templates, run integration hooks, and replay collaborative operations in deterministic test scenarios.
- Acceptance Criteria: Advanced canvas workflows no longer rely on “real implementation would…” comments in exposed paths.
- Dependencies: P07-T01 through P07-T03.
- Risks / Failure Modes: Some advanced flows may need feature gating if backend maturity is lower than expected.
- Cleanup / Migration Notes where relevant: Remove placeholder service loops and incomplete object construction helpers.
- Observability / Diagnostics Notes where relevant: Emit template-apply, integration-job, and collaboration replay diagnostics.
- Rollback / Safety Notes: Gate partially completed advanced flows rather than exposing them optimistically.
- References / Context: This phase distinguishes real canvas depth from structural aspiration.
- Example scenarios where useful: Applying a board template creates actual objects instead of relying on a comment that JSON parsing will come later.

### P07-T05

- Phase ID: P07
- Task ID: P07-T05
- Task Title: Add canvas completion harnesses for lifecycle, import/export, and tool fidelity
- Priority: P0
- Category: Testing / Regression Protection
- Atomic Completion Tasks Covered: 60
- Objective: Protect canvas completion work with scenario-level coverage instead of isolated model tests only.
- Why This Matters Now: Canvas has many rich seams where regressions can hide behind breadth.
- Completion Gap Statement: Current coverage catches some model behavior but does not fully guarantee finished canvas workflows.
- User / Product Impact: Advanced boards, imports, and exports can regress without being noticed.
- Repository Evidence: [test_phase12_canvas_advanced.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase12_canvas_advanced.cpp), [test_phase14_canvas_extensibility.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase14_canvas_extensibility.cpp)
- Scope: Board lifecycle, import/export fidelity, selection tools, templates, and collaboration replay.
- Out of Scope: Pure pixel-level UI verification.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit/test_phase12_canvas_advanced.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase14_canvas_extensibility.cpp`; `/Users/ryanrentfro/code/markamp/tests/integration`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Features / Systems / Components: Canvas, tests, regressions, smoke coverage.
- Current Behavior: Some tests still encode unimplemented import or deserialization expectations.
- Intended Completed Behavior: Tests assert real canvas behavior and reject regressions back into placeholder states.
- Missing Pieces: Scenario fixtures, board round-trip tests, collaboration harnesses, importer/exporter validations.
- Technical Approach: Upgrade current unit tests and add integration fixtures covering mixed-object boards and import/export round trips.
- Implementation Steps: Replace unimplemented-path expectations; add real import/export tests; add tool-flow harnesses; create smoke boards for regression suites.
- Validation Steps: Break a finished importer or selection flow intentionally and verify the harness fails.
- Acceptance Criteria: Canvas completion regressions are detectable automatically.
- Dependencies: P07-T01 through P07-T04.
- Risks / Failure Modes: Board fixtures can become brittle if object serialization is unstable during migration.
- Cleanup / Migration Notes where relevant: Remove tests that explicitly bless unimplemented canvas deserialization.
- Observability / Diagnostics Notes where relevant: Store round-trip diffs and importer/exporter logs with failed runs.
- Rollback / Safety Notes: Keep fixtures versioned as serialization evolves.
- References / Context: This phase keeps canvas from remaining a broad but unfinished subsystem.
- Example scenarios where useful: A PDF import/export round trip across a real board is protected by integration coverage.
