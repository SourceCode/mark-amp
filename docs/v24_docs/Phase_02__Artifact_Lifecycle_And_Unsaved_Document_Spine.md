# Phase 02 - Artifact Lifecycle And Unsaved Document Spine

## Phase Goal

Make text files, notebooks, and canvases use one authoritative creation and unsaved-artifact model instead of divergent UI-local strategies.

## Measurable Outcome

- No primary artifact family relies on pseudo untitled paths.
- All creation entry points route through shared artifact services.
- Shell surfaces can reason about unsaved state consistently.

### Task P02-T01

- Phase ID: `P02`
- Task ID: `P02-T01`
- Task Title: Replace pseudo untitled paths with canonical unsaved artifact records
- Priority: `P0`
- Category: `Artifact Lifecycle`
- Objective: Eliminate fake file identifiers like `Untitled-N.md` and `untitled:` from primary creation paths.
- Why This Matters Now: Split untitled strategies are still the root cause of broken creation and save flows.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` and `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` still treat unsaved content as path-like strings.
- User / Product Impact: Users cannot reliably create, rename, save, reopen, or restore new artifacts.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`
- Prior Plan References: `v20` Phase 01 and 02, `v23` Phase 03
- Scope: Text file, notebook, and canvas unsaved identity design
- Out of Scope: Final save-dialog UX polish
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TextArtifactLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`
- Related Features / Systems / Components: Artifact IDs, tab titles, dirty state, session restore
- Current Behavior: Multiple surfaces synthesize pseudo paths and hope later save logic resolves them.
- Intended Completed Behavior: Unsaved artifacts have real IDs, metadata, dirty state, and optional future save targets.
- Missing Pieces: Canonical unsaved-artifact model adoption in shell and tabs
- Technical Approach: Make artifact records first-class and keep path assignment separate until save target confirmation.
- Implementation Steps: Extend artifact record model; migrate tab identity; route creation through lifecycle services; remove pseudo-path checks.
- Validation Steps: Create unsaved file, notebook, and canvas from all entry points; rename/save/restore each.
- Acceptance Criteria: No production creation path depends on fake path strings to represent unsaved primary artifacts.
- Dependencies: `P01-T02`, `P01-T03`
- Parallelization Notes: Blocks most later lifecycle and shell work.
- Risks / Failure Modes: Mixed identity schemes can corrupt restore and dirty-state behavior during migration.
- Cleanup / Migration Notes: Retire pseudo untitled code branches in tab and layout logic.
- Observability / Diagnostics Notes: Log artifact creation source, artifact ID, save target assignment, and migration fallback usage.
- Rollback / Safety Notes: Maintain a temporary compatibility adapter while removing old pseudo-path branches.
- References / Context: `/Users/ryanrentfro/code/markamp/src/core/FileEntryPointResolver.h`
- Example scenarios where useful: `Cmd+N` creates a text artifact with a generated artifact ID but no filesystem path until first save.

### Task P02-T02

- Phase ID: `P02`
- Task ID: `P02-T02`
- Task Title: Route every creation entry point through one artifact creation service
- Priority: `P0`
- Category: `Core Workflow Completion`
- Objective: Make menus, toolbar, tab bar, command palette, explorer, and empty states converge on one creation chain.
- Why This Matters Now: Creation is still split by surface.
- Execution Gap Statement: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, and main-shell flows still create artifacts differently.
- User / Product Impact: Users see inconsistent naming, placement, tab activation, and dirty-state behavior depending on where they click.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/core/FileEntryPointResolver.h`
- Prior Plan References: `v20` Phase 02-04, `v21` Phase 02-04
- Scope: All user-visible entry points for new file, new notebook, and new canvas
- Out of Scope: Secondary template-library expansion
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.h`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Features / Systems / Components: Commands, entry-point resolution, artifact registration
- Current Behavior: Different surfaces create different initial objects and lifecycle states.
- Intended Completed Behavior: One creation service resolves placement, metadata, surface activation, and initial content for every artifact family.
- Missing Pieces: Unified creation API and shell adoption
- Technical Approach: Create a shell-owned artifact factory that accepts entry-point context and desired artifact type.
- Implementation Steps: Define creation request model; migrate each entry point; remove direct file-system or tab-only creation logic.
- Validation Steps: Compare creation results across all entry points for file, notebook, and canvas.
- Acceptance Criteria: Each creation entry point produces the same artifact metadata and shell state for the same artifact type.
- Dependencies: `P02-T01`
- Parallelization Notes: Can begin once the unsaved record model is defined.
- Risks / Failure Modes: Partial migration leaves two active creation paths and reintroduces inconsistency.
- Cleanup / Migration Notes: Delete or gate direct creation branches after migration.
- Observability / Diagnostics Notes: Emit creation-source and creation-type telemetry for smoke runs.
- Rollback / Safety Notes: Keep old entry points as thin delegates until full migration is verified.
- References / Context: `v20` assessment creation-flow chain
- Example scenarios where useful: New notebook from command palette and new notebook from welcome surface must resolve identically.

### Task P02-T03

- Phase ID: `P02`
- Task ID: `P02-T03`
- Task Title: Standardize initial metadata, bootstrap content, and naming contracts
- Priority: `P1`
- Category: `Artifact Lifecycle`
- Objective: Ensure new artifacts start with deterministic initial state.
- Why This Matters Now: Creation consistency is not only about persistence; it also affects presentation, trust, and restore.
- Execution Gap Statement: Existing creation flows vary in default name, content, placement, and open/focus behavior.
- User / Product Impact: New artifacts can open in unexpected places or with incomplete initial content.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`, `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.h`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasTemplateEngine.h`
- Prior Plan References: `v20` Phase 02-04
- Scope: Default metadata, templates, bootstrap content, display labels
- Out of Scope: User-authored template gallery
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/FileEntryPointResolver.h`, `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.cpp`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Features / Systems / Components: Templates, startup content, display naming
- Current Behavior: Defaults are scattered across UI and service layers.
- Intended Completed Behavior: Defaults are owned centrally and applied consistently by artifact type.
- Missing Pieces: Shared bootstrap policy and metadata contract
- Technical Approach: Introduce artifact bootstrap descriptors with per-type defaults and template hooks.
- Implementation Steps: Define descriptors; move defaults out of UI controls; attach template resolution; update tab and tree labels.
- Validation Steps: Snapshot bootstrap state for each artifact type and entry point.
- Acceptance Criteria: New artifacts of the same type are indistinguishable in metadata and initial state regardless of entry point.
- Dependencies: `P02-T02`
- Parallelization Notes: Can run with `P02-T04` after creation routing stabilizes.
- Risks / Failure Modes: Template and bootstrap code can drift if still partially local.
- Cleanup / Migration Notes: Remove hardcoded default titles from UI event handlers.
- Observability / Diagnostics Notes: Record bootstrap descriptor IDs in creation logs.
- Rollback / Safety Notes: Keep legacy defaults available behind compatibility toggles during migration.
- References / Context: `CanvasTemplateEngine`, notebook creation services
- Example scenarios where useful: A new Markdown file opens with canonical empty content and default language mode, not whatever the originating surface guessed.

