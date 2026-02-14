# Phase 16 -- Search UI Panel Integration

**Priority:** High (user-facing feature completing the search pipeline)
**Estimated Scope:** ~10 files created/modified
**Dependencies:** Phase 14 (SearchService), Phase 15 (QueryService); Existing EventBus, Config, PluginContext, LayoutManager, ActivityBar, TabBar

---

## Objective

Implement the search UI that integrates with the full-text search engine and SQL query interface. This adds a `SearchPanel` to the sidebar with search input, filters, result list with highlighting, and result navigation. It also includes find-and-replace functionality and search history.

The SearchPanel is the primary user-facing component for knowledge discovery. It must be responsive (debounced search-as-you-type), visually rich (highlighted snippets, grouped results), and keyboard-driven (full navigation without mouse).

---

## Prerequisites

- Phase 14 completed: `SearchService`, `SearchQuery`, `SearchResult`, `SearchHit`, `SearchGroup`
- Phase 15 completed: `QueryService`, `QueryResult`, `SqlValidator`
- `src/core/EventBus.h` -- event pub/sub
- `src/core/Config.h` -- search UI settings
- `src/ui/LayoutManager.h` -- sidebar panel registration
- `src/ui/ActivityBar.h` -- activity bar item for search
- `src/ui/TabBar.h` -- open documents on result click
- `src/core/ThemeEngine.h` -- themed rendering of search results
- `src/core/Events.h` -- `EditorContentChangedEvent` for "search in current doc"

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `app/src/search/index.ts` | Search panel UI | `SearchPanel` wxPanel component |
| `app/src/search/util.ts` | Search helpers, result rendering | `SearchResultRenderer` |
| `app/src/search/spread.ts` | Replace in search results | `SearchPanel::perform_replace()` |
| `kernel/api/search.go` | Search API endpoints | `SearchService` (Phase 14) |
| `app/src/search/assets.ts` | Asset search tab | `SearchPanel` asset search mode |

---

## MarkAmp Integration Points

1. **ActivityBar** -- New `ActivityBarItem::Search` entry (already exists in `Events.h` enum). Clicking shows the SearchPanel in the sidebar.
2. **LayoutManager** -- SearchPanel registered as a sidebar panel, swappable with FileTreeCtrl, ExtensionsBrowserPanel, etc.
3. **TabBar / EditorPanel** -- Clicking a search result opens the target document in a tab (or activates existing tab) and scrolls to the matching block.
4. **SearchService (Phase 14)** -- All search operations delegate to `SearchService::search()`.
5. **QueryService (Phase 15)** -- SQL mode queries delegate to `QueryService::execute_sql()`.
6. **ThemeEngine** -- All SearchPanel rendering uses themed colors for backgrounds, text, highlights, and borders.
7. **EditorPanel** -- "Search in current document" mode scopes the query to the active document's blocks.
8. **Config** -- Debounce interval, history size, default filters.

---

## Data Structures to Implement

All UI structures go in `src/ui/SearchPanel.h`:

