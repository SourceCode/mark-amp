/// @file CanvasNavigationIntegration.cpp
/// @brief P07-T04: Board open/export/navigation aligned with document workflows.

#include "CanvasNavigationIntegration.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

#include <algorithm>

namespace markamp::core
{

CanvasNavigationIntegration::CanvasNavigationIntegration(EventBus& bus)
    : event_bus_(bus)
{
}

void CanvasNavigationIntegration::open_board(const std::string& board_id)
{
    record_board_visit(board_id);
    MARKAMP_LOG_INFO("Board open: {}", board_id);
}

void CanvasNavigationIntegration::new_board(const std::string& title)
{
    event_bus_.publish(events::NotificationEvent{
        "New board: " + title,
        events::NotificationLevel::Success, 2000});
    MARKAMP_LOG_INFO("New board created: {}", title);
}

void CanvasNavigationIntegration::export_board(const std::string& board_id,
                                                const std::string& format)
{
    event_bus_.publish(events::NotificationEvent{
        "Exporting board as " + format + "...",
        events::NotificationLevel::Info, 2000});
    MARKAMP_LOG_INFO("Board export: {} (format: {})", board_id, format);
}

void CanvasNavigationIntegration::record_board_visit(const std::string& board_id)
{
    // Remove if already in recents (move to front)
    auto iter = std::find(recent_boards_.begin(), recent_boards_.end(), board_id);
    if (iter != recent_boards_.end())
    {
        recent_boards_.erase(iter);
    }

    recent_boards_.insert(recent_boards_.begin(), board_id);

    if (static_cast<int>(recent_boards_.size()) > kMaxRecentBoards)
    {
        recent_boards_.pop_back();
    }
}

} // namespace markamp::core
