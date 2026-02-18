/// @file NotebookExecutionPipeline.cpp
/// @brief V8 Phase 15 – Notebook execution orchestration implementation.

#include "core/NotebookExecutionPipeline.h"

#include "core/Events.h"
#include "core/KernelManager.h"
#include "core/NotebookCellManager.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookExecutionPipeline::NotebookExecutionPipeline(EventBus& event_bus,
                                                     NotebookCellManager& cell_manager,
                                                     KernelManager& kernel_manager)
    : event_bus_(event_bus)
    , cell_manager_(cell_manager)
    , kernel_manager_(kernel_manager)
{
}

// ============================================================================
// Execute single cell
// ============================================================================

auto NotebookExecutionPipeline::execute_cell(const std::string& cell_id) -> void
{
    auto cell = cell_manager_.get_cell(cell_id);
    if (!cell.has_value())
    {
        return; // Cell not found.
    }

    // Only execute code cells.
    if (cell->metadata.type != CellType::kCode)
    {
        return;
    }

    ExecutionQueueEntry entry;
    entry.cell_id = cell_id;
    entry.position = static_cast<int>(queue_.size());
    entry.pending = true;
    queue_.push_back(std::move(entry));

    if (!executing_)
    {
        advance_queue();
    }
}

// ============================================================================
// Execute range
// ============================================================================

auto NotebookExecutionPipeline::execute_range(ExecutionScope scope,
                                              const std::string& cursor_cell_id) -> void
{
    auto cell_ids = resolve_scope(scope, cursor_cell_id);

    for (int idx = 0; idx < static_cast<int>(cell_ids.size()); ++idx)
    {
        ExecutionQueueEntry entry;
        entry.cell_id = cell_ids[static_cast<size_t>(idx)];
        entry.position = idx;
        entry.pending = true;
        queue_.push_back(std::move(entry));
    }

    if (!executing_ && !queue_.empty())
    {
        advance_queue();
    }
}

// ============================================================================
// Interrupt
// ============================================================================

auto NotebookExecutionPipeline::interrupt() -> void
{
    executing_ = false;
    current_cell_id_.reset();
    queue_.clear();
}

// ============================================================================
// Restart and run all
// ============================================================================

auto NotebookExecutionPipeline::restart_and_run_all(const std::string& /*kernel_id*/) -> void
{
    // Clear current state.
    interrupt();

    // Queue all code cells.
    execute_range(ExecutionScope::kAllCells);
}

// ============================================================================
// Queue access
// ============================================================================

auto NotebookExecutionPipeline::execution_queue() const -> const std::vector<ExecutionQueueEntry>&
{
    return queue_;
}

auto NotebookExecutionPipeline::is_executing() const -> bool
{
    return executing_;
}

auto NotebookExecutionPipeline::current_cell() const -> std::optional<std::string>
{
    return current_cell_id_;
}

auto NotebookExecutionPipeline::queue_size() const -> int
{
    return static_cast<int>(queue_.size());
}

auto NotebookExecutionPipeline::completed_count() const -> int
{
    return static_cast<int>(std::count_if(queue_.begin(),
                                          queue_.end(),
                                          [](const ExecutionQueueEntry& entry)
                                          { return !entry.pending; }));
}

// ============================================================================
// Callbacks
// ============================================================================

auto NotebookExecutionPipeline::on_cell_complete(CellCompleteCallback callback) -> void
{
    callbacks_.push_back(std::move(callback));
}

// ============================================================================
// Mark completed
// ============================================================================

auto NotebookExecutionPipeline::mark_completed(const std::string& cell_id, bool success) -> void
{
    // Find and mark the entry in the queue.
    for (auto& entry : queue_)
    {
        if (entry.cell_id == cell_id && entry.pending)
        {
            entry.pending = false;
            break;
        }
    }

    // Publish event.
    events::NotebookCellExecutedEvent evt;
    evt.notebook_id = "current";
    evt.cell_id = cell_id;
    evt.execution_count = cell_manager_.next_execution_count();
    evt.elapsed_ms = 0.0;
    event_bus_.publish(evt);

    // Notify callbacks.
    for (const auto& callback : callbacks_)
    {
        callback(cell_id, success);
    }

    // Advance to next cell.
    if (current_cell_id_.has_value() && current_cell_id_.value() == cell_id)
    {
        current_cell_id_.reset();
        advance_queue();
    }
}

// ============================================================================
// Clear queue
// ============================================================================

auto NotebookExecutionPipeline::clear_queue() -> void
{
    queue_.clear();
    current_cell_id_.reset();
    executing_ = false;
}

// ============================================================================
// Private helpers
// ============================================================================

auto NotebookExecutionPipeline::advance_queue() -> void
{
    // Find the next pending entry.
    for (auto& entry : queue_)
    {
        if (entry.pending)
        {
            current_cell_id_ = entry.cell_id;
            executing_ = true;
            return;
        }
    }

    // No more pending cells.
    executing_ = false;
    current_cell_id_.reset();
}

auto NotebookExecutionPipeline::resolve_scope(ExecutionScope scope,
                                              const std::string& cursor_cell_id) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    const auto& cells = cell_manager_.cells();

    // Find cursor position.
    int cursor_pos = -1;
    for (int idx = 0; idx < static_cast<int>(cells.size()); ++idx)
    {
        if (cells[static_cast<size_t>(idx)].metadata.cell_id == cursor_cell_id)
        {
            cursor_pos = idx;
            break;
        }
    }

    for (int idx = 0; idx < static_cast<int>(cells.size()); ++idx)
    {
        const auto& cell = cells[static_cast<size_t>(idx)];
        if (cell.metadata.type != CellType::kCode)
        {
            continue; // Skip non-code cells.
        }

        bool include = false;
        switch (scope)
        {
            case ExecutionScope::kSingleCell:
                include = (cell.metadata.cell_id == cursor_cell_id);
                break;
            case ExecutionScope::kAllCells:
                include = true;
                break;
            case ExecutionScope::kAboveCursor:
                include = (cursor_pos >= 0 && idx <= cursor_pos);
                break;
            case ExecutionScope::kBelowCursor:
                include = (cursor_pos >= 0 && idx >= cursor_pos);
                break;
        }

        if (include)
        {
            result.push_back(cell.metadata.cell_id);
        }
    }

    return result;
}

} // namespace markamp::core
