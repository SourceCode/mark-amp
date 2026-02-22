# Phase 47 -- Peek View System

## Objective

Implement a VSCode-style Peek View system that renders inline embedded editors below the current editor line for viewing definitions, references, implementations, call hierarchies, and type hierarchies without leaving the current file. The peek view appears as an expandable bordered panel that can be resized, navigated with keyboard, and chained (peeking from within a peek).

## Prerequisites

- Phase 46 complete (Diff Editor and Merge Tool)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h` -- main editor component
- `/Users/ryanrentfro/code/markamp/src/core/LanguageProviderRegistry.h` -- language service integration
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.h` -- for "open in full editor" from peek
- Phase 42 (ThemedScrollbar for peek editor scrolling)
- Phase 41 (IconButton, ThemedButton for peek toolbar)

## VSCode Reference Behavior

- Peek Definition: Alt+F12 opens inline editor below cursor line showing the definition
- Peek References: Shift+F12 opens inline editor with list of references on the left, preview on the right
- Peek header: filename with line number, result count, navigation arrows, close button
- Peek body: mini-editor with syntax highlighting, line numbers, scrollbar
- Peek resize: drag the bottom border to resize vertically (min 3 lines, max 20 lines)
- Peek navigation: Up/Down arrows cycle through results in the reference list
- Peek chain: invoking peek from within a peek replaces the content (no nested peek)
- Peek close: Escape closes the peek, or click outside
- Peek open in full editor: double-click or Enter on a result opens it in a new tab

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/PeekView.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_peek_view.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Define PeekProvider Interface

**Description:** Create an interface that language providers implement to supply peek data (definitions, references, implementations, call hierarchy, type hierarchy).

**Implementation Details:** `IPeekProvider` interface with methods: `find_definition(file, line, col) -> vector<PeekLocation>`, `find_references(file, line, col) -> vector<PeekLocation>`, `find_implementations(file, line, col) -> vector<PeekLocation>`, `find_call_hierarchy(file, line, col) -> PeekHierarchy`, `find_type_hierarchy(file, line, col) -> PeekHierarchy`. `PeekLocation` struct: `{ string file_path; int line; int column; string preview_text; string context; }`. `PeekHierarchy` struct: `{ PeekLocation item; vector<PeekHierarchy> children; }`. Create a `PeekProviderRegistry` that maps file types to providers.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.cpp` (create)

**Acceptance Criteria:**
- IPeekProvider interface compiles with all 5 methods
- PeekLocation carries all necessary location data
- PeekHierarchy supports recursive tree structure
- PeekProviderRegistry maps file types to providers
- Default markdown provider supplies heading-based definitions

**Dependencies:** None

---

### Task 02 -- Create PeekView Container

**Description:** Create the main PeekView panel that embeds inline in the editor below the invocation line, containing a header bar, an optional result list, and a preview editor.

**Implementation Details:** PeekView extends ThemeAwareWindow. It renders as a bordered panel (1px accent_primary border, 2px border-radius on top corners) that inserts itself into the editor's line layout. Height: configurable, default 200px. Layout: (1) header bar (24px): filename, line number, result count ("1 of N"), left/right navigation arrows (IconButton), close X button, (2) body: result list (left, 200px width) and preview editor (right, flex). The peek view is positioned by the EditorPanel directly below the line where it was invoked, pushing subsequent lines down.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (create)

**Acceptance Criteria:**
- Peek view renders inline below the invocation line
- Header shows filename, result count, and navigation
- Body shows result list and preview editor
- Close button (X) and Escape key close the peek
- Peek view pushes subsequent editor lines down

**Dependencies:** Phase 41 Task 03 (IconButton)

---

### Task 03 -- Create PeekResultList

**Description:** Create the result list panel shown on the left side of the peek view when there are multiple results (e.g., multiple references).

**Implementation Details:** PeekResultList extends ThemeAwareWindow. Renders a scrollable list of PeekLocation items. Each row (20px height): file icon (14x14), filename (truncated), line number (muted). Selected row has accent background. Up/Down arrow keys navigate the list. Enter on a row opens the file in a full editor tab. Single-click selects the row and updates the preview. The list uses virtual scrolling for large result sets (1000+ references). The list width is configurable (drag the splitter between list and preview).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (create)

**Acceptance Criteria:**
- Result list renders all PeekLocations
- Arrow keys navigate the list
- Selected item updates the preview editor
- Enter opens the item in a full editor tab
- Virtual scrolling handles 1000+ results

**Dependencies:** Task 02

---

### Task 04 -- Create PeekEditorHost

**Description:** Create the preview editor that shows the content of the selected peek result with syntax highlighting, line numbers, and a themed scrollbar.

**Implementation Details:** PeekEditorHost embeds a read-only EditorPanel configured for peek mode: no tab bar, no toolbar, reduced margin, themed scrollbar (Phase 42). The editor loads the file from the selected PeekLocation and scrolls to center the target line. The target line is highlighted with a subtle accent background. The editor supports full syntax highlighting based on file type. Content is loaded lazily (only when the result is selected). The editor respects the current theme. If the file is the same as the parent editor, the content is shared (no duplicate loading).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp` (create)

