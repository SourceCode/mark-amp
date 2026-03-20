# Phase 01: Artifact Creation Spine And Shell Ownership

## Outcome

Create the authoritative artifact lifecycle layer that all file, notebook, and canvas workflows depend on. This phase removes fake creation paths and makes the shell own artifact identity, registration, open state, and lifecycle dispatch.

## Improvement Count

210 atomic improvements across 6 execution tasks.

### P01-T01

- Phase ID: P01
- Task ID: P01-T01
- Task Title: Introduce a unified artifact registry and unsaved artifact model
- Priority: P0
- Category: Core Creation Flow
- Atomic Improvements Covered: 35
- Objective: Define one artifact record model for files, notebooks, and canvases, including unsaved identities, workspace placement, tab metadata, dirty state, and persistence status.
- Why This Matters Now: The current product breaks because each artifact type uses a separate identity scheme.
- Problem Statement: `Untitled-N.md`, notebook IDs, and board IDs do not share one shell-owned lifecycle.
- User Impact: Users cannot trust creation, open, or restore behavior.
- Scope: Add registry contracts, unsaved IDs, artifact kinds, and lifecycle states used by shell, tabs, and persistence.
- Out of Scope: Final surface polish.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.h`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/canvas/CanvasWorkbench.cpp`
- Related Systems / Components: Workbench shell, tabs, persistence, session restore, notebook manager, canvas workbench.
- Current Behavior: Artifact identity is fragmented and partially path-based.
- Intended Behavior: Every artifact has one registry record whether saved or unsaved.
- Technical Approach: Add `ArtifactRecord`, `ArtifactKind`, `ArtifactId`, `ArtifactCreationRequest`, and registry/query/update APIs.
- Implementation Steps: Define model; add service; connect shell controller; expose lookup/update hooks; adapt tabs; adapt save flows.
- Validation Steps: Create unsaved records for file, notebook, and canvas; verify IDs survive open/save/rename transitions.
- Acceptance Criteria: No creation path depends on fake disk paths to exist in tabs.
- Dependencies: None.
- Risks / Failure Modes: Overfitting to text files; leaking UI assumptions into core models.
- UX Notes: Unsaved artifacts need clear labels and consistent dirty indicators.
- Styling / Highlighting Notes where relevant: Registry must carry enough metadata for language and theme selection.
- Observability / Diagnostics Notes: Log artifact create/register/promote/save/delete transitions with stable IDs.
- Rollback / Safety Notes: Keep adapter shims for legacy path-based consumers during migration.
- References / Context: `MainFrame.cpp`, `TabBar.cpp`, and `LayoutManager.cpp` currently create/open text artifacts inconsistently.
- Example scenarios where useful: User creates a new notebook, closes it unsaved, reopens session, and still sees the unsaved notebook draft.

### P01-T02

- Phase ID: P01
- Task ID: P01-T02
- Task Title: Create a shell-owned artifact creation service
- Priority: P0
- Category: Command / Event Wiring
- Atomic Improvements Covered: 35
- Objective: Replace direct UI creation logic with one creation service that returns registered artifacts and mount instructions.
- Why This Matters Now: Menu, tab bar, file tree, and palette currently create artifacts differently.
- Problem Statement: Creation behavior is duplicated in widgets and cannot be validated centrally.
- User Impact: Entry-point-dependent behavior makes the product feel random and broken.
- Scope: Creation service, request routing, naming strategy, template lookup, default parent resolution, cancellation cleanup.
- Out of Scope: Deep template authoring UI.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Menus, palette, tab bar, explorer, workspace shell.
- Current Behavior: Widgets call into their own ad hoc creation code.
- Intended Behavior: All entry points publish one typed creation request and receive one typed result.
- Technical Approach: Add creation commands plus a service layer that resolves artifact kind, parent, name, template, and open behavior.
- Implementation Steps: Define request/result types; migrate menu handlers; migrate tab-bar button; migrate explorer hooks; add cancel-safe cleanup.
- Validation Steps: Run every creation entry point and confirm all route to the same service.
- Acceptance Criteria: No widget directly invents untitled file names or board IDs.
- Dependencies: P01-T01.
- Risks / Failure Modes: Partial migration leaves shadow paths alive.
- UX Notes: Creation dialogs and inline flows must use the same validation rules.
- Styling / Highlighting Notes where relevant: Initial artifact metadata should include language/style defaults.
- Observability / Diagnostics Notes: Emit per-entry-point telemetry so migration residue is visible.
- Rollback / Safety Notes: Keep legacy commands behind feature flags only until coverage is complete.
- References / Context: `FileTreeCtrl.cpp` uses real disk creation while `MainFrame.cpp` and `TabBar.cpp` do not.
- Example scenarios where useful: `Cmd+N`, command palette, welcome empty state, and explorer context menu all create the same kind of new file.

