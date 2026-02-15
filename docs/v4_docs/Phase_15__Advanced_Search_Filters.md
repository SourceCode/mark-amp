# Phase 15 -- Advanced Search Filters and Search UI

## Objective

Build the Search Panel UI and extend the SearchEngine (Phase 05) with advanced filtering: boolean operators (AND, OR, NOT), scoped search (tag:, path:, title:, content:), date range filters, frontmatter field queries, and saved searches. The Search Panel provides the interactive search interface with real-time results, sort options, and filter chips.

## Prerequisites

- Phase 05 (SearchEngine -- core search)
- Phase 04 (TagService -- tag-based filtering)
- Phase 02 (VaultService -- document metadata)
- Existing MainFrame sidebar infrastructure

## Feature References (PRD)

- PRD #24: Search Filters (regex, boolean, scoped)
- PRD #7: Full-Text Search (UI)

## Data Structures to Implement

### File: `src/ui/SearchPanel.h`

```cpp
#pragma once

#include "core/SearchEngine.h"

#include <wx/panel.h>
#include <wx/srchctrl.h>
#include <wx/listctrl.h>
#include <wx/choice.h>

#include <string>
#include <vector>

namespace markamp::core { class EventBus; class ThemeEngine; class SearchEngine; }

namespace markamp::ui
{

struct SearchFilter
{
    std::string field;       // "tag", "path", "title", "type", "date", "field"
    std::string operator_;   // "is", "contains", "before", "after", "not"
    std::string value;
};

struct SavedSearch
{
    std::string name;
    std::string query_string;
    std::vector<SearchFilter> filters;
    markamp::core::SearchSortOrder sort;
};

class SearchPanel : public wxPanel
{
public:
    SearchPanel(wxWindow* parent,
                markamp::core::EventBus& event_bus,
                markamp::core::SearchEngine& search_engine,
                markamp::core::ThemeEngine& theme_engine);

    /// Focus the search input and optionally pre-fill query.
    auto focus_search(const std::string& initial_query = "") -> void;

    /// Execute a search programmatically.
    auto execute_search(const markamp::core::SearchQuery& query) -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::SearchEngine& search_engine_;
    markamp::core::ThemeEngine& theme_engine_;

    wxSearchCtrl* search_input_{nullptr};
    wxChoice* sort_choice_{nullptr};
    wxChoice* method_choice_{nullptr};
    wxPanel* filter_chips_panel_{nullptr};
    wxPanel* results_panel_{nullptr};

    markamp::core::SearchResult current_result_;
    std::vector<SearchFilter> active_filters_;
    std::vector<SavedSearch> saved_searches_;

    auto create_ui() -> void;
    auto on_search_text(wxCommandEvent& event) -> void;
    auto on_search_enter(wxCommandEvent& event) -> void;
    auto on_sort_changed(wxCommandEvent& event) -> void;
    auto on_result_clicked(const std::string& document_id, int line_number) -> void;
    auto display_results(const markamp::core::SearchResult& result) -> void;
    auto parse_query_syntax(const std::string& input) const
        -> std::pair<std::string, std::vector<SearchFilter>>;
    auto apply_filters_to_query(markamp::core::SearchQuery& query) const -> void;
    auto add_filter(const SearchFilter& filter) -> void;
    auto remove_filter(int index) -> void;
    auto render_filter_chips() -> void;
    auto apply_theme() -> void;

    // Saved searches
    auto save_current_search(const std::string& name) -> void;
    auto load_saved_search(const SavedSearch& search) -> void;

    markamp::core::Subscription theme_changed_sub_;
    markamp::core::Subscription global_search_sub_;
};

} // namespace markamp::ui
```

### Query Syntax Extension in `src/core/SearchQueryParser.h`

