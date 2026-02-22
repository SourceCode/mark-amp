# Phase 32: Go-To System

## Overview

Build a comprehensive Go-To navigation system that provides fast, keyboard-driven access to
files, symbols, lines, definitions, references, and problems. This phase unifies multiple
navigation modalities into a cohesive system that integrates with the Command Palette V2
(Phase 31), the Outline Panel (Phase 33), and the BreadcrumbBar. The existing codebase has
`NavigationCommandProvider` with 12 placeholder commands (execute_fn returns true but does
nothing), `NavigationAccessibility` for screen reader announcements, `OutlineService` for
heading navigation, and `BreadcrumbBar` with file/heading path display. This phase wires
all of these into functional end-to-end workflows.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| NavigationCommandProvider | `/Users/ryanrentfro/code/markamp/src/core/NavigationCommandProvider.h` | 12 commands, all placeholder execute_fn |
| NavigationAccessibility | `/Users/ryanrentfro/code/markamp/src/core/NavigationAccessibility.h` | Screen reader text generators, not wired |
| OutlineService | `/Users/ryanrentfro/code/markamp/src/core/OutlineService.h` | Heading extraction, breadcrumb path, scroll_to_heading |
| OutlinePanelController | `/Users/ryanrentfro/code/markamp/src/core/OutlinePanelController.h` | build_outline, find_active, scroll_to_heading |
| BreadcrumbBar | `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h` | File path + heading path display, click callback |
| BreadcrumbModel | `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.h` | Segments, overflow, history |
| CommandRegistry | `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h` | Central command registration with when-clause |
| Events.h | `/Users/ryanrentfro/code/markamp/src/core/Events.h` | CursorPositionChangedEvent, EditorScrollChangedEvent |

## Dependencies

- Phase 31 (Command Palette V2) provides the @ and : mode UIs that this phase wires to real data.
- Phase 33 (Outline Panel) consumes the same symbol data and must stay synchronized.

---

## Tasks

### Task 1: NavigationService Core

**Title:** Create the centralized NavigationService that owns the navigation stack

**Description:** Build a `NavigationService` that maintains a navigation history stack
(back/forward), dispatches go-to requests, and coordinates between the editor, outline,
and breadcrumb systems.

**Implementation Details:**
- Create `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h` and `.cpp`.
- Constructor: `NavigationService(EventBus&, OutlineService&, WorkspaceService&)`.
- Navigation stack: `std::vector<NavigationEntry>` with `int current_index_`.
- `NavigationEntry` struct: `std::string document_id`, `int line`, `int column`,
  `std::chrono::steady_clock::time_point timestamp`.
- Methods: `go_to(document_id, line, col)`, `go_back()`, `go_forward()`,
  `can_go_back()`, `can_go_forward()`, `current_location()`.
- On `go_to`, truncate forward history and push new entry.
- Stack max size: 100 entries (drop oldest when exceeded).
- Emit `NavigationChangedEvent` on every navigation.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (NavigationChangedEvent)
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- `go_to` pushes entries onto the stack and truncates forward history.
- `go_back` and `go_forward` move the cursor through the stack.
- `NavigationChangedEvent` fires on every navigation with the target location.
- Stack does not exceed 100 entries.
- Duplicate consecutive locations are coalesced (not pushed twice).

**Dependencies:** None.

---

### Task 2: Go to File (Cmd+P)

**Title:** Wire Go-to-File to open files from the workspace

**Description:** When the user selects a file in the Command Palette's file picker mode
(Phase 31 Task 3), open that file in the editor and push a navigation entry.

**Implementation Details:**
- In `CommandPalette`, when a file is selected in `kQuickOpen` mode, emit
  `FileOpenRequestEvent{ std::string file_path }` via EventBus.
