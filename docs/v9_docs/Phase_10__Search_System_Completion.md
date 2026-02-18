# Phase 10: Search System Completion

## Overview
SearchEngine (1008 lines) and SearchQueryParser exist with substantial logic, but SearchPanel is 9 lines (empty stub). IncrementalSearcher, ContentIndexer, and FindReplaceService exist in core but are not connected to a functional search UI. This phase builds the full search experience.

## Prerequisites
- Phase 06 (Workbench navigation for Search sidebar mode)
- Phase 07 (Editor improvements for find/replace)

## Tasks

### Task 1: Implement SearchPanel UI
**Files:** `src/ui/SearchPanel.cpp`, `src/ui/SearchPanel.h`
**Description:** SearchPanel is a 9-line empty stub. Build a full workspace search panel: search input with regex/case/word toggles, results grouped by file with match context, replace input with preview, and match navigation.
**Acceptance Criteria:**
- Search input with regex, case-sensitive, whole-word toggles
- Results grouped by file with 2 lines of context per match
- Match count per file and total shown
- Replace input with "Replace" and "Replace All" buttons
- Keyboard: Enter to search, Escape to clear

### Task 2: Wire SearchEngine to SearchPanel
**Files:** `src/ui/SearchPanel.cpp`, `src/core/SearchEngine.cpp`
**Description:** Connect the SearchPanel UI to the existing SearchEngine for workspace-wide search. SearchEngine already supports full-text search with ranking.
**Acceptance Criteria:**
- Typing in search input triggers SearchEngine query (debounced 250ms)
- Results populated from SearchEngine response
- `SearchCompletedEvent` emitted with timing and result count
- Results update as user refines query

### Task 3: Wire IncrementalSearcher for Real-Time Results
**Files:** `src/core/IncrementalSearcher.cpp`, `src/ui/SearchPanel.cpp`
**Description:** IncrementalSearcher exists for fast incremental results. Wire it so results appear as the user types, with the most relevant results first.
**Acceptance Criteria:**
- Results start appearing after 2 characters typed
- Incremental updates as more characters typed
- Results ranked by relevance (exact match > prefix > fuzzy)
- Cancel previous search when new character typed

### Task 4: Add Search Result Navigation
**Files:** `src/ui/SearchPanel.cpp`, `src/core/Events.h`
**Description:** Clicking a search result opens the file and scrolls to the match. Arrow keys navigate between results. F4/Shift+F4 cycle through matches.
**Acceptance Criteria:**
- Click opens file at match location
- Match highlighted in editor
- Arrow keys navigate result list
- F4 / Shift+F4 cycle through all matches across files
- `SearchResultNavigatedEvent` emitted

### Task 5: Add Search Replace with Preview
**Files:** `src/ui/SearchPanel.cpp`, `src/core/FindReplaceService.cpp`
**Description:** Wire FindReplaceService (exists in core) to the search panel. Show replacement preview: each match with before/after text. Confirm before replacing all.
**Acceptance Criteria:**
- Replace input visible below search input
- Each result shows preview of replacement
- "Replace" replaces current match and moves to next
- "Replace All" shows confirmation dialog with count
- `SearchReplaceCompletedEvent` emitted with stats

### Task 6: Add Search Filters (File Type, Path, Date)
**Files:** `src/ui/SearchPanel.cpp`, `src/core/SearchQueryParser.cpp`
**Description:** SearchQueryParser exists with advanced syntax. Expose filters in the UI: file type (*.md, *.json), path include/exclude, date range (modified within).
**Acceptance Criteria:**
- Filter bar with file type, path, and date controls
- Filters compose with search query
- Advanced syntax: `file:*.md path:docs/ modified:7d`
- Filter state persisted for session

### Task 7: Add Search History
**Files:** `src/ui/SearchPanel.cpp`, `src/core/Config.h`
**Description:** Maintain a history of recent searches (last 50). Show history dropdown on search input focus. History persisted across sessions.
**Acceptance Criteria:**
- Down arrow in empty search input shows history
- History items clickable to re-run search
- History persisted to config (last 50)
- Clear history command available

### Task 8: Add Search and Replace in Selection
**Files:** `src/ui/EditorPanel.cpp`, `src/core/FindReplaceService.cpp`
**Description:** Add scope to in-editor find/replace: search within selection only. Useful for targeted replacements.
**Acceptance Criteria:**
- Toggle: "Search in Selection" when text is selected
- Matches limited to selection range
- Replace operates only within selection
- Selection expanded if replacement changes length

### Task 9: Add Regex Search with Capture Groups
**Files:** `src/core/FindReplaceService.cpp`, `src/core/SafeRegex.cpp`
**Description:** Replace supports regex capture group references ($1, $2, etc.) in replacement text. Uses SafeRegex for bounded execution.
**Acceptance Criteria:**
- Capture groups in regex patterns captured
- $1, $2, etc. in replacement text substituted
- $0 substitutes entire match
- Invalid regex shows error message in search panel

