# Phase 49: End-to-End Integration Testing

## Overview
With 48 phases of feature development complete, this phase focuses on end-to-end integration testing: verifying that all features work together correctly, testing cross-surface workflows, and ensuring the product behaves as a cohesive system rather than a collection of independent features.

## Prerequisites
- All previous phases (1-48) substantially complete

## Tasks

### Task 1: Test Complete Workspace Lifecycle
**Files:** `tests/integration/test_workspace_lifecycle.cpp`
**Description:** Test the full workspace lifecycle: create workspace, add files, configure settings, use all surfaces, close and reopen.
**Acceptance Criteria:**
- Create new workspace from scratch
- Add files via: new file, template, import
- Configure: theme, settings, automations
- Use: editor, canvas, notebook, graph
- Close: all state saved
- Reopen: all state restored perfectly

### Task 2: Test Cross-Surface Navigation Flow
**Files:** `tests/integration/test_cross_surface.cpp`
**Description:** Test navigation across all surfaces: editor -> graph -> canvas -> notebook and back. Verify history, breadcrumbs, and state restoration.
**Acceptance Criteria:**
- Navigate: editor wiki-link -> graph node -> canvas object -> notebook cell
- Back button returns to each previous surface
- Breadcrumbs show complete trail
- Each surface state preserved (scroll, selection, zoom)
- Forward button works after back
- 20-step navigation history works correctly

### Task 3: Test Editor-to-Preview Sync
**Files:** `tests/integration/test_editor_preview_sync.cpp`
**Description:** Test editor and preview synchronization: edit Markdown, verify preview updates, test scroll sync, test live preview mode.
**Acceptance Criteria:**
- Edit heading: preview updates in < 300ms
- Scroll editor: preview follows
- Scroll preview: editor follows
- Click preview element: editor scrolls to source
- Live preview mode: inline rendering correct
- All Markdown elements render correctly

### Task 4: Test Search-to-Edit Workflow
**Files:** `tests/integration/test_search_edit.cpp`
**Description:** Test search workflow: search across workspace, navigate to result, edit match, verify search updates, replace all.
**Acceptance Criteria:**
- Search: finds matches across 100 files
- Navigate: click result opens file at match
- Edit: modify match text
- Re-search: updated results reflect edit
- Replace all: all matches replaced correctly
- Undo: replace all is undoable per file

### Task 5: Test Canvas Board Complete Workflow
**Files:** `tests/integration/test_canvas_workflow.cpp`
**Description:** Test canvas board workflow: create board, add objects (sticky, text, shape, connector, image), manipulate (move, resize, group), save and reload.
**Acceptance Criteria:**
- Create board from template
- Add one of each object type
- Move, resize, and style objects
- Create connections between objects
- Group and ungroup objects
- Save, close, reopen: all objects preserved

### Task 6: Test Notebook Execution Workflow
**Files:** `tests/integration/test_notebook_workflow.cpp`
**Description:** Test notebook workflow: create notebook, add cells, execute code, view output, export.
**Acceptance Criteria:**
- Create notebook
- Add code cell, markdown cell
- Execute code cell (output rendered)
- Edit and re-execute (output updates)
- Export to .ipynb format
- Reload exported notebook: matches original

### Task 7: Test Knowledge Graph Workflow
**Files:** `tests/integration/test_graph_workflow.cpp`
**Description:** Test graph workflow: create linked documents, view graph, filter, navigate between nodes, create new links from graph.
**Acceptance Criteria:**
- Create 10 documents with wiki-links between them
- Graph shows all nodes and edges
- Filter by tag: correct subset shown
- Click node: opens document in editor
- Local graph: shows correct neighborhood
- Backlinks panel: correct incoming links

### Task 8: Test Extension Lifecycle
**Files:** `tests/integration/test_extension_lifecycle.cpp`
**Description:** Test extension lifecycle: install, activate, use features, deactivate, uninstall. Verify no leaked resources.
**Acceptance Criteria:**
- Install extension from manifest
- Extension activates on workspace open
- Extension features functional (commands, contributions)
- Deactivate: features removed cleanly
- Uninstall: all files and registrations removed
- Memory: no leaks after install/uninstall cycle

### Task 9: Test Theme Switching Workflow
**Files:** `tests/integration/test_theme_workflow.cpp`
**Description:** Test theme switching: switch between all 64 themes, verify every surface updates correctly.
**Acceptance Criteria:**
- Switch between 5 different themes
- Editor: syntax highlighting, background, cursor update
- Canvas: background, grid, object colors update
- Preview: content styling updates
- Panels: all panel chrome updates
- No visual artifacts after switch

### Task 10: Test File Operations with Side Effects
**Files:** `tests/integration/test_file_side_effects.cpp`
**Description:** Test file operations with cross-system side effects: rename file updates backlinks, delete file updates graph, move file updates search index.
**Acceptance Criteria:**
- Rename file: all wiki-links updated across workspace
- Delete file: graph node removed, backlinks marked orphaned
- Move file: search index updated, wiki-links updated
- Create file: appears in graph, indexed for search
- All side effects within 2 seconds

