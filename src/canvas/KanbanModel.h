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
};

/// Kanban lane.
struct KanbanLane
{
    std::string lane_id;
    std::string title;
    int position{0};
    double width{250.0};
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
};

} // namespace markamp::canvas
