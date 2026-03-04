/// @file NavigationService.cpp
/// @brief V13 Phase 32 Task 1 — Navigation history stack implementation.

#include "core/NavigationService.h"

#include "core/Events.h"

namespace markamp::core
{

NavigationService::NavigationService(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

void NavigationService::go_to(const std::string& document_id, int line, int column)
{
    // Skip duplicate consecutive locations
    if (is_duplicate(document_id, line, column))
    {
        return;
    }

    // Truncate forward history
    if (current_index_ >= 0 && current_index_ < static_cast<int>(stack_.size()) - 1)
    {
        stack_.erase(stack_.begin() + current_index_ + 1, stack_.end());
    }

    // Push new entry
    NavigationEntry entry;
    entry.document_id = document_id;
    entry.line = line;
    entry.column = column;
    entry.timestamp = std::chrono::steady_clock::now();
    stack_.push_back(std::move(entry));

    // Enforce max stack size
    if (static_cast<int>(stack_.size()) > kMaxStackSize)
    {
        stack_.erase(stack_.begin());
    }

    current_index_ = static_cast<int>(stack_.size()) - 1;
    emit_navigation_event();
}

auto NavigationService::go_back() -> bool
{
    if (!can_go_back())
    {
        return false;
    }

    --current_index_;
    emit_navigation_event();
    return true;
}

auto NavigationService::go_forward() -> bool
{
    if (!can_go_forward())
    {
        return false;
    }

    ++current_index_;
    emit_navigation_event();
    return true;
}

auto NavigationService::can_go_back() const -> bool
{
    return current_index_ > 0;
}

auto NavigationService::can_go_forward() const -> bool
{
    return current_index_ >= 0 && current_index_ < static_cast<int>(stack_.size()) - 1;
}

auto NavigationService::current_location() const -> const NavigationEntry*
{
    if (current_index_ >= 0 && current_index_ < static_cast<int>(stack_.size()))
    {
        return &stack_[static_cast<size_t>(current_index_)];
    }
    return nullptr;
}

auto NavigationService::history() const -> const std::vector<NavigationEntry>&
{
    return stack_;
}

auto NavigationService::current_index() const -> int
{
    return current_index_;
}

auto NavigationService::stack_size() const -> size_t
{
    return stack_.size();
}

void NavigationService::clear()
{
    stack_.clear();
    current_index_ = -1;
}

void NavigationService::emit_navigation_event()
{
    const auto* loc = current_location();
    if (loc != nullptr)
    {
        events::NavigationChangedEvent evt;
        evt.document_id = loc->document_id;
        evt.line = loc->line;
        evt.column = loc->column;
        event_bus_.publish(evt);
    }
}

auto NavigationService::is_duplicate(const std::string& document_id, int line, int column) const
    -> bool
{
    const auto* current = current_location();
    if (current == nullptr)
    {
        return false;
    }

    return current->document_id == document_id && current->line == line &&
           current->column == column;
}

} // namespace markamp::core
