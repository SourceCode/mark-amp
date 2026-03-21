/// @file NotebookCommandRouter.h
/// @brief V20 P03-T04: Notebook command routing and context propagation.
///
/// Ensures notebook-specific commands (save, run, add cell, etc.) are routed
/// to the correct notebook artifact and cell. Provides context keys for
/// conditional UI enablement when a notebook is active.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <optional>
#include <string>

namespace markamp::core
{

/// Active notebook context for command routing.
struct NotebookContext
{
    ArtifactId active_notebook_id;
    std::string active_cell_id;
    bool is_notebook_focused{false};
    bool is_cell_editing{false};
    bool has_running_cell{false};

    [[nodiscard]] auto has_active_notebook() const noexcept -> bool
    {
        return !active_notebook_id.empty();
    }

    [[nodiscard]] auto has_active_cell() const noexcept -> bool
    {
        return !active_cell_id.empty();
    }
};

/// Routes notebook-specific commands to the correct target.
class NotebookCommandRouter
{
public:
    explicit NotebookCommandRouter(EventBus& bus);

    /// Set the active notebook context.
    void set_context(const NotebookContext& context);

    /// Clear the active notebook context (e.g., when switching to editor).
    void clear_context();

    /// Get current notebook context.
    [[nodiscard]] auto context() const -> const NotebookContext& { return context_; }

    /// Whether a notebook is currently active and focused.
    [[nodiscard]] auto is_notebook_active() const -> bool
    {
        return context_.is_notebook_focused && context_.has_active_notebook();
    }

    /// Get the active notebook's artifact ID.
    [[nodiscard]] auto active_notebook_id() const -> const ArtifactId&
    {
        return context_.active_notebook_id;
    }

    /// Get the active cell ID.
    [[nodiscard]] auto active_cell_id() const -> const std::string&
    {
        return context_.active_cell_id;
    }

    /// Set the active cell.
    void set_active_cell(const std::string& cell_id);

    /// Whether save should target the notebook (vs. the last text file).
    [[nodiscard]] auto should_save_notebook() const -> bool
    {
        return context_.is_notebook_focused;
    }

    /// Total context updates.
    [[nodiscard]] auto update_count() const noexcept -> int { return update_count_; }

private:
    EventBus& event_bus_;
    NotebookContext context_;
    int update_count_{0};
};

} // namespace markamp::core
