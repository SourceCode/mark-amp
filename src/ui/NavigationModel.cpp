#include "NavigationModel.h"

#include <algorithm>

namespace markamp::ui
{

void NavigationModel::push_location(NavHistoryEntry entry)
{
    // Truncate forward history when pushing new location
    if (history_index_ >= 0 && history_index_ < static_cast<int>(history_.size()) - 1)
    {
        history_.erase(history_.begin() + history_index_ + 1, history_.end());
    }
    history_.push_back(std::move(entry));
    history_index_ = static_cast<int>(history_.size()) - 1;
}

auto NavigationModel::go_back() -> const NavHistoryEntry*
{
    if (!can_go_back())
    {
        return nullptr;
    }
    --history_index_;
    return &history_[static_cast<size_t>(history_index_)];
}

auto NavigationModel::go_forward() -> const NavHistoryEntry*
{
    if (!can_go_forward())
    {
        return nullptr;
    }
    ++history_index_;
    return &history_[static_cast<size_t>(history_index_)];
}

auto NavigationModel::can_go_back() const -> bool
{
    return history_index_ > 0;
}

auto NavigationModel::can_go_forward() const -> bool
{
    return history_index_ >= 0 && history_index_ < static_cast<int>(history_.size()) - 1;
}

auto NavigationModel::current() const -> const NavHistoryEntry*
{
    if (history_index_ < 0 || history_index_ >= static_cast<int>(history_.size()))
    {
        return nullptr;
    }
    return &history_[static_cast<size_t>(history_index_)];
}

void NavigationModel::set_targets(std::vector<NavTarget> targets)
{
    targets_ = std::move(targets);
}

auto NavigationModel::targets_by_type(NavTargetType type) const -> std::vector<NavTarget>
{
    std::vector<NavTarget> result;
    for (const auto& target : targets_)
    {
        if (target.type == type)
        {
            result.push_back(target);
        }
    }
    return result;
}

auto NavigationModel::mru_targets() const -> std::vector<NavTarget>
{
    auto sorted = targets_;
    std::sort(sorted.begin(),
              sorted.end(),
              [](const NavTarget& lhs, const NavTarget& rhs)
              { return lhs.use_count > rhs.use_count; });
    return sorted;
}

void NavigationModel::set_breadcrumb(const std::vector<std::string>& segments)
{
    breadcrumb_ = segments;
}

auto NavigationModel::breadcrumb() const -> const std::vector<std::string>&
{
    return breadcrumb_;
}

void NavigationModel::set_active_surface(const std::string& surface)
{
    active_surface_ = surface;
}

auto NavigationModel::active_surface() const -> const std::string&
{
    return active_surface_;
}

} // namespace markamp::ui
