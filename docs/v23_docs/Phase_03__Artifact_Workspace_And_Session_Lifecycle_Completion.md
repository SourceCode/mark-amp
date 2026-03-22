# Phase 03: Artifact Workspace And Session Lifecycle Completion

## Outcome

Finish the shell-owned lifecycle for text files, notebooks, canvases, workspace membership, and open-surface identity so creation, open, rename, duplicate, and session-state ownership no longer diverge.

## Completion Count

300 atomic completion tasks across 5 execution tasks.

### P03-T01

- Phase ID: P03
- Task ID: P03-T01
- Task Title: Finish unsaved artifact ownership and retire fake untitled-path behavior
- Priority: P0
- Category: File / Workspace Completion
- Atomic Completion Tasks Covered: 60
- Objective: Replace pseudo-files and dummy IDs with authoritative unsaved artifact records.
- Why This Matters Now: Fake identifiers still leak into creation and open flows.
- Completion Gap Statement: The codebase still contains fake untitled-path history and dummy-node tactics instead of one real unsaved-artifact contract.
- User / Product Impact: New file flows remain fragile, inconsistent, and hard to restore safely.
- Repository Evidence: [UnsavedDocumentBuffer.h](/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h), [FileTreeCtrl.cpp](/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp), [MainFrame.cpp](/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp)
- Scope: Untitled text artifacts, inline tree creation, tab ownership, save-target promotion, dirty tracking.
- Out of Scope: Notebook and canvas-specific semantics handled in later tasks.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/UnsavedDocumentBuffer.h`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Features / Systems / Components: New file, save, tab opening, file tree, session restore.
- Current Behavior: Fake or dummy identifiers still appear in text creation pathways.
- Intended Completed Behavior: Unsaved text artifacts are first-class shell records with stable IDs and promotion rules.
- Missing Pieces: Canonical identity, tree integration, promotion semantics, and restore metadata.
- Technical Approach: Route all unsaved text creation through a shell-owned artifact registry and remove fake path assumptions.
- Implementation Steps: Inventory untitled flows; unify on unsaved IDs; map save/save-as promotion; align tree and tab updates; remove dummy-path logic.
- Validation Steps: Create, edit, rename, save, reopen, and restore unsaved text artifacts from every entry point.
- Acceptance Criteria: No production text workflow depends on fake filesystem paths or inline dummy file identities.
- Dependencies: Phase 01.
- Risks / Failure Modes: Cross-surface assumptions about filesystem-backed IDs can break if migration is partial.
- Cleanup / Migration Notes where relevant: Delete compatibility paths once all entry points adopt the unsaved artifact model.
- Observability / Diagnostics Notes where relevant: Log artifact-ID creation, promotion, and restore events.
- Rollback / Safety Notes: Preserve old path-to-tab resolution as a guarded fallback during migration only.
- References / Context: This closes the lifecycle debt identified in `v20`.
- Example scenarios where useful: A new unsaved file survives tab switching, save-as, and session restore without pretending to already exist on disk.

### P03-T02

- Phase ID: P03
- Task ID: P03-T02
- Task Title: Unify notebook document identity with workspace and shell lifecycle ownership
- Priority: P0
- Category: Notebook Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make notebooks shell-owned artifacts rather than side-channel data records.
- Why This Matters Now: Notebook model richness still outruns notebook shell ownership.
- Completion Gap Statement: Notebook creation, dirty tracking, mounting, and persistence are split across several partial pathways.
- User / Product Impact: Notebook behavior remains less trustworthy than core editor behavior.
- Repository Evidence: [NotebookShellHost.h](/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.h), [NotebookDocumentLifecycle.cpp](/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp), [Notebook.cpp](/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp)
- Scope: Notebook creation, open, dirty state, save, session restore, workspace registration, active-surface ownership.
- Out of Scope: Kernel runtime internals handled in Phase 06.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.h`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Features / Systems / Components: Notebook tabs, shell controller, persistence, command entry points.
- Current Behavior: Notebook identity and lifecycle are still split between shell state and model storage assumptions.
- Intended Completed Behavior: Notebooks behave like first-class shell artifacts with deterministic lifecycle ownership.
- Missing Pieces: Stable artifact IDs, open/save authority, session metadata, and shell/controller integration.
- Technical Approach: Rebase notebook lifecycle on the same shell-owned artifact contract used by other primary surfaces.
- Implementation Steps: Map notebook entry points; centralize lifecycle ownership; align dirty/save events; update restore snapshots; remove duplicate direct event paths.
- Validation Steps: Create, open, edit, save, restore, duplicate, and rename notebooks from every visible entry point.
- Acceptance Criteria: Notebook lifecycle no longer bypasses shell-owned artifact state.
- Dependencies: P03-T01.
- Risks / Failure Modes: Kernel/session assumptions can leak into document identity if boundaries stay unclear.
- Cleanup / Migration Notes where relevant: Retire older notebook-open event shortcuts once the new lifecycle is complete.
- Observability / Diagnostics Notes where relevant: Emit notebook lifecycle transitions and failed-save diagnostics.
- Rollback / Safety Notes: Keep migration adapters only until all notebook entry points are rebased.
- References / Context: This is the completion counterpart to the lifecycle concerns raised in earlier passes.
- Example scenarios where useful: A notebook created from the palette appears in the workspace and restores correctly after restart.

