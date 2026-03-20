# Phase 02: File Creation And Text Artifact Lifecycle

## Outcome

Make text-file creation, editing, save, rename, duplicate, reopen, and restore behave like a professional IDE instead of a markdown-first tab hack.

## Improvement Count

204 atomic improvements across 6 execution tasks.

### P02-T01

- Phase ID: P02
- Task ID: P02-T01
- Task Title: Replace fake untitled file paths with unsaved text-document records
- Priority: P0
- Category: Core Creation Flow
- Atomic Improvements Covered: 34
- Objective: Stop using `Untitled-N.md` as if it were already a real path.
- Why This Matters Now: This is the direct cause of broken new-file creation from major entry points.
- Problem Statement: `LayoutManager::OpenFileInTab` fails because the file does not exist yet.
- User Impact: New file commands appear broken or inconsistent.
- Scope: Unsaved text-document IDs, buffer attachment, tab labels, save-target promotion.
- Out of Scope: Notebook or canvas work.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Tabs, file buffers, editor panel, persistence orchestrator.
- Current Behavior: New file paths are invented in UI code and then opened as if already saved.
- Intended Behavior: New files open as unsaved documents and only gain a filesystem path on save.
- Technical Approach: Introduce unsaved document buffer records and save promotion.
- Implementation Steps: Remove untitled-path generation; add unsaved IDs; update tabs; update save/save as behavior.
- Validation Steps: Create new file from menu, tab bar, palette, and empty state without touching disk until save.
- Acceptance Criteria: No failed `ifstream` open occurs for unsaved new files.
- Dependencies: Phase 01.
- Risks / Failure Modes: Hidden path assumptions in recent-files or breadcrumb code.
- UX Notes: Show a stable `Untitled` label with clear modified state.
- Styling / Highlighting Notes where relevant: New file language selection should influence initial styling immediately.
- Observability / Diagnostics Notes: Count attempts to open non-existent untitled file paths.
- Rollback / Safety Notes: Keep legacy file-open path only for real filesystem targets.
- References / Context: `LayoutManager.cpp` already has partial `untitled:` breadcrumb logic that is not used consistently.
- Example scenarios where useful: User presses `Cmd+N`, types text, closes app, and restore offers the unsaved draft.

### P02-T02

- Phase ID: P02
- Task ID: P02-T02
- Task Title: Unify all new-file entry points and workspace placement logic
- Priority: P0
- Category: Artifact Lifecycle
- Atomic Improvements Covered: 34
- Objective: Ensure menu, toolbar, palette, explorer, tab bar, and welcome state all create text files through one path.
- Why This Matters Now: Entry-point divergence is a product-trust killer.
- Problem Statement: Explorer creation writes to disk immediately while other flows do not.
- User Impact: Users get different naming, placement, and focus behavior depending on where they click.
- Scope: Entry-point audit, target-directory resolution, parent-folder defaults, inline rename rules.
- Out of Scope: Advanced project templates.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Related Systems / Components: Explorer, tabs, commands, workspace tree.
- Current Behavior: File creation semantics vary by surface.
- Intended Behavior: All file-creation paths share the same validation, naming, and mount behavior.
- Technical Approach: Route explorer and non-explorer creation through the same service with different placement hints.
- Implementation Steps: Replace explorer direct `ofstream`; feed placement context to creation service; unify focus and selection behavior.
- Validation Steps: Create files in root, nested folder, and no-workspace state from all entry points.
- Acceptance Criteria: Created file location and activation behavior are predictable and identical by policy.
- Dependencies: P02-T01.
- Risks / Failure Modes: Inline explorer flows may fight modal flows.
- UX Notes: Explorer should still feel native while using the same backend.
- Styling / Highlighting Notes where relevant: New-file prompts and inline editors should share typography and state styling.
- Observability / Diagnostics Notes: Capture creation source and resolved target folder for support logs.
- Rollback / Safety Notes: Keep explorer-only directory creation path temporarily if needed.
- References / Context: `FileTreeCtrl.cpp` uses `StartInlineEdit` and writes real files immediately.
- Example scenarios where useful: Creating a file in a selected subfolder from the palette should land in that folder and open immediately.

