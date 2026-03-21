/// @file NotebookCellChrome.h
/// @brief V20 P08-T02: Notebook cell chrome and presentation system.
///
/// Standardizes cell containers, active cell highlighting, output areas,
/// execution states, and add-cell affordances for visual parity with editor.
#pragma once

#include "EventBus.h"

#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Cell execution state for visual treatment.
enum class CellExecutionState
{
    kIdle,       ///< Not running
    kQueued,     ///< Waiting to execute
    kRunning,    ///< Currently executing
    kSuccess,    ///< Completed successfully
    kError,      ///< Completed with error
    kCancelled   ///< User cancelled execution
};

/// Cell type for chrome differentiation.
enum class CellChromeType
{
    kCode,       ///< Code cell with prompt gutter
    kMarkdown,   ///< Markdown cell with rendered output
    kRaw         ///< Raw cell with minimal chrome
};

/// Chrome state for a single notebook cell.
struct CellChromeState
{
    std::string cell_id;
    CellChromeType cell_type{CellChromeType::kCode};
    CellExecutionState execution_state{CellExecutionState::kIdle};
    bool is_active{false};
    bool is_selected{false};
    bool is_collapsed{false};
    bool has_output{false};
    bool has_error{false};
    int execution_order{0};       ///< Execution counter (0 = not yet run)

    [[nodiscard]] auto is_running() const noexcept -> bool
    {
        return execution_state == CellExecutionState::kRunning;
    }

    [[nodiscard]] auto needs_prompt() const noexcept -> bool
    {
        return cell_type == CellChromeType::kCode;
    }
};

/// Manages notebook cell chrome state and presentation rules.
class NotebookCellChrome
{
public:
    explicit NotebookCellChrome(EventBus& bus);

    /// Set cell chrome state.
    void set_cell_state(const std::string& cell_id, const CellChromeState& state);

    /// Get cell chrome state.
    [[nodiscard]] auto cell_state(const std::string& cell_id) const -> const CellChromeState*;

    /// Set active cell (clears previous).
    void set_active_cell(const std::string& cell_id);

    /// Get active cell ID.
    [[nodiscard]] auto active_cell_id() const noexcept -> const std::string& { return active_cell_id_; }

    /// Update execution state for a cell.
    void set_execution_state(const std::string& cell_id, CellExecutionState state);

    /// Collapse/expand a cell.
    void set_collapsed(const std::string& cell_id, bool collapsed);

    /// Remove cell chrome state.
    void remove_cell(const std::string& cell_id);

    /// Total tracked cells.
    [[nodiscard]] auto cell_count() const noexcept -> int
    {
        return static_cast<int>(cells_.size());
    }

    /// Total state updates.
    [[nodiscard]] auto update_count() const noexcept -> int { return update_count_; }

private:
    EventBus& event_bus_;
    std::unordered_map<std::string, CellChromeState> cells_;
    std::string active_cell_id_;
    int update_count_{0};
};

} // namespace markamp::core
