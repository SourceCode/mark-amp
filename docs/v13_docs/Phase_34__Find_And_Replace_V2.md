# Phase 34: Find & Replace V2

## Overview

Redesign the in-file find bar and project-wide search into a cohesive, production-grade
search and replace system. The current codebase has extensive search infrastructure:
`SearchService` (FTS5-backed full-text search), `SearchEngine` (in-process inverted index
with BM25 scoring), `FindReplaceService` (regex find/replace with capture groups),
`IncrementalSearcher` (background progressive search), `SearchQueryParser` (advanced query
syntax), `SearchHistoryManager` (persistent history with deduplication), `SearchPanelModel`
(grouped results with replace preview), `FindReplaceModel` (match tracking with scope),
`SearchSidebarPanel` (basic UI), and `SearchPanel.h` (data-only, no wxPanel). This phase
builds the full UI layer on top of this rich backend, redesigns the find bar, adds
project-wide search with a dedicated results panel, and wires the complete replace workflow.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| SearchService | `/Users/ryanrentfro/code/markamp/src/core/SearchService.h` | Full FTS5 search, rebuild index, stats |
| SearchEngine | `/Users/ryanrentfro/code/markamp/src/core/SearchEngine.h` | In-process BM25 inverted index |
| FindReplaceService | `/Users/ryanrentfro/code/markamp/src/core/FindReplaceService.h` | Regex/literal find, replace_all, preview |
| IncrementalSearcher | `/Users/ryanrentfro/code/markamp/src/core/IncrementalSearcher.h` | Background search with progressive delivery |
| SearchQueryParser | `/Users/ryanrentfro/code/markamp/src/core/SearchQueryParser.h` | Quoted phrases, scope prefixes, negation, OR |
| SearchHistoryManager | `/Users/ryanrentfro/code/markamp/src/core/SearchHistory.h` | Persistent history, deduplication, autocomplete |
| SearchPanelModel | `/Users/ryanrentfro/code/markamp/src/ui/SearchPanelModel.h` | Grouped results, replace preview, history |
| FindReplaceModel | `/Users/ryanrentfro/code/markamp/src/ui/FindReplaceModel.h` | Match count, navigation, scope |
| SearchSidebarPanel | `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.h` | Basic sidebar with search input and results list |
| SearchPanel.h | `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.h` | Data-only (structs), no wxPanel class |

## Dependencies

- Phase 32 (Go-To System) references search for "Go to References" (Shift+F12).
- Phase 35 (Quick Open) shares frecency-scored file list infrastructure.

---

## Tasks

### Task 1: In-File Find Bar Redesign

**Title:** Build a modern in-file find bar as an editor-embedded panel

**Description:** Replace the basic search functionality with a floating find bar at the
top of the editor (similar to Cmd+F in VSCode). The bar contains: a search input, toggle
buttons for Match Case / Whole Word / Regex, a match counter, and previous/next navigation.

**Implementation Details:**
- Create `FindBar` as a `wxPanel` that sits at the top of the `EditorPanel` layout.
- The bar contains:
  - `wxTextCtrl` for search input with placeholder "Find".
  - Three icon toggle buttons: Match Case [Aa], Whole Word [ab], Regex [.*].
  - Match counter label: "3 of 47".
  - Up/Down arrow buttons for previous/next match.
  - Close button (X) on the right.
- Wire toggle state to `FindReplaceModel` (case_sensitive, whole_word, regex).
- On input change, use `IncrementalSearcher` for progressive results.
- Highlight all matches in the editor using `wxStyledTextCtrl` indicators.
- Show/hide with Cmd+F / Escape.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h` (embed FindBar)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp`

**Acceptance Criteria:**
- Cmd+F shows the find bar at the top of the editor.
- Toggle buttons control Match Case, Whole Word, and Regex.
- Match counter shows "N of M" and updates in real-time.
- Up/Down arrows navigate between matches.
- All matches are highlighted in the editor with indicator markers.
- Escape hides the find bar.

**Dependencies:** None.

---

### Task 2: Replace Bar Extension

**Title:** Add replace input to the find bar

**Description:** Extend the find bar with a replace row that appears when the user
activates Replace mode (Cmd+H or a toggle in the find bar).

**Implementation Details:**
- Add a second row below the find input with:
  - `wxTextCtrl` for replace input with placeholder "Replace".
  - "Replace" button (replace current match).
  - "Replace All" button.
  - Preserve Case toggle.
