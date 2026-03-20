/// @file QuickOpenIntegration.cpp
/// @brief P05-T03: Unifies quick open, command palette, and navigation indexes.

#include "QuickOpenIntegration.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

QuickOpenIntegration::QuickOpenIntegration(EventBus& bus)
    : event_bus_(bus)
{
}

void QuickOpenIntegration::activate(const QuickOpenItem& item)
{
    switch (item.mode)
    {
    case QuickOpenMode::kFile:
    case QuickOpenMode::kRecent:
    {
        // Open file — same behavior as explorer navigation
        events::ActiveFileChangedEvent open_evt;
        open_evt.file_id = item.file_path;
        event_bus_.publish(open_evt);

        record_mru(item.file_path);
        MARKAMP_LOG_DEBUG("Quick open file: {}", item.file_path);
        break;
    }
    case QuickOpenMode::kSymbol:
    {
        // Open file at symbol location
        events::ActiveFileChangedEvent open_evt;
        open_evt.file_id = item.file_path;
        event_bus_.publish(open_evt);

        if (item.line_number > 0)
        {
            events::CursorPositionChangedEvent cursor_evt;
            cursor_evt.line = item.line_number;
            cursor_evt.column = 1;
            event_bus_.publish(cursor_evt);
        }

        record_mru(item.file_path);
        MARKAMP_LOG_DEBUG("Quick open symbol: {} at L{}", item.label, item.line_number);
        break;
    }
    case QuickOpenMode::kCommand:
    {
        MARKAMP_LOG_DEBUG("Quick open command: {}", item.label);
        // Command execution is handled by CommandRegistry
        break;
    }
    }
}

void QuickOpenIntegration::record_mru(const std::string& file_path)
{
    if (file_path.empty())
    {
        return;
    }

    // Remove existing entry if present (move to front)
    auto iter = std::find(mru_list_.begin(), mru_list_.end(), file_path);
    if (iter != mru_list_.end())
    {
        mru_list_.erase(iter);
    }

    mru_list_.insert(mru_list_.begin(), file_path);

    // Cap size
    if (static_cast<int>(mru_list_.size()) > kMaxMruSize)
    {
        mru_list_.pop_back();
    }
}

void QuickOpenIntegration::clear_mru()
{
    mru_list_.clear();
}

} // namespace markamp::core
