# Phase 17: Search Panel V2

## Objective

Build a comprehensive workspace-wide search and replace panel for the sidebar, replacing the existing data-only `SearchPanel.h` stub with a full wxWidgets implementation. The current codebase has `SearchPanel.h` containing `SearchFilterState`, `SearchResultItem`, `ReplaceResult`, and `SearchHistoryEntry` data structures but no UI class. The sidebar already has `SidebarMode::kSearch`. This phase delivers a complete search experience matching VSCode's search panel: regex/case/word toggles, file pattern filters, result tree, match highlighting, inline replace preview, and search history.

## Prerequisites

- `SearchPanel.h` at `src/ui/SearchPanel.h` with data structures defined
- `SidebarMode::kSearch` in `src/ui/SidebarMode.h`
- `SidebarPanelRegistry` for registering the panel
- `ActivityBar` with search icon
- `core::Search.h` with `SearchMethod`, `SearchBlockType`, `SearchSortOrder`, `SearchGroupBy`
- `EventBus` and `ThemeEngine`
- Phase 12 complete (editor groups for opening search results in correct group)

## Deliverables

A complete workspace search and replace panel with 25+ tasks.

---

## Task 1: SearchSidebarPanel wxWidgets Class

**Title:** Create the main SearchSidebarPanel widget

**Description:** Create the wxWidgets panel class that serves as the search sidebar. It contains a search input area at the top, toggles, and a scrollable results area below.

**Implementation Details:**
- Create `src/ui/SearchSidebarPanel.h` / `.cpp` (the existing `SearchSidebarPanel.h` may need to be extended or replaced):
  ```cpp
  class SearchSidebarPanel : public wxPanel {
  public:
      SearchSidebarPanel(wxWindow* parent, core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus);
      void Focus(); // Focus the search input
      void SetSearchText(const std::string& text); // Pre-fill search
  private:
      void CreateLayout();
      wxPanel* search_input_area_;    // Top section with inputs
      wxScrolledWindow* results_area_; // Scrollable results
  };
  ```
- Register with `SidebarPanelRegistry` for `SidebarMode::kSearch`
- Panel height: fills entire sidebar

**Files Affected:**
- `src/ui/SearchSidebarPanel.h` (rewrite if exists, or create)
- `src/ui/SearchSidebarPanel.cpp` (new or rewrite)
- `src/ui/LayoutManager.cpp` (register search panel)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Panel registers for kSearch sidebar mode
- Clicking search activity bar icon shows the panel
- Panel fills sidebar area
- Basic layout with input area and results area

**Dependencies:** None

---

## Task 2: Search Input with Regex/Case/Word Toggles

**Title:** Build the search input field with toggle buttons

**Description:** Create a styled search input field with three toggle buttons on the right side: regex mode (.*), case sensitive (Aa), and whole word (Ab|).

**Implementation Details:**
- Custom-painted input area:
  - Text input: full width minus toggle buttons area
  - Toggle buttons: 24x24 each, icon-only, toggle on/off state
  - Regex toggle: ".*" text, accent when active
  - Case toggle: "Aa" text, accent when active
  - Word toggle: "Ab|" text with word boundary indicator, accent when active
- State: `bool regex_mode_{false}`, `bool case_sensitive_{false}`, `bool whole_word_{false}`
- On input change, trigger search after 300ms debounce
- Cmd+Shift+F focuses this input from anywhere

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (build input area)

**Acceptance Criteria:**
- Search input with 3 toggle buttons
- Toggles are visually on/off
- Input triggers search on type (debounced)
- Cmd+Shift+F focuses the input
- Toggles affect search behavior

**Dependencies:** Task 1

---

## Task 3: Replace Input with Preview

**Title:** Add a replace input field below the search input

**Description:** Add a toggleable replace input that appears when the user clicks a replace toggle button. The replace field supports the same regex back-references when regex mode is active.

**Implementation Details:**
- "Replace" toggle button (chevron) expands/collapses the replace input row
- Replace input: same width as search input
- Action buttons after replace input:
  - Replace current match (single arrow icon)
  - Replace all matches (double arrow icon)
