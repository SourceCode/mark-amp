/// @file SearchNavigationContract.cpp
/// @brief P05-T02: Search result navigation contract.

#include "SearchNavigationContract.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <filesystem>

namespace markamp::core
{

SearchNavigationContract::SearchNavigationContract(EventBus& bus)
    : event_bus_(bus)
{
}

void SearchNavigationContract::search(const std::string& query, bool regex,
                                       bool case_sensitive)
{
    current_query_ = query;
    state_ = SearchResultState::kSearching;
    results_.clear();

    MARKAMP_LOG_INFO("Search initiated: '{}' (regex={}, case_sensitive={})",
                     query, regex, case_sensitive);

    // The actual search is delegated to SearchService/SearchEngine.
    // This contract manages the navigation pipeline after results arrive.
    // Results will be populated by the search service via add_results().
}

void SearchNavigationContract::activate_result(const SearchMatch& match)
{
    // Validate file still exists
    if (!std::filesystem::exists(match.file_path))
    {
        event_bus_.publish(events::NotificationEvent{
            "File no longer exists: " + match.file_path,
            events::NotificationLevel::Warning, 3000});
        MARKAMP_LOG_WARN("Search result activation failed — file missing: {}",
                         match.file_path);
        return;
    }

    // Open file and reveal match location
    events::ActiveFileChangedEvent open_evt;
    open_evt.file_id = match.file_path;
    event_bus_.publish(open_evt);

    // Publish cursor position to jump to match
    events::CursorPositionChangedEvent cursor_evt;
    cursor_evt.line = match.line_number;
    cursor_evt.column = match.column;
    event_bus_.publish(cursor_evt);

    MARKAMP_LOG_DEBUG("Search result activated: {}:{}", match.file_path, match.line_number);
}

void SearchNavigationContract::clear()
{
    results_.clear();
    current_query_.clear();
    state_ = SearchResultState::kIdle;
}

} // namespace markamp::core