### P01-T03

- Phase ID: P01
- Task ID: P01-T03
- Task Title: Extend WorkbenchShellController from surface switching to active artifact ownership
- Priority: P0
- Category: State / Persistence
- Atomic Improvements Covered: 35
- Objective: Make the shell controller aware of the active artifact, active surface, and active context together.
- Why This Matters Now: Surface switching without artifact ownership still leaves the shell blind.
- Problem Statement: `WorkbenchShellController` currently switches surfaces but does not manage open artifacts.
- User Impact: Focus, restore, and command targeting are unstable.
- Scope: Active artifact state, artifact-open requests, artifact-close requests, surface-target resolution, focus restore.
- Out of Scope: Full dock layout redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.h`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Shell state, layout manager, tabs, focus routing.
- Current Behavior: Shell owns mode changes only.
- Intended Behavior: Shell owns artifact context as well as mode.
- Technical Approach: Add artifact-aware methods and events, then make layout respond to shell decisions instead of bypassing them.
- Implementation Steps: Extend shell API; publish artifact-activated events; connect tabs; connect explorer; connect notebook/canvas hosts.
- Validation Steps: Switch between file, notebook, and canvas while verifying active context updates.
- Acceptance Criteria: Commands can resolve the active artifact from shell state alone.
- Dependencies: P01-T01, P01-T02.
- Risks / Failure Modes: Double sources of truth with `LayoutManager`.
- UX Notes: Active artifact transitions should preserve focus intent.
- Styling / Highlighting Notes where relevant: Surface-specific styling may need active artifact hints.
- Observability / Diagnostics Notes: Record shell transitions with old/new artifact IDs and sources.
- Rollback / Safety Notes: Preserve existing UI delegate contract while adding artifact methods incrementally.
- References / Context: `open_file` in `WorkbenchShellController.cpp` only publishes `FileOpenedEvent`.
- Example scenarios where useful: Opening a notebook should switch to notebook surface and mark it active without losing workspace shell context.

### P01-T04

- Phase ID: P01
- Task ID: P01-T04
- Task Title: Build canonical artifact-open and artifact-mount adapters
- Priority: P0
- Category: Editor Workflow
- Atomic Improvements Covered: 35
- Objective: Separate artifact opening from surface mounting so layout, tabs, notebooks, and canvases all mount from the same shell contract.
- Why This Matters Now: Opening and mounting are currently conflated in `LayoutManager`.
- Problem Statement: File open logic assumes a text editor mount and filesystem-backed path.
- User Impact: Non-file artifact types do not behave like first-class citizens.
- Scope: Artifact-open requests, surface mount adapters, tab descriptors, notebook mount descriptors, canvas mount descriptors.
- Out of Scope: Deep per-surface feature completion.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Systems / Components: Layout manager, notebook host, canvas workspace, tabs.
- Current Behavior: File open is editor-centric; notebook and canvas bypass that stack.
- Intended Behavior: Shell resolves artifact kind, then surface adapters mount the right host.
- Technical Approach: Introduce artifact mount interfaces implemented by editor, notebook, and canvas workspaces.
- Implementation Steps: Define mount contracts; adapt layout; route open requests; return mounted handles; connect activation events.
- Validation Steps: Open one file, one notebook, and one board and verify unified lifecycle hooks fire.
- Acceptance Criteria: `LayoutManager` no longer assumes every artifact is a text file path.
- Dependencies: P01-T01, P01-T03.
- Risks / Failure Modes: Tab model and surface model drifting apart.
- UX Notes: Opening should feel immediate and predictable regardless of artifact type.
- Styling / Highlighting Notes where relevant: Mount descriptors should carry language and presentation defaults.
- Observability / Diagnostics Notes: Track mount latency and mount failure reasons by artifact kind.
- Rollback / Safety Notes: Maintain legacy file-open adapter during migration.
- References / Context: `BoardOpenRequestEvent` currently routes directly to `CanvasWorkspacePanel::NewBoard`.
- Example scenarios where useful: Reopening a saved canvas from recents mounts the canvas surface without constructing a fake text tab.

### P01-T05

- Phase ID: P01
- Task ID: P01-T05
- Task Title: Standardize artifact naming, template bootstrap, and initial metadata
- Priority: P1
- Category: Core Creation Flow
- Atomic Improvements Covered: 35
- Objective: Make default names, default content, and artifact metadata deterministic and product-wide.
- Why This Matters Now: Creation feels arbitrary and inconsistent today.
- Problem Statement: Untitled names, notebook titles, and board labels are generated in unrelated ways.
- User Impact: New artifacts feel provisional and fragile instead of deliberate.
- Scope: Name generators, template descriptors, default locations, default language metadata, starter content.
- Out of Scope: User-authored template marketplace.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/CanvasWorkspacePanel.cpp`
- Related Systems / Components: Creation service, templates, workspace tree, tabs.
- Current Behavior: `Untitled-N.md`, `Untitled Notebook`, and `Untitled Board N` are disconnected.
- Intended Behavior: All artifact kinds use one naming and bootstrap policy.
- Technical Approach: Add metadata profiles by artifact kind and entry point.
- Implementation Steps: Define default names; define starter content; define workspace placement; connect metadata to creation requests.
- Validation Steps: Create artifacts from multiple entry points and compare names/content/locations.
- Acceptance Criteria: Default labels and starter content are consistent and discoverable.
- Dependencies: P01-T02.
- Risks / Failure Modes: Overwriting user custom defaults later.
- UX Notes: Names should be editable immediately and behave consistently on cancel.
- Styling / Highlighting Notes where relevant: Templates should declare syntax/language hints up front.
- Observability / Diagnostics Notes: Log template selection, default-name collisions, and bootstrap failures.
- Rollback / Safety Notes: Keep fallback defaults in config if template lookup fails.
- References / Context: Notebook creation currently defaults into a knowledgebase data directory rather than a clear workspace flow.
- Example scenarios where useful: Creating a Python file from a workspace empty state starts with `.py`, Python language metadata, and a save-ready unsaved record.