- The replace row shows/hides with a disclosure toggle or Cmd+H.
- Wire to `FindReplaceService::replace_single` and `replace_all`.
- Show a confirmation dialog before Replace All if > 10 matches.
- After Replace All, show a toast: "Replaced N occurrences."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.h` (replace row)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp`

**Acceptance Criteria:**
- Cmd+H shows the find bar with replace row visible.
- "Replace" replaces the current match and advances to the next.
- "Replace All" replaces all matches with confirmation for > 10.
- Preserve Case toggle works (match original casing pattern).
- Post-replace toast shows the count.

**Dependencies:** Task 1.

---

### Task 3: Find in Selection

**Title:** Add "Find in Selection" scope to the find bar

**Description:** Allow the user to restrict find/replace to the currently selected text
range in the editor.

**Implementation Details:**
- Add a "Find in Selection" toggle button to the find bar (icon: text with bracket markers).
- When active, only search within the `wxStyledTextCtrl::GetSelectionStart()` to
  `GetSelectionEnd()` range.
- If no text is selected when toggled on, show a message: "Select text first."
- The selection range is captured when the toggle is activated and remains fixed
  (does not change if the user clicks in the editor).
- Wire to `FindReplaceModel::set_scope(FindScope::kSelection)`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.h` (selection scope toggle)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp`

**Acceptance Criteria:**
- "Find in Selection" toggle restricts search to the selected range.
- Match counter reflects only matches within the selection.
- Replace only affects matches within the selection.
- Toggling off restores full-document search.
- No selection shows an error message.

**Dependencies:** Task 1.

---

### Task 4: Regex Match Group Highlighting

**Title:** Highlight capture groups in regex match results

**Description:** When regex mode is active, show capture group highlighting in the
editor and in the replace preview.

**Implementation Details:**
- When a regex match is found with capture groups, use different indicator styles
  (or colors) for each group.
- In the replace input, show live preview of `$1`, `$2`, etc. expansions.
- Below the replace input, show a preview line: "Preview: [expanded replacement text]".
- Use `FindReplaceService::expand_replacement` for expansion.
- Color-code groups: Group 1 = blue, Group 2 = green, Group 3 = orange, etc.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.h` (regex preview)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp`

**Acceptance Criteria:**
- Regex capture groups are highlighted with different colors.
- Replace preview shows expanded $1, $2 values.
- Preview updates in real-time as the replace input changes.
- Invalid regex shows an error message below the input.

**Dependencies:** Task 1.

---

### Task 5: Search Across Project (Cmd+Shift+F)

**Title:** Build the project-wide search UI in the sidebar

**Description:** Redesign `SearchSidebarPanel` into a full-featured project search
panel with query input, scope controls, and a tree-structured results display.

**Implementation Details:**
- Rebuild `SearchSidebarPanel` with:
  - Search input with the same toggle buttons (Case, Word, Regex).
  - Replace input row (collapsed by default, expanded with a toggle).
  - "Files to include" input (glob patterns, e.g., `*.md`).
  - "Files to exclude" input (glob patterns, e.g., `node_modules/**`).
  - Results tree grouped by file, each file expandable to show matches.
- Route search to `SearchEngine::search` or `FindReplaceService::find_all`
  depending on scope.
- Progressive result delivery via `IncrementalSearcher` with a progress indicator.
- Match count badge: "47 results in 12 files".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.h` (full redesign)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`

**Acceptance Criteria:**
- Cmd+Shift+F opens the search sidebar.
- Results are grouped by file in a tree view.
- Include/exclude glob patterns filter results.
- Match count shows "N results in M files".
- Progressive results appear as the search runs.

**Dependencies:** Task 1 (for consistent toggle buttons).

---

### Task 6: Search Results Panel

**Title:** Create a dedicated search results panel for project-wide results

**Description:** In addition to the sidebar, provide a full-width bottom panel for
reviewing search results with more space for context and replace previews.

**Implementation Details:**
- Create `SearchResultsPanel` as a bottom panel (like Output/Problems panels).
- Show results in a table: File | Line | Match Context | Replace Preview.
- Each row is clickable to navigate to the match.
- Group by file with collapsible headers showing file path and match count.
- Support batch replace: checkbox per match to include/exclude from Replace All.
- Wire `SearchPanelModel::toggle_match_inclusion` for per-match selection.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Search results panel appears in the bottom panel area.
- Results are grouped by file with collapse headers.
- Each match row shows line number and context.
- Clicking a match navigates the editor to that location.
- Checkboxes allow per-match include/exclude for replace.