- Preserve case option: checkbox "Preserve Case" that matches the case pattern of each match
- Preview: when typing in replace, show inline preview of the replacement in the results

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add replace input, preview logic)

**Acceptance Criteria:**
- Replace input toggleable
- Replace and Replace All buttons functional
- Preserve case checkbox
- Regex back-references work in replace
- Inline preview shows expected replacement

**Dependencies:** Task 2

---

## Task 4: Include/Exclude File Pattern Filters

**Title:** Add file inclusion and exclusion pattern inputs

**Description:** Add two additional input fields (collapsible): "Files to Include" and "Files to Exclude" that accept glob patterns to scope the search.

**Implementation Details:**
- Toggle expand button below the replace input reveals include/exclude fields
- Include patterns: comma-separated globs (e.g., "*.cpp, *.h, src/**")
- Exclude patterns: comma-separated globs (e.g., "*.min.js, node_modules/**, build/**")
- Default exclude patterns from workspace config
- Apply patterns to the search scope before executing search
- Display current filter status: "Searching in *.cpp, *.h"

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add filter inputs)

**Acceptance Criteria:**
- Include/exclude inputs visible when expanded
- Glob patterns correctly parsed
- Patterns applied to search scope
- Default excludes from config
- Filter status displayed

**Dependencies:** Task 2

---

## Task 5: Search Execution Engine

**Title:** Implement the workspace search execution engine

**Description:** Create the search backend that scans workspace files and returns matches. Supports plain text, regex, case-sensitive, and whole-word search modes.

**Implementation Details:**
- Create `src/core/WorkspaceSearchEngine.h` / `.cpp`:
  ```cpp
  class WorkspaceSearchEngine {
  public:
      struct SearchOptions {
          std::string query;
          bool regex_mode{false};
          bool case_sensitive{false};
          bool whole_word{false};
          std::vector<std::string> include_patterns;
          std::vector<std::string> exclude_patterns;
          int max_results{10000};
      };

      struct SearchMatch {
          std::string file_path;
          int line_number;
          int column;
          int match_length;
          std::string line_content; // full line text
          std::string context_before; // 1 line before
          std::string context_after;  // 1 line after
      };

      struct SearchResult {
          std::vector<SearchMatch> matches;
          int files_searched{0};
          int files_with_matches{0};
          std::chrono::milliseconds duration;
          bool truncated{false}; // true if max_results hit
      };

      auto Search(const SearchOptions& options, const std::string& workspace_root) -> SearchResult;
  };
  ```
- Run search on background thread, publish progress events
- Cancel ongoing search when new search starts
- Use `std::regex` for regex mode, manual matching for plain text

**Files Affected:**
- `src/core/WorkspaceSearchEngine.h` (new)
- `src/core/WorkspaceSearchEngine.cpp` (new)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Plain text search works across all workspace files
- Regex search uses std::regex
- Case sensitivity and whole word modes work
- Include/exclude patterns filter files
- Search runs on background thread
- Progress events published
- Cancelable

**Dependencies:** None

---

## Task 6: Search Results Tree (Grouped by File)

**Title:** Display search results in a tree grouped by file

**Description:** Show search results in a tree structure: top-level nodes are files (with match count), child nodes are individual matches (with line preview and match highlighting).

**Implementation Details:**
- Custom-rendered scrollable panel for results:
  ```
  src/ui/TabBar.cpp (5 matches)
      12:  void TabBar::AddTab(const std::string& [file_path], ...
      45:  void TabBar::RemoveTab(const std::string& [file_path])
      ...
  src/ui/BreadcrumbBar.cpp (2 matches)
      34:  void BreadcrumbBar::SetFilePath(const std::vector<std::string>& [segments])
      ...
  ```
- File nodes: file-type icon + relative path + match count badge
- Match nodes: line number + line text with match highlighted (bold/accent color)
- File nodes are collapsible
- Click match: open file and navigate to that line/column