```cpp
#pragma once

#include "SearchEngine.h"

#include <string>
#include <vector>

namespace markamp::core
{

struct ParsedQueryToken
{
    enum class Type : uint8_t { Term, Phrase, ScopePrefix, Operator, Negation };
    Type type{Type::Term};
    std::string value;
    std::string scope;  // For scope prefix: "tag", "path", "title"
};

class SearchQueryParser
{
public:
    /// Parse advanced query syntax into structured tokens.
    /// Supports: "exact phrase", tag:value, path:prefix, -negation, OR operator
    [[nodiscard]] auto parse(const std::string& input) const -> std::vector<ParsedQueryToken>;

    /// Build a SearchQuery from parsed tokens.
    [[nodiscard]] auto build_query(const std::vector<ParsedQueryToken>& tokens) const
        -> SearchQuery;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`parse_query_syntax(input)`** -- Parse user input like `tag:project "exact phrase" -excluded path:/notes`. Extract scope prefixes, quoted phrases, negated terms, and plain keywords.

2. **`SearchQueryParser::parse(input)`** -- Tokenize: quoted strings become Phrase tokens. `tag:value` becomes ScopePrefix tokens. `-term` becomes Negation tokens. `OR` becomes Operator tokens. Everything else is a Term.

3. **`display_results(result)`** -- Render search results in the results panel. Each result shows: document title (clickable), snippet with highlighted matches, file path, match count. Paginate if >20 results.

4. **`add_filter(filter)`** -- Add a SearchFilter chip to the filter bar. Re-execute search with new filter applied.

5. **`render_filter_chips()`** -- Display active filters as removable chips (tag: "project" [x], path: "/notes" [x]).

6. **`on_search_text(event)`** -- Debounce 300ms. Parse query syntax. Execute search. Display results. This enables real-time search-as-you-type.

7. **`save_current_search(name)`** -- Save current query + filters to config for later recall.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchPanelOpenedEvent)
std::string initial_query;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchResultNavigatedEvent)
std::string document_id;
int line_number{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_search_panel.cpp`

1. **Parse plain query** -- "hello world" -> 2 Term tokens.
2. **Parse quoted phrase** -- `"exact match"` -> 1 Phrase token.
3. **Parse scope prefix** -- `tag:project` -> ScopePrefix with scope="tag", value="project".
4. **Parse negation** -- `-excluded` -> Negation token.
5. **Parse OR operator** -- `cat OR dog` -> Term, Operator, Term.
6. **Combined query** -- `tag:work "weekly report" -draft path:/notes` -> correct token mix.
7. **Filter chip add/remove** -- Add filter. Verify chip displayed. Remove. Verify gone.
8. **Sort change re-executes** -- Change sort to ModifiedDesc. Verify results re-ordered.
9. **Real-time search** -- Type query. Verify results update after debounce.
10. **Result click navigation** -- Click result. Verify SearchResultNavigatedEvent published.
11. **Saved search** -- Save search. Load it. Verify query and filters restored.

## Acceptance Criteria

- [ ] Query syntax supports quoted phrases, scope prefixes, negation, OR
- [ ] Filter chips display and are removable
- [ ] Real-time search updates results as user types (debounced)
- [ ] Results show highlighted snippets with match context
- [ ] Sort options: relevance, modified, created, alphabetical
- [ ] Click on result navigates to the document
- [ ] Saved searches persist and can be recalled
- [ ] All 11 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/SearchQueryParser.h` | Query syntax parser |
| CREATE | `src/core/SearchQueryParser.cpp` | Implementation |
| CREATE | `src/ui/SearchPanel.h` | SearchPanel UI |
| CREATE | `src/ui/SearchPanel.cpp` | Full implementation |
| MODIFY | `src/ui/MainFrame.cpp` | Register SearchPanel in sidebar |
| MODIFY | `src/core/Events.h` | Add 2 search UI events |
| MODIFY | `src/CMakeLists.txt` | Add new .cpp files |
| CREATE | `tests/unit/test_search_panel.cpp` | 11 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_search_panel target |

## Architecture Notes

- SearchQueryParser is a pure parser (no service dependencies)
- SearchPanel uses SearchEngine for execution and TagService for tag filter autocomplete
- Debounced search: 300ms delay after last keystroke before executing
- Constructor injection for panel dependencies

## Estimated Complexity

**L** -- Query parser, filter chips UI, real-time search, saved searches, 11 tests.