### P03-T03

- Phase ID: P03
- Task ID: P03-T03
- Task Title: Finish canvas artifact lifecycle ownership and remove split shell/workbench creation paths
- Priority: P0
- Category: Canvas Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make board creation, opening, duplication, and rename flow through one authoritative lifecycle path.
- Why This Matters Now: Canvas behavior is still split between richer workbench logic and thinner shell placeholder logic.
- Completion Gap Statement: Board identity and workspace registration still diverge between shell UI and actual canvas workbench services.
- User / Product Impact: Canvas documents remain more brittle than files and notebooks.
- Repository Evidence: [CanvasArtifactLifecycle.h](/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.h), [CanvasWorkspacePanel.cpp](/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp), [CanvasWorkbench.cpp](/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp)
- Scope: Board creation, open, rename, duplicate, save, session registration, active-canvas ownership.
- Out of Scope: Tool and export details handled in Phase 07.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/CanvasArtifactLifecycle.h`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Canvas workspace, board tabs, shell controller, persistence.
- Current Behavior: Canvas lifecycle remains split across shell events, panel-local logic, and workbench methods.
- Intended Completed Behavior: The shell owns board lifecycle and the workbench implements it, without duplicate or placeholder entry chains.
- Missing Pieces: Canonical board identity, event topology cleanup, and restore integration.
- Technical Approach: Route all canvas artifact operations through one shell-owned lifecycle service and remove private event-bus shortcuts.
- Implementation Steps: Inventory board entry points; unify on lifecycle service; remove placeholder shell creation; align title/tab/session updates; finish duplicate and rename flows.
- Validation Steps: Create and reopen boards across all menu, toolbar, panel, and restore paths.
- Acceptance Criteria: Canvas lifecycle no longer depends on split shell/workbench logic.
- Dependencies: P03-T01.
- Risks / Failure Modes: Existing canvas UI may assume local state that is not shell-owned.
- Cleanup / Migration Notes where relevant: Delete panel-local creation counters and placeholder identity logic.
- Observability / Diagnostics Notes where relevant: Add board lifecycle telemetry and restore-failure logs.
- Rollback / Safety Notes: Keep compatibility event listeners while old routes are drained.
- References / Context: This phase closes the product-completion gap behind the earlier canvas creation findings.
- Example scenarios where useful: A board created from the activity bar and a board reopened from session restore are the same artifact type to the shell.

### P03-T04

- Phase ID: P03
- Task ID: P03-T04
- Task Title: Complete workspace membership, multi-root awareness, and artifact-to-project registration
- Priority: P1
- Category: File / Workspace Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make artifact lifecycles aware of actual workspace ownership instead of incidental paths.
- Why This Matters Now: Several lifecycle seams are still workspace-light or stubbed.
- Completion Gap Statement: Artifact services and workspace settings still stop short of a durable multi-root-aware contract.
- User / Product Impact: Move, duplicate, save, and restore behavior can drift from actual project structure.
- Repository Evidence: [WorkspaceSettings.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp), [RepositoryService.cpp](/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp), [test_phase20_file_management.cpp](/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp)
- Scope: Workspace roots, project registration, workspace file ownership, multi-root routing, recent-item coherence.
- Out of Scope: Git semantics handled in Phase 09.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp`; `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.h`; `/Users/ryanrentfro/code/markamp/tests/unit/test_phase20_file_management.cpp`
- Related Features / Systems / Components: Workspace settings, repository snapshots, recent items, project trees.
- Current Behavior: Workspace-facing code still contains stubbed or shallow persistence assumptions.
- Intended Completed Behavior: Every artifact can be resolved against its owning workspace/project deterministically.
- Missing Pieces: Real workspace persistence, multi-root resolution, and project-aware lifecycle APIs.
- Technical Approach: Finish workspace identity and make all artifact services consume it rather than infer it ad hoc.
- Implementation Steps: Replace workspace stubs; define root-resolution APIs; update artifact registry and recent-item flows; add multi-root test coverage.
- Validation Steps: Exercise create/open/save/move across single-root and multi-root workspaces.
- Acceptance Criteria: Workspace ownership is explicit and persistent for all primary artifact types.
- Dependencies: P03-T01 through P03-T03.
- Risks / Failure Modes: Multi-root assumptions can expose hidden path-based coupling across services.
- Cleanup / Migration Notes where relevant: Remove fallback current-working-directory assumptions once workspace ownership is explicit.
- Observability / Diagnostics Notes where relevant: Record workspace-root resolution failures and fallback usage.
- Rollback / Safety Notes: Preserve single-root compatibility while multi-root behavior is stabilized.
- References / Context: This is necessary to move from “works on the happy path” to actual IDE-grade lifecycle integrity.
- Example scenarios where useful: A notebook created in workspace B does not silently save into workspace A’s data dir.

