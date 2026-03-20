# Phase 03: Notebook Creation And Computational Document Flows

## Outcome

Promote notebooks from a partially isolated subsystem into a first-class artifact type with proper creation, storage, open, save, execution, restore, and shell integration.

## Improvement Count

204 atomic improvements across 6 execution tasks.

### P03-T01

- Phase ID: P03
- Task ID: P03-T01
- Task Title: Rebase notebook creation on the unified artifact pipeline
- Priority: P0
- Category: Notebook Workflow
- Atomic Improvements Covered: 34
- Objective: Move notebook creation behind the same shell-owned creation system as text files.
- Why This Matters Now: Notebook creation is currently event-driven and shallow at the shell layer.
- Problem Statement: `NotebookShellHost` and palette commands do not form a real artifact workflow.
- User Impact: Users cannot depend on new notebook creation or restore.
- Scope: New notebook commands, unsaved notebook records, save-target resolution, mount into notebook host.
- Out of Scope: Advanced notebook collaboration.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Shell controller, notebook host, tabs, persistence.
- Current Behavior: New notebook palette command just publishes `NotebookCreatedEvent`.
- Intended Behavior: New notebook creates a registered notebook artifact and mounts it immediately.
- Technical Approach: Route notebook creation through artifact service and notebook mount adapter.
- Implementation Steps: Replace event-only path; add notebook unsaved record; connect notebook host to shell state; update palette/menu/toolbar hooks.
- Validation Steps: Create notebook from palette, menu, and empty state, then save and reopen it.
- Acceptance Criteria: Notebook creation no longer depends on notification-only code paths.
- Dependencies: Phase 01.
- Risks / Failure Modes: Existing notebook IDs and save paths may not map cleanly.
- UX Notes: Notebook creation should open directly into the first editable cell.
- Styling / Highlighting Notes where relevant: Default notebook visual treatment should match editor quality.
- Observability / Diagnostics Notes: Emit notebook create/mount/save/open traces with artifact IDs.
- Rollback / Safety Notes: Keep notebook manager creation adapter temporarily for migration.
- References / Context: `NotebookShellHost.cpp` currently only publishes notifications on create/open.
- Example scenarios where useful: User creates a notebook, inserts code, saves into the workspace, and reopens it from the tree.

### P03-T02