### Task P02-T04

- Phase ID: `P02`
- Task ID: `P02-T04`
- Task Title: Normalize tab, tree, and workspace registration for unsaved artifacts
- Priority: `P1`
- Category: `Core Workflow Completion`
- Objective: Make unsaved artifacts appear consistently across visible navigation surfaces.
- Why This Matters Now: The shell still mixes path-based and artifact-based assumptions.
- Execution Gap Statement: Unsaved artifacts can appear in tabs without coherent explorer or workspace representation.
- User / Product Impact: Navigation, selection, and restore feel unreliable.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/ExplorerPanel.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Prior Plan References: `v19` workspace/session flows, `v20` Phase 05
- Scope: Tab metadata, open editors, explorer projection, recent items placeholder semantics
- Out of Scope: Final MRU ranking heuristics
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/ActiveDocumentTracker.h`, `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`, `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
- Related Features / Systems / Components: Tab identity, navigation history, explorer state
- Current Behavior: Navigation surfaces reason differently about pathless or not-yet-saved artifacts.
- Intended Completed Behavior: All shell surfaces display and track unsaved artifacts coherently.
- Missing Pieces: Artifact-to-surface projection rules
- Technical Approach: Build a shared projection layer from artifact record to visible shell surface state.
- Implementation Steps: Define projection model; update tabs/open-editors/explorer/recent items; handle save-target changes cleanly.
- Validation Steps: Create unsaved artifacts and verify coherent state across all navigation surfaces.
- Acceptance Criteria: Unsaved artifacts remain stable across shell surfaces before and after first save.
- Dependencies: `P02-T01`, `P02-T02`
- Parallelization Notes: Can proceed once artifact IDs and creation routing are stable.
- Risks / Failure Modes: Surfaces can desynchronize if some still key by path.
- Cleanup / Migration Notes: Remove duplicate local projections once shared model exists.
- Observability / Diagnostics Notes: Log projection updates when artifact IDs gain save targets.
- Rollback / Safety Notes: Provide compatibility mapping from old path-based keys during migration.
- References / Context: ActiveDocumentTracker and navigation surfaces
- Example scenarios where useful: Saving an unsaved notebook replaces its display label everywhere without losing active-tab or recent-item continuity.

