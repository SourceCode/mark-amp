/// @file NotebookCellChrome.cpp
/// @brief V20 P08-T02: Notebook cell chrome implementation.

#include "NotebookCellChrome.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

NotebookCellChrome::NotebookCellChrome(EventBus& bus)
    : event_bus_(bus)
{
}

void NotebookCellChrome::set_cell_state(const std::string& cell_id, const CellChromeState& state)
{
    ++update_count_;
    cells_[cell_id] = state;
    MARKAMP_LOG_DEBUG("Cell chrome set: {} (type={}, exec={})", cell_id,
                     static_cast<int>(state.cell_type), static_cast<int>(state.execution_state));
}

auto NotebookCellChrome::cell_state(const std::string& cell_id) const -> const CellChromeState*
{
    auto it = cells_.find(cell_id);
    return it != cells_.end() ? &it->second : nullptr;
}

void NotebookCellChrome::set_active_cell(const std::string& cell_id)
{
    // Deactivate previous
    if (!active_cell_id_.empty())
    {
        auto prev = cells_.find(active_cell_id_);
        if (prev != cells_.end())
        {
            prev->second.is_active = false;
        }
    }

    active_cell_id_ = cell_id;

    auto current = cells_.find(cell_id);
    if (current != cells_.end())
    {
        current->second.is_active = true;
    }

    ++update_count_;

    events::CellChromeChangedEvent evt;
    evt.cell_id = cell_id;
    evt.is_active = true;
    event_bus_.publish(evt);

    MARKAMP_LOG_DEBUG("Active cell changed: {}", cell_id);
}

void NotebookCellChrome::set_execution_state(const std::string& cell_id, CellExecutionState state)
{
    auto it = cells_.find(cell_id);
    if (it != cells_.end())
    {
        it->second.execution_state = state;
        ++update_count_;

        events::CellExecutionStateChangedEvent evt;
        evt.cell_id = cell_id;
        evt.state = static_cast<int>(state);
        event_bus_.publish(evt);

        MARKAMP_LOG_DEBUG("Cell execution state: {} -> {}", cell_id, static_cast<int>(state));
    }
}

void NotebookCellChrome::set_collapsed(const std::string& cell_id, bool collapsed)
{
    auto it = cells_.find(cell_id);
    if (it != cells_.end())
    {
        it->second.is_collapsed = collapsed;
        ++update_count_;
    }
}

void NotebookCellChrome::remove_cell(const std::string& cell_id)
{
    cells_.erase(cell_id);
    if (active_cell_id_ == cell_id)
    {
        active_cell_id_.clear();
    }
    ++update_count_;
}

} // namespace markamp::core