### P03-T05

- Phase ID: P03
- Task ID: P03-T05
- Task Title: Finish shell session identity and open-surface restoration ownership
- Priority: P0
- Category: Persistence Completion
- Atomic Completion Tasks Covered: 60
- Objective: Make open-surface state and restore semantics flow from artifact truth instead of incidental tab state.
- Why This Matters Now: Restore quality depends on artifact identity being stable across the shell.
- Completion Gap Statement: Session restore remains partially snapshot-oriented and not fully tied to canonical artifact ownership.
- User / Product Impact: Users cannot fully trust reopen, crash recovery, or last-session continuity.
- Repository Evidence: [WorkspaceSessionRestore.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp), [WorkbenchShellController.cpp](/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp), [MarkAmpApp.cpp](/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp)
- Scope: Open tabs, active panel/surface, pinned state, reopen order, crash recovery, startup restore.
- Out of Scope: Low-level autosave mechanics handled in Phase 04.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Features / Systems / Components: Startup, session restore, shell, tabs, artifacts.
- Current Behavior: Session restore logic exists but is only partially authoritative over surface identity.
- Intended Completed Behavior: The shell restores real artifacts and real surfaces from canonical persisted state.
- Missing Pieces: Stable persisted identifiers, open-order rules, and recovery/error paths for missing or unsaved artifacts.
- Technical Approach: Make session restore consume the canonical artifact registry and explicit surface ownership data.
- Implementation Steps: Align restore schema with artifact IDs; remove tab-local assumptions; handle missing artifacts gracefully; restore active context deterministically.
- Validation Steps: Restart after mixed file/notebook/canvas sessions, unsaved content, and intentionally missing files.
- Acceptance Criteria: Restore no longer depends on fragile tab-path or panel-local state.
- Dependencies: P03-T01 through P03-T04.
- Risks / Failure Modes: Partial restore migrations can strand users between old and new session formats.
- Cleanup / Migration Notes where relevant: Add migration for older session snapshots while new restore data rolls out.
- Observability / Diagnostics Notes where relevant: Log restore decisions, migrations, and unresolved artifact references.
- Rollback / Safety Notes: Keep a session-format version gate with safe downgrade handling.
- References / Context: This task closes the shell continuity gap that still prevents the product from feeling finished.
- Example scenarios where useful: After a crash, the shell restores an unsaved file, an open notebook, and the active canvas correctly.