### Task P02-T05

- Phase ID: `P02`
- Task ID: `P02-T05`
- Task Title: Define artifact-family-specific completion rules on top of the shared spine
- Priority: `P1`
- Category: `Core Workflow Completion`
- Objective: Prevent notebook and canvas from re-diverging after shared artifact work lands.
- Why This Matters Now: Shared lifecycle work will fail if each artifact family keeps its own shell exceptions.
- Execution Gap Statement: Notebook and canvas already have partial lifecycle hosts that can drift back into local rules.
- User / Product Impact: Users would still experience three different products inside one app.
- Repository Evidence: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`, `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`, `/Users/ryanrentfro/code/markamp/src/core/TextArtifactLifecycle.cpp`
- Prior Plan References: `v19` notebook/canvas phases, `v20` Phase 03-04, `v23` Phase 06-07
- Scope: Family-specific rules for bootstrap, save semantics, restore, and shell activation
- Out of Scope: Advanced notebook kernels or canvas collaboration features
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookArtifactLifecycle.h`, `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.h`
- Related Features / Systems / Components: Artifact family policies, shared spine
- Current Behavior: Families already differ in ownership and save assumptions.
- Intended Completed Behavior: Family-specific rules sit on top of, not outside of, the shared artifact spine.
- Missing Pieces: Explicit policy contracts and shared assertions
- Technical Approach: Define artifact-family adapters with required shared lifecycle guarantees.
- Implementation Steps: Create policy interface; attach to text/notebook/canvas; verify shell invariants.
- Validation Steps: Run the same lifecycle smoke suite across all three artifact families.
- Acceptance Criteria: Family-specific behavior never bypasses the shared artifact creation, dirty-state, save, and restore contracts.
- Dependencies: `P02-T01` through `P02-T04`
- Parallelization Notes: Feeds directly into notebook and canvas phases.
- Risks / Failure Modes: Exceptions introduced for convenience can recreate split behavior.
- Cleanup / Migration Notes: Remove family-local lifecycle logic that duplicates shared rules.
- Observability / Diagnostics Notes: Emit invariant violations during development builds.
- Rollback / Safety Notes: Keep shared assertions non-fatal initially, then promote to hard validation.
- References / Context: Shared artifact-family convergence
- Example scenarios where useful: A canvas may save to a board file and a notebook to notebook JSON, but both still obey the same shell dirty-state and restore contracts.
