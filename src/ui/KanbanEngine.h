/// @file KanbanEngine.h
/// @brief V4 Phase 25 – Kanban Board View (pure-logic engine, no wxWidgets).

#pragma once

#include "core/TaskService.h"

#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{
class EventBus;
class VaultService;
} // namespace markamp::core

namespace markamp::ui
{

// ============================================================================
// KanbanColumn: one column in the board
// ============================================================================

struct KanbanColumn
{
    std::string title;
    std::string heading_text;
    int line_number{0};
    std::vector<markamp::core::Task> cards;
    std::string color_hex;
    int max_cards{0};       // 0 = unlimited (WIP limit)
    int completed_count{0}; // (#37) number of completed cards
};

// ============================================================================
// KanbanBoard: the full board data model
// ============================================================================

struct KanbanBoard
{
    std::string document_id;
    std::string title;
    std::vector<KanbanColumn> columns;

    [[nodiscard]] auto total_cards() const -> int;
    [[nodiscard]] auto find_card(const std::string& task_id) const
        -> std::optional<std::pair<int, int>>; // (column_idx, card_idx)
};

// ============================================================================
// KanbanEngine: parsing and manipulation logic
// ============================================================================

class KanbanEngine
{
public:
    KanbanEngine(markamp::core::EventBus& event_bus, markamp::core::VaultService& vault_service);

    /// Parse kanban-structured Markdown into a board.
    [[nodiscard]] static auto parse_kanban_markdown(const std::string& markdown) -> KanbanBoard;

    /// Move a card from one column to another, returning updated markdown.
    [[nodiscard]] static auto
    move_card(const std::string& markdown, int from_col, int card_idx, int to_col, int to_row)
        -> std::string;

    /// Reorder a card within the same column, returning updated markdown.
    [[nodiscard]] static auto
    reorder_card(const std::string& markdown, int col_idx, int from_row, int to_row) -> std::string;

    /// Toggle a card's checkbox status in the markdown.
    [[nodiscard]] static auto toggle_card(const std::string& markdown, int col_idx, int card_idx)
        -> std::string;

    // ── Improvements (#37-38) ─────────────────────────────────

    /// Add a new card to a column at a given row, returning updated markdown.
    [[nodiscard]] static auto
    add_card(const std::string& markdown, int col_idx, int at_row, const std::string& card_title)
        -> std::string;

    /// Per-column statistics.
    struct ColumnStatistics
    {
        int total_cards{0};
        int completed_cards{0};
        bool wip_exceeded{false};
    };

    /// Get statistics for all columns in the board.
    [[nodiscard]] static auto column_statistics(const KanbanBoard& board)
        -> std::vector<ColumnStatistics>;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::VaultService& vault_service_;
};

} // namespace markamp::ui