### P02-T03

- Phase ID: P02
- Task ID: P02-T03
- Task Title: Complete text-document lifecycle for save, save as, rename, duplicate, move, and delete
- Priority: P0
- Category: Artifact Lifecycle
- Atomic Improvements Covered: 34
- Objective: Finish the end-to-end file lifecycle after creation.
- Why This Matters Now: Creation without robust follow-through still leaves the product incomplete.
- Problem Statement: Save and rename logic lives in multiple places and is not artifact-aware.
- User Impact: Users cannot trust common file operations.
- Scope: Save, save as, rename, duplicate, move, delete, reopen, close-with-dirty, external-change detection.
- Out of Scope: Git integration.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/core/DocumentPersistenceOrchestrator.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- Related Systems / Components: File buffers, tabs, explorer, persistence, recents.
- Current Behavior: Save is editor-centric and optimistic; rename/delete are explorer-centric.
- Intended Behavior: Artifact lifecycle operations update registry, tabs, shell state, explorer, and session data together.
- Technical Approach: Add artifact-operation orchestrators and reconcile path changes centrally.
- Implementation Steps: Centralize file ops; add duplicate/move APIs; refresh explorer and tabs from one event; update recents and session records.
- Validation Steps: Save unsaved file, rename open file, duplicate open file, move file between folders, delete dirty file with confirmation.
- Acceptance Criteria: All file lifecycle operations preserve correct active tab and state.
- Dependencies: P02-T01, P02-T02.
- Risks / Failure Modes: Path churn causing stale watchers or buffer maps.
- UX Notes: Dirty prompts and post-operation focus must be consistent.
- Styling / Highlighting Notes where relevant: Duplicate and rename dialogs must match app chrome.
- Observability / Diagnostics Notes: Log old path, new path, artifact ID, and operation result.
- Rollback / Safety Notes: Use atomic file-write and safe-move strategies.
- References / Context: `SaveActiveFileAs` mutates buffer maps directly in `LayoutManager.cpp`.
- Example scenarios where useful: User renames an open file from the explorer and the active tab updates without reopening.

### P02-T04