```cpp
#pragma once

#include "core/EventBus.h"
#include "core/Search.h"
#include "core/SqlQuery.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/timer.h>
#include <wx/listctrl.h>

#include <deque>
#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{
class Config;
class SearchService;
class QueryService;
} // namespace markamp::core

namespace markamp::ui
{

/// Filter state for search panel.
struct SearchFilterState
{
    /// Active search method.
    core::SearchMethod method{core::SearchMethod::Keyword};

    /// Selected block types for filtering (empty = all).
    std::set<core::SearchBlockType> selected_types;

    /// Selected notebook filter (empty = all).
    std::string notebook_filter;

    /// Path prefix filter (empty = all).
    std::string path_filter;

    /// Grouping mode.
    core::SearchGroupBy group_by{core::SearchGroupBy::NoGroup};

    /// Sort order.
    core::SearchSortOrder sort_order{core::SearchSortOrder::Relevance};

    /// Whether to search only in current document.
    bool current_doc_only{false};

    /// Case sensitivity.
    bool case_sensitive{false};
};

/// A rendered search result item for the result list.
struct SearchResultItem
{
    /// The search hit data.
    core::SearchHit hit;

    /// Pre-rendered HTML snippet for display.
    std::string rendered_snippet;

    /// Whether this item is currently selected.
    bool selected{false};

    /// Whether this item is a group header (when grouping by document).
    bool is_group_header{false};

    /// Group header data (only if is_group_header == true).
    std::string group_title;
    int group_hit_count{0};
    bool group_expanded{true};
};

/// Replace operation result.
struct ReplaceResult
{
    /// Number of replacements made.
    int replace_count{0};

    /// Number of blocks affected.
    int blocks_affected{0};

    /// Any errors encountered.
    std::vector<std::string> errors;

    /// Whether the operation succeeded.
    [[nodiscard]] auto is_success() const -> bool
    {
        return errors.empty();
    }
};

/// Search history entry.
struct SearchHistoryEntry
{
    /// The search query string.
    std::string query;

    /// When this search was performed.
    std::chrono::system_clock::time_point timestamp;

    /// Number of results returned.
    int result_count{0};
};

/// The main search panel UI component.
/// Provides search input, filters, result list, and replace functionality.
class SearchPanel : public wxPanel
{
public:
    SearchPanel(wxWindow* parent,
                core::ThemeEngine& theme_engine,
                core::EventBus& event_bus,
                core::Config& config,
                core::SearchService& search_service,
                core::QueryService& query_service);
    ~SearchPanel() override;

    // Non-copyable, non-movable (wxWidgets panel)
    SearchPanel(const SearchPanel&) = delete;
    auto operator=(const SearchPanel&) -> SearchPanel& = delete;
    SearchPanel(SearchPanel&&) = delete;
    auto operator=(SearchPanel&&) -> SearchPanel& = delete;

    /// Set focus to the search input field.
    void FocusSearchInput();

    /// Set the search query programmatically (e.g., from command palette).
    void SetQuery(const std::string& query);

    /// Set "search in current document" mode.
    void SetCurrentDocMode(bool enabled, const std::string& doc_id = "");

    /// Get the current filter state.
    [[nodiscard]] auto GetFilterState() const -> const SearchFilterState&;

    /// Clear all search results and reset the panel.
    void ClearResults();

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Config& config_;
    core::SearchService& search_service_;
    core::QueryService& query_service_;

    // ── UI Controls ──

    wxSearchCtrl* search_input_{nullptr};
    wxChoice* method_selector_{nullptr};       // Keyword / Phrase / Regex / SQL
    wxPanel* filter_panel_{nullptr};           // Expandable filter panel
    wxPanel* result_panel_{nullptr};           // Scrollable result list
    wxStaticText* result_count_label_{nullptr};
    wxPanel* replace_panel_{nullptr};          // Find & Replace controls
    wxTextCtrl* replace_input_{nullptr};
    wxButton* replace_btn_{nullptr};
    wxButton* replace_all_btn_{nullptr};
    wxCheckBox* group_by_doc_checkbox_{nullptr};
    wxChoice* sort_selector_{nullptr};

    // Block type filter checkboxes
    std::vector<std::pair<core::SearchBlockType, wxCheckBox*>> type_checkboxes_;

    // ── State ──

    SearchFilterState filter_state_;
    std::vector<SearchResultItem> result_items_;
    int selected_index_{-1};
    std::string current_doc_id_;
    bool replace_mode_{false};

    // ── Search History ──

    std::deque<SearchHistoryEntry> search_history_;
    static constexpr int kMaxHistorySize = 20;

    // ── Debounce ──

    wxTimer search_debounce_timer_;
    static constexpr int kDebounceMs = 300;

    // ── Event Subscriptions ──

    core::Subscription content_changed_sub_;
    core::Subscription active_file_sub_;

    // ── Internal Methods ──

    /// Build all UI controls.
    void BuildUI();

    /// Apply theme colors to all controls.
    void ApplyTheme();

    /// Execute a search with the current query and filters.
    void ExecuteSearch();

    /// Execute a SQL query (when method == SQL).
    void ExecuteSqlQuery();

    /// Render search results into the result panel.
    void RenderResults(const core::SearchResult& result);

    /// Render SQL query results into a table-like display.
    void RenderSqlResults(const core::QueryResult& result);

    /// Navigate to a search result (open doc, scroll to block).
    void NavigateToResult(int index);

    /// Perform replace on the selected result.
    void PerformReplace();

    /// Perform replace on all results.
    void PerformReplaceAll();

    /// Add a query to search history.
    void AddToHistory(const std::string& query, int result_count);

    /// Show search history dropdown.
    void ShowHistoryDropdown();

    /// Update the result count label.
    void UpdateResultCount(int count, double elapsed_ms);

    // ── Event Handlers ──

    void OnSearchInput(wxCommandEvent& event);
    void OnSearchDebounce(wxTimerEvent& event);
    void OnMethodChanged(wxCommandEvent& event);
    void OnSortChanged(wxCommandEvent& event);
    void OnGroupByDocChanged(wxCommandEvent& event);
    void OnTypeFilterChanged(wxCommandEvent& event);
    void OnResultClicked(wxMouseEvent& event);
    void OnResultDoubleClicked(wxMouseEvent& event);
    void OnReplaceClicked(wxCommandEvent& event);
    void OnReplaceAllClicked(wxCommandEvent& event);
    void OnKeyDown(wxKeyEvent& event);
};

} // namespace markamp::ui
```