- Phase ID: P03
- Task ID: P03-T02
- Task Title: Align notebook storage model with workspace expectations
- Priority: P0
- Category: State / Persistence
- Atomic Improvements Covered: 34
- Objective: Stop treating notebooks as knowledgebase-only artifacts when the IDE needs workspace-aware documents.
- Why This Matters Now: Storage location confusion blocks discoverability and trust.
- Problem Statement: `NotebookManager::create_notebook` defaults into `knowledgebase.data_dir`.
- User Impact: New notebooks may not appear where users expect in the project/workspace.
- Scope: Workspace-relative notebook placement, standalone notebook placement, import/migration rules, tree visibility.
- Out of Scope: Full notebook package format redesign.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceOpenOrchestrator.cpp`
- Related Systems / Components: Notebook manager, explorer, workspace shell, recents.
- Current Behavior: Notebook persistence model is misaligned with standard IDE workflows.
- Intended Behavior: Notebook placement is explicit, workspace-aware, and visible in the shell.
- Technical Approach: Add location strategies and migration/adaptor rules between legacy and workspace-backed notebooks.
- Implementation Steps: Define notebook save format; add workspace insertion logic; update open and recents; add migration prompts for legacy notebooks.
- Validation Steps: Create notebooks inside workspace, outside workspace, and from legacy storage.
- Acceptance Criteria: Users can see and manage notebooks like real project artifacts.
- Dependencies: P03-T01.
- Risks / Failure Modes: Legacy notebook references break if migration is rushed.
- UX Notes: Surface storage choice clearly on first save if ambiguity remains.
- Styling / Highlighting Notes where relevant: Notebook tree rows and labels should distinguish notebooks clearly.
- Observability / Diagnostics Notes: Log notebook storage resolution and migration outcomes.
- Rollback / Safety Notes: Maintain read support for legacy notebook locations.
- References / Context: `Notebook.cpp` currently uses `knowledgebase.data_dir`.
- Example scenarios where useful: User creates a notebook in an app repo and sees it in explorer beside source files.

### P03-T03

- Phase ID: P03
- Task ID: P03-T03
- Task Title: Finish notebook lifecycle for open, save, rename, duplicate, delete, and restore
- Priority: P0
- Category: Artifact Lifecycle
- Atomic Improvements Covered: 34
- Objective: Give notebooks parity with files as durable project artifacts.
- Why This Matters Now: Creation without lifecycle completion still leaves notebooks second-class.
- Problem Statement: `NotebookDocumentLifecycle` tracks sets but does not orchestrate real operations.
- User Impact: Notebook state can drift from shell and storage state.
- Scope: Open, save, save as, rename, duplicate, delete, close-with-dirty, restore, recents.
- Out of Scope: Notebook diff/merge depth.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`; `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp`
- Related Systems / Components: Notebook manager, persistence, shell, recents, session restore.
- Current Behavior: Lifecycle is tracked but not fully enforced.
- Intended Behavior: Notebook operations update shell, tree, tabs, dirty state, restore state, and storage together.
- Technical Approach: Add notebook lifecycle orchestrator or extend artifact lifecycle service to notebook kind.
- Implementation Steps: Centralize notebook operations; wire shell updates; persist open-state metadata; add rename/duplicate flows.
- Validation Steps: Rename open notebook, duplicate notebook, restore notebook after restart.
- Acceptance Criteria: Notebook lifecycle operations are deterministic and recoverable.
- Dependencies: P03-T01, P03-T02.
- Risks / Failure Modes: Cell execution state may be lost on duplicate or restore.
- UX Notes: Dirty prompts should mention notebook title and unsaved cells.
- Styling / Highlighting Notes where relevant: Notebook tab chrome and tree chrome must mirror file quality.
- Observability / Diagnostics Notes: Log notebook lifecycle transitions and cell-dirty reasons.
- Rollback / Safety Notes: Preserve backups before destructive notebook operations.
- References / Context: `NotebookDocumentLifecycle.cpp` `save()` currently just clears dirty state.
- Example scenarios where useful: User edits notebook metadata and cells, saves, closes, and later restores the same notebook and active cell.

### P03-T04

- Phase ID: P03
- Task ID: P03-T04
- Task Title: Repair notebook host mounting, active-context propagation, and command targeting
- Priority: P1
- Category: Command / Event Wiring
- Atomic Improvements Covered: 34
- Objective: Make notebook surfaces first-class in focus, command routing, and panel integration.
- Why This Matters Now: Even a saved notebook will feel broken if commands target the wrong surface.
- Problem Statement: Notebook shell hosting is too thin to participate correctly in workbench context.
- User Impact: Keyboard shortcuts, save commands, and context menus may act on the wrong thing.
- Scope: Active notebook tracking, active cell tracking, shell context keys, toolbar and menu targeting, side-panel coupling.
- Out of Scope: New notebook feature categories.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/core/WorkbenchShellController.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
- Related Systems / Components: Shell controller, commands, toolbar, focus model.
- Current Behavior: Notebook host does not appear to drive authoritative context.
- Intended Behavior: Notebook focus sets the active artifact and routes actions correctly.
- Technical Approach: Add notebook-specific context keys and host callbacks into shell state.
- Implementation Steps: Connect notebook activation events; add save/run/new-cell commands; fix toolbar and menu enablement; sync side panels.
- Validation Steps: Use keyboard save/run shortcuts and context menus inside notebooks.
- Acceptance Criteria: Notebook commands target the active notebook and active cell reliably.
- Dependencies: P03-T01 through P03-T03, Phase 06.
- Risks / Failure Modes: Command collisions with editor shortcuts.
- UX Notes: Notebook context should be visually obvious but not noisy.
- Styling / Highlighting Notes where relevant: Notebook-focused surfaces need clear active-cell and active-notebook chrome.
- Observability / Diagnostics Notes: Track command target resolution for notebook actions.
- Rollback / Safety Notes: Keep manual host state override hooks for debugging.
- References / Context: `MainFrame.cpp` still registers notebook commands as direct event publishing.
- Example scenarios where useful: Pressing save inside a notebook should save the notebook artifact, not the last text file.