- `MainFrame` or `EditorPanel` subscribes to `FileOpenRequestEvent` and opens the file.
- After the file is opened, `NavigationService::go_to(document_id, 0, 0)` is called.
- Record the file access in `FrecencyTracker` for ranking.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (file selection handler)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (FileOpenRequestEvent)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp` (record navigation)

**Acceptance Criteria:**
- Selecting a file in Cmd+P opens it in the editor.
- The navigation stack records the file open.
- FrecencyTracker records the access for future ranking.
- Opening an already-open file switches to its tab without creating a duplicate.

**Dependencies:** Phase 31 Task 3 (file picker mode), Task 1 (NavigationService).

---

### Task 3: Go to Symbol in File (Cmd+Shift+O / @)

**Title:** Wire symbol mode to navigate to headings and code symbols within the current file

**Description:** Connect the Command Palette's @ mode to the `OutlineService` to provide
document symbols. Selecting a symbol navigates the editor to that line.

**Implementation Details:**
- In the palette's @ mode, query `OutlineService::get_flat_outline(current_document_id)`.
- Map `OutlineEntry` to display items with heading level indicators.
- On selection, call `OutlineService::scroll_to_heading(block_id)` or emit a
  `GoToLineEvent` for the entry's `source_line`.
- Push a `NavigationEntry` with the target line.
- If a `MarkdownSymbolProvider` (Phase 31 Task 4) exists, use it as the data source.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (@ mode data sourcing)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`

**Acceptance Criteria:**
- @ mode shows all headings in the current Markdown document.
- Selecting a heading scrolls the editor to that line.
- Navigation stack is updated.
- Heading level is visually indicated (H1 bold, H2 semi-bold, etc.).
- Filtering "## Sec" matches only H2 headings containing "Sec".

**Dependencies:** Phase 31 Task 4, Task 1, OutlineService.

---

### Task 4: Go to Symbol in Workspace (Cmd+T / #)

**Title:** Implement workspace-wide symbol search with # prefix

**Description:** Add a new palette mode triggered by `#` that searches for symbols across
all documents in the workspace, not just the current file.

**Implementation Details:**
- Add `PaletteMode::kWorkspaceSymbol` triggered by `#` prefix.
- Query all open documents' outlines via `OutlineService` (iterate workspace files).
- For performance, maintain a cached workspace symbol index that updates on
  document open/close/change events.