**Acceptance Criteria:**
- Preview editor shows file content with syntax highlighting
- Target line is centered and highlighted
- Themed scrollbar works within peek editor
- Content loads lazily on result selection
- Same-file content is shared (not duplicated)

**Dependencies:** Task 02

---

### Task 05 -- Implement Peek Definition (Alt+F12)

**Description:** Wire the Peek Definition command that opens a PeekView showing the definition of the symbol under the cursor.

**Implementation Details:** Register command `editor.peekDefinition` with shortcut `Alt+F12`. On invocation: get the word under cursor, call `IPeekProvider::find_definition()`, create PeekView below the current line. If only one result, hide the result list and show the preview only (full width). If multiple results (e.g., overloaded functions), show the result list. If no results, show a brief notification "No definition found". The peek view receives focus so keyboard navigation works immediately.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Alt+F12 opens peek view with definition
- Single result shows preview only (no list)
- Multiple results show list and preview
- No results shows notification
- Peek receives focus on open

**Dependencies:** Tasks 01, 02, 03, 04

---

### Task 06 -- Implement Peek References (Shift+F12)

**Description:** Wire the Peek References command that opens a PeekView showing all references to the symbol under the cursor.

**Implementation Details:** Register command `editor.peekReferences` with shortcut `Shift+F12`. On invocation: call `IPeekProvider::find_references()`. Always show the result list (references typically have multiple results). The result list groups by file: file path header rows (non-selectable, bold) followed by reference rows showing line number and context text. The first reference is selected by default. Reference count shows in the header ("23 references in 7 files").

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (modify)

**Acceptance Criteria:**
- Shift+F12 opens peek with references
- References grouped by file in result list
- First reference selected by default
- Header shows total count and file count
- Clicking a reference shows its context in preview

**Dependencies:** Tasks 01, 02, 03, 04

---

### Task 07 -- Implement Peek Implementation

**Description:** Wire the Peek Implementation command that shows implementations of an interface or abstract method.

**Implementation Details:** Register command `editor.peekImplementation` with shortcut `Cmd+F12`. Behavior is identical to Peek References but calls `IPeekProvider::find_implementations()`. For Markdown files, this maps to: finding all documents that embed or transclude the current heading. The result list shows each implementation location. If no implementations exist (e.g., concrete class), show "No implementations found" notification.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Cmd+F12 opens peek with implementations
- For Markdown, shows transclusion/embed references
- No results shows notification
- Result list and preview work correctly
- Keyboard shortcut documented

**Dependencies:** Tasks 01, 02, 03, 04

---

### Task 08 -- Implement Peek View Resize

**Description:** Allow the user to resize the peek view vertically by dragging its bottom border.