**Dependencies:** Task 5.

---

### Task 7: File Type Filter

**Title:** Add file type filter dropdown to project search

**Description:** Add a dropdown that filters results by file type (Markdown, Code, Config,
All). This is a convenience wrapper around include glob patterns.

**Implementation Details:**
- Add a `wxChoice` dropdown to the search sidebar: "All Files", "Markdown (.md)",
  "Code (.cpp, .h)", "Config (.json, .yaml, .toml)", "Custom...".
- Selecting a type sets the include glob pattern automatically.
- "Custom..." opens the include pattern input for manual editing.
- Persist last-used file type in Config.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.h` (file type dropdown)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`

**Acceptance Criteria:**
- File type dropdown filters search results.
- Selecting "Markdown" only searches `.md` files.
- "Custom..." activates the include pattern input.
- Last-used type persists across sessions.

**Dependencies:** Task 5.

---

### Task 8: Search History Integration

**Title:** Wire SearchHistoryManager to the search UI

**Description:** Show search history in the search input as a dropdown and provide
autocomplete suggestions as the user types.

**Implementation Details:**
- When the search input is focused and empty, show a dropdown of recent searches
  from `SearchHistoryManager::get_history()`.
- As the user types, show `SearchHistoryManager::find_matching(prefix)` results.
- Selecting a history entry populates the input and optionally re-executes the search.
- Each history entry shows: query text, result count, relative time ("3 hours ago").
- "Clear History" option at the bottom of the dropdown.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (history dropdown)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp` (history for in-file search)

**Acceptance Criteria:**
- Empty search input shows recent searches.
- Typing filters history entries.
- Selecting a history entry populates the input.
- "Clear History" clears all entries.
- History entries show result count and time.

**Dependencies:** Task 5, SearchHistoryManager.

---

### Task 9: Search Scope Selector (Workspace/Folder/File)

**Title:** Add scope selector for project-wide search

**Description:** Allow the user to choose the search scope: entire workspace, a specific
folder, or the current file.

**Implementation Details:**
- Add a scope selector below the search input in the sidebar.
- Options: "Workspace", "Current Folder" (folder of the active file), "Current File",
  "Open Files Only".
- "Current Folder" auto-fills with the active file's directory.
- A "Browse..." option allows selecting an arbitrary folder.
- Wire scope to `FindReplaceOptions::Scope` and `SearchScope::folders`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.h` (scope selector)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp`

**Acceptance Criteria:**
- Scope selector shows Workspace, Current Folder, Current File, Open Files.
- Selecting a scope restricts search results accordingly.
- "Current Folder" auto-fills with the active file's directory.
- "Browse..." opens a folder chooser dialog.

**Dependencies:** Task 5.

---

### Task 10: Live Replace Preview

**Title:** Show live preview of replacements before applying them

**Description:** In the results panel, show what each match will look like after
replacement. Users can review changes before committing.

**Implementation Details:**
- When a replace text is entered, compute previews using
  `FindReplaceService::preview_replace`.
- In the search results panel, show each match with a diff view:
  - Red strikethrough on the matched text.
  - Green highlighted replacement text.
- Update previews in real-time as the replace input changes.
- Debounce preview computation by 300ms.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp` (preview rendering)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (inline preview)

**Acceptance Criteria:**
- Replace preview shows before/after for each match.
- Matched text shown with red strikethrough.
- Replacement text shown in green.
- Preview updates as replace text changes.
- Preview is debounced to avoid lag.

**Dependencies:** Task 6.

---

### Task 11: Undo Project-Wide Replace

**Title:** Implement undo for project-wide replace operations

**Description:** After a project-wide Replace All, the user should be able to undo the
entire batch operation.

**Implementation Details:**
- Before executing Replace All, save a snapshot of all affected files.
- Store snapshots in a `ReplaceUndoBuffer` with a single undo action that reverts
  all files to their pre-replace state.
- Expose "Undo Last Replace All" command in CommandRegistry.
- Show a notification: "Replaced N in M files. [Undo]" with a clickable undo link.
- Undo buffer holds only the most recent Replace All (not a full undo stack).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/ReplaceUndoBuffer.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/ReplaceUndoBuffer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (undo integration)