**Files Affected:**
- `src/ui/SearchResultsTree.h` (new)
- `src/ui/SearchResultsTree.cpp` (new)
- `src/ui/SearchSidebarPanel.cpp` (integrate results tree)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Results grouped by file
- File nodes show icon, path, and match count
- Match lines show line number and highlighted match
- Click navigates to match position
- Files collapsible
- Scroll for large result sets

**Dependencies:** Task 5

---

## Task 7: Result Line Preview with Match Highlighting

**Title:** Highlight the matching text within result line previews

**Description:** In each search result line, highlight the matched text with accent color/bold to make it visually distinguishable from the surrounding context.

**Implementation Details:**
- In `DrawResultLine()`:
  ```cpp
  // Draw text before match in normal color
  dc.SetTextForeground(text_color);
  dc.DrawText(line.substr(0, match_start), x, y);
  // Draw match in accent color + bold
  dc.SetTextForeground(accent_color);
  wxFont bold_font = font;
  bold_font.SetWeight(wxFONTWEIGHT_BOLD);
  dc.SetFont(bold_font);
  dc.DrawText(line.substr(match_start, match_length), x + pre_width, y);
  // Draw text after match
  dc.SetFont(font);
  dc.SetTextForeground(text_color);
  dc.DrawText(line.substr(match_start + match_length), x + pre_width + match_width, y);
  ```
- Multiple matches on the same line: highlight all
- For replace mode: show struck-through match + replacement text

**Files Affected:**
- `src/ui/SearchResultsTree.cpp` (implement match highlighting in rendering)

**Acceptance Criteria:**
- Matched text highlighted in accent color and bold
- Multiple matches per line all highlighted
- Replace preview shows strike-through + replacement
- Highlighting uses theme colors

**Dependencies:** Task 6

---

## Task 8: Result Navigation (F4 Next Match)

**Title:** Implement keyboard navigation through search results

**Description:** Press F4 to jump to the next search result, Shift+F4 for previous. The focused result is highlighted in the results tree and the corresponding file line is shown in the editor.

**Implementation Details:**
- Track `int current_result_index_{-1}` in SearchSidebarPanel
- F4: increment index, scroll to result, open file, navigate to line
- Shift+F4: decrement index
- Update results tree highlight to show current result
- Wrap around at beginning/end of results
- Subscribe to F4 key event globally (when search panel is active)

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (implement result navigation)
- `src/ui/MainFrame.cpp` (wire F4 shortcut)

**Acceptance Criteria:**
- F4 navigates to next result
- Shift+F4 navigates to previous
- Current result highlighted in tree
- File opens and scrolls to match line
- Wraps at boundaries

**Dependencies:** Task 6

---

## Task 9: Search History Dropdown

**Title:** Maintain and display search history

**Description:** Store the last 20 search queries and display them in a dropdown when the search input is focused. Selecting a history item repopulates the search.

**Implementation Details:**
- Add `std::vector<SearchHistoryEntry> search_history_` (from SearchPanel.h data types)
- On each search execution, push to history (deduplicate)
- Show dropdown below search input on focus or down-arrow press:
  - Each entry: search query + result count + timestamp
  - Click to re-execute that search
- Clear history button at bottom of dropdown
- Persist history to config

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (implement history dropdown)
- `src/core/Config.h` (persist search history)

**Acceptance Criteria:**
- Last 20 searches stored
- Dropdown shows history on focus
- Click re-executes search
- Clear history button
- History persists across sessions

**Dependencies:** Task 2

---

## Task 10: Clear Results

**Title:** Add clear button to dismiss search results

**Description:** Add a clear ("X") button in the results header that dismisses all current search results and resets the search state.

**Implementation Details:**
- Clear button in the results area header, right-aligned
- On click:
  ```cpp
  search_results_.clear();
  current_result_index_ = -1;
  results_tree_->ClearResults();
  UpdateResultCount();
  ```
- Also clear when the search input is emptied
- Keyboard shortcut: Escape when focus is in search panel clears results, second Escape moves focus to editor

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add clear button and logic)