### P03-T05

- Phase ID: P03
- Task ID: P03-T05
- Task Title: Upgrade notebook first-run, empty-state, and execution-readiness UX
- Priority: P1
- Category: Styling / Visual Design
- Atomic Improvements Covered: 34
- Objective: Make notebooks immediately understandable and visually premium.
- Why This Matters Now: Notebooks need to feel intentional, not bolted on.
- Problem Statement: Current shell evidence suggests notebook onboarding and first-run behavior are too thin.
- User Impact: Users may not know how to create cells, run cells, or save notebooks.
- Scope: Empty notebook state, starter cells, add-cell affordances, kernel/status hints, save prompts, error states.
- Out of Scope: Full interactive tutorial.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/core/NotebookShellHost.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: Notebook host, toolbars, status bar, notifications.
- Current Behavior: Creation and open notifications do not provide a polished workflow.
- Intended Behavior: A new notebook opens into a clear, beautiful, usable computational document surface.
- Technical Approach: Add notebook empty-state layout, action affordances, and readiness/status messaging.
- Implementation Steps: Design first-cell bootstrap; add add-cell CTAs; surface kernel status; add save/retry guidance; improve empty/error states.
- Validation Steps: First-time notebook create and first execution attempt should be understandable without docs.
- Acceptance Criteria: New notebooks no longer feel hidden or underexplained.
- Dependencies: P03-T01 through P03-T04.
- Risks / Failure Modes: Too much onboarding chrome crowds the content.
- UX Notes: The first-run flow should disappear once the user starts working.
- Styling / Highlighting Notes where relevant: Cell chrome, prompt chrome, and execution states must match premium IDE polish.
- Observability / Diagnostics Notes: Measure notebook create-to-first-cell-edit and create-to-first-save completion.
- Rollback / Safety Notes: Keep first-run hints dismissible and config-driven.
- References / Context: Notebook user guidance is currently mostly absent from the visible workflow.
- Example scenarios where useful: User creates a notebook, sees an initial markdown cell and code cell, then saves after first execution.

### P03-T06

- Phase ID: P03
- Task ID: P03-T06
- Task Title: Add notebook workflow regression harnesses and restore tests
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 34
- Objective: Protect notebook creation and lifecycle work with direct coverage.
- Why This Matters Now: Notebook integration is currently too partial to trust.
- Problem Statement: Event-driven shallow integrations are easy to break unnoticed.
- User Impact: Notebook features can ship looking present but behaving unreliably.
- Scope: Creation, save, rename, duplicate, reopen, restore, execution-state placeholder tests, workspace-placement tests.
- Out of Scope: Full kernel correctness testing.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/src/core/Notebook.cpp`; `/Users/ryanrentfro/code/markamp/src/core/NotebookDocumentLifecycle.cpp`
- Related Systems / Components: Test harness, notebook manager, shell, restore.
- Current Behavior: No clear release gate is preventing notebook workflow regressions.
- Intended Behavior: Notebook lifecycle is covered by repeatable smoke and integration suites.
- Technical Approach: Build notebook artifact fixtures and shell-level lifecycle tests.
- Implementation Steps: Add create/open/save tests; add restore tests; add invalid-storage tests; add workspace-tree presence tests.
- Validation Steps: Run notebook suites in CI and local smoke runs.
- Acceptance Criteria: Notebook creation and restore regressions are visible before release.
- Dependencies: P03-T01 through P03-T05.
- Risks / Failure Modes: Tests may overmock shell behavior.
- UX Notes: Include keyboard-only and command-palette-first flows.
- Styling / Highlighting Notes where relevant: Add screenshot or style contract checks for notebook empty and dirty states.
- Observability / Diagnostics Notes: Notebook flow traces should be persisted on failure.
- Rollback / Safety Notes: Use temporary notebook storage and cleanup helpers.
- References / Context: `NotebookShellHost.cpp` and `NotebookDocumentLifecycle.cpp` are currently too thin to rely on without tests.
- Example scenarios where useful: Create notebook, add cells, save, close, reopen workspace, and verify the same notebook remains active.
