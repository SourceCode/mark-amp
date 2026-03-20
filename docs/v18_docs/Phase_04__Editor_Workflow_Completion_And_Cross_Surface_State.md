# Phase 04: Editor Workflow Completion And Cross Surface State

## Phase Goal
Make document editing workflows complete, recoverable, and consistently reflected across all shell surfaces.

## Tasks
### Task 1
- Phase ID: P04
- Task ID: P04-T01
- Task Title: Unify Save Save As Save All Revert And Dirty State
- Objective: Ensure document persistence behavior is identical across all triggers and fully reflected in the shell.
- Problem Statement: Save flows are spread across frame, layout manager, editor, tab bar, and status bar.
- Scope: Save, save as, save all, revert, dirty state, title updates, tab indicators, status updates.
- Out of Scope: Version-control diff/merge features.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
- Related Systems / Components: file buffers, tab state, status bar, window title, notifications.
- Current Behavior: Dirty-state changes and save feedback are handled in several places.
- Intended Behavior: Save lifecycle is single-source and all visible shell indicators stay synchronized.
- Technical Approach: Centralize document persistence orchestration and publish clear state events.
- Implementation Steps:
  1. Define document save lifecycle.
  2. Route save-related actions through one orchestrator.
  3. Publish dirty/saved/reverted state changes once.
  4. Update all shell observers to consume the same events.
- Edge Cases / Failure Modes: Untitled files, save failures, external file deletion, partial save-all failure.
- UX Considerations: Saving should feel immediate and trustworthy.
- Dependencies: P02-T02.
- Validation Steps: Trigger saves from menu, toolbar, shortcut, tab context menu, and close prompts.
- Acceptance Criteria: Save state is never out of sync across editor, tab strip, title bar, and status bar.
- Rollback / Safety Notes: Keep file writes atomic and preserve user content on failure.
- References / Context: `onSave()`, `SaveActiveFile()`, and tab/status updates are currently distributed.
- Example interactions or usage scenarios where helpful: Closing with unsaved tabs should offer one coherent save-all decision path.

### Task 2
- Phase ID: P04
- Task ID: P04-T02
- Task Title: Normalize Editor View Mode And Split State Propagation
- Objective: Make editor, split, and preview mode changes reflect consistently across toolbar, preview, split view, and status bar.
- Problem Statement: View mode changes are event-driven but implemented by multiple consumers with possible drift.
- Scope: Editor mode, split mode, preview mode, active group focus, toolbar state, preview refresh behavior.
- Out of Scope: New preview rendering features.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/SplitView.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: ViewModeChangedEvent, split view, preview panel, toolbar, status bar.
- Current Behavior: Consumers subscribe separately and may apply incomplete UI updates.
- Intended Behavior: One view mode change fully updates state, visible controls, and focus.
- Technical Approach: Standardize view-mode owner and enforce state propagation order.
- Implementation Steps:
  1. Identify source of truth for view mode.
  2. Normalize event payload and application order.
  3. Ensure focus moves to the correct surface after mode switches.
  4. Add shell feedback for unavailable preview/split contexts.
- Edge Cases / Failure Modes: No active file; switching while preview render is pending.
- UX Considerations: View switches should never feel like separate products.
- Dependencies: P02-T02, P03-T04.
- Validation Steps: Toggle modes from toolbar, menu, and palette and compare results.
- Acceptance Criteria: Mode state, selection, and status indicators remain aligned.
- Rollback / Safety Notes: Preserve existing preview functionality while consolidating ownership.
- References / Context: `PreviewPanel`, `SplitView`, `Toolbar`, and `StatusBarPanel` all consume view mode changes.
- Example interactions or usage scenarios where helpful: Entering preview mode should update toolbar toggle, status label, focus target, and content split immediately.