**Acceptance Criteria:**
- Clear button visible in results header
- Clicking clears all results
- Empty search input also clears
- Escape clears then defocuses

**Dependencies:** Task 6

---

## Task 11: Collapse/Expand All Results

**Title:** Add buttons to collapse or expand all result file groups

**Description:** Add collapse-all and expand-all buttons in the results header to quickly show/hide all match details.

**Implementation Details:**
- Two icon buttons in results header:
  - Collapse All: all file nodes collapsed (show only file names)
  - Expand All: all file nodes expanded (show all matches)
- Default: expanded
- Keyboard: Cmd+Shift+[ to collapse all, Cmd+Shift+] to expand all

**Files Affected:**
- `src/ui/SearchResultsTree.cpp` (implement collapse/expand all)
- `src/ui/SearchSidebarPanel.cpp` (add header buttons)

**Acceptance Criteria:**
- Collapse All hides all match details
- Expand All shows all match details
- Individual file groups still independently toggleable
- State preserved during search updates

**Dependencies:** Task 6

---

## Task 12: Result Count Badge

**Title:** Show total match count and file count

**Description:** Display result statistics in the search panel header: "N results in M files" with elapsed time.

**Implementation Details:**
- After search completes:
  ```cpp
  std::string status = std::to_string(total_matches) + " results in " +
                       std::to_string(files_with_matches) + " files";
  if (result.truncated) status += " (showing first " + std::to_string(max_results) + ")";
  status += " (" + std::to_string(duration_ms) + "ms)";
  result_count_label_->SetLabel(status);
  ```
- Badge also shown in activity bar for search icon
- During search: show "Searching..." with spinning indicator

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add result count display)
- `src/ui/ActivityBar.cpp` (set badge on search icon)

**Acceptance Criteria:**
- Match count and file count displayed
- Truncation warning shown when applicable
- Search duration shown
- "Searching..." during active search
- Activity bar badge shows result count

**Dependencies:** Task 5

---

## Task 13: Search in Selection

**Title:** Add "Search in Selection" mode

**Description:** When text is selected in the editor, provide an option to restrict the search to only the selected text range.

**Implementation Details:**
- Add "Search in Selection" toggle button (icon: text with brackets) next to the other toggles
- When active:
  - Store the selection range (file, start_line, end_line)
  - Search only scans lines within that range
  - Results outside the range filtered out
- Visual: selection range displayed as "lines 10-50 in file.md"
- Deactivate when selection changes significantly

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add selection scope)

**Acceptance Criteria:**
- Toggle button for selection mode
- Search restricted to selected lines
- Selection range displayed
- Works across multiple selections
- Deactivates gracefully

**Dependencies:** Task 2

---

## Task 14: Preserve Case in Replace

**Title:** Implement case-preserving replace

**Description:** When "Preserve Case" is enabled, the replacement text matches the case pattern of each individual match. For example, replacing "foo" with "bar": "Foo" becomes "Bar", "FOO" becomes "BAR", "foo" becomes "bar".

**Implementation Details:**
- Detect case pattern of each match:
  - All uppercase: apply uppercase to replacement
  - First letter uppercase: capitalize replacement
  - All lowercase: lowercase replacement
  - Mixed: use replacement as-is
- Apply case transform per match:
  ```cpp
  auto TransformCase(const std::string& replacement, const std::string& match) -> std::string {
      if (IsAllUpper(match)) return ToUpper(replacement);
      if (IsFirstUpper(match)) return Capitalize(replacement);
      if (IsAllLower(match)) return ToLower(replacement);
      return replacement; // mixed case: use as-is
  }
  ```

**Files Affected:**
- `src/core/WorkspaceSearchEngine.cpp` (add case-preserving replace logic)
- `src/ui/SearchSidebarPanel.cpp` (wire preserve case option)

**Acceptance Criteria:**
- Case patterns correctly detected
- Replacement text transformed per match
- Works with regex and plain text modes
- Preview shows correct case in results

**Dependencies:** Task 3

---

## Task 15: Context Lines Around Matches