**Acceptance Criteria:**
- Replace All creates a snapshot before modifying files.
- "Undo Last Replace All" reverts all changes.
- Notification with undo link appears after Replace All.
- Only the most recent Replace All is undoable.
- Undo correctly handles files that were modified between replace and undo.

**Dependencies:** Task 10.

---

### Task 12: Match Navigation in Editor

**Title:** Navigate between search matches in the editor with visual indicators

**Description:** When search matches are highlighted in the editor, F3/Shift+F3 navigate
to the next/previous match. The current match is highlighted differently from other matches.

**Implementation Details:**
- Use `wxStyledTextCtrl` indicators: one indicator style for "all matches" (subtle highlight),
  another for "current match" (bold highlight with border).
- F3: move to next match (wrapping at document end).
- Shift+F3: move to previous match (wrapping at document start).
- Update the match counter in the find bar: "3 of 47".
- Center the current match in the viewport.
- Remove all indicators when the find bar is closed.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp` (F3 navigation)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (indicator management)

**Acceptance Criteria:**
- F3 moves to the next match.
- Shift+F3 moves to the previous match.
- Current match has a distinct highlight.
- Match counter updates on navigation.
- Closing the find bar clears all match indicators.

**Dependencies:** Task 1.

---

### Task 13: Search Query Syntax Help

**Title:** Add inline help for advanced search query syntax

**Description:** Show a help popup explaining the supported search syntax (quoted phrases,
scope prefixes, negation, OR) from `SearchQueryParser`.

**Implementation Details:**
- Add a "?" help icon button in the search sidebar.
- Clicking shows a popup with syntax documentation:
  - `"exact phrase"` - Quoted phrase search
  - `tag:important` - Search by tag
  - `path:notes/` - Search by path
  - `-excluded` - Exclude term
  - `cats OR dogs` - Boolean OR
- The popup is dismissible with Escape or clicking outside.
- Show the help popup the first time the user opens project search.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (help popup)

**Acceptance Criteria:**
- Help icon appears in the search sidebar.
- Clicking shows syntax documentation.
- All supported syntax from SearchQueryParser is documented.
- Popup is dismissible.
- First-time users see the help automatically.

**Dependencies:** Task 5.

---

### Task 14: Preserve Case Replace

**Title:** Implement preserve-case replacement logic

**Description:** When "Preserve Case" is toggled, the replacement text adapts to the
casing pattern of each match.

**Implementation Details:**
- Analyze each match's casing pattern:
  - All uppercase: replace is all uppercase.
  - All lowercase: replace is all lowercase.
  - Title Case: replace is title case.
  - camelCase: replace follows camelCase pattern.
  - Mixed/other: replace as-is.
- Implement `preserve_case_replace(matched_text, replacement) -> std::string`.
- Wire to the Preserve Case toggle in the find bar and search sidebar.
- Show the case-adjusted preview in the live replace preview.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/FindReplaceService.h` (preserve_case_replace)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/FindReplaceService.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp` (toggle wire)

**Acceptance Criteria:**
- "HELLO" matched with replacement "world" becomes "WORLD".
- "hello" matched with replacement "World" becomes "world".
- "Hello" matched with replacement "world" becomes "World".
- "camelCase" matched preserves camelCase pattern in replacement.
- Preview shows the case-adjusted replacement.

**Dependencies:** Task 2.

---

### Task 15: Search Result Export

**Title:** Export search results to a file

**Description:** Allow the user to export search results (with context) to a Markdown
or plain text file.

**Implementation Details:**
- Add an "Export Results" button in the search results panel.
- Export formats: Markdown (grouped by file with code blocks), plain text, CSV.
- Each exported result includes: file path, line number, match line text.
- File chooser dialog for export destination.
- Use `SearchExporter` (header exists at `/Users/ryanrentfro/code/markamp/src/core/SearchExporter.h`).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/SearchExporter.h` (implement if header-only)
- New: `/Users/ryanrentfro/code/markamp/src/core/SearchExporter.cpp` (if needed)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp` (export button)

**Acceptance Criteria:**
- "Export Results" shows a file chooser.
- Markdown export groups results by file with proper formatting.
- CSV export includes file, line, column, match text, context.
- Export includes the search query and timestamp in a header.

**Dependencies:** Task 6.

---

### Task 16: ANSI Color Support in Search Results

**Title:** Render ANSI color codes in search result context

**Description:** When search results include terminal-style ANSI color codes (from
grep-like output), render them with appropriate colors.

**Implementation Details:**
- Parse ANSI escape sequences in match context strings.
- Map ANSI colors to theme-appropriate wxColour values.
- Render colored text spans in the search results panel.
- Strip ANSI codes for plain-text operations (copy, export).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/AnsiColorParser.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/AnsiColorParser.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp`