---

## Key Functions to Implement

### SearchPanel methods (`src/ui/SearchPanel.cpp`)

1. **`void BuildUI()`** -- Construct the panel layout using wxBoxSizer. Top section: search input (wxSearchCtrl) with history dropdown button. Second row: method selector (wxChoice with Keyword/Phrase/Regex/SQL options). Collapsible filter panel: block type checkboxes (Document, Heading, Paragraph, Code, List, Table, Blockquote), notebook dropdown, path input. Result area: scrollable panel with result items. Bottom: status bar with result count and execution time. Replace panel (toggled): replace input and Replace/Replace All buttons.

2. **`void ExecuteSearch()`** -- Build a `SearchQuery` from the current input and filter state. Call `search_service_.search(query)`. Store results. Call `RenderResults()`. Update result count label. Add to search history. Handle errors by showing notification.

3. **`void ExecuteSqlQuery()`** -- When search method is SQL, validate the SQL via `query_service_.validate_sql()`. If valid, execute via `query_service_.execute_sql()`. Render results in a tabular format via `RenderSqlResults()`.

4. **`void RenderResults(const core::SearchResult& result)`** -- Clear the result panel. For each `SearchHit`, create a `SearchResultItem` with pre-rendered HTML snippet. If group-by-document is enabled, insert group header items. Populate the result list. Highlight the selected item.

5. **`void NavigateToResult(int index)`** -- Get the `SearchHit` at the given index. Publish `TabSwitchedEvent` with the document path to open/activate the tab. Publish `OutlineScrollToEvent` (Phase 11) or a new `SearchResultNavigatedEvent` with the block ID so the editor scrolls to the matching block.

6. **`void PerformReplace()`** -- Get the selected result item. Get the replace text from `replace_input_`. Execute the replacement on the block content (find the match range, replace the text). Publish `EditorContentChangedEvent`. Update the search results (remove the replaced item, re-run search to update counts).

7. **`void PerformReplaceAll()`** -- Iterate all result items. For each, execute the replacement. Collect results into a `ReplaceResult`. Show a notification with the count. Re-run search to update results.

8. **`void OnSearchDebounce(wxTimerEvent& event)`** -- Called when the debounce timer fires after user stops typing. Calls `ExecuteSearch()` or `ExecuteSqlQuery()` based on current method.

9. **`void OnKeyDown(wxKeyEvent& event)`** -- Handle keyboard shortcuts: Enter (execute search), Escape (close/clear), Up/Down arrows (navigate results), Ctrl+Enter (navigate to selected result), Ctrl+Shift+H (toggle replace mode).

10. **`void ShowHistoryDropdown()`** -- Display a dropdown list of recent searches. Clicking an entry populates the search input and executes the search. History entries show the query and result count.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Search UI events (Phase 16)
// ============================================================================

MARKAMP_DECLARE_EVENT(SearchPanelOpenedEvent);
MARKAMP_DECLARE_EVENT(SearchPanelClosedEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchResultNavigatedEvent)
std::string block_id;
std::string root_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchReplaceCompletedEvent)
int replace_count{0};
int blocks_affected{0};
MARKAMP_DECLARE_EVENT_END;
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.search.ui.debounce_ms` | int | `300` | Debounce interval for search-as-you-type |
| `knowledgebase.search.ui.history_size` | int | `20` | Number of recent searches to remember in panel |
| `knowledgebase.search.ui.show_filters` | bool | `true` | Show filter panel by default |
| `knowledgebase.search.ui.show_replace` | bool | `false` | Show replace panel by default |
| `knowledgebase.search.ui.result_preview_lines` | int | `3` | Lines of context to show per result |
| `knowledgebase.search.ui.group_by_document` | bool | `false` | Default group-by-document state |

---

## Test Cases

File: `tests/unit/test_search_panel.cpp`

```cpp
#include "core/Search.h"
#include "core/SearchService.h"
#include "core/SqlQuery.h"
#include "core/QueryService.h"
#include "core/EventBus.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

Note: UI tests are split into unit tests (logic/data flow) and would-be integration tests (wxWidgets rendering, tested manually or via wxUIActionSimulator). The Catch2 tests focus on testable logic.

1. **SearchFilterState construction** -- Create a `SearchFilterState`. Set method to `Phrase`, add type filters, set case_sensitive. Build a `SearchQuery` from it. Verify: all fields map correctly. `SearchQuery::method == SearchMethod::Phrase`, `types` contains selected types, `case_sensitive == true`.

