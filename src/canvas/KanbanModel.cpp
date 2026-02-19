#include "KanbanModel.h"

#include <algorithm>

namespace markamp::canvas
{

void KanbanModel::set_lanes(std::vector<KanbanLane> lanes)
{
    lanes_ = std::move(lanes);
}
auto KanbanModel::lanes() const -> const std::vector<KanbanLane>&
{
    return lanes_;
}

void KanbanModel::add_lane(KanbanLane lane)
{
    lanes_.push_back(std::move(lane));
}

void KanbanModel::remove_lane(const std::string& lane_id)
{
    lanes_.erase(std::remove_if(lanes_.begin(),
                                lanes_.end(),
                                [&](const KanbanLane& lane) { return lane.lane_id == lane_id; }),
                 lanes_.end());
    // Remove cards in that lane
    cards_.erase(std::remove_if(cards_.begin(),
                                cards_.end(),
                                [&](const KanbanCard& card) { return card.lane_id == lane_id; }),
                 cards_.end());
}

void KanbanModel::set_cards(std::vector<KanbanCard> cards)
{
    cards_ = std::move(cards);
}
auto KanbanModel::cards() const -> const std::vector<KanbanCard>&
{
    return cards_;
}

void KanbanModel::add_card(KanbanCard card)
{
    cards_.push_back(std::move(card));
}

void KanbanModel::remove_card(const std::string& card_id)
{
    cards_.erase(std::remove_if(cards_.begin(),
                                cards_.end(),
                                [&](const KanbanCard& card) { return card.card_id == card_id; }),
                 cards_.end());
}

auto KanbanModel::cards_in_lane(const std::string& lane_id) const -> std::vector<KanbanCard>
{
    std::vector<KanbanCard> result;
    for (const auto& card : cards_)
    {
        if (card.lane_id == lane_id)
        {
            result.push_back(card);
        }
    }
    std::sort(result.begin(),
              result.end(),
              [](const KanbanCard& lhs, const KanbanCard& rhs)
              { return lhs.position < rhs.position; });
    return result;
}

auto KanbanModel::card_count_in_lane(const std::string& lane_id) const -> int
{
    int count = 0;
    for (const auto& card : cards_)
    {
        if (card.lane_id == lane_id)
        {
            ++count;
        }
    }
    return count;
}

void KanbanModel::move_card(const std::string& card_id,
                            const std::string& target_lane_id,
                            int position)
{
    for (auto& card : cards_)
    {
        if (card.card_id == card_id)
        {
            card.lane_id = target_lane_id;
            card.position = position;
            return;
        }
    }
}

} // namespace markamp::canvas
