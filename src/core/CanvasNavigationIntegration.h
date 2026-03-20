/// @file CanvasNavigationIntegration.h
/// @brief P07-T04: Board open/export/navigation aligned with document workflows.
///
/// Ensures boards participate in the same open/save/export/navigation
/// patterns as documents.
#pragma once

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;

/// Integrates canvas board navigation with workbench workflows.
class CanvasNavigationIntegration
{
public:
    explicit CanvasNavigationIntegration(EventBus& bus);

    /// Open a board by ID, switching to canvas mode.
    void open_board(const std::string& board_id);

    /// Create a new board.
    void new_board(const std::string& title);

    /// Export the active board.
    void export_board(const std::string& board_id, const std::string& format);

    /// Record a board in navigation history.
    void record_board_visit(const std::string& board_id);

    /// Get recent boards.
    [[nodiscard]] auto recent_boards() const -> const std::vector<std::string>&
    {
        return recent_boards_;
    }

    /// Get recent board count.
    [[nodiscard]] auto recent_count() const -> int
    {
        return static_cast<int>(recent_boards_.size());
    }

private:
    EventBus& event_bus_;
    std::vector<std::string> recent_boards_;
    static constexpr int kMaxRecentBoards = 20;
};

} // namespace markamp::core
