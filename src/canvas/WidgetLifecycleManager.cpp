// ============================================================================
// File: src/canvas/WidgetLifecycleManager.cpp
// Phase 14: Canvas Extensibility — widget lifecycle state machine
// ============================================================================
#include "canvas/WidgetLifecycleManager.h"

#include "core/Events.h"

namespace markamp::canvas
{

WidgetLifecycleManager::WidgetLifecycleManager(core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ── Instance management ───────────────────────────────────────────

auto WidgetLifecycleManager::track_widget(const std::string& widget_id) -> bool
{
    if (widget_id.empty() || instances_.contains(widget_id))
    {
        return false;
    }

    WidgetInstanceState instance;
    instance.widget_id = widget_id;
    instance.state = WidgetLifecycleState::kUninitialized;
    instance.transition_count = 0;

    instances_.emplace(widget_id, std::move(instance));
    return true;
}

auto WidgetLifecycleManager::untrack_widget(const std::string& widget_id) -> bool
{
    return instances_.erase(widget_id) > 0;
}

// ── State transitions ─────────────────────────────────────────────

auto WidgetLifecycleManager::initialize_widget(const std::string& widget_id)
    -> LifecycleTransitionResult
{
    return transition(widget_id, WidgetLifecycleState::kInitializing);
}

auto WidgetLifecycleManager::activate_widget(const std::string& widget_id)
    -> LifecycleTransitionResult
{
    return transition(widget_id, WidgetLifecycleState::kActive);
}

auto WidgetLifecycleManager::suspend_widget(const std::string& widget_id)
    -> LifecycleTransitionResult
{
    return transition(widget_id, WidgetLifecycleState::kSuspended);
}

auto WidgetLifecycleManager::error_widget(const std::string& widget_id,
                                          const std::string& error_message)
    -> LifecycleTransitionResult
{
    auto result = transition(widget_id, WidgetLifecycleState::kError);
    if (result.success)
    {
        auto iter = instances_.find(widget_id);
        if (iter != instances_.end())
        {
            iter->second.error_message = error_message;
        }
    }
    return result;
}

auto WidgetLifecycleManager::destroy_widget(const std::string& widget_id)
    -> LifecycleTransitionResult
{
    return transition(widget_id, WidgetLifecycleState::kDestroyed);
}

// ── Query ─────────────────────────────────────────────────────────

auto WidgetLifecycleManager::widget_state(const std::string& widget_id) const
    -> const WidgetInstanceState*
{
    auto iter = instances_.find(widget_id);
    return iter != instances_.end() ? &iter->second : nullptr;
}

auto WidgetLifecycleManager::tracked_widgets() const
    -> const std::unordered_map<std::string, WidgetInstanceState>&
{
    return instances_;
}

auto WidgetLifecycleManager::widgets_in_state(WidgetLifecycleState state) const
    -> std::vector<const WidgetInstanceState*>
{
    std::vector<const WidgetInstanceState*> result;
    for (const auto& [wid, instance] : instances_)
    {
        if (instance.state == state)
        {
            result.push_back(&instance);
        }
    }
    return result;
}

auto WidgetLifecycleManager::tracked_count() const -> size_t
{
    return instances_.size();
}

auto WidgetLifecycleManager::active_count() const -> size_t
{
    size_t count = 0;
    for (const auto& [wid, instance] : instances_)
    {
        if (instance.state == WidgetLifecycleState::kActive)
        {
            ++count;
        }
    }
    return count;
}

// ── Callbacks ─────────────────────────────────────────────────────

auto WidgetLifecycleManager::set_on_state_change(LifecycleCallback callback) -> void
{
    on_state_change_ = std::move(callback);
}

// ── Utility ───────────────────────────────────────────────────────

auto WidgetLifecycleManager::state_name(WidgetLifecycleState state) -> std::string
{
    switch (state)
    {
        case WidgetLifecycleState::kUninitialized:
            return "uninitialized";
        case WidgetLifecycleState::kInitializing:
            return "initializing";
        case WidgetLifecycleState::kActive:
            return "active";
        case WidgetLifecycleState::kSuspended:
            return "suspended";
        case WidgetLifecycleState::kError:
            return "error";
        case WidgetLifecycleState::kDestroyed:
            return "destroyed";
    }
    return "unknown";
}

auto WidgetLifecycleManager::is_valid_transition(WidgetLifecycleState from_state,
                                                 WidgetLifecycleState to_state) -> bool
{
    // Destroyed is a terminal state — no transitions out
    if (from_state == WidgetLifecycleState::kDestroyed)
    {
        return false;
    }

    switch (to_state)
    {
        case WidgetLifecycleState::kUninitialized:
            return false; // Cannot go back to uninitialized

        case WidgetLifecycleState::kInitializing:
            return from_state == WidgetLifecycleState::kUninitialized;

        case WidgetLifecycleState::kActive:
            return from_state == WidgetLifecycleState::kInitializing ||
                   from_state == WidgetLifecycleState::kSuspended;

        case WidgetLifecycleState::kSuspended:
            return from_state == WidgetLifecycleState::kActive;

        case WidgetLifecycleState::kError:
            // Can enter error from any non-destroyed state
            return from_state != WidgetLifecycleState::kDestroyed;

        case WidgetLifecycleState::kDestroyed:
            // Can be destroyed from any state
            return true;
    }
    return false;
}

auto WidgetLifecycleManager::clear() -> void
{
    instances_.clear();
}

// ── Private ───────────────────────────────────────────────────────

auto WidgetLifecycleManager::transition(const std::string& widget_id, WidgetLifecycleState target)
    -> LifecycleTransitionResult
{
    auto iter = instances_.find(widget_id);
    if (iter == instances_.end())
    {
        return {false,
                "Widget not tracked: " + widget_id,
                WidgetLifecycleState::kUninitialized,
                WidgetLifecycleState::kUninitialized};
    }

    auto& instance = iter->second;
    const auto previous = instance.state;

    if (!is_valid_transition(previous, target))
    {
        return {false,
                "Invalid transition: " + state_name(previous) + " -> " + state_name(target),
                previous,
                previous};
    }

    instance.state = target;
    ++instance.transition_count;

    // Fire callback
    if (on_state_change_)
    {
        on_state_change_(widget_id, previous, target);
    }

    // Publish event
    core::events::CanvasWidgetInsertedEvent evt;
    evt.widget_id = widget_id;
    evt.object_id = state_name(target);
    evt.app_id = "lifecycle";
    event_bus_.publish(evt);

    return {true, "", previous, target};
}

} // namespace markamp::canvas