2. **Debounced search trigger** -- Simulate rapid query changes (set query, wait 100ms, set query again, wait 100ms, set final query, wait 400ms). Verify: only ONE search execution occurred (after the final debounce period). This tests the debounce timer logic extracted from the panel.

3. **Result list rendering** -- Given a `SearchResult` with 5 hits (3 from doc A, 2 from doc B), render without grouping. Verify: 5 `SearchResultItem` entries. Render with `group_by == ByDocument`. Verify: 7 items (2 group headers + 5 results). Group headers have `is_group_header == true`.

4. **Filter application** -- Create a `SearchFilterState` with `selected_types = {Heading, Paragraph}`, `case_sensitive = true`. Build `SearchQuery`. Verify: `query.types` has 2 entries. Execute search. Verify: results contain only Heading and Paragraph blocks.

5. **Result click navigation** -- Given a result with `block_id = "X"` in document at path `"/docs/notes.md"`. Call `NavigateToResult()`. Verify: `SearchResultNavigatedEvent` published with `block_id == "X"`. Verify: the appropriate tab-switching event would be published.

6. **Search history storage** -- Execute 3 different searches. Verify: `search_history_` contains 3 entries in reverse chronological order. Execute a 4th search that duplicates the 2nd. Verify: history has 4 entries (no dedup in display, but most recent usage moves to top).

7. **Group-by-document mode** -- Search returns 10 hits across 4 documents. Toggle group_by_doc. Verify: `SearchResultItem` list contains 4 group headers each followed by their hits. Collapse a group. Verify: its hits are hidden (items marked with group_expanded=false are skipped in rendering).

8. **Replace preview** -- Set search query "old_text", replace text "new_text". Select a result. Call `PerformReplace()` (logic only, no actual file write in unit test). Verify: the replacement would produce correct content (match range replaced with new text). Verify: `SearchReplaceCompletedEvent` would fire with `replace_count == 1`.

9. **Keyboard navigation** -- Simulate Down arrow key presses on the result list. Verify: `selected_index_` increments. At last item, Down wraps to 0 (or stays). Up arrow decrements. Enter on selected item calls navigate.

10. **Empty state display** -- Execute search with no results. Verify: result_items_ is empty. `selected_index_ == -1`. Result count label shows "0 results".

---

## Acceptance Criteria

- [ ] SearchPanel renders in the sidebar with search input, method selector, and filter panel
- [ ] Search-as-you-type with configurable debounce interval
- [ ] All 4 search methods (Keyword, Phrase, Regex, SQL) accessible via method selector
- [ ] Block type filter checkboxes correctly restrict search results
- [ ] Notebook and path filters correctly scope the search
- [ ] Result list shows highlighted snippets with match context
- [ ] Group-by-document mode shows document headers with expand/collapse
- [ ] Sort selector changes result ordering (Relevance, Created, Updated, etc.)
- [ ] Clicking a result navigates to the target document and block
- [ ] Replace mode: single replace and replace-all function correctly
- [ ] Search history stores last N queries and is accessible via dropdown
- [ ] Keyboard navigation: Enter searches, Escape closes, Up/Down navigate results
- [ ] SQL mode validates queries and renders results in tabular format
- [ ] `SearchPanelOpenedEvent` / `SearchPanelClosedEvent` fire on show/hide
- [ ] `SearchResultNavigatedEvent` fires with correct block_id on result click
- [ ] All theme colors applied from ThemeEngine (no hardcoded colors)
- [ ] All 10 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/ui/SearchPanel.h` | `SearchFilterState`, `SearchResultItem`, `ReplaceResult`, `SearchHistoryEntry`, `SearchPanel` class |
| `src/ui/SearchPanel.cpp` | Full SearchPanel implementation: BuildUI, search execution, result rendering, replace, history, keyboard |
| `tests/unit/test_search_panel.cpp` | All 10 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `SearchPanelOpenedEvent`, `SearchPanelClosedEvent`, `SearchResultNavigatedEvent`, `SearchReplaceCompletedEvent` |
| `src/ui/LayoutManager.h` / `.cpp` | Register SearchPanel as sidebar panel, handle `ActivityBarItem::Search` selection |
| `src/ui/ActivityBar.h` / `.cpp` | Ensure `ActivityBarItem::Search` is wired to show SearchPanel |
| `src/ui/MainFrame.cpp` | Add `Ctrl+Shift+F` keyboard shortcut for global search (publishes `GlobalSearchRequestEvent`), wire SearchPanel creation |
| `src/CMakeLists.txt` | Add `ui/SearchPanel.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_search_panel` test target |
| `resources/config_defaults.json` | Add 6 `knowledgebase.search.ui.*` config keys |