**Implementation Details:** The peek view has a 4px drag handle at its bottom edge. On mouse-down on the handle, begin resize. On mouse-move, update the peek height (minimum 80px / ~3 lines, maximum 500px / ~20 lines). The cursor changes to `kResizeNS` over the drag handle. The resize is smooth (no step snapping). The peek height preference is remembered per peek type (definition peek vs. reference peek) and persists during the session. On resize, the editor lines below the peek adjust their position.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)

**Acceptance Criteria:**
- Bottom border is draggable for resize
- Minimum height: 80px, maximum: 500px
- Cursor changes to NS resize over drag handle
- Editor lines adjust below the peek
- Height preference persists per peek type during session

**Dependencies:** Task 02

---

### Task 09 -- Implement Peek View Navigation (Arrow Keys)

**Description:** When the peek result list has focus, Up/Down arrows cycle through results, updating the preview. Left/Right arrows expand/collapse file groups.

**Implementation Details:** The PeekResultList handles keyboard events: Down arrow moves to the next result, Up arrow moves to the previous. When reaching the end, wrap to the beginning. Left arrow collapses a file group header. Right arrow expands a collapsed group. Enter opens the selected result in a full editor tab and closes the peek. The preview editor updates in real time as the user navigates (lazy loading ensures no lag for distant files). The header updates the "N of M" counter.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (modify)

**Acceptance Criteria:**
- Up/Down navigate results with wrap-around
- Preview updates on each navigation step
- Left/Right collapse/expand file groups
- Enter opens in full editor and closes peek
- Header counter updates on navigation

**Dependencies:** Task 03

---

### Task 10 -- Implement Peek Close Behavior

**Description:** Define all the ways a peek view can be closed: Escape key, click outside, close button, opening the result in a full editor, invoking a different peek.

**Implementation Details:** PeekView closes on: (1) Escape key press, (2) close X button click, (3) click outside the peek view bounds (on the parent editor), (4) Enter on a result (opens in full editor first), (5) invoking a new peek command (replaces current peek). On close: animate the peek view collapsing (150ms, height to 0), remove from the editor line layout, restore focus to the parent editor at the original cursor position. If the user scrolled in the peek editor, the parent editor's scroll position is preserved.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Escape closes the peek
- Click outside closes the peek
- Close button works
- Enter opens result and closes peek
- New peek replaces current peek

**Dependencies:** Task 02

---

### Task 11 -- Implement Peek Open in Full Editor

**Description:** Allow opening the currently previewed result in a full editor tab, navigating to the exact line and column.

**Implementation Details:** Double-click on a result in the PeekResultList, or press Enter, opens the file in a new editor tab (via `PaneManager::open_in_pane()`), scrolls to the target line, and places the cursor at the target column. If the file is already open in another tab, switch to that tab instead of opening a duplicate. After opening, the peek view closes. A toolbar button "Open in Editor" (arrow-up-right icon) provides the same functionality for keyboard-free access.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (modify)

**Acceptance Criteria:**
- Double-click opens file in full editor tab
- Enter opens file and closes peek
- Cursor positioned at exact line and column
- Existing open tabs are reused (no duplicates)
- "Open in Editor" toolbar button works

**Dependencies:** Task 09

---

### Task 12 -- Implement Peek View Chain

**Description:** When the user invokes a peek command from within an existing peek view, replace the peek content instead of nesting (which would be too complex).

**Implementation Details:** If a peek is already open and the user invokes another peek (e.g., peek definition from within a reference peek), the current peek content is replaced with the new results. A "back" button appears in the peek header (left arrow) that restores the previous peek content. The navigation stack can hold up to 5 levels. Back button is only visible when the stack has history. The peek header shows a breadcrumb trail of the navigation path: "file1.md > func > file2.md > class".

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)

**Acceptance Criteria:**
- Peek from within peek replaces content (no nesting)
- Back button restores previous peek content
- Navigation stack supports up to 5 levels
- Breadcrumb shows navigation path
- Back button hidden when no history