**Title:** Show context lines around each match in results

**Description:** Optionally show 1-3 lines of context before and after each match result to provide more context without opening the file.

**Implementation Details:**
- Add "Show Context" toggle in results header
- When enabled, each match result shows:
  ```
  10:  // previous line (context, dimmed)
  11:  void TabBar::AddTab(const std::string& [file_path], ...  (match, highlighted)
  12:  // next line (context, dimmed)
  ```
- Context lines in `TextMuted` color, match lines in normal color
- Configurable context line count (1-3, default 1)

**Files Affected:**
- `src/ui/SearchResultsTree.cpp` (add context line rendering)
- `src/core/WorkspaceSearchEngine.h` (include context in SearchMatch)

**Acceptance Criteria:**
- Context lines shown above/below matches
- Context in muted color
- Configurable context count
- Togglable via button
- Context not shown when collapsed

**Dependencies:** Task 6

---

## Task 16: Replace in Files

**Title:** Implement batch replace across multiple files

**Description:** When "Replace All" is clicked, apply the replacement to all matches across all files in the workspace. Show a confirmation dialog with the scope, and provide an undo mechanism.

**Implementation Details:**
- On "Replace All":
  1. Show confirmation: "Replace N occurrences in M files?"
  2. For each file with matches:
     - Read file content
     - Apply all replacements (from end to start to preserve positions)
     - Write file
  3. Publish `FileContentChangedEvent` for each modified file
  4. Update open editors with new content
  5. Show summary: "Replaced N occurrences in M files"
- Undo: create a batch undo entry that reverts all changes
- Progress: show progress bar for large operations

**Files Affected:**
- `src/core/WorkspaceSearchEngine.cpp` (implement file replacement)
- `src/ui/SearchSidebarPanel.cpp` (wire replace all button)

**Acceptance Criteria:**
- Confirmation dialog before bulk replace
- All matches replaced across files
- Open editors updated
- Summary shown after completion
- Progress bar for large operations
- Files saved after replacement

**Dependencies:** Task 5

---

## Task 17: Single Match Replace

**Title:** Replace the current match and navigate to next

**Description:** The single-replace button replaces the currently focused match and automatically navigates to the next match.

**Implementation Details:**
- When "Replace Current" button is clicked:
  1. Apply replacement at the current match position
  2. Update the results (remove replaced match)
  3. Navigate to the next match (F4 behavior)
  4. Update the file in the editor
- Works in the currently open editor for the focused match's file

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (implement single replace)

**Acceptance Criteria:**
- Current match replaced with replacement text
- Automatically advances to next match
- Results tree updated (replaced match removed)
- Editor content updated
- File marked as modified

**Dependencies:** Tasks 3, 8

---

## Task 18: Search Progress Indicator

**Title:** Show search progress during long searches

**Description:** Display a progress indicator in the search panel header during workspace searches, showing the number of files scanned and matches found so far.

**Implementation Details:**
- During search, publish progress events from background thread:
  ```cpp
  struct SearchProgressEvent {
      int files_scanned;
      int total_files;
      int matches_found;
  };
  ```
- Display in search panel: "Searching: 142/1203 files (27 matches)..."
- Show a thin progress bar below the search input
- Cancel button: stop the current search

**Files Affected:**
- `src/core/WorkspaceSearchEngine.cpp` (publish progress events)
- `src/ui/SearchSidebarPanel.cpp` (display progress)
- `src/core/Events.h` (add progress event)

**Acceptance Criteria:**
- Progress shown during search
- File count and match count update in real-time
- Progress bar advances proportionally
- Cancel button stops search
- Final results shown after completion

**Dependencies:** Task 5

---

## Task 19: Search Result Actions

**Title:** Add action buttons on individual search results

**Description:** Each search result match should have action buttons on hover: "Dismiss" (remove from results), "Replace" (replace just this match), "Open in Side" (open in new editor group).

**Implementation Details:**
- On hover over a match result, show action icons at right edge:
  - "x": dismiss this result (remove from list, not from file)
  - Arrow: replace this single match
  - Split icon: open file in side editor group
