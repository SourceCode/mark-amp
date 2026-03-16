#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Card priority.
enum class CardPriority : uint8_t
{
    kNone,
    kLow,
    kMedium,
    kHigh,
    kCritical,
};

/// Kanban card.
struct KanbanCard
{
    std::string card_id;
    std::string title;
    std::string lane_id;
    int position{0};
    CardPriority priority{CardPriority::kNone};
    std::string label;
    std::string color;

    // ── Round 6 Batch 3-4 (#26-31) ──────────────────────────────

    /// (#26) Whether a title is set.
    [[nodiscard]] auto has_title() const noexcept -> bool
    {
        return !title.empty();
    }

    /// (#27) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }

    /// (#28) Whether a color is set.
    [[nodiscard]] auto has_color() const noexcept -> bool
    {
        return !color.empty();
    }

    /// (#29) Whether priority is critical.
    [[nodiscard]] auto is_critical() const noexcept -> bool
    {
        return priority == CardPriority::kCritical;
    }

    /// (#30) Whether priority is high.
    [[nodiscard]] auto is_high() const noexcept -> bool
    {
        return priority == CardPriority::kHigh;
    }

    /// (#31) Whether any priority is set.
    [[nodiscard]] auto has_priority() const noexcept -> bool
    {
        return priority != CardPriority::kNone;
    }
};

/// Kanban lane.
struct KanbanLane
{
    std::string lane_id;
    std::string title;
    int position{0};
    double width{250.0};

    // ── Round 6 Batch 4 (#32-33) ────────────────────────────────

    /// (#32) Whether a title is set.
    [[nodiscard]] auto has_title() const noexcept -> bool
    {
        return !title.empty();
    }

    /// (#33) Whether a lane ID is set.
    [[nodiscard]] auto has_id() const noexcept -> bool
    {
        return !lane_id.empty();
    }
};

/// Testable model for Kanban Object Fundamentals (Phase 59).
///
/// Encapsulates:
/// - Lane management (add/remove/reorder)
/// - Card management (add/remove/move between lanes)
/// - Drag reorder within and across lanes
/// - Card priority and labels
class KanbanModel
{
public:
    // ── Lanes ───────────────────────────────────────────────────────

    void set_lanes(std::vector<KanbanLane> lanes);
    [[nodiscard]] auto lanes() const -> const std::vector<KanbanLane>&;
    void add_lane(KanbanLane lane);
    void remove_lane(const std::string& lane_id);

    // ── Cards ───────────────────────────────────────────────────────

    void set_cards(std::vector<KanbanCard> cards);
    [[nodiscard]] auto cards() const -> const std::vector<KanbanCard>&;
    void add_card(KanbanCard card);
    void remove_card(const std::string& card_id);

    [[nodiscard]] auto cards_in_lane(const std::string& lane_id) const -> std::vector<KanbanCard>;
    [[nodiscard]] auto card_count_in_lane(const std::string& lane_id) const -> int;

    // ── Move ────────────────────────────────────────────────────────

    void move_card(const std::string& card_id, const std::string& target_lane_id, int position);

private:
    std::vector<KanbanLane> lanes_;
    std::vector<KanbanCard> cards_;

    // ── Round 6 Batch 4 (#34-36) ────────────────────────────────

    /// (#34) Number of lanes.
    [[nodiscard]] auto lane_count() const noexcept -> size_t
    {
        return lanes_.size();
    }

    /// (#35) Number of cards.
    [[nodiscard]] auto card_count() const noexcept -> size_t
    {
        return cards_.size();
    }

    /// (#36) Whether cards exist.
    [[nodiscard]] auto has_cards() const noexcept -> bool
    {
        return !cards_.empty();
    }
};

} // namespace markamp::canvas
