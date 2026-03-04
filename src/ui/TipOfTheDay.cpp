// ============================================================================
// File: src/ui/TipOfTheDay.cpp
// Phase 48: Welcome and Onboarding — Tip of the Day model
// ============================================================================
#include "TipOfTheDay.h"

#include <algorithm>

namespace markamp::ui
{

void TipOfTheDayModel::add_tip(Tip tip)
{
    tips_.push_back(std::move(tip));
}

auto TipOfTheDayModel::tip_for_today() const -> const Tip*
{
    if (tips_.empty() || !enabled_)
    {
        return nullptr;
    }

    // Find first tip not in cooldown.
    for (size_t i = 0; i < tips_.size(); ++i)
    {
        bool in_cooldown = std::ranges::find(shown_tip_ids_, tips_[i].id) != shown_tip_ids_.end();
        if (!in_cooldown)
        {
            return &tips_[i];
        }
    }

    // All shown — return first (cycle resets).
    return &tips_[0];
}

void TipOfTheDayModel::mark_shown(const std::string& tip_id)
{
    // Avoid duplicates.
    if (std::ranges::find(shown_tip_ids_, tip_id) == shown_tip_ids_.end())
    {
        shown_tip_ids_.push_back(tip_id);
    }
}

void TipOfTheDayModel::next_tip()
{
    if (!tips_.empty())
    {
        current_index_ = (current_index_ + 1) % static_cast<int>(tips_.size());
    }
}

void TipOfTheDayModel::prev_tip()
{
    if (!tips_.empty())
    {
        current_index_ =
            current_index_ <= 0 ? static_cast<int>(tips_.size()) - 1 : current_index_ - 1;
    }
}

} // namespace markamp::ui
