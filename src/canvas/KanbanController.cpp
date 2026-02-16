#include "KanbanController.h"

#include "canvas/Board.h"
#include "canvas/KanbanObjects.h"
#include "canvas/UndoRedoStack.h"

#include <array>

namespace markamp::canvas
{

KanbanController::KanbanController(Board& board, UndoRedoStack& undo_stack)
    : board_(board)
    , undo_stack_(undo_stack)
{
}

auto KanbanController::create_kanban(const Point2D& position, int column_count) -> ObjectId
{
    origin_ = position;
    column_ids_.clear();

    static constexpr std::array<const char*, 3> kDefaultTitles = {"To Do", "In Progress", "Done"};
    const int count = std::max(1, column_count);

    ObjectId first_col_id = kInvalidObjectId;
    for (int idx = 0; idx < count; ++idx)
    {
        const char* title = (idx < 3) ? kDefaultTitles[static_cast<size_t>(idx)] : "Column";
        const ObjectId col_id = add_column(title);
        if (idx == 0)
        {
            first_col_id = col_id;
        }
    }

    relayout();
    return first_col_id;
}

auto KanbanController::add_column(const std::string& title) -> ObjectId
{
    auto column = std::make_unique<KanbanColumn>();
    column->set_title(title);
    const ObjectId col_id = board_.add_object(std::move(column));
    column_ids_.push_back(col_id);
    return col_id;
}

auto KanbanController::remove_column(ObjectId column_id) -> void
{
    auto* column = dynamic_cast<KanbanColumn*>(board_.get_object_mut(column_id));
    if (column == nullptr)
    {
        return;
    }

    // Remove all cards in the column.
    auto cards = column->card_ids(); // Copy.
    for (const auto& card_id : cards)
    {
        board_.remove_object(card_id);
    }

    // Remove column from our list.
    column_ids_.erase(std::remove(column_ids_.begin(), column_ids_.end(), column_id),
                      column_ids_.end());

    board_.remove_object(column_id);
}

auto KanbanController::add_card(ObjectId column_id, const std::string& title) -> ObjectId
{
    auto* column = dynamic_cast<KanbanColumn*>(board_.get_object_mut(column_id));
    if (column == nullptr)
    {
        return kInvalidObjectId;
    }

    auto card = std::make_unique<KanbanCard>();
    card->set_title(title);
    card->set_column_id(column_id);
    card->set_card_index(column->card_count());

    const ObjectId card_id = board_.add_object(std::move(card));
    column->add_card(card_id);

    position_cards_in_column(column_id);
    return card_id;
}

auto KanbanController::move_card(ObjectId card_id, ObjectId to_column_id, int index) -> void
{
    auto* card = dynamic_cast<KanbanCard*>(board_.get_object_mut(card_id));
    if (card == nullptr)
    {
        return;
    }

    // Remove from source column.
    const ObjectId src_col_id = card->column_id();
    auto* src_col = dynamic_cast<KanbanColumn*>(board_.get_object_mut(src_col_id));
    if (src_col != nullptr)
    {
        src_col->remove_card(card_id);
    }

    // Insert into destination column.
    auto* dst_col = dynamic_cast<KanbanColumn*>(board_.get_object_mut(to_column_id));
    if (dst_col != nullptr)
    {
        dst_col->insert_card(card_id, index);
        card->set_column_id(to_column_id);
        card->set_card_index(index);
    }

    position_cards_in_column(src_col_id);
    position_cards_in_column(to_column_id);
}

auto KanbanController::relayout() -> void
{
    constexpr double kColumnGap = 20.0;
    double x_offset = origin_.x;

    for (const auto& col_id : column_ids_)
    {
        auto* column = dynamic_cast<KanbanColumn*>(board_.get_object_mut(col_id));
        if (column == nullptr)
        {
            continue;
        }

        auto col_xform = Transform2D::identity();
        col_xform.tx = x_offset;
        col_xform.ty = origin_.y;
        column->set_transform(col_xform);
        x_offset += column->column_width() + kColumnGap;

        position_cards_in_column(col_id);
    }
}

auto KanbanController::position_cards_in_column(ObjectId column_id) -> void
{
    auto* column = dynamic_cast<KanbanColumn*>(board_.get_object_mut(column_id));
    if (column == nullptr)
    {
        return;
    }

    constexpr double kHeaderHeight = 50.0;
    constexpr double kCardGap = 10.0;
    constexpr double kCardPadding = 10.0;

    const auto& col_xform = column->transform();
    const auto& cards = column->card_ids();

    for (size_t idx = 0; idx < cards.size(); ++idx)
    {
        auto* card = dynamic_cast<KanbanCard*>(board_.get_object_mut(cards[idx]));
        if (card == nullptr)
        {
            continue;
        }

        card->set_card_index(static_cast<int>(idx));
        auto card_xform = Transform2D::identity();
        card_xform.tx = col_xform.tx + kCardPadding;
        card_xform.ty = col_xform.ty + kHeaderHeight + static_cast<double>(idx) * (80.0 + kCardGap);
        card->set_transform(card_xform);
    }
}

// ── Workflow helpers (#15-18) ───────────────────────────────

auto KanbanController::archive_card(ObjectId card_id) -> void
{
    auto* card = dynamic_cast<KanbanCard*>(board_.get_object_mut(card_id));
    if (card == nullptr)
    {
        return;
    }

    // Remove from column.
    const ObjectId col_id = card->column_id();
    auto* column = dynamic_cast<KanbanColumn*>(board_.get_object_mut(col_id));
    if (column != nullptr)
    {
        column->remove_card(card_id);
        position_cards_in_column(col_id);
    }

    // Mark archived via metadata.
    card->set_metadata("archived", "true");
}

auto KanbanController::duplicate_card(ObjectId card_id) -> ObjectId
{
    const auto* card = dynamic_cast<const KanbanCard*>(board_.get_object(card_id));
    if (card == nullptr)
    {
        return kInvalidObjectId;
    }

    auto copy_ptr = card->clone();
    auto* copy = dynamic_cast<KanbanCard*>(copy_ptr.get());
    if (copy == nullptr)
    {
        return kInvalidObjectId;
    }

    const ObjectId col_id = card->column_id();
    const ObjectId new_id = board_.add_object(std::move(copy_ptr));

    auto* column = dynamic_cast<KanbanColumn*>(board_.get_object_mut(col_id));
    if (column != nullptr)
    {
        column->add_card(new_id);
        position_cards_in_column(col_id);
    }

    return new_id;
}

auto KanbanController::reorder_column(ObjectId col_a, ObjectId col_b) -> void
{
    auto iter_a = std::find(column_ids_.begin(), column_ids_.end(), col_a);
    auto iter_b = std::find(column_ids_.begin(), column_ids_.end(), col_b);
    if (iter_a == column_ids_.end() || iter_b == column_ids_.end())
    {
        return;
    }
    std::swap(*iter_a, *iter_b);
    relayout();
}

auto KanbanController::column_stats(ObjectId column_id) const -> ColumnStats
{
    ColumnStats stats;
    const auto* column = dynamic_cast<const KanbanColumn*>(board_.get_object(column_id));
    if (column == nullptr)
    {
        return stats;
    }

    stats.card_count = column->card_ids().size();
    stats.wip_exceeded =
        (column->wip_limit() > 0) && (static_cast<int>(stats.card_count) > column->wip_limit());

    for (const auto& cid : column->card_ids())
    {
        const auto* card = dynamic_cast<const KanbanCard*>(board_.get_object(cid));
        if (card != nullptr)
        {
            stats.total_story_points += card->story_points();
        }
    }

    return stats;
}

// ── Filtering & Aggregation (#34-35) ───────────────────────

auto KanbanController::filter_cards(const std::function<bool(const KanbanCard&)>& predicate) const
    -> std::vector<ObjectId>
{
    std::vector<ObjectId> result;
    for (const auto& col_id : column_ids_)
    {
        const auto* column = dynamic_cast<const KanbanColumn*>(board_.get_object(col_id));
        if (column == nullptr)
        {
            continue;
        }
        for (const auto& card_id : column->card_ids())
        {
            const auto* card = dynamic_cast<const KanbanCard*>(board_.get_object(card_id));
            if (card != nullptr && predicate(*card))
            {
                result.push_back(card_id);
            }
        }
    }
    return result;
}

auto KanbanController::total_story_points() const -> int
{
    int total = 0;
    for (const auto& col_id : column_ids_)
    {
        const auto stats = column_stats(col_id);
        total += stats.total_story_points;
    }
    return total;
}

} // namespace markamp::canvas
