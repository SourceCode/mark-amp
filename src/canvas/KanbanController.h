#pragma once

#include "canvas/CanvasTypes.h"

#include <cstddef>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;
class UndoRedoStack;

/// Controller for Kanban board operations: creating columns, adding/moving cards,
/// and maintaining layout consistency.
class KanbanController
{
public:
    KanbanController(Board& board, UndoRedoStack& undo_stack);

    /// Create a complete Kanban board with N columns at the given position.
    auto create_kanban(const Point2D& position, int column_count = 3) -> ObjectId;

    /// Add a new column with the given title.
    auto add_column(const std::string& title) -> ObjectId;

    /// Remove a column and all its cards.
    auto remove_column(ObjectId column_id) -> void;

    /// Add a new card to a column.
    auto add_card(ObjectId column_id, const std::string& title) -> ObjectId;

    /// Move a card to a different column at the specified index.
    auto move_card(ObjectId card_id, ObjectId to_column_id, int index) -> void;

    /// Re-layout all columns and their cards.
    auto relayout() -> void;

    // ── Workflow helpers (#15-18) ───────────────────────────────

    /// Remove a card from its column and mark it archived.
    auto archive_card(ObjectId card_id) -> void;

    /// Clone a card and insert the copy in the same column.
    auto duplicate_card(ObjectId card_id) -> ObjectId;

    /// Swap the positions of two columns.
    auto reorder_column(ObjectId col_a, ObjectId col_b) -> void;

    /// Statistics for a single column.
    struct ColumnStats
    {
        size_t card_count{0};
        bool wip_exceeded{false};
        int total_story_points{0};
    };

    /// Return statistics for a column.
    [[nodiscard]] auto column_stats(ObjectId column_id) const -> ColumnStats;

    // ── Filtering & Aggregation (#34-35) ───────────────────────

    /// Filter cards across all columns by predicate. Returns matching card IDs.
    [[nodiscard]] auto
    filter_cards(const std::function<bool(const class KanbanCard&)>& predicate) const
        -> std::vector<ObjectId>;

    /// Sum story points across all columns.
    [[nodiscard]] auto total_story_points() const -> int;

private:
    Board& board_;
    [[maybe_unused]] UndoRedoStack& undo_stack_;
    std::vector<ObjectId> column_ids_;
    Point2D origin_{0.0, 0.0};

    /// Re-position cards within a column.
    auto position_cards_in_column(ObjectId column_id) -> void;
};

} // namespace markamp::canvas