### P01-T06

- Phase ID: P01
- Task ID: P01-T06
- Task Title: Add creation-flow smoke harnesses and migration guards
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 35
- Objective: Protect the new artifact spine from regressions while legacy paths are being retired.
- Why This Matters Now: Migration will touch the shell, tabs, explorer, notebook host, and canvas host at once.
- Problem Statement: Broken creation flows currently fail silently or only log.
- User Impact: Core product workflows can regress without obvious warnings.
- Scope: Smoke paths, diagnostic events, regression tests, legacy-path detection, fail-fast assertions in debug builds.
- Out of Scope: Full performance benchmarks.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`; `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`
- Related Systems / Components: Testing, telemetry, shell logging, CI smoke coverage.
- Current Behavior: Creation regressions are easy to miss.
- Intended Behavior: CI and local smoke runs catch duplicate path injection, orphaned artifacts, and mount failures.
- Technical Approach: Add integration-style tests plus runtime counters for legacy path usage.
- Implementation Steps: Define smoke matrix; add test harness helpers; add diagnostic counters; block release if legacy creation path is used.
- Validation Steps: Run smoke suite for file, notebook, and canvas create/save/reopen flows.
- Acceptance Criteria: Debug builds can identify any remaining direct untitled-path or local-event-bus creation route.
- Dependencies: P01-T01 through P01-T05.
- Risks / Failure Modes: Weak harnesses give false confidence.
- UX Notes: Error messaging from harness-found failures should map to user-visible defects.
- Styling / Highlighting Notes where relevant: Include baseline screenshot checks for default artifact surfaces.
- Observability / Diagnostics Notes: Persist flow traces for failed create/save/reopen runs.
- Rollback / Safety Notes: Keep diagnostics lightweight in release builds.
- References / Context: `WorkspaceSessionRestore.cpp` and `DocumentPersistenceOrchestrator.cpp` are still too optimistic to trust without harnesses.
- Example scenarios where useful: CI creates a new canvas, adds content, saves, reopens workspace, and verifies the same board reappears.