**Dependencies:** Task 02

---

### Task 13 -- Implement Call Hierarchy Peek

**Description:** Add a call hierarchy view in the peek panel that shows incoming and outgoing calls for the symbol under the cursor.

**Implementation Details:** Register command `editor.peekCallHierarchy` with shortcut `Cmd+Shift+H`. The peek view shows a tree structure in the result list instead of a flat list. Root: the current symbol. Children: incoming calls (functions that call this function) or outgoing calls (functions called by this function). A toggle button in the header switches between "Incoming" and "Outgoing" modes. Each tree node is expandable (lazy-load children on expand). Selecting a node previews the call site in the peek editor.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Call hierarchy renders as tree in result list
- Incoming/Outgoing toggle switches direction
- Tree nodes expand lazily
- Selecting a node previews the call site
- Works for function calls in supported languages

**Dependencies:** Tasks 01, 02, 03

---

### Task 14 -- Implement Type Hierarchy Peek

**Description:** Add a type hierarchy view showing base classes and derived classes for a type.

**Implementation Details:** Register command `editor.peekTypeHierarchy` with shortcut `Cmd+Shift+T`. Similar to call hierarchy (Task 13) but shows type relationships. Root: the current type. Up direction: base classes/interfaces. Down direction: derived classes/implementations. Toggle between "Supertypes" and "Subtypes" in the header. For Markdown, this maps to heading hierarchy: supertypes are parent headings, subtypes are child headings. Tree nodes expand lazily via the IPeekProvider.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Type hierarchy renders as tree
- Supertypes/Subtypes toggle works
- Markdown heading hierarchy is supported
- Tree nodes expand lazily
- Selecting a node previews the type definition

**Dependencies:** Tasks 01, 02, 03

---

### Task 15 -- Implement Peek View Syntax Highlighting

**Description:** Ensure the peek editor has full syntax highlighting matching the file type of the peeked file.

**Implementation Details:** PeekEditorHost determines the file type from the PeekLocation's file path extension. It configures the embedded EditorPanel with the appropriate syntax highlighter from GrammarEngine. Highlighting runs asynchronously (via AsyncHighlighter) so the peek content appears immediately with plain text, then colorizes. If the peeked file is very large, only highlight the visible region (50 lines centered on the target line).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp` (modify)

**Acceptance Criteria:**
- Peek editor shows correct syntax highlighting
- Highlighting is async (content appears before colors)
- Large files only highlight visible region
- Different file types use correct grammar
- Theme colors are consistent with main editor

**Dependencies:** Task 04

---

### Task 16 -- Implement Peek View Line Numbers

**Description:** Show line numbers in the peek editor that match the original file's line numbers.

**Implementation Details:** The peek editor's line numbers start from the file's actual line numbers (not from 1). If the peeked file starts showing from line 150, the first visible line number is 150. Line numbers render in the same style as the main editor's gutter (muted color, right-aligned). The target line's number is highlighted with accent color. Line numbers use the same font size as the peek editor content.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekEditorHost.cpp` (modify)

**Acceptance Criteria:**
- Line numbers match original file line numbers
- Target line number is highlighted
- Line number style matches main editor
- Line numbers are right-aligned in gutter
- Scrolling updates line numbers correctly

**Dependencies:** Task 04

---

### Task 17 -- Implement Peek View Theme-Aware Styling

**Description:** Ensure the peek view renders correctly in all themes: border colors, background colors, header colors, and result list colors all derive from theme tokens.

**Implementation Details:** PeekView uses theme tokens: `bg_peek` (slightly different from editor background for contrast), `border_peek` (accent_primary), `bg_peek_header` (bg_panel), `fg_peek_header` (fg_primary), `bg_peek_result_hover` (bg_hover). The peek border is 2px accent_primary at the top (to visually anchor it to the editor) and 1px border_default on the other sides. On theme change, all peek elements refresh their colors. Add these tokens to ThemeEngine if they do not exist (falling back to existing similar tokens).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.h` (modify if needed)