### Task 3
- Phase ID: P04
- Task ID: P04-T03
- Task Title: Complete Find Replace And Selection Based Editor Commands
- Objective: Ensure edit actions advertised in shortcuts, menus, and palette are actually supported end to end.
- Problem Statement: Many editor commands are published as events but need systematic verification of active-editor routing, no-op feedback, and selection semantics.
- Scope: Find, replace, duplicate line, comment toggle, move line, sort, transform case, selection expand/shrink, wrap toggle, go to line.
- Out of Scope: New text transformation features beyond existing intent.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp`
- Related Systems / Components: EventBus, active editor resolution, editor widget commands, find bar.
- Current Behavior: Layout manager subscribes to many edit request events directly, but the behavior is fragmented and lightly guarded.
- Intended Behavior: Every published edit command either completes correctly or gives the user a clear reason it cannot run.
- Technical Approach: Build an editor command adapter layer that validates context before execution.
- Implementation Steps:
  1. Enumerate command-to-editor method mappings.
  2. Add active-editor guards and feedback.
  3. Normalize selection behavior and cursor updates.
  4. Remove duplicate shortcut registrations or conflicting bindings.
- Edge Cases / Failure Modes: No editor active; readonly editor; multi-cursor scenarios; empty selections.
- UX Considerations: Editor commands should feel dependable and discoverable.
- Dependencies: P02-T02 and P02-T05.
- Validation Steps: Trigger each command from command palette and from shortcut with/without active editor.
- Acceptance Criteria: Advertised editor commands are not dead ends.
- Rollback / Safety Notes: Keep command handling thin over editor primitives.
- References / Context: `LayoutManager` currently acts as a broad event-to-editor adapter.
- Example interactions or usage scenarios where helpful: Running “Toggle Comment” with no selection should still do something sensible or explain why it cannot.

### Task 4
- Phase ID: P04
- Task ID: P04-T04
- Task Title: Synchronize Tab Bar Breadcrumbs And Navigation History
- Objective: Keep document navigation surfaces aligned with active file and editor state.
- Problem Statement: Tabs, breadcrumbs, history, and title updates can desynchronize when files switch or close.
- Scope: Tab switch/close, open editors section, breadcrumbs, navigation history panel, window title, active file events.
- Out of Scope: Semantic symbol navigation improvements beyond synchronization.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/OpenEditorsSection.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`
- Related Systems / Components: ActiveFileChangedEvent, TabSwitchedEvent, history service, title bar.
- Current Behavior: Some state changes use `CallAfter`, some use direct event responses, and some surfaces track their own notions of active content.
- Intended Behavior: There is one active document truth and all navigation surfaces reflect it promptly.
- Technical Approach: Standardize active-document publication and make secondary views observers only.
- Implementation Steps:
  1. Identify active-document source of truth.
  2. Remove redundant local state where possible.
  3. Standardize event ordering on close/switch.
  4. Add history updates for palette/navigation commands.
- Edge Cases / Failure Modes: Closing active tab in a split group; switching to already-open file; preview tab replacement.
- UX Considerations: Navigation surfaces should reinforce orientation, not create uncertainty.
- Dependencies: P03-T02.
- Validation Steps: Open, reorder, close, duplicate, and switch tabs while observing breadcrumbs/history/title.
- Acceptance Criteria: All navigation chrome stays synchronized.
- Rollback / Safety Notes: Preserve asynchronous close handling where required by widget timing.
- References / Context: `MainFrame` manually updates title on multiple related events.
- Example interactions or usage scenarios where helpful: Closing the active tab should immediately advance title, breadcrumb, and history focus to the next chosen tab.

### Task 5
- Phase ID: P04
- Task ID: P04-T05
- Task Title: Surface Editor State Into Status And Shell Metadata
- Objective: Ensure cursor, language, file size, encoding, zoom, EOL, indent, and git state are all fed from real editor/workspace state.
- Problem Statement: `StatusBarPanel` has many setters and subscriptions, but the full metadata pipeline is not clearly complete.
- Scope: Status bar metadata, active editor metadata extraction, workspace/git integration, save flash and progress interplay.
- Out of Scope: Advanced SCM workflows.
- Relevant Full Filepaths:
  - `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`
  - `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/GitService.cpp`
  - `/Users/ryanrentfro/code/markamp/src/core/Events.h`
- Related Systems / Components: editor stats events, git status, file metadata, status bar.
- Current Behavior: Status bar shows a rich model, but some data paths appear shell-local or event-fragile.
- Intended Behavior: Status metadata always reflects the actual active document and workspace.
- Technical Approach: Define an active-document metadata publisher and connect status bar to it.
- Implementation Steps:
  1. Map each status item to its source event or query.
  2. Fill missing publishers.
  3. Eliminate stale metadata on file switch and close.
  4. Integrate git/workspace state updates.
- Edge Cases / Failure Modes: Untitled file, binary file, no repository, no active editor.
- UX Considerations: Status information should be correct without flicker or lag.
- Dependencies: P04-T01 and P04-T04.
- Validation Steps: Switch among files with different metadata and observe status updates.
- Acceptance Criteria: Status bar document metadata is trustworthy.
- Rollback / Safety Notes: Keep expensive metadata recomputation off hot paths where possible.
- References / Context: `StatusBarPanel` already supports extensive metadata and is a natural convergence point.
- Example interactions or usage scenarios where helpful: Switching from a Markdown file to a large JSON file updates language, size, encoding, and git branch immediately.

