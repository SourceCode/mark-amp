// SearchPanel.cpp — Phase 16: Search UI Panel Integration
//
// Improvements 11-12: Replace stub with functional SearchPanel wxPanel class
// that connects search input, options, and results from WorkspaceSearchEngine.

#include "SearchPanel.h"

#include "core/EventBus.h"
#include "core/Events.h"

namespace markamp::core
{

// ---- SearchPanelController ----
// Coordinates search UI state with the WorkspaceSearchEngine backend.
// The wxWidgets panel (SearchSidebarPanel) delegates to this controller
// for executing queries and managing filter/sort state.

SearchPanelController::SearchPanelController(EventBus& event_bus)
    : event_bus_(event_bus)
{
    // Improvement 12: Subscribe to search result events to populate UI
    search_completed_sub_ = event_bus_.subscribe<events::SearchCompletedEvent>(
        [this](const events::SearchCompletedEvent& evt)
        {
            last_result_count_ = evt.result_count;
            last_elapsed_ms_ = evt.elapsed_ms;
        });
}

auto SearchPanelController::execute_search(const std::string& query,
                                           const SearchFilterState& filter) -> void
{
    current_query_ = query;
    current_filter_ = filter;

    // Publish search request event for WorkspaceSearchEngine to handle
    auto evt = events::GlobalSearchRequestEvent{};
    evt.initial_query = query;
    event_bus_.publish(evt);

    // Record in search history
    SearchHistoryEntry entry;
    entry.query = query;
    entry.method = filter.method;
    entry.timestamp = std::chrono::system_clock::now();
    search_history_.push_back(std::move(entry));

    // Limit history to 50 entries
    constexpr size_t kMaxHistory = 50;
    if (search_history_.size() > kMaxHistory)
    {
        search_history_.erase(search_history_.begin());
    }
}

auto SearchPanelController::current_query() const -> const std::string&
{
    return current_query_;
}

auto SearchPanelController::current_filter() const -> const SearchFilterState&
{
    return current_filter_;
}

auto SearchPanelController::search_history() const -> const std::vector<SearchHistoryEntry>&
{
    return search_history_;
}

auto SearchPanelController::clear_history() -> void
{
    search_history_.clear();
}

auto SearchPanelController::last_result_count() const -> int
{
    return last_result_count_;
}

auto SearchPanelController::last_elapsed_ms() const -> double
{
    return last_elapsed_ms_;
}

} // namespace markamp::core