**Acceptance Criteria:**
- Peek view renders correctly in Dark theme
- Peek view renders correctly in Light theme
- Theme switch updates all peek colors immediately
- Peek background has subtle contrast from editor background
- Border colors derive from theme tokens

**Dependencies:** Task 02

---

### Task 18 -- Integrate Peek with EditorPanel Scroll

**Description:** When a peek view is open, ensure the parent editor handles scrolling correctly: the peek view stays attached to its invocation line and moves with it.

**Implementation Details:** The peek view is logically attached to a line number in the parent editor. When the editor scrolls, the peek view moves with that line (since it is part of the editor's line layout). If the invocation line scrolls out of view, the peek view clips at the viewport edge. When scrolling brings the line back into view, the peek view reappears. The editor's scroll calculations must account for the peek view's height when computing total content height and line positions below the peek.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Peek view moves with its invocation line during scroll
- Editor scroll calculations account for peek height
- Peek clips at viewport edge when scrolled out
- Lines below peek have correct positions
- No jitter or misalignment during fast scrolling

**Dependencies:** Task 02

---

### Task 19 -- Implement Markdown-Specific Peek Providers

**Description:** Create peek providers for Markdown files: peek heading definition (navigate to heading), peek backlinks (show all files linking to current file), peek embeds (show embedded content).

**Implementation Details:** Create `MarkdownPeekProvider` implementing `IPeekProvider`. For `find_definition()`: given a `[[wiki-link]]` or `[text](url)`, find the target document/heading. For `find_references()`: query BacklinkService for all documents that link to the current document. For `find_implementations()`: find all documents that embed/transclude the current heading. Call hierarchy: not applicable for Markdown (return empty). Type hierarchy: heading hierarchy (parent headings above, child headings below). Register this provider for `.md` files.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.cpp` (modify)

**Acceptance Criteria:**
- Wiki links resolve to target documents
- Backlinks are returned as references
- Embed/transclusion targets are returned as implementations
- Heading hierarchy works for type hierarchy
- Provider registers for .md file extension

**Dependencies:** Task 01

---

### Task 20 -- Implement Peek for C++ Files

**Description:** Create a peek provider for C++ files using the existing syntax analysis infrastructure.

**Implementation Details:** Create `CppPeekProvider` implementing `IPeekProvider`. For `find_definition()`: search for matching function/class/variable declarations in the same file and included headers (basic heuristic: search for the symbol name in .h files in the same directory). For `find_references()`: search for the symbol name across all open files and workspace files (using SearchService). For `find_implementations()`: search for method implementations matching a header declaration. This is a heuristic-based provider (not a full LSP), suitable until LSP integration is added.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PeekProvider.cpp` (modify)

**Acceptance Criteria:**
- C++ definitions are found in header files
- References are found across workspace files
- Implementations match header declarations
- Search is fast enough for interactive use (under 500ms)
- Provider registers for .h, .hpp, .cpp, .cc file extensions

**Dependencies:** Task 01

---

### Task 21 -- Add Peek Commands to Command Palette

**Description:** Register all peek commands in the Command system.

**Implementation Details:** Register: `editor.peekDefinition` (Alt+F12), `editor.peekReferences` (Shift+F12), `editor.peekImplementation` (Cmd+F12), `editor.peekCallHierarchy` (Cmd+Shift+H), `editor.peekTypeHierarchy` (Cmd+Shift+T), `peek.close` (Escape), `peek.back` (Alt+Left), `peek.openInEditor` (Enter), `peek.nextResult` (Down), `peek.previousResult` (Up). All commands categorized under "Peek" in the palette. Context keys: `peekVisible` is true when a peek is open, enabling/disabling peek-specific commands.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Command.cpp` (modify)

**Acceptance Criteria:**
- All peek commands appear in Command Palette
- Keyboard shortcuts work from editor context
- Peek-specific commands only appear when peek is open
- Commands are categorized under "Peek"
- Context keys correctly reflect peek state

**Dependencies:** Tasks 05-14

---

### Task 22 -- Implement Peek Accessibility

**Description:** Ensure peek views are accessible: screen reader announces peek open/close, result navigation, and preview content.

**Implementation Details:** On peek open: announce "Peek view opened: {peek type}, {N} results, showing {filename}". On result navigation: announce "{filename}, line {N}, {preview text}". On peek close: announce "Peek view closed". PeekView has role `wxACC_ROLE_GROUPING`. PeekResultList has role `wxACC_ROLE_LIST`. Result items have role `wxACC_ROLE_LISTITEM`. Peek editor has role `wxACC_ROLE_TEXT`. Focus management: when peek opens, focus moves to result list. When peek closes, focus returns to parent editor.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PeekResultList.cpp` (modify)

**Acceptance Criteria:**
- VoiceOver announces peek open and close
- Result navigation is announced
- Focus management is correct
- All peek elements have correct roles
- Keyboard-only usage is fully supported

**Dependencies:** Tasks 02, 03

---

### Task 23 -- Implement Peek View Animation

**Description:** Animate peek view opening and closing: expand from 0 height on open, collapse to 0 height on close.

**Implementation Details:** On open: peek view starts at 0px height and expands to target height over 200ms with ease-out-cubic. On close: peek collapses from current height to 0px over 150ms with ease-in-cubic, then is destroyed. During animation, the editor lines below the peek smoothly shift position. The AnimationDriver (Phase 41) handles the interpolation. If the user presses Escape rapidly (before open animation completes), the peek is immediately destroyed without the close animation.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PeekView.cpp` (modify)

**Acceptance Criteria:**
- Peek expands smoothly from 0 to target height
- Peek collapses smoothly on close
- Editor lines shift smoothly during animation
- Rapid Escape during opening skips animation
- Animation timing: 200ms open, 150ms close

**Dependencies:** Task 02, Phase 41 Task 19

---

### Task 24 -- Performance Validation for Peek Operations

**Description:** Ensure peek operations are responsive: opening a peek should take under 200ms including provider query and UI rendering.

**Implementation Details:** Profile the full peek lifecycle: (1) provider query time, (2) peek view creation time, (3) content loading time, (4) syntax highlighting time. Assert total time under 200ms for local files. For large result sets (1000+ references), the result list must render instantly (virtual scrolling) while results continue loading in the background. The preview editor must show content within 50ms of result selection. Test with: small project (100 files), medium project (1000 files), large project (10000 files).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_peek_view.cpp` (extend)

**Acceptance Criteria:**
- Peek opens in under 200ms for local files
- Result list renders instantly for 1000+ results
- Preview updates in under 50ms on result selection
- No frame drops during peek animation
- Background loading completes for all results

**Dependencies:** Tasks 01-15

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new peek view files to CMakeLists.txt and create unit tests.

**Implementation Details:** Add all new .h/.cpp files to CMakeLists.txt. Create `test_peek_view.cpp` with sections: (1) PeekProviderRegistry maps file types correctly, (2) MarkdownPeekProvider resolves wiki-links, (3) MarkdownPeekProvider returns backlinks as references, (4) PeekLocation struct serializes/deserializes, (5) PeekHierarchy tree structure traversal, (6) PeekView navigation stack (chain) with back, (7) PeekView resize respects min/max bounds, (8) PeekResultList groups by file correctly, (9) CppPeekProvider finds definitions in header files, (10) Peek close restores parent editor focus.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_peek_view.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- Provider tests work without GUI
- No undefined symbol errors
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- Peek Definition works (Alt+F12)
- Peek References works with result list (Shift+F12)
- Peek Implementation works (Cmd+F12)
- Call and type hierarchy peeks work
- Peek view resizes, navigates, and chains correctly
- Peek view has correct theme-aware styling
- Peek opens and closes with smooth animation
- Markdown and C++ peek providers are functional
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
