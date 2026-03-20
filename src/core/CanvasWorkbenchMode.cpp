/// @file CanvasWorkbenchMode.cpp
/// @brief P07-T01: Canvas as a first-class workbench shell surface.

#include "CanvasWorkbenchMode.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

CanvasWorkbenchMode::CanvasWorkbenchMode(EventBus& bus, Config& cfg)
    : event_bus_(bus)
    , config_(cfg)
{
}

void CanvasWorkbenchMode::enter()
{
    if (active_)
    {
        return;
    }

    active_ = true;
    surface_ = WorkbenchSurface::kCanvas;

    MARKAMP_LOG_INFO("Canvas workbench mode entered");
}

void CanvasWorkbenchMode::exit()
{
    if (!active_)
    {
        return;
    }

    persist_state();
    active_ = false;
    surface_ = WorkbenchSurface::kEditor;

    MARKAMP_LOG_INFO("Canvas workbench mode exited");
}

void CanvasWorkbenchMode::set_active_board(const std::string& board_id,
                                            const std::string& board_title)
{
    board_id_ = board_id;
    board_title_ = board_title;
    MARKAMP_LOG_DEBUG("Active board: {} ({})", board_title, board_id);
}

void CanvasWorkbenchMode::persist_state()
{
    if (!board_id_.empty())
    {
        config_.set("canvas.lastBoardId", board_id_);
        config_.set("canvas.lastBoardTitle", board_title_);
        MARKAMP_LOG_DEBUG("Canvas state persisted: {}", board_id_);
    }
}

void CanvasWorkbenchMode::restore_state()
{
    board_id_ = config_.get_string("canvas.lastBoardId");
    board_title_ = config_.get_string("canvas.lastBoardTitle");
    if (!board_id_.empty())
    {
        MARKAMP_LOG_INFO("Canvas state restored: {} ({})", board_title_, board_id_);
    }
}

} // namespace markamp::core