- Display results as: [symbol name] [dim: filename:line].
- Selecting a result opens the file and navigates to the symbol's line.
- Limit results to 100 with progressive loading.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (kWorkspaceSymbol mode)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (# mode implementation)
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSymbolIndex.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSymbolIndex.cpp`

**Acceptance Criteria:**
- `#` prefix activates workspace symbol search.
- Results include symbols from all workspace documents.
- Selecting a result opens the correct file at the correct line.
- Results show filename context alongside the symbol name.
- Index updates incrementally when documents change.

**Dependencies:** Task 1, OutlineService.

---

### Task 5: Go to Line (Ctrl+G / :)

**Title:** Wire line picker mode to editor navigation

**Description:** Complete the Go-to-Line workflow from Phase 31 Task 5 by wiring it to
the editor's scroll-to-line capability and the navigation stack.

**Implementation Details:**
- Subscribe to `GoToLineEvent` in `EditorPanel`.
- On receipt, call `wxStyledTextCtrl::GotoLine(line - 1)` (convert 1-based to 0-based).
- Ensure the line is scrolled to the center of the viewport, not the top.
- Push a `NavigationEntry` for the target line.
- Ctrl+G global shortcut opens the palette directly in `:` mode.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (GoToLineEvent handler)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`

**Acceptance Criteria:**
- Ctrl+G opens the palette in line-picker mode.
- Entering a line number and pressing Enter scrolls the editor to that line.
- The target line appears in the center of the viewport.
- Navigation stack is updated.
- Cursor is placed at column 0 of the target line.

**Dependencies:** Phase 31 Task 5, Task 1.

---

### Task 6: Go to Definition (F12)

**Title:** Implement Go-to-Definition for Markdown links and references

**Description:** F12 on a wiki-link `[[target]]`, markdown link `[text](url)`, or
block reference `((block-id))` navigates to the target document or heading.

**Implementation Details:**
- On F12 keypress, determine the token under the cursor.
- If it is a `[[wiki-link]]`, resolve it to a document path via the workspace file index.
- If it is a `[text](url)`, resolve relative URLs to workspace files.
- If it is a `((block-ref))`, resolve the block ID to a document + line.
- Navigate to the target using `NavigationService::go_to`.
- If the target is in a different file, open it first.
- If resolution fails, show a status bar message: "Cannot find definition for 'X'."

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/LinkResolver.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/LinkResolver.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (F12 handler)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`

**Acceptance Criteria:**
- F12 on `[[MyDocument]]` opens MyDocument.md.
- F12 on `[link](./other.md)` opens other.md.
- F12 on `((block-id))` opens the file containing that block and scrolls to it.
- Failed resolution shows an error in the status bar.
- Navigation stack records the jump.

**Dependencies:** Task 1.

---

### Task 7: Go to Declaration

**Title:** Implement Go-to-Declaration for Markdown heading references

**Description:** Go-to-Declaration navigates to the first declaration/definition of a
heading anchor. For `[[Doc#Heading]]`, it navigates to the heading within the document.

**Implementation Details:**
- Parse the link under cursor. If it contains `#anchor`, resolve the anchor to a heading
  in the target document using `OutlineService::search_headings`.
- Navigate to the heading line.
- If no anchor, behave identically to Go-to-Definition.
- Register command `navigation.goToDeclaration` in `CommandRegistry`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/LinkResolver.h` (anchor resolution)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/LinkResolver.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationCommandProvider.h` (add command)

**Acceptance Criteria:**
- `[[Doc#Section 2]]` navigates to the "Section 2" heading in Doc.md.
- Plain `[[Doc]]` navigates to the top of Doc.md.
- Command is registered and available in the palette.

**Dependencies:** Task 6 (LinkResolver).

---

### Task 8: Go to Type Definition

**Title:** Implement Go-to-Type-Definition for frontmatter type references

**Description:** For documents with YAML frontmatter `type: note-type`, Go-to-Type-Definition
navigates to the template or schema file that defines that note type.

**Implementation Details:**
- Parse frontmatter of the current document to extract the `type` field.
- Look up the type in a template directory (e.g., `templates/{type}.md`).
- Navigate to the template file.
- If no type is defined, show "No type definition found for this document."
- Register command `navigation.goToTypeDefinition` in `CommandRegistry`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/LinkResolver.h` (type resolution)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/LinkResolver.cpp`

**Acceptance Criteria:**
- F12+Shift on a document with `type: meeting-notes` opens `templates/meeting-notes.md`.
- Documents without a `type` field show an appropriate message.
- Command registered in CommandRegistry.

**Dependencies:** Task 6.

---

### Task 9: Go to Implementation

**Title:** Implement Go-to-Implementation for template usage sites

**Description:** The inverse of Go-to-Type-Definition: from a template file, find all
documents that use this template (i.e., have `type: <template-name>` in frontmatter).

**Implementation Details:**
- When invoked on a template file, search all workspace documents for
  `type: <template-filename-without-extension>` in their frontmatter.
- Show results in a Quick Pick (reuse palette in a temporary list mode).
- Selecting a result navigates to that document.
- If only one implementation exists, navigate directly.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/TemplateUsageFinder.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/TemplateUsageFinder.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationCommandProvider.h`

**Acceptance Criteria:**
- From a template file, Go-to-Implementation shows all documents using that template.
- Single-result case navigates directly.
- Multi-result case shows a picker.

**Dependencies:** Task 4 (workspace search infrastructure).

---

### Task 10: Go to References (Shift+F12)

**Title:** Find all references to the current document or heading

**Description:** Show all locations across the workspace that reference the current
document (backlinks) or the current heading (heading references).

**Implementation Details:**
- On Shift+F12, determine the current document or heading context.
- Search all workspace documents for `[[current-doc]]`, `[[current-doc#heading]]`,
  and `[text](path-to-current-doc)` patterns.
- Display results in a references panel (or reuse the search results panel).
- Each reference shows: file name, line number, and the line content.
- Clicking a reference navigates to it.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/BacklinkFinder.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/BacklinkFinder.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (display references)

**Acceptance Criteria:**
- Shift+F12 shows all documents that link to the current file.
- References include wiki-links, markdown links, and block references.
- Each reference is clickable and navigates to the source location.
- Reference count is displayed.

**Dependencies:** Task 6 (LinkResolver).

---

### Task 11: Peek Definition (Alt+F12)

**Title:** Show an inline preview of the definition without leaving the current file

**Description:** Alt+F12 shows a small embedded editor view within the current editor
that displays the target content, similar to VSCode's peek widget.

**Implementation Details:**
- Create `PeekDefinitionWidget` as a floating `wxPanel` positioned below the current line.
- The widget contains: a title bar (filename:line), a read-only `wxStyledTextCtrl` showing
  5-10 lines of context around the target, and close/open-in-editor buttons.
- Resolve the link under cursor using `LinkResolver`.
- Load the target file content and display the relevant lines.
- Escape closes the peek widget. Enter opens the file in the main editor.
- Only one peek widget can be open at a time.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/PeekDefinitionWidget.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/PeekDefinitionWidget.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (Alt+F12 handler)

**Acceptance Criteria:**
- Alt+F12 on a link shows an inline preview below the cursor line.
- The preview shows 5-10 lines of context with the target highlighted.
- Escape closes the peek. Enter opens in main editor.
- The peek widget is styled with the current theme.
- Only one peek widget is visible at a time.

**Dependencies:** Task 6 (LinkResolver).

---

### Task 12: Go to Next/Previous Problem (F8/Shift+F8)

**Title:** Navigate between diagnostics (warnings, errors, broken links)

**Description:** F8 navigates to the next problem in the current document. Shift+F8
navigates to the previous problem. Problems include broken links, invalid frontmatter,
and lint warnings.

**Implementation Details:**
- Create `DiagnosticService` that maintains a sorted list of diagnostics per document.
- Each diagnostic: `int line`, `int column`, `DiagnosticSeverity severity`,
  `std::string message`, `std::string source`.
- `next_diagnostic(current_line)` and `prev_diagnostic(current_line)` find the
  closest diagnostic in the respective direction, wrapping around at document boundaries.
- Wire F8/Shift+F8 shortcuts through `ShortcutManager`.
- Show the diagnostic message in the status bar when navigating to it.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/DiagnosticService.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/DiagnosticService.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (F8 handler)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (diagnostic display)

**Acceptance Criteria:**
- F8 navigates to the next problem below the cursor.
- Shift+F8 navigates to the previous problem above the cursor.
- At the end of the document, wraps to the first problem.
- Status bar shows the diagnostic message.
- Navigation stack is updated.

**Dependencies:** Task 1 (NavigationService).

---

### Task 13: Go to Next/Previous Change

**Title:** Navigate between document change markers

**Description:** Navigate to the next/previous modified line since the last save. Shows
a gutter indicator on changed lines and allows jumping between them.

**Implementation Details:**
- Track changed lines by comparing current content against the last-saved version.
- Store `std::set<int> changed_lines_` updated on each edit event.
- `next_change(current_line)` and `prev_change(current_line)` navigate to the nearest
  changed line.
- Register shortcuts: Alt+F5 (next change), Alt+Shift+F5 (previous change).
- Show a colored marker in the editor gutter for changed lines.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/ChangeTracker.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/ChangeTracker.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (gutter markers, navigation)

**Acceptance Criteria:**
- Changed lines show a colored marker in the gutter.
- Alt+F5 navigates to the next changed line.
- Alt+Shift+F5 navigates to the previous changed line.
- Saving the document clears all change markers.
- Navigation wraps around at document boundaries.

**Dependencies:** None.

---

### Task 14: Go Back / Go Forward (Alt+Left / Alt+Right)

**Title:** Wire back/forward navigation to the NavigationService history stack

**Description:** Replace the placeholder execute_fn in `NavigationCommandProvider` for
`kGoBack` and `kGoForward` with real navigation that uses the `NavigationService` stack.

**Implementation Details:**
- Replace the placeholder lambdas for `nav_commands::kGoBack` and `kGoForward`
  in `NavigationCommandProvider::register_built_in_commands()`.
- New execute_fn for GoBack: call `NavigationService::go_back()` which emits
  a `NavigationChangedEvent` that the editor subscribes to.
- Similarly for GoForward.
- Wire `NavigationAccessibility::announce_back_forward` to the accessibility layer.
- Update `is_enabled_fn` to query `NavigationService::can_go_back/forward()`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationCommandProvider.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`

**Acceptance Criteria:**
- Alt+Left navigates to the previous location in the stack.
- Alt+Right navigates to the next location in the stack.
- Back/Forward commands are disabled when the stack is exhausted.
- Screen reader announces the navigation target.

**Dependencies:** Task 1 (NavigationService).

---

### Task 15: Breadcrumb Navigation Integration

**Title:** Wire breadcrumb clicks to Go-To navigation and add sibling dropdowns

**Description:** Currently `BreadcrumbBar` has a click callback but it only receives a
path string. Enhance it to: show a dropdown of sibling files/headings when clicking a
segment, and navigate to the clicked item.

**Implementation Details:**
- On segment click, determine segment type (file path segment vs heading segment).
- For file path segments: show a dropdown listing sibling files in that directory.
- For heading segments: show a dropdown listing sibling headings at the same level.
- Populate sibling lists from `WorkspaceService::find_files` and
  `OutlineService::get_flat_outline`.
- Selecting a sibling navigates to it via `NavigationService::go_to`.
- Update `BreadcrumbModel::siblings` with actual data.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.cpp` (dropdown menus)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbModel.cpp` (sibling data)

**Acceptance Criteria:**
- Clicking a file path segment shows a dropdown of sibling files.
- Clicking a heading segment shows a dropdown of sibling headings.
- Selecting a sibling navigates to it.
- Breadcrumb segments update when the active document changes.
- Dropdown is themed consistently with the rest of the UI.

**Dependencies:** Task 1, OutlineService, WorkspaceService.

---

### Task 16: Wire All NavigationCommandProvider Commands

**Title:** Replace all placeholder execute_fn callbacks with real implementations

**Description:** The remaining 10 commands in `NavigationCommandProvider` (GoToDefinition,
PeekDefinition, ShowBrokenLinks, FollowLink, OpenInSide, NavigateToCanvas,
NavigateToGraph, NavigateToNotebook, CopyDeepLink, GoToHeading) all have placeholder
`execute_fn` that just return true. Wire them to real implementations.

**Implementation Details:**
- GoToDefinition: delegate to LinkResolver + NavigationService.
- PeekDefinition: delegate to PeekDefinitionWidget creation.
- ShowBrokenLinks: delegate to BacklinkFinder with broken-link filter.
- FollowLink: same as GoToDefinition but triggered via Ctrl+Click.
- OpenInSide: open target in a split editor pane.
- NavigateToCanvas/Graph/Notebook: emit surface switch events.
- CopyDeepLink: construct `markamp://` URI and copy to clipboard.
- GoToHeading: open palette in @ mode.
- Register all commands in `CommandRegistry` during app initialization.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationCommandProvider.h` (all execute_fn)
- Modified: app initialization code to register NavigationCommandProvider commands.

**Acceptance Criteria:**
- All 12 navigation commands have functional execute_fn implementations.
- Each command is registered in CommandRegistry and appears in the palette.
- Each command fires the correct navigation event.
- Each command correctly evaluates its `is_enabled_fn`.

**Dependencies:** Tasks 6, 10, 11, 14, 15.

---

### Task 17: Navigation History Panel

**Title:** Create a navigation history panel showing recent locations

**Description:** Add a panel (accessible via View menu or command) that shows the
navigation history stack as a chronological list. Each entry shows: document name,
line number, and timestamp.

**Implementation Details:**
- Create `NavigationHistoryPanel` as a sidebar panel (wxPanel).
- Display `NavigationService` history as a list of entries.
- Current location highlighted with an indicator.
- Double-clicking an entry navigates to it.
- History clears when the workspace closes.
- Show a "Clear History" button at the top.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/NavigationHistoryPanel.cpp`

**Acceptance Criteria:**
- Panel shows all navigation entries in chronological order.
- Current location is visually highlighted.
- Double-clicking an entry navigates to it.
- History updates in real-time as the user navigates.
- "Clear History" button works.

**Dependencies:** Task 1 (NavigationService).

---

### Task 18: Wire NavigationAccessibility to Platform Accessibility

**Title:** Connect screen reader announcements to wxWidgets accessibility API

**Description:** The `NavigationAccessibility` class generates descriptive text but is
never wired to any platform accessibility output. Connect it.

**Implementation Details:**
- On each `NavigationChangedEvent`, call the appropriate
  `NavigationAccessibility::announce_*` method.
- Pass the generated text to `wxAccessible::NotifyEvent` or platform-specific
  accessibility API (ARIA on macOS via NSAccessibility).
- Announce: navigation target, breadcrumb changes, heading jumps, broken links.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp` (announce on nav)
- Modified: platform-specific accessibility integration.

**Acceptance Criteria:**
- VoiceOver (macOS) announces navigation targets when Back/Forward is used.
- Heading jumps are announced with level and text.
- Broken link detection is announced.
- All announcements are descriptive and not overly verbose.

**Dependencies:** Task 14, Task 16.

---

### Task 19: Keyboard Shortcut Registration for All Go-To Commands

**Title:** Register all Go-To shortcuts in ShortcutManager

**Description:** Ensure all Go-To commands have registered keyboard shortcuts that do
not conflict with existing bindings.

**Implementation Details:**
- Register in `ShortcutManager::register_shortcut`:
  - Cmd+P: Go to File
  - Cmd+Shift+O: Go to Symbol in File
  - Cmd+T: Go to Symbol in Workspace
  - Ctrl+G: Go to Line
  - F12: Go to Definition
  - Alt+F12: Peek Definition
  - Shift+F12: Go to References
  - F8: Go to Next Problem
  - Shift+F8: Go to Previous Problem
  - Alt+Left: Go Back
  - Alt+Right: Go Forward
- Check for conflicts with `ShortcutManager::has_conflict` before registration.
- All shortcuts in "Navigation" category.

**Files Affected:**
- Modified: app initialization code (shortcut registration).

**Acceptance Criteria:**
- All shortcuts are registered without conflicts.
- Each shortcut triggers the correct navigation command.
- Shortcuts appear in the ShortcutOverlay.
- Shortcuts are listed in the command palette alongside their commands.

**Dependencies:** Task 16.

---

### Task 20: Go-To Integration with Tab Bar

**Title:** Add Go-to-File integration with tab switching

**Description:** Cmd+P results should prioritize currently open tabs. Additionally,
Ctrl+Tab should cycle through open tabs in MRU order.

**Implementation Details:**
- When in file picker mode, boost scores for files that are currently open in tabs.
- Add a "Open Tabs" section at the top when the query is empty.
- Ctrl+Tab opens a quick-switch overlay showing open tabs in MRU order.
- Holding Ctrl and pressing Tab cycles through tabs; releasing Ctrl selects.
- The TabBar tracks its own MRU order.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (tab boost)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` (MRU tracking)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (MRU logic)

**Acceptance Criteria:**
- Open tabs appear at the top of Cmd+P results.
- Ctrl+Tab shows a quick-switch overlay in MRU tab order.
- Releasing Ctrl selects the highlighted tab.
- Tab MRU order updates on tab activation.

**Dependencies:** Phase 31 Task 3 (file picker).

---

### Task 21: Unit Tests for Go-To System

**Title:** Comprehensive test suite for NavigationService and related components

**Description:** Write Catch2 tests covering the navigation stack, link resolution,
backlink finding, and diagnostic navigation.

**Implementation Details:**
- Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_goto_system.cpp`
- Sections:
  - NavigationService: push, pop, back, forward, stack overflow, coalescing.
  - LinkResolver: wiki-link resolution, markdown link resolution, block ref resolution, anchor resolution.
  - BacklinkFinder: find all references, empty references, cross-document.
  - DiagnosticService: next/previous, wrapping, severity filtering.
  - ChangeTracker: changed lines, save clears changes.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/test_goto_system.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- All test sections pass.
- NavigationService stack tests cover edge cases (empty, full, duplicate suppression).
- LinkResolver tests cover all link types.
- At least 25 test cases across all sections.

**Dependencies:** Tasks 1, 6, 10, 12, 13.

---

### Task 22: Global Navigation Shortcut Chord Support

**Title:** Support chord-style shortcuts (e.g., Ctrl+K Ctrl+D) for navigation commands

**Description:** Some navigation commands require two-key sequences (chords). Implement
chord detection in the shortcut system for navigation-specific commands.

**Implementation Details:**
- Extend `ShortcutManager` to support chord sequences: first key enters a "pending chord"
  state, second key completes the chord within a timeout (1000ms).
- Show a "Waiting for second key..." indicator in the status bar during pending state.
- Register chord shortcuts:
  - Ctrl+K Ctrl+D: Go to Definition
  - Ctrl+K Ctrl+I: Show hover/info
- If the second key times out, cancel the chord and process the key normally.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h` (chord support)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (chord indicator)

**Acceptance Criteria:**
- Ctrl+K followed by Ctrl+D within 1s triggers Go-to-Definition.
- "Waiting for second key..." appears in status bar after first key.
- Timeout (>1s) cancels the chord.
- Non-chord shortcuts still work normally.
- Chord shortcuts are displayed correctly in the palette and shortcut overlay.

**Dependencies:** Task 19.

---

### Task 23: Go-To Command Registration in CommandRegistry

**Title:** Register all Go-To commands in CommandRegistry with proper metadata

**Description:** Ensure every Go-To command is registered in `CommandRegistry` with
complete metadata (title, category, description, shortcut, when-clause, icon).

**Implementation Details:**
- Create a `GoToCommandRegistrar` function called during app initialization.
- Register all 14+ Go-To commands with:
  - Category: "Navigation" or "Go To"
  - Icon: appropriate navigation icon
  - When-clause: context-appropriate (e.g., "editorTextFocus" for F12)
  - Shortcut: matching the registered ShortcutManager binding
- Commands should be searchable in the palette via category "Go To".

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/GoToCommandRegistrar.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/GoToCommandRegistrar.cpp`
- Modified: app initialization code.

**Acceptance Criteria:**
- All Go-To commands appear in the command palette under "Go To" or "Navigation" category.
- Each command has a complete description.
- When-clauses correctly enable/disable commands based on context.
- Searching "go to" in the palette shows all navigation commands.

**Dependencies:** Task 16, Task 19.

---

### Task 24: Cross-Document Navigation Stack Persistence

**Title:** Persist navigation history across application restarts

**Description:** Save the navigation stack to disk so that the user can resume their
navigation history after restarting the application.

**Implementation Details:**
- On application shutdown, serialize `NavigationService` history to
  `~/.markamp/navigation_history.json`.
- On startup, deserialize and restore the stack.
- Store only the last 50 entries (trimmed on save).
- Each entry stores: `document_id`, `line`, `column`, `timestamp_epoch`.
- If a referenced document no longer exists, skip that entry on load.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.h` (serialize/deserialize)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/NavigationService.cpp`

**Acceptance Criteria:**
- Navigation history is saved on app shutdown.
- History is restored on app startup.
- Entries for deleted files are skipped.
- Maximum 50 entries persisted.
- File format is human-readable JSON.

**Dependencies:** Task 1.

---

### Task 25: Performance Benchmarks for Navigation Operations

**Title:** Add performance benchmarks for critical navigation paths

**Description:** Ensure navigation operations meet latency targets: Go-To-File < 50ms,
Go-To-Symbol < 100ms, Link resolution < 20ms, Back/Forward < 5ms.

**Implementation Details:**
- Create benchmark file: `/Users/ryanrentfro/code/markamp/tests/unit/bench_goto.cpp`
- Benchmark scenarios:
  - Navigate to a file in a 1000-file workspace.
  - Search symbols across 100 documents.
  - Resolve 50 wiki-links.
  - Navigate back/forward 100 times.
  - Find references for a file with 200 backlinks.
- Use `std::chrono::high_resolution_clock` for timing.
- Report P50 and P99 latencies.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/bench_goto.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- All benchmarks complete and print latency reports.
- Go-To-File P99 < 50ms on a 1000-file workspace.
- Go-To-Symbol P99 < 100ms on 100 documents.
- Back/Forward P99 < 5ms.
- Link resolution P99 < 20ms.

**Dependencies:** Tasks 1, 4, 6, 10.