- Phase ID: P02
- Task ID: P02-T04
- Task Title: Upgrade editor mount and buffer synchronization for real IDE behavior
- Priority: P1
- Category: Editor Workflow
- Atomic Improvements Covered: 34
- Objective: Make the editor mount process robust for unsaved files, external files, large files, and reopening.
- Why This Matters Now: Text creation depends on reliable editor attachment and buffer restore.
- Problem Statement: Editor/session state is tracked inside layout-managed buffers with too many assumptions.
- User Impact: Cursor restore, split behavior, and reopened tabs may be unreliable.
- Scope: Buffer model cleanup, editor session state, split groups, active editor tracking, reopen semantics.
- Out of Scope: New editor component replacement.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp`
- Related Systems / Components: Editor panel, split view, file buffers, shell state.
- Current Behavior: Layout owns too much buffer and tab responsibility.
- Intended Behavior: Editor mounts from artifact records and syncs session state through clear contracts.
- Technical Approach: Separate editor session state from persistence path state and normalize split-group behavior.
- Implementation Steps: Add editor mount context; persist cursor/scroll/selection by artifact ID; clean group switching; repair reopen logic.
- Validation Steps: Split an unsaved file, save it, reopen workspace, and confirm cursor/scroll restore.
- Acceptance Criteria: Editor state survives common operations without aliasing bugs.
- Dependencies: P02-T01, P01-T04.
- Risks / Failure Modes: Group managers and tab bars may still cache file paths only.
- UX Notes: Reopened files should feel exactly where the user left them.
- Styling / Highlighting Notes where relevant: Active/inactive editor chrome should stay visually coherent in split layouts.
- Observability / Diagnostics Notes: Add restore traces for cursor, scroll, and active group decisions.
- Rollback / Safety Notes: Preserve old buffer maps behind adapters until migration completes.
- References / Context: `LayoutManager.cpp` stores editor session details into `file_buffers_`.
- Example scenarios where useful: User opens three files in two groups, restarts, and sees the same arrangement.

### P02-T05

- Phase ID: P02
- Task ID: P02-T05
- Task Title: Finish empty-state, naming, and first-save UX for new text files
- Priority: P1
- Category: Styling / Visual Design
- Atomic Improvements Covered: 34
- Objective: Make file creation feel polished from the first click to the first save.
- Why This Matters Now: Functional correctness alone will not make the IDE feel complete.
- Problem Statement: Current creation flows do not offer premium empty states, naming guidance, or save affordances.
- User Impact: New users do not understand what happened after creating a file.
- Scope: Welcome-state actions, placeholder starter content, first-save prompts, inline naming affordances, cancel behavior, error feedback.
- Out of Scope: Full tutorial flows.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`; `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: Welcome surface, status bar, tabs, editor.
- Current Behavior: Creation success and next steps are weakly communicated.
- Intended Behavior: New files feel immediately editable, named, and save-ready.
- Technical Approach: Add first-save affordances, name-edit prompts, empty editor guidance, and non-blocking status messaging.
- Implementation Steps: Design empty-state CTA set; add default tab label states; add save chips/prompts; improve error toasts and retry actions.
- Validation Steps: Create and save a file from an empty workspace and from a populated workspace.
- Acceptance Criteria: Users can discover how to name and save new files without trial and error.
- Dependencies: P02-T01 through P02-T04.
- Risks / Failure Modes: Too much ceremony slows power users.
- UX Notes: Keep advanced flows keyboard-first and low-friction.
- Styling / Highlighting Notes where relevant: Placeholder styling must distinguish temporary text from user content.
- Observability / Diagnostics Notes: Track cancel rates and save-after-create completion rates.
- Rollback / Safety Notes: First-save prompts should never block manual path selection.
- References / Context: Current file creation routes provide little trustworthy feedback.
- Example scenarios where useful: User creates a file from the welcome screen, names it, saves it, and sees it inserted into the workspace tree.

### P02-T06

- Phase ID: P02
- Task ID: P02-T06
- Task Title: Add file-creation integration tests and edge-case coverage
- Priority: P0
- Category: Diagnostics / Regression Protection
- Atomic Improvements Covered: 34
- Objective: Protect the new text-artifact lifecycle against regressions.
- Why This Matters Now: File creation is currently broken and will remain fragile without dedicated coverage.
- Problem Statement: Existing tests are not obviously guarding the full create-open-save-restore chain.
- User Impact: Core workflows can silently regress.
- Scope: Unit tests, integration tests, smoke tests, large-file edge cases, invalid path cases, cancel flows, crash recovery setup.
- Out of Scope: Visual golden-image suite for every editor state.
- Relevant Full Filepaths: `/Users/ryanrentfro/code/markamp/tests/unit`; `/Users/ryanrentfro/code/markamp/scripts/smoke_test.sh`; `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- Related Systems / Components: Test harness, shell, persistence, editor.
- Current Behavior: Broken new-file behavior survived into the current product state.
- Intended Behavior: CI blocks regressions in create/save/restore and entry-point unification.
- Technical Approach: Add artifact-lifecycle fixtures and smoke paths that simulate user actions across surfaces.
- Implementation Steps: Define matrix; add create/save/rename/reopen cases; add invalid path tests; add unsaved-draft restore tests.
- Validation Steps: Run test matrix on macOS, Windows, and Linux CI where possible.
- Acceptance Criteria: File creation regressions are release-blocking.
- Dependencies: P02-T01 through P02-T05.
- Risks / Failure Modes: UI-only tests become flaky if hooks are weak.
- UX Notes: Include keyboard-only test paths.
- Styling / Highlighting Notes where relevant: Add baseline checks for unsaved tab chrome and error-state styling.
- Observability / Diagnostics Notes: Save failure reasons and timing should be visible in test logs.
- Rollback / Safety Notes: Use temp directories and crash-safe cleanup.
- References / Context: Broken new file behavior is present in command, menu, and tab bar entry points today.
- Example scenarios where useful: Creating a file, typing, cancelling save, then saving later from the tab close prompt.

