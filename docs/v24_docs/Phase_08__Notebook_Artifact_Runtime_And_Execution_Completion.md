# Phase 08 - Notebook Artifact, Runtime, And Execution Completion

## Phase Goal

Promote notebooks from partial model support to first-class shell artifacts with reliable creation, execution, save, restore, and surface behavior.

## Measurable Outcome

- Notebooks use the same artifact spine as text and canvas.
- Kernel and execution flows are no longer synthetic in production.
- Notebook shell behavior is restoreable, command-complete, and trustworthy.

### Task P08-T01

- Phase ID: `P08`
- Task ID: `P08-T01`
- Task Title: Bind notebook creation, open, save, and restore to the shared artifact spine
- Priority: `P0`
- Category: `Notebook Completion`
- Objective: Ensure notebooks no longer bypass workspace-grade artifact lifecycle rules.
- Why This Matters Now: Notebook lifecycle remains split between model code and thin shell integration.
- Execution Gap Statement: Notebook creation and shell hosting still do not fully own persistence, restore, and tab integration.
- User / Product Impact: Users cannot trust notebooks as first-class IDE documents.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`
- Prior Plan References: `v20` Phase 03, `v23` Phase 06
- Scope: Notebook artifact registration, shell activation, dirty state, save/restore
- Out of Scope: Advanced collaborative execution
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`
- Related Features / Systems / Components: Notebook lifecycle, tabs, persistence, session restore
- Current Behavior: Notebook support is real in parts, but still not integrated like a core IDE artifact family.
- Intended Completed Behavior: Notebooks behave like first-class artifacts in creation, save, restore, and navigation flows.
- Missing Pieces: Shared artifact spine adoption in notebook host paths
- Technical Approach: Make notebook lifecycle services comply with the shared artifact-family adapter contract from Phase 02.
- Implementation Steps: Replace direct event-only notebook creation; wire host to artifact records; persist notebook open state.
- Validation Steps: Create, edit, save, autosave, restore, and reopen notebooks through all shell entry points.
- Acceptance Criteria: Notebook lifecycle is indistinguishable in shell trust from text-document lifecycle.
- Dependencies: `P02-T05`, `P03-T03`
- Parallelization Notes: Foundation for all later notebook runtime and UI work.
- Risks / Failure Modes: Notebook-specific metadata may be lost if artifact models are too file-centric.
- Cleanup / Migration Notes: Remove shell-notification-only notebook branches after full lifecycle routing.
- Observability / Diagnostics Notes: Emit notebook artifact lifecycle events with host and persistence outcomes.
- Rollback / Safety Notes: Maintain notebook-session compatibility for existing saved work.
- References / Context: Notebook artifact and shell host files
- Example scenarios where useful: A new notebook created from the menu restores in the correct tab group after restart.

### Task P08-T02