- Actions appear on hover, disappear on mouse leave
- Dismiss: useful for curating results list
- Replace: applies replacement to just this one match

**Files Affected:**
- `src/ui/SearchResultsTree.cpp` (add hover action buttons)

**Acceptance Criteria:**
- Action buttons appear on hover
- Dismiss removes result from list
- Replace applies replacement to single match
- Open in Side opens in new editor group
- Actions disappear on mouse leave

**Dependencies:** Task 6

---

## Task 20: Search Scope Indicator

**Title:** Show the current search scope clearly

**Description:** Display the current search scope (entire workspace, specific folder, file type filter) prominently so the user knows where the search is looking.

**Implementation Details:**
- Below the filter inputs, show a scope summary:
  ```
  Searching in: src/**/*.cpp, src/**/*.h
  Excluding: build/**, test/**
  ```
- If searching in a specific folder (via context menu "Search in Folder"), show:
  ```
  Searching in: src/ui/
  ```
- Clickable to edit/clear filters
- Icon indicating scope (globe for workspace, folder for directory)

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add scope display)

**Acceptance Criteria:**
- Scope clearly displayed
- Include/exclude patterns shown
- Folder-scoped search indicated
- Clickable to edit filters
- Updates when filters change

**Dependencies:** Task 4

---

## Task 21: Search Panel Keyboard Shortcuts

**Title:** Complete keyboard shortcut support for search panel

**Description:** Implement all search-related keyboard shortcuts: Cmd+Shift+F (focus search), Cmd+Shift+H (focus replace), F4/Shift+F4 (navigate results), Enter (search), Cmd+Enter (replace all), Escape (close/clear).

**Implementation Details:**
- Keyboard shortcut map:
  - Cmd+Shift+F: focus search input (from anywhere)
  - Cmd+Shift+H: focus search + show replace
  - Enter (in search input): execute search
  - Cmd+Enter (in replace input): replace all
  - Shift+Enter (in replace input): replace current
  - F4: next result
  - Shift+F4: previous result
  - Escape: clear results if any, then close panel
  - Tab: cycle between search/replace/filter inputs
  - Cmd+Shift+1: toggle regex
  - Cmd+Shift+C: toggle case
  - Cmd+Shift+W: toggle whole word

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (implement keyboard shortcuts)
- `src/ui/MainFrame.cpp` (register global shortcuts)

**Acceptance Criteria:**
- All shortcuts functional
- Shortcuts work from any context
- Modifier keys shown in UI
- Discoverable in command palette

**Dependencies:** Tasks 2, 3

---

## Task 22: Search Panel Theme Integration

**Title:** Full theme support for search panel

**Description:** Ensure all search panel elements respond to theme changes correctly.

**Implementation Details:**
- Color mappings:
  - Panel background: `BgPanel`
  - Input background: `BgApp`
  - Input border: `BorderLight`
  - Input text: `TextMain`
  - Toggle active: `AccentPrimary`
  - Toggle inactive: `TextMuted`
  - Result file name: `TextMain`
  - Result line text: `TextMuted`
  - Match highlight: `AccentPrimary` background + bold
  - Context lines: `TextMuted` at 70%
  - Badge: `AccentSecondary` background
- Subscribe to theme change events

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (theme all elements)
- `src/ui/SearchResultsTree.cpp` (theme result rendering)

**Acceptance Criteria:**
- All elements themed correctly
- Updates on theme change
- Dark and light themes both work
- No hardcoded colors

**Dependencies:** Tasks 1-6

---

## Task 23: Replace Preview in Results

**Title:** Show inline replacement preview in search results

**Description:** When replace text is entered, show a visual preview of what the replacement would look like alongside each match: the original text struck through and the replacement text shown in green.

**Implementation Details:**
- In each result match, when replace text is non-empty:
  ```
  12:  void TabBar::AddTab(const std::string& ̶f̶i̶l̶e̶_̶p̶a̶t̶h̶ new_path, ...
  ```
