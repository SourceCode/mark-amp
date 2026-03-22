# Phase 09: Canvas Shell, Workbench, Persistence, And Interaction Integrity

## Phase Intent

Close the gap between canvas model capability and canvas shell trustworthiness.

## Release-Ready Exit Criteria

- canvas creation, open, save, duplicate, and restore are shell-authoritative,
- the canvas shell no longer uses placeholder event topology or placeholder controls on the release path,
- core interaction, persistence, and panel behavior are dependable.

## Task Count

3

## Task P09-T01

- Phase ID: P09
- Task ID: P09-T01
- Task Title: Remove `CanvasWorkspacePanel` placeholder shell behavior and bind it to the real canvas workbench
- Priority: P0
- Category: Canvas Hardening
- Objective: replace local placeholder shell behavior with real workbench-backed canvas ownership.
- Why This Matters Now: the canvas release path still visibly exposes placeholder shell patterns.
- Release Gap Statement: `CanvasWorkspacePanel` remains visually and architecturally thinner than the underlying canvas system.
- User / Product Impact: canvas will feel unreliable or toy-like despite deeper model investment.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Prior Plan References: `v20 Phase 04`; `v22 Phase 10`; `v24 Phase 09`
- Scope: board creation/open, shell controls, inspector, minimap, event bus ownership, board title and active context.
- Out of Scope: advanced canvas collaboration depth beyond release-path basics.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInspector.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: canvas shell; workbench; inspector; minimap; event bus
- Current Behavior: `CanvasWorkspacePanel` still uses Unicode placeholder icons, placeholder inspector text, and a private event bus.
- Intended Release-Ready Behavior: the visible canvas shell is a real adapter over the authoritative canvas subsystem and shell state.
- Missing Pieces: event-bus correction; workbench binding; release-path control replacement.
- Technical Approach: make `CanvasWorkspacePanel` consume the main shell event bus and real workbench services instead of local placeholder state.
- Implementation Steps:
1. Remove private event bus creation and placeholder board creation behavior.
2. Bind visible controls to real workbench commands and selection state.
3. Replace placeholder shell copy and controls on the release path.
- Validation Steps:
1. Create, open, and switch boards through shell controls.
2. Verify inspector, minimap, and tool state follow real board state.
- Acceptance Criteria: canvas shell is no longer architecturally isolated from the main application shell.
- Dependencies: P02-T03; P04-T01
- Parallelization Notes: persistence and interaction work can proceed once shell ownership is stable.
- Risks / Failure Modes: event routing regressions; stale board IDs; shell/workbench desync.
- Observability / Diagnostics Notes: expose current board ID, tool, and selection diagnostics.
- Rollback / Safety Notes: keep workbench model as source of truth during migration.
- References / Context: `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`

## Task P09-T02

- Phase ID: P09
- Task ID: P09-T02
- Task Title: Make canvas artifact persistence and reopen flows trustworthy
- Priority: P0
- Category: Canvas Hardening
- Objective: ensure canvas boards are saved, duplicated, renamed, reopened, and restored through the artifact spine and canvas persistence layer.
- Why This Matters Now: canvas cannot be release-grade without dependable lifecycle and reopen continuity.
- Release Gap Statement: canvas workbench capability is ahead of shell and persistence convergence.
- User / Product Impact: users need confidence that canvas work is durable and recoverable like other artifacts.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`; `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Prior Plan References: `v17 Phase 10`; `v24 Phase 09`
- Scope: board save/save as, duplicate, rename, reopen, restore, recent-item continuity.
- Out of Scope: deep collaborative board conflict resolution.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/BoardSerializer.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasExportService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/ArtifactRegistry.cpp`
- Related Features / Systems / Components: board serializer; artifact registry; session restore; recent items
- Current Behavior: workbench lifecycle and shell lifecycle are not yet fully unified.
- Intended Release-Ready Behavior: canvas boards obey the same save and restore trust guarantees as other artifacts.
- Missing Pieces: persistence orchestration alignment; reopen tests; duplicate and rename contract checks.
- Technical Approach: route board persistence through the same shell persistence authority used by other artifact kinds while preserving canvas-specific serialization.
- Implementation Steps:
1. Bind board lifecycle operations into the artifact registry and persistence coordinator.
2. Validate rename/duplicate/save semantics.
3. Add save/reopen/restart smoke coverage.
- Validation Steps:
1. Save and reopen boards from multiple shell entry points.
2. Restart and confirm board reopen continuity.
- Acceptance Criteria: canvas boards pass lifecycle smoke scenarios without shell-local shortcuts.
- Dependencies: P03-T01; P09-T01
- Parallelization Notes: interaction stability can proceed in parallel after lifecycle contracts stabilize.
- Risks / Failure Modes: wrong board-path tracking; stale session records; duplicate board identity.
- Observability / Diagnostics Notes: log board artifact state transitions and serializer failures.
- Rollback / Safety Notes: preserve readable import/open compatibility for older board files.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.cpp`

## Task P09-T03

- Phase ID: P09
- Task ID: P09-T03
- Task Title: Stabilize release-path canvas interaction and selection workflows
- Priority: P1
- Category: Canvas Hardening
- Objective: harden the core interaction loop users actually depend on before release.
- Why This Matters Now: rich canvas systems do not matter if basic interaction still feels fragile.
- Release Gap Statement: shell and interaction polish still lag behind deeper canvas subsystem breadth.
- User / Product Impact: unstable select, pan, edit, and persistence behavior breaks trust quickly in canvas workflows.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/SelectionManager.h`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasInputManager.cpp`
- Prior Plan References: `v17 Phase 01`; `v17 Phase 02`; `v24 Phase 09`
- Scope: tool switching, selection, object manipulation, dirty-state updates, save prompts after interaction.
- Out of Scope: advanced shape recognition or multiplayer polish outside release-critical trust.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/canvas/UndoRedoStack.h`; `/Users/ryanrentfro/code/markamp/src/canvas/SnapEngine.h`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasRenderer.cpp`
- Related Features / Systems / Components: tools; selection; dirty state; undo/redo
- Current Behavior: the canvas subsystem is broad, but release-path interaction still needs tighter shell-grade proof.
- Intended Release-Ready Behavior: primary interaction workflows are smooth, state-correct, and persistence-aware.
- Missing Pieces: interaction smoke tests; dirty-state coupling; selection/tool consistency checks.
- Technical Approach: focus validation and bug fixing on the minimum release-path interaction matrix rather than broad canvas expansion.
- Implementation Steps:
1. Define the release-path interaction matrix.
2. Fix dirty-state and undo/redo coupling bugs found in the matrix.
3. Add smoke coverage for primary tools and object edits.
- Validation Steps:
1. Exercise create/select/move/edit/delete/undo/redo/save flows.
2. Verify dirty-state and restore integrity.
- Acceptance Criteria: core canvas interactions behave predictably and pass smoke coverage.
- Dependencies: P09-T01; P09-T02
- Parallelization Notes: suitable for parallel implementation and validation workers.
- Risks / Failure Modes: hidden interaction regressions; incorrect dirty-state updates.
- Observability / Diagnostics Notes: record interaction and undo/redo event summaries during smoke runs.
- Rollback / Safety Notes: prioritize correctness over flashy interaction polish.
- References / Context: `/Users/ryanrentfro/code/markamp/src/canvas/CanvasToolHost.cpp`