- Phase ID: `P08`
- Task ID: `P08-T02`
- Task Title: Replace stubbed kernel lifecycle with real process and execution plumbing
- Priority: `P0`
- Category: `Notebook Completion`
- Objective: Move notebook execution out of the current simulated kernel model.
- Why This Matters Now: Execution is the central notebook value proposition and remains explicitly stubbed.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp` still simulates lifecycle and ZeroMQ behavior.
- User / Product Impact: Notebook execution cannot be trusted.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/KernelManager.cpp`
- Prior Plan References: `v19` Phase 07, `v23` Phase 06
- Scope: Kernel discovery, start/stop, interrupt, restart, execute, result handling
- Out of Scope: New kernel types beyond current supported adapters
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/KernelAdapters.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookExecutionPipeline.cpp`
- Related Features / Systems / Components: Kernels, execution pipeline, notebook runtime
- Current Behavior: Kernel lifecycle is synthetic.
- Intended Completed Behavior: Notebook execution uses real process and protocol integration with observable state transitions.
- Missing Pieces: Process transport, adapter completion, result streaming, failure handling
- Technical Approach: Implement real kernel transport and adapter lifecycle while keeping test doubles separate.
- Implementation Steps: Complete adapter detection; add process spawn/connection logic; wire results, errors, and interrupts.
- Validation Steps: Execute representative notebook cells, interrupt kernels, restart, and recover from failure.
- Acceptance Criteria: Production notebook execution no longer depends on simulated kernel lifecycle.
- Dependencies: `P08-T01`
- Parallelization Notes: Can proceed with notebook shell work, but runtime behavior gates final signoff.
- Risks / Failure Modes: Process and transport failures can destabilize the shell without strict error handling.
- Cleanup / Migration Notes: Move simulation behavior into explicit test-only doubles.
- Observability / Diagnostics Notes: Log kernel state transitions, execution IDs, transport errors, and restart attempts.
- Rollback / Safety Notes: Keep notebook open/read-only behavior available if execution backends fail.
- References / Context: Kernel manager and adapters
- Example scenarios where useful: Running a code cell changes kernel state to busy, streams output, and returns to idle without synthetic shortcuts.

### Task P08-T03

- Phase ID: `P08`
- Task ID: `P08-T03`
- Task Title: Complete notebook cell actions, variable inspection, and command routing
- Priority: `P1`
- Category: `Notebook Completion`
- Objective: Make notebook cell-level operations real and context-aware.
- Why This Matters Now: Cell UI without trustworthy action chains still leaves notebooks feeling partial.
- Execution Gap Statement: Variable inspection and some cell action support remain incomplete or weakly routed.
- User / Product Impact: Users cannot rely on notebook affordances beyond basic editing.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookCellManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookCellManager.h`
- Prior Plan References: `v21` control integrity, `v23` Phase 06
- Scope: Run cell, run all, move, duplicate, delete, collapse, inspect variables, magic commands
- Out of Scope: Advanced collaborative notebook editing
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/MagicCommandEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/WidgetManager.cpp`
- Related Features / Systems / Components: Cell actions, notebook commands, variable inspection
- Current Behavior: Some cell affordances are present, but deeper behavior remains incomplete.
- Intended Completed Behavior: Cell actions and notebook-specific commands route through canonical handlers and runtime services.
- Missing Pieces: Variable-inspection completion, command routing, output/widget follow-through
- Technical Approach: Bind notebook cell actions to the shared action model with notebook-context keys and runtime adapters.
- Implementation Steps: Inventory cell actions; route commands; implement or gate variable inspection; complete magic-command execution links.
- Validation Steps: Trigger all visible cell actions and confirm effect, state update, and recovery behavior.
- Acceptance Criteria: Notebook cell affordances are real, routed, and correctly gated by runtime availability.
- Dependencies: `P04-T03`, `P08-T02`
- Parallelization Notes: Can proceed once kernel capability state is exposed.
- Risks / Failure Modes: Cell action states can drift from runtime state if context propagation is weak.
- Cleanup / Migration Notes: Hide any remaining pseudo-actions until implemented.
- Observability / Diagnostics Notes: Emit notebook action dispatch, runtime capability, and result diagnostics.
- Rollback / Safety Notes: Maintain safe no-data fallback for variable views when runtime omits support.
- References / Context: Cell manager, magic engine, widget manager
- Example scenarios where useful: `Run Cell and Advance` executes, updates output, moves focus, and keeps notebook dirty state accurate.

### Task P08-T04

- Phase ID: `P08`
- Task ID: `P08-T04`
- Task Title: Finish notebook output persistence, diff, and mixed-content restore behavior
- Priority: `P1`
- Category: `Notebook Completion`
- Objective: Make outputs and cell state survive save, reload, and comparison workflows correctly.
- Why This Matters Now: Notebook trust depends on content fidelity, not just successful execution.
- Execution Gap Statement: Notebook shell and output handling still lag behind a full document-grade lifecycle.
- User / Product Impact: Outputs can be lost, restored incorrectly, or appear inconsistent across sessions.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookExportEngine.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookDiffEngine.cpp`
- Prior Plan References: `v20` Phase 03 and 08, `v22` Phase 09
- Scope: Output persistence, reload fidelity, diff support, rich output restore
- Out of Scope: New output media types beyond existing model
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookSerializer.cpp`
- Related Features / Systems / Components: Notebook outputs, serialization, diffing
- Current Behavior: Output handling is present but not yet proven as full lifecycle state.
- Intended Completed Behavior: Notebook output state persists and restores faithfully, and diff/merge tools understand it.
- Missing Pieces: Output-state schema discipline and restore tests
- Technical Approach: Make notebook serializer and diff engine authoritative over both cell content and output state.
- Implementation Steps: Audit serializer; normalize output payload persistence; update diff engine; add restore assertions.
- Validation Steps: Save/reopen executed notebooks and compare serialized output fidelity and diffs.
- Acceptance Criteria: Notebook output content survives save/restore and diff flows with defined fidelity rules.
- Dependencies: `P08-T01`, `P08-T02`
- Parallelization Notes: Can run with notebook UI surfacing once execution results are real.
- Risks / Failure Modes: Large output payloads can create performance and persistence issues.
- Cleanup / Migration Notes: Remove transient-only output assumptions from shell logic.
- Observability / Diagnostics Notes: Emit output serialization size, truncation, and restore mismatch diagnostics.
- Rollback / Safety Notes: Support graceful omission rules for unsupported transient outputs with explicit UI disclosure.
- References / Context: Notebook serializer and output renderer
- Example scenarios where useful: A notebook with markdown, code, image output, and text output reopens with intact structure and display.

### Task P08-T05

- Phase ID: `P08`
- Task ID: `P08-T05`
- Task Title: Tie notebook UI polish to the now-authoritative notebook host
- Priority: `P2`
- Category: `UI Quality / Styling`
- Objective: Apply notebook visual-system work only after notebook behavior is trustworthy.
- Why This Matters Now: Earlier UI plans were right, but notebook polish should now attach to the completed host.
- Execution Gap Statement: Notebook UI work risks decorating a shell that is still behaviorally incomplete.
- User / Product Impact: Polished-looking notebooks with unreliable behavior still feel broken.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookCellChrome.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CellOutputRenderer.cpp`
- Prior Plan References: `v22` Phase 09, `v24` Phase 07
- Scope: Cell chrome, output spacing, execution states, toolbar styling, active/focus states
- Out of Scope: Notebook runtime behavior itself
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookToolbar.cpp`
- Related Features / Systems / Components: Notebook UI, visual system, runtime states
- Current Behavior: Notebook visual language is still thinner than its intended product role.
- Intended Completed Behavior: Notebook UI communicates execution, structure, and mixed-content state with premium clarity.
- Missing Pieces: Host-driven visual state model and token adoption
- Technical Approach: Bind notebook chrome and output UI to authoritative runtime and artifact state first, then apply shared design tokens.
- Implementation Steps: Expose state model; migrate notebook UI surfaces; align with design-system and icon rules.
- Validation Steps: Visual and interaction review across idle, running, error, and restored notebook states.
- Acceptance Criteria: Notebook UI polish reflects real notebook state and no longer masks workflow gaps.
- Dependencies: `P08-T01` through `P08-T04`, `P07-T02`
- Parallelization Notes: Follows core notebook completion rather than leading it.
- Risks / Failure Modes: UI can still drift if it reintroduces local notebook-only style logic.
- Cleanup / Migration Notes: Remove thin placeholder chrome once premium host UI is complete.
- Observability / Diagnostics Notes: Capture notebook UI state mismatches in debug builds.
- Rollback / Safety Notes: Keep notebook UI changes layered over stable state contracts.
- References / Context: `v22` notebook UI assessment
- Example scenarios where useful: A running cell shows the correct execution, selection, and output states while remaining visually consistent with the rest of the shell.