### Task 10: Add Global Search Command Palette Integration
**Files:** `src/ui/CommandPalette.cpp`, `src/ui/MainFrame.cpp`
**Description:** Add commands: "Search in Workspace", "Search in Current File", "Search and Replace". Keyboard shortcuts: Cmd+Shift+F (workspace), Cmd+F (file), Cmd+H (replace).
**Acceptance Criteria:**
- Three search commands in command palette
- Workspace search activates Search sidebar mode
- File search activates in-editor find bar
- Replace search activates with replace input visible

### Task 11: Add Search Index Auto-Rebuild
**Files:** `src/core/ContentIndexer.cpp`, `src/core/SearchEngine.cpp`
**Description:** ContentIndexer exists but auto-rebuild on file changes may not be wired. Ensure the search index updates when files are created, modified, or deleted in the workspace.
**Acceptance Criteria:**
- File save triggers index update for that file
- File create/delete triggers index update
- Batch file operations (e.g., git checkout) trigger full reindex
- `SearchIndexUpdatedEvent` emitted per file update

### Task 12: Add SQL Query Panel
**Files:** `src/core/QueryService.cpp`, `src/core/SqlQuery.cpp`
**Description:** QueryService and SqlQuery exist for SQL-based block queries. Add a command palette accessible SQL query panel that can query document metadata.
**Acceptance Criteria:**
- Command: "Open Query Panel"
- SQL input with syntax highlighting
- Results displayed in table format
- `QueryExecutedEvent` emitted with timing

### Task 13: Add Saved Searches
**Files:** `src/ui/SearchPanel.cpp`, `src/core/Config.h`
**Description:** Allow users to save frequently used search queries with a name. Saved searches appear as bookmarks in the search panel.
**Acceptance Criteria:**
- "Save Search" button saves current query with user-provided name
- Saved searches listed in sidebar of search panel
- Clicking saved search populates and runs the query
- Delete and rename for saved searches

### Task 14: Add Search Result Grouping Options
**Files:** `src/ui/SearchPanel.cpp`
**Description:** Results can be grouped by: file (default), directory, match type, or shown as flat list. User toggles grouping mode.
**Acceptance Criteria:**
- Four grouping modes selectable
- File grouping: results under file headers
- Directory grouping: results under directory then file
- Flat list: all matches in relevance order
- Grouping persisted

### Task 15: Add Search Result Export
**Files:** `src/ui/SearchPanel.cpp`
**Description:** Export search results to a new Markdown file: file paths, match contexts, and match count summary.
**Acceptance Criteria:**
- "Export Results" button in search panel
- Exports as Markdown with file paths and match contexts
- Opens the exported file in a new tab
- Includes query, filter, and match count summary

### Task 16: Add Search Badge on Activity Bar
**Files:** `src/ui/SearchPanel.cpp`, `src/ui/ActivityBar.cpp`
**Description:** When a search has results, show the total match count as a badge on the Search activity bar item.
**Acceptance Criteria:**
- Badge shows total match count from last search
- Badge updates when search results change
- Badge clears when search is cleared
- Badge format: "42" for counts, "99+" for large counts

### Task 17: Add Search Performance Tests
**Files:** `tests/unit/test_search_engine.cpp`, `tests/unit/test_search_query_parser.cpp`
**Description:** Test search performance: query response time, incremental update time, and large workspace handling.
**Acceptance Criteria:**
- Simple query on 1000-file workspace: < 100ms
- Incremental update after file change: < 50ms
- Regex query: < 200ms
- Memory usage: < 50MB for 1000-file index

### Task 18: Add Search Accessibility
**Files:** `tests/unit/test_accessibility.cpp`
**Description:** Verify search panel keyboard navigation: Tab between input/toggles/results, Enter to navigate, keyboard shortcuts work.
**Acceptance Criteria:**
- Full keyboard navigation without mouse
- Tab order: search input, toggles, replace input, results
- Arrow keys navigate results
- Screen reader announces match count and navigation

### Task 19: Add Search Panel Theme Compliance
**Files:** `src/ui/SearchPanel.cpp`
**Description:** Ensure search panel uses theme tokens for all visual elements. Match highlight color, result background, toggle states all from theme.
**Acceptance Criteria:**
- Zero hard-coded colors
- Match highlight uses `editor_find_hit` token
- Toggle active state uses accent color
- Works correctly with all 64 themes

### Task 20: Add Content Indexer Coverage Tests
**Files:** `tests/unit/test_search_engine.cpp`
**Description:** Test that ContentIndexer correctly indexes all Markdown elements: headings, paragraphs, code blocks, links, frontmatter, and tags.
**Acceptance Criteria:**
- Headings indexed and searchable by level
- Code block content searchable
- Frontmatter fields searchable
- Tags searchable with # prefix
- Wiki links searchable

## Testing Requirements
- Search finds results across multiple files
- Replace works correctly with regex and capture groups
- Incremental search updates in real-time
- Performance budgets met for large workspaces

## Phase Completion Criteria
- Full workspace search with results panel
- Search and replace with regex and preview
- Filters, history, and saved searches
- Real-time incremental results
- Full keyboard navigation and accessibility
- All tests pass