**Acceptance Criteria:**
- ANSI colored text renders with correct colors in results.
- Standard 16 ANSI colors are supported.
- Stripping ANSI codes produces clean plain text.
- Theme switch updates ANSI color mapping.

**Dependencies:** Task 6.

---

### Task 17: Search Progress and Cancellation

**Title:** Show search progress indicator and support cancellation

**Description:** For long-running project-wide searches, show a progress indicator and
allow the user to cancel.

**Implementation Details:**
- Use `IncrementalSearcher`'s `CompleteCallback` and progressive `MatchCallback`.
- Show a progress bar or spinner in the search sidebar during search.
- Display "Searching... (42 files scanned)" updating progressively.
- "Cancel" button (or Escape) calls `IncrementalSearcher::cancel()`.
- After cancellation, show partial results with "Search cancelled. Showing N of M matches."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (progress, cancel)

**Acceptance Criteria:**
- Progress indicator appears during search.
- File count updates as files are scanned.
- Cancel button stops the search.
- Partial results are shown after cancellation.
- Completion clears the progress indicator.

**Dependencies:** Task 5.

---

### Task 18: Search Bookmarks

**Title:** Allow bookmarking specific search results for later reference

**Description:** Users can bookmark individual search matches. Bookmarked results appear
in a persistent "Bookmarks" section.

**Implementation Details:**
- Right-click a search result > "Bookmark This Match".
- Bookmarked matches are stored in `Config` (key: "search.bookmarks").
- Each bookmark: `{file_path, line_number, match_text, query}`.
- "Bookmarks" section in the search sidebar showing all bookmarked matches.
- Bookmarks survive search changes and application restart.
- "Clear All Bookmarks" option.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (bookmarks)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp` (bookmark action)

**Acceptance Criteria:**
- Right-click shows "Bookmark This Match" option.
- Bookmarked matches appear in a persistent section.
- Bookmarks survive application restart.
- Clicking a bookmark navigates to the match.
- "Clear All" removes all bookmarks.

**Dependencies:** Task 5.

---

### Task 19: Multi-Line Search

**Title:** Support multi-line search patterns

**Description:** Allow search patterns that span multiple lines, especially useful for
finding code blocks or multi-line Markdown constructs.

**Implementation Details:**
- Add a "Multi-line" toggle to the regex mode.
- When active, the search input expands to a multi-line text area.
- `\n` in the pattern matches newlines in the document.
- Wire to `FindReplaceOptions::multiline`.
- Multi-line matches show the full match span in results.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.h` (multi-line input expansion)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp`

**Acceptance Criteria:**
- Multi-line toggle expands the search input.
- Patterns with `\n` match across line boundaries.
- Multi-line matches are highlighted in the editor across lines.
- Results show the complete multi-line match context.

**Dependencies:** Task 1.

---

### Task 20: Search Sidebar Theme Integration

**Title:** Full theme support for the redesigned search UI

**Description:** Apply theme tokens to all search UI elements consistently.

**Implementation Details:**
- Theme tokens: `search.background`, `search.inputBackground`, `search.inputBorder`,
  `search.matchHighlight`, `search.currentMatchHighlight`, `search.resultBackground`,
  `search.resultHoverBackground`, `search.fileHeaderBackground`, `search.countBadge`.
- Apply to: FindBar, SearchSidebarPanel, SearchResultsPanel.
- Subscribe to `ThemeChangedEvent`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FindBar.cpp` (theme)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (theme)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp` (theme)

**Acceptance Criteria:**
- All search UI elements use theme tokens.
- Theme switch updates all colors immediately.
- Match highlights are consistent between find bar and search panel.

**Dependencies:** Tasks 1, 5, 6.

---

### Task 21: Find Bar Keyboard Shortcuts

**Title:** Register all find/replace keyboard shortcuts

**Description:** Ensure all find/replace shortcuts are registered and do not conflict.

**Implementation Details:**
- Register:
  - Cmd+F: Open find bar
  - Cmd+H: Open find bar with replace
  - Cmd+Shift+F: Open project search
  - F3: Find next
  - Shift+F3: Find previous
  - Cmd+D: Find next occurrence of selection
  - Cmd+Shift+L: Select all occurrences
  - Alt+Enter: Select all matches (in find bar context)
  - Escape: Close find bar
- All in "Search" category.

**Files Affected:**
- Modified: shortcut registration code.

**Acceptance Criteria:**
- All listed shortcuts are registered.
- No conflicts with existing shortcuts.
- Shortcuts appear in the shortcut overlay.

**Dependencies:** Task 1.

---

### Task 22: Select All Occurrences

**Title:** Implement "Select All Occurrences" (Cmd+Shift+L) for multi-cursor editing

**Description:** When text is selected, Cmd+Shift+L creates multiple cursors at all
occurrences of that text in the document.

**Implementation Details:**
- Find all occurrences of the selected text in the current document.
- Create a multi-selection in `wxStyledTextCtrl` using `AddSelection`.
- Typing now edits all occurrences simultaneously.
- Escape exits multi-cursor mode.
- Cmd+D adds the next occurrence to the selection incrementally.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (multi-selection)

**Acceptance Criteria:**
- Cmd+Shift+L selects all occurrences of the selected text.
- Typing edits all selections simultaneously.
- Escape exits multi-cursor mode.
- Cmd+D adds one occurrence at a time.

**Dependencies:** None.

---

### Task 23: Search Result Navigation with Context

**Title:** Show surrounding context lines for each search result

**Description:** In the search results panel, show 1-2 lines of context above and below
each match for better orientation.

**Implementation Details:**
- For each `FindMatch`, extract 1-2 context lines above and below.
- Display context lines in dimmed text.
- The match line shows the match highlighted in bold.
- Context line count configurable: `"search.contextLines"` (default: 1).
- Collapsing a file group hides context to save space.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchResultsPanel.cpp` (context lines)

