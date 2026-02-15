/// @file NavigationService.cpp
/// @brief V4 Phase 20 – Contextual Pane Navigation implementation.

#include "ui/NavigationService.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/VaultService.h"

namespace markamp::ui
{

// ============================================================================
// NavigationHistory
// ============================================================================

auto NavigationHistory::push(const NavigationEntry& entry) -> void
{
    // Truncate forward history
    if (current_index_ < static_cast<int>(entries_.size()) - 1)
    {
        entries_.erase(entries_.begin() + current_index_ + 1, entries_.end());
    }
    entries_.push_back(entry);
    current_index_ = static_cast<int>(entries_.size()) - 1;
}

auto NavigationHistory::go_back() -> const NavigationEntry*
{
    if (!can_go_back())
    {
        return nullptr;
    }
    --current_index_;
    return &entries_[static_cast<size_t>(current_index_)];
}

auto NavigationHistory::go_forward() -> const NavigationEntry*
{
    if (!can_go_forward())
    {
        return nullptr;
    }
    ++current_index_;
    return &entries_[static_cast<size_t>(current_index_)];
}

auto NavigationHistory::can_go_back() const -> bool
{
    return current_index_ > 0;
}

auto NavigationHistory::can_go_forward() const -> bool
{
    return current_index_ < static_cast<int>(entries_.size()) - 1;
}

auto NavigationHistory::current() const -> const NavigationEntry*
{
    if (current_index_ >= 0 && current_index_ < static_cast<int>(entries_.size()))
    {
        return &entries_[static_cast<size_t>(current_index_)];
    }
    return nullptr;
}

auto NavigationHistory::size() const -> int
{
    return static_cast<int>(entries_.size());
}

auto NavigationHistory::entries() const -> const std::vector<NavigationEntry>&
{
    return entries_;
}

// ============================================================================
// NavigationService
// ============================================================================

NavigationService::NavigationService(core::EventBus& event_bus, core::VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

auto NavigationService::navigate_to(int pane_id,
                                    const std::string& document_id,
                                    LinkOpenBehavior behavior) -> void
{
    (void)behavior; // UI-dependent dispatch handled upstream

    NavigationEntry entry;
    entry.document_id = document_id;
    histories_[pane_id].push(entry);

    core::events::NavigationEvent evt;
    evt.document_id = document_id;
    evt.direction = "forward";
    event_bus_.publish(evt);
}

auto NavigationService::follow_wikilink(int pane_id, const std::string& link_target) -> bool
{
    auto resolved = vault_service_.resolve_wikilink(link_target);
    if (!resolved.has_value())
    {
        return false;
    }
    navigate_to(pane_id, *resolved);
    return true;
}

auto NavigationService::go_back(int pane_id) -> const NavigationEntry*
{
    return histories_[pane_id].go_back();
}

auto NavigationService::go_forward(int pane_id) -> const NavigationEntry*
{
    return histories_[pane_id].go_forward();
}

auto NavigationService::history_for(int pane_id) -> NavigationHistory&
{
    return histories_[pane_id];
}

auto NavigationService::breadcrumb_trail(int pane_id) const -> std::vector<std::string>
{
    std::vector<std::string> trail;
    auto iter = histories_.find(pane_id);
    if (iter == histories_.end())
    {
        return trail;
    }

    for (const auto& entry : iter->second.entries())
    {
        // Resolve document_id to title via vault
        auto entry_info = vault_service_.find_by_name(entry.document_id);
        if (entry_info.has_value())
        {
            trail.push_back(entry_info->title);
        }
        else
        {
            trail.push_back(entry.document_id);
        }
    }
    return trail;
}

auto NavigationService::can_resolve(const std::string& link_target) const -> bool
{
    return vault_service_.resolve_wikilink(link_target).has_value();
}

} // namespace markamp::ui
