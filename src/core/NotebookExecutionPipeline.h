/// @file NotebookExecutionPipeline.h
/// @brief V8 Phase 15 – Notebook execution orchestration layer.
/// Wires NotebookCellManager → KernelManager → CellOutputRenderer.

#pragma once

#include "core/EventBus.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class NotebookCellManager;
class KernelManager;

// ============================================================================
// Execution scope
// ============================================================================

/// Determines which cells to execute.
enum class ExecutionScope : uint8_t
{
    kSingleCell,  ///< Execute one cell
    kAllCells,    ///< Execute all code cells in order
    kAboveCursor, ///< Execute all code cells above (inclusive) cursor
    kBelowCursor  ///< Execute all code cells below (inclusive) cursor
};

// ============================================================================
// Execution queue entry
// ============================================================================

/// A single entry in the execution queue.
struct ExecutionQueueEntry
{
    std::string cell_id;
    int position{0};
    bool pending{true}; ///< true = waiting, false = completed
};

/// Callback for individual cell completion notifications.
using CellCompleteCallback = std::function<void(const std::string& cell_id, bool success)>;

// ============================================================================
// NotebookExecutionPipeline
// ============================================================================

class NotebookExecutionPipeline
{
public:
    NotebookExecutionPipeline(EventBus& event_bus,
                              NotebookCellManager& cell_manager,
                              KernelManager& kernel_manager);

    /// Execute a single cell by ID.
    auto execute_cell(const std::string& cell_id) -> void;

    /// Execute a range of cells based on scope and cursor position.
    auto execute_range(ExecutionScope scope, const std::string& cursor_cell_id = "") -> void;

    /// Interrupt the current execution and clear the queue.
    auto interrupt() -> void;

    /// Restart the kernel and then run all cells.
    auto restart_and_run_all(const std::string& kernel_id) -> void;

    /// Get the current execution queue.
    [[nodiscard]] auto execution_queue() const -> const std::vector<ExecutionQueueEntry>&;

    /// Check if any cell is currently executing.
    [[nodiscard]] auto is_executing() const -> bool;

    /// Get the ID of the currently executing cell (if any).
    [[nodiscard]] auto current_cell() const -> std::optional<std::string>;

    /// Register a callback for cell completion events.
    auto on_cell_complete(CellCompleteCallback callback) -> void;

    /// Mark a cell execution as completed (called when kernel responds).
    auto mark_completed(const std::string& cell_id, bool success) -> void;

    /// Get the total number of cells queued for execution.
    [[nodiscard]] auto queue_size() const -> int;

    /// Get the number of completed cells in the current batch.
    [[nodiscard]] auto completed_count() const -> int;

    /// Clear the execution queue without interrupting.
    auto clear_queue() -> void;

private:
    EventBus& event_bus_;
    NotebookCellManager& cell_manager_;
    [[maybe_unused]] KernelManager& kernel_manager_;

    std::vector<ExecutionQueueEntry> queue_;
    std::optional<std::string> current_cell_id_;
    bool executing_{false};
    std::vector<CellCompleteCallback> callbacks_;

    /// Advance to the next cell in the queue.
    auto advance_queue() -> void;

    /// Build a list of cell IDs to execute based on scope.
    [[nodiscard]] auto resolve_scope(ExecutionScope scope, const std::string& cursor_cell_id) const
        -> std::vector<std::string>;
};

} // namespace markamp::core
