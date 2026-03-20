/// @file NotebookCellController.h
/// @brief P08-T03: Cell editing and execution controls.
///
/// Connects cell management and execution pipeline to visible notebook
/// controls with state tracking per cell.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;

/// Cell execution state.
enum class CellExecutionState
{
    kIdle,
    kRunning,
    kSuccess,
    kFailure,
    kCancelled,
};

/// A notebook cell descriptor.
struct CellDescriptor
{
    std::string cell_id;
    std::string cell_type; ///< "code", "markdown"
    CellExecutionState execution_state{CellExecutionState::kIdle};
    int execution_order{0};
};

/// Controls cell editing and execution actions.
class NotebookCellController
{
public:
    explicit NotebookCellController(EventBus& bus);

    /// Add a cell at a position.
    void add_cell(const std::string& notebook_id, const std::string& cell_type, int position);

    /// Remove a cell.
    void remove_cell(const std::string& notebook_id, const std::string& cell_id);

    /// Move a cell to a new position.
    void move_cell(const std::string& notebook_id, const std::string& cell_id, int new_position);

    /// Run a single cell.
    void run_cell(const std::string& notebook_id, const std::string& cell_id);

    /// Run all cells.
    void run_all(const std::string& notebook_id);

    /// Cancel execution.
    void cancel_execution(const std::string& notebook_id);

    /// Get cell execution state.
    [[nodiscard]] auto cell_state(const std::string& cell_id) const -> CellExecutionState;

    /// Check if any cell is running.
    [[nodiscard]] auto is_executing() const -> bool { return executing_; }

private:
    EventBus& event_bus_;
    bool executing_{false};
};

} // namespace markamp::core