### Task 11: Test Undo/Redo Across Operations
**Files:** `tests/integration/test_undo_redo.cpp`
**Description:** Test undo/redo across all operation types: editor edits, canvas object manipulation, settings changes, file operations.
**Acceptance Criteria:**
- Editor: 100 undos in sequence, all correct
- Canvas: create, move, resize, delete all undoable
- Settings: undo reverts setting change
- File rename: undo restores name and links
- Undo depth: 100 operations
- Redo after undo: correct state

### Task 12: Test Performance Under Load
**Files:** `tests/integration/test_performance_load.cpp`
**Description:** Test performance with realistic load: 500 files in workspace, 200 objects on canvas, 50-cell notebook, 100 graph nodes.
**Acceptance Criteria:**
- Startup: < 3 seconds with 500-file workspace
- Editor: typing latency < 16ms
- Canvas: 60fps with 200 objects
- Graph: interactive with 100 nodes
- Search: < 100ms across 500 files
- Memory: < 500MB for full workspace

### Task 13: Test Concurrent Operations
**Files:** `tests/integration/test_concurrent_ops.cpp`
**Description:** Test concurrent operations: background sync while editing, search index update while navigating, auto-save while switching tabs.
**Acceptance Criteria:**
- Edit while search index updates: no jank
- Navigate while auto-save: no delay
- Switch tabs while background export: smooth
- Resize window while rendering: correct layout
- All concurrent operations: no deadlocks
- All concurrent operations: no data corruption

### Task 14: Test Error Recovery
**Files:** `tests/integration/test_error_recovery.cpp`
**Description:** Test error recovery: corrupt file handling, invalid config recovery, extension crash recovery, network failure during sync.
**Acceptance Criteria:**
- Corrupt .md file: shows error, allows manual fix
- Invalid config: falls back to defaults with warning
- Extension crash: extension disabled, app continues
- Network failure during sync: offline mode activates
- All errors: clear user notification with recovery action
- No data loss in any error scenario

### Task 15: Test Accessibility Complete Workflow
**Files:** `tests/integration/test_accessibility_workflow.cpp`
**Description:** Test complete workflow using only keyboard and screen reader: open file, edit, search, navigate, use canvas, review flashcards.
**Acceptance Criteria:**
- Open workspace: Tab to file tree, Enter to open
- Edit: full editing without mouse
- Search: Cmd+F, type, navigate results
- Canvas: create object, select, move via keyboard
- Graph: navigate nodes via Tab/arrows
- All actions announced by screen reader mock

### Task 16: Test Sync and Conflict Resolution
**Files:** `tests/integration/test_sync_workflow.cpp`
**Description:** Test sync workflow: configure provider, sync files, simulate conflict, resolve conflict, verify final state.
**Acceptance Criteria:**
- Configure sync provider (mock)
- Initial sync: all files uploaded
- Simulate remote change: sync detects
- Conflict: both local and remote changed
- Resolve: keep local/remote/merge
- Final state consistent after resolution

### Task 17: Test Export Pipeline
**Files:** `tests/integration/test_export_pipeline.cpp`
**Description:** Test export pipeline: export to all formats, verify output validity, test batch export, test presentation export.
**Acceptance Criteria:**
- HTML export: valid HTML5 output
- PDF export: correct page count
- Markdown export: valid CommonMark
- Presentation: correct slide count
- Batch: all files exported without error
- Canvas export: image includes all objects

### Task 18: Test Multi-Window Workflow
**Files:** `tests/integration/test_multi_window.cpp`
**Description:** Test multi-window: create second window, drag tab between windows, edit same file in both, verify sync.
**Acceptance Criteria:**
- Second window creates successfully
- Tab drag: moves file to new window
- Same file in both: edits sync
- Close one window: other unaffected
- Close all: app exits cleanly
- State restored on relaunch

### Task 19: Test Upgrade Compatibility
**Files:** `tests/integration/test_upgrade.cpp`
**Description:** Test upgrade compatibility: load workspace from previous version, verify all data migrates correctly.
**Acceptance Criteria:**
- Load v8 workspace: no data loss
- Config format migration: old config values preserved
- Board format migration: all objects preserved
- Notebook format: cells and outputs preserved
- Extension compatibility: existing extensions still work
- Theme compatibility: custom themes still load

### Task 20: Create Integration Test Report
**Files:** `tests/integration/generate_report.cpp`
**Description:** Generate comprehensive integration test report: pass/fail summary, timing, screenshots, failure details.
**Acceptance Criteria:**
- Report generated after test run
- Summary: total tests, passed, failed, skipped
- Timing: per-test and total
- Failure details: expected vs actual, stack trace
- Coverage: which features tested
- Report format: HTML for CI, Markdown for review

## Testing Requirements
- All major feature workflows tested end-to-end
- Cross-surface interactions verified
- Performance under realistic load measured
- Error recovery verified for all error paths

## Phase Completion Criteria
- All 20 integration test suites passing
- Cross-surface navigation works seamlessly
- No data loss in any error scenario
- Performance meets all budgets under load
- Accessibility workflow complete via keyboard only
- Report generated with full coverage
