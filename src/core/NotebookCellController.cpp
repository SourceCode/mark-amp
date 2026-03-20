/// @file NotebookCellController.cpp
/// @brief P08-T03: Cell editing and execution controls.

#include "NotebookCellController.h"

#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

NotebookCellController::NotebookCellController(EventBus& bus)
    : event_bus_(bus)
{
}

void NotebookCellController::add_cell(const std::string& notebook_id,
                                       const std::string& cell_type, int position)
{
    MARKAMP_LOG_INFO("Cell added: {} type={} pos={}", notebook_id, cell_type, position);
}

void NotebookCellController::remove_cell(const std::string& notebook_id,
                                          const std::string& cell_id)
{
    MARKAMP_LOG_INFO("Cell removed: {} cell={}", notebook_id, cell_id);
}

void NotebookCellController::move_cell(const std::string& notebook_id,
                                        const std::string& cell_id, int new_position)
{
    MARKAMP_LOG_INFO("Cell moved: {} cell={} pos={}", notebook_id, cell_id, new_position);
}

void NotebookCellController::run_cell(const std::string& notebook_id,
                                       const std::string& cell_id)
{
    executing_ = true;
    event_bus_.publish(events::NotificationEvent{
        "Running cell...",
        events::NotificationLevel::Info, 1500});
    MARKAMP_LOG_INFO("Cell execution: {} cell={}", notebook_id, cell_id);
}

void NotebookCellController::run_all(const std::string& notebook_id)
{
    executing_ = true;
    event_bus_.publish(events::NotificationEvent{
        "Running all cells...",
        events::NotificationLevel::Info, 2000});
    MARKAMP_LOG_INFO("Run all cells: {}", notebook_id);
}

void NotebookCellController::cancel_execution(const std::string& notebook_id)
{
    executing_ = false;
    event_bus_.publish(events::NotificationEvent{
        "Execution cancelled",
        events::NotificationLevel::Warning, 2000});
    MARKAMP_LOG_INFO("Execution cancelled: {}", notebook_id);
}

auto NotebookCellController::cell_state(const std::string& /*cell_id*/) const
    -> CellExecutionState
{
    // In full implementation, this would look up per-cell state
    return CellExecutionState::kIdle;
}

} // namespace markamp::core