**Acceptance Criteria:**
- Each search result shows context lines above and below.
- Context lines use dimmed text.
- Match text is highlighted in the result line.
- Context line count is configurable.
- Collapsed file groups hide context.

**Dependencies:** Task 6.

---

### Task 24: Wire SearchPanel.h Data Types to UI

**Title:** Connect the data-only SearchPanel.h structs to the full search UI

**Description:** `SearchPanel.h` defines `SearchFilterState`, `SearchResultItem`,
`ReplaceResult`, and `SearchHistoryEntry` but notes "The full UI implementation will be
in a future UI-focused batch." Wire these data types to the search UI.

**Implementation Details:**
- Use `SearchFilterState` as the source of truth for search sidebar filter toggles.
- Use `SearchResultItem` as the display model for search results.
- Use `ReplaceResult` for replace operation feedback.
- Bridge between `SearchResultItem` (UI model) and `SearchHit` (core model) in the
  search sidebar's event handlers.
- Remove the "deferred" comment from `SearchPanel.h`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.h` (remove deferred comment)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SearchSidebarPanel.cpp` (use SearchPanel types)

**Acceptance Criteria:**
- `SearchFilterState` drives the sidebar filter toggles.
- `SearchResultItem` is used for result rendering.
- `ReplaceResult` provides feedback after replace operations.
- The "deferred" comment is removed.
- No duplicate type definitions between SearchPanel.h and SearchPanelModel.h.

**Dependencies:** Task 5.

---

### Task 25: Unit Tests for Find & Replace V2

**Title:** Comprehensive test suite for find and replace functionality

**Description:** Write Catch2 tests for the find bar model, search result grouping,
replace preview, preserve case, and search history integration.

**Implementation Details:**
- Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_find_replace_v2.cpp`
- Sections:
  - FindReplaceModel: match count, navigation with wrapping, scope changes.
  - SearchPanelModel: grouping by file, collapse toggle, match inclusion.
  - PreserveCase: all-upper, all-lower, title case, camelCase, mixed.
  - SearchHistoryManager: add, deduplicate, autocomplete, persistence round-trip.
  - FindReplaceService: regex find, literal find, replace preview, capture group expansion.
  - SearchQueryParser: phrases, negation, scope prefixes, OR operator.
  - ReplaceUndoBuffer: snapshot, restore, single-undo policy.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/test_find_replace_v2.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- All test sections pass.
- PreserveCase tests cover all casing patterns.
- SearchQueryParser tests cover all syntax elements.
- At least 35 test cases across all sections.

**Dependencies:** Tasks 1, 2, 11, 14.