- Original match text: red strikethrough
- Replacement text: green, inserted inline
- Toggle preview on/off
- Preserve case transformations shown in preview

**Files Affected:**
- `src/ui/SearchResultsTree.cpp` (add replace preview rendering)

**Acceptance Criteria:**
- Original match shown with strikethrough
- Replacement shown in green
- Case preservation visible in preview
- Preview toggleable
- Updates as replace text changes

**Dependencies:** Tasks 3, 7

---

## Task 24: Saved Search Queries

**Title:** Allow saving and naming search queries

**Description:** Users can save frequently used search queries with a name for quick re-execution. Saved searches persist and are accessible from a dropdown.

**Implementation Details:**
- Add "Save Search" button (star icon) in search input area
- Save dialog: enter name for the search
- Saved searches stored in config:
  ```yaml
  saved_searches:
    - name: "TODO comments"
      query: "TODO|FIXME|HACK"
      regex: true
      case_sensitive: false
    - name: "Console logs"
      query: "console.log"
      include: "*.ts,*.js"
  ```
- Dropdown accessible from search input (star icon menu)
- Edit/delete saved searches

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add save/load search)
- `src/core/Config.h` (add saved searches)

**Acceptance Criteria:**
- Save button stores current search
- Saved searches in dropdown menu
- Click re-executes saved search
- Edit/delete options
- Persists across sessions

**Dependencies:** Task 9

---

## Task 25: Search Panel Accessibility

**Title:** Ensure search panel is fully accessible

**Description:** Add accessibility support: proper roles, keyboard navigation, screen reader announcements for search results, and high-contrast support.

**Implementation Details:**
- Search input: role "searchbox", aria-label "Search"
- Toggle buttons: role "toggle", aria-checked state
- Results tree: role "tree" with "treeitem" nodes
- Screen reader announcements:
  - "N results found in M files"
  - "Navigated to result N of M"
  - "Replaced N occurrences"
- All interactive elements keyboard-accessible
- Tab order: search input -> toggles -> replace input -> results

**Files Affected:**
- `src/ui/SearchSidebarPanel.cpp` (add accessibility)
- `src/ui/SearchResultsTree.cpp` (add accessible roles)

**Acceptance Criteria:**
- All elements keyboard accessible
- Screen reader announcements for state changes
- Proper roles and labels
- Tab order logical
- High-contrast mode supported

**Dependencies:** Tasks 1-12

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Panel Shell (Task 1) | Medium |
| Search Input (Task 2) | Medium |
| Replace Input (Task 3) | Medium |
| File Filters (Task 4) | Low |
| Search Engine (Task 5) | High |
| Results Tree (Tasks 6-7) | High |
| Navigation (Task 8) | Medium |
| History (Task 9) | Medium |
| Clear/Collapse (Tasks 10-11) | Low |
| Result Count (Task 12) | Low |
| Search in Selection (Task 13) | Medium |
| Preserve Case (Task 14) | Medium |
| Context Lines (Task 15) | Medium |
| Batch Replace (Task 16) | High |
| Single Replace (Task 17) | Medium |
| Progress (Task 18) | Medium |
| Result Actions (Task 19) | Medium |
| Scope Display (Task 20) | Low |
| Keyboard Shortcuts (Task 21) | Medium |
| Theme (Task 22) | Low |
| Replace Preview (Task 23) | Medium |
| Saved Searches (Task 24) | Medium |
| Accessibility (Task 25) | Medium |

## Files Created

- `src/ui/SearchSidebarPanel.h` (or major rewrite)
- `src/ui/SearchSidebarPanel.cpp` (or major rewrite)
- `src/ui/SearchResultsTree.h`
- `src/ui/SearchResultsTree.cpp`
- `src/core/WorkspaceSearchEngine.h`
- `src/core/WorkspaceSearchEngine.cpp`

## Files Modified

- `src/ui/LayoutManager.cpp`
- `src/ui/MainFrame.cpp`
- `src/ui/ActivityBar.cpp`
- `src/core/Events.h`
- `src/core/Config.h`
- `CMakeLists.txt`
