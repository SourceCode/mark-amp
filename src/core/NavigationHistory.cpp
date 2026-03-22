/// @file NavigationHistory.cpp
/// @brief V24 P10-T04: NavigationHistory implementation.
#include "NavigationHistory.h"

namespace markamp::core
{

void NavigationHistory::push(NavigationEntry entry)
{
    // Clear any forward history
    if (current_index_ + 1 < static_cast<int>(entries_.size())) {
        entries_.erase(entries_.begin() + current_index_ + 1, entries_.end());
    }

    entries_.push_back(std::move(entry));
    current_index_ = static_cast<int>(entries_.size()) - 1;

    // Trim if over max size
    if (static_cast<int>(entries_.size()) > max_size_) {
        int excess = static_cast<int>(entries_.size()) - max_size_;
        entries_.erase(entries_.begin(), entries_.begin() + excess);
        current_index_ -= excess;
        if (current_index_ < 0) current_index_ = 0;
    }
}

auto NavigationHistory::go_back() -> std::optional<NavigationEntry>
{
    if (!can_go_back()) return std::nullopt;
    --current_index_;
    return entries_[static_cast<size_t>(current_index_)];
}

auto NavigationHistory::go_forward() -> std::optional<NavigationEntry>
{
    if (!can_go_forward()) return std::nullopt;
    ++current_index_;
    return entries_[static_cast<size_t>(current_index_)];
}

auto NavigationHistory::can_go_back() const noexcept -> bool
{
    return current_index_ > 0;
}

auto NavigationHistory::can_go_forward() const noexcept -> bool
{
    return current_index_ >= 0 &&
           current_index_ < static_cast<int>(entries_.size()) - 1;
}

auto NavigationHistory::current() const -> const NavigationEntry*
{
    if (current_index_ < 0 || current_index_ >= static_cast<int>(entries_.size()))
        return nullptr;
    return &entries_[static_cast<size_t>(current_index_)];
}

auto NavigationHistory::breadcrumbs() const -> std::vector<const NavigationEntry*>
{
    std::vector<const NavigationEntry*> result;
    for (int i = 0; i <= current_index_ && i < static_cast<int>(entries_.size()); ++i) {
        result.push_back(&entries_[static_cast<size_t>(i)]);
    }
    return result;
}

void NavigationHistory::clear()
{
    entries_.clear();
    current_index_ = -1;
}

void NavigationHistory::set_max_size(int max_size)
{
    max_size_ = max_size > 0 ? max_size : 1;
}

} // namespace markamp::core
