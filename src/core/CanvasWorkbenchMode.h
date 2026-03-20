/// @file CanvasWorkbenchMode.h
/// @brief P07-T01: Canvas as a first-class workbench shell surface.
///
/// Manages mode entry/exit hooks, shell metadata, title updates,
/// command availability, and session persistence for canvas mode.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;
class Config;

/// Workbench surface mode.
enum class WorkbenchSurface
{
    kEditor,
    kCanvas,
    kGraph,
};

/// Manages canvas workbench mode lifecycle.
class CanvasWorkbenchMode
{
public:
    CanvasWorkbenchMode(EventBus& bus, Config& cfg);

    /// Enter canvas mode.
    void enter();

    /// Exit canvas mode (return to editor).
    void exit();

    /// Check if canvas mode is active.
    [[nodiscard]] auto is_active() const -> bool { return active_; }

    /// Get the current surface.
    [[nodiscard]] auto current_surface() const -> WorkbenchSurface { return surface_; }

    /// Set the active board for title/metadata.
    void set_active_board(const std::string& board_id, const std::string& board_title);

    /// Get the active board ID.
    [[nodiscard]] auto active_board_id() const -> const std::string& { return board_id_; }

    /// Get the active board title.
    [[nodiscard]] auto active_board_title() const -> const std::string& { return board_title_; }

    /// Persist current canvas state for session restore.
    void persist_state();

    /// Restore canvas state from persisted session.
    void restore_state();

private:
    EventBus& event_bus_;
    Config& config_;
    bool active_{false};
    WorkbenchSurface surface_{WorkbenchSurface::kEditor};
    std::string board_id_;
    std::string board_title_;
};

} // namespace markamp::core
