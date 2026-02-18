// ============================================================================
// File: src/canvas/CanvasObjectLifecycleEvents.cpp
// Phase 12: Canvas Advanced Objects — object lifecycle event emission
// ============================================================================
#include "canvas/CanvasObjectLifecycleEvents.h"

#include <algorithm>

namespace markamp::canvas
{

CanvasObjectEventEmitter::CanvasObjectEventEmitter() = default;

// ── Event Emission ────────────────────────────────────────────────

auto CanvasObjectEventEmitter::emit(const CanvasLifecycleEvent& event) -> void
{
    // Record in history
    event_history_.push_back(event);
    if (event_history_.size() > kMaxEventHistory)
    {
        event_history_.erase(event_history_.begin());
    }

    // Notify all matching subscribers
    for (const auto& sub : subscriptions_)
    {
        if (!sub.filter_by_type || sub.type_filter == event.type)
        {
            sub.callback(event);
        }
    }
}

auto CanvasObjectEventEmitter::emit_batch(const std::vector<CanvasLifecycleEvent>& events) -> void
{
    for (const auto& event : events)
    {
        emit(event);
    }
}

// ── Convenience Emitters ──────────────────────────────────────────

auto CanvasObjectEventEmitter::emit_created(ObjectId obj_id,
                                            CanvasObjectType obj_type,
                                            const std::string& obj_name) -> void
{
    CanvasLifecycleEvent event;
    event.type = CanvasLifecycleEventType::kObjectCreated;
    event.object_id = obj_id;
    event.object_type = obj_type;
    event.object_name = obj_name;
    emit(event);
}

auto CanvasObjectEventEmitter::emit_deleted(ObjectId obj_id,
                                            CanvasObjectType obj_type,
                                            const std::string& obj_name) -> void
{
    CanvasLifecycleEvent event;
    event.type = CanvasLifecycleEventType::kObjectDeleted;
    event.object_id = obj_id;
    event.object_type = obj_type;
    event.object_name = obj_name;
    emit(event);
}

auto CanvasObjectEventEmitter::emit_moved(ObjectId obj_id,
                                          const Point2D& old_pos,
                                          const Point2D& new_pos) -> void
{
    CanvasLifecycleEvent event;
    event.type = CanvasLifecycleEventType::kObjectMoved;
    event.object_id = obj_id;
    event.old_position = old_pos;
    event.new_position = new_pos;
    emit(event);
}

auto CanvasObjectEventEmitter::emit_resized(
    ObjectId obj_id, double old_w, double old_h, double new_w, double new_h) -> void
{
    CanvasLifecycleEvent event;
    event.type = CanvasLifecycleEventType::kObjectResized;
    event.object_id = obj_id;
    event.old_width = old_w;
    event.old_height = old_h;
    event.new_width = new_w;
    event.new_height = new_h;
    emit(event);
}

auto CanvasObjectEventEmitter::emit_lock_changed(ObjectId obj_id, bool locked) -> void
{
    CanvasLifecycleEvent event;
    event.type = locked ? CanvasLifecycleEventType::kObjectLocked
                        : CanvasLifecycleEventType::kObjectUnlocked;
    event.object_id = obj_id;
    event.new_state = locked;
    emit(event);
}

auto CanvasObjectEventEmitter::emit_visibility_changed(ObjectId obj_id, bool visible) -> void
{
    CanvasLifecycleEvent event;
    event.type = CanvasLifecycleEventType::kObjectVisibilityChanged;
    event.object_id = obj_id;
    event.new_state = visible;
    emit(event);
}

// ── Subscription ──────────────────────────────────────────────────

auto CanvasObjectEventEmitter::subscribe(CanvasLifecycleCallback callback)
    -> LifecycleSubscriptionId
{
    const auto sub_id = next_id_++;
    subscriptions_.push_back({sub_id, std::move(callback), false, {}});
    return sub_id;
}

auto CanvasObjectEventEmitter::subscribe_to(CanvasLifecycleEventType type,
                                            CanvasLifecycleCallback callback)
    -> LifecycleSubscriptionId
{
    const auto sub_id = next_id_++;
    subscriptions_.push_back({sub_id, std::move(callback), true, type});
    return sub_id;
}

auto CanvasObjectEventEmitter::unsubscribe(LifecycleSubscriptionId subscription_id) -> void
{
    const auto iter = std::ranges::find_if(subscriptions_,
                                           [subscription_id](const Subscription& sub)
                                           { return sub.sub_id == subscription_id; });
    if (iter != subscriptions_.end())
    {
        subscriptions_.erase(iter);
    }
}

auto CanvasObjectEventEmitter::subscription_count() const -> size_t
{
    return subscriptions_.size();
}

// ── Event History ─────────────────────────────────────────────────

auto CanvasObjectEventEmitter::recent_events() const -> const std::vector<CanvasLifecycleEvent>&
{
    return event_history_;
}

auto CanvasObjectEventEmitter::clear_history() -> void
{
    event_history_.clear();
}

// ── Event Name ────────────────────────────────────────────────────

auto CanvasObjectEventEmitter::event_type_name(CanvasLifecycleEventType type) -> std::string
{
    switch (type)
    {
        case CanvasLifecycleEventType::kObjectCreated:
            return "ObjectCreated";
        case CanvasLifecycleEventType::kObjectDeleted:
            return "ObjectDeleted";
        case CanvasLifecycleEventType::kObjectModified:
            return "ObjectModified";
        case CanvasLifecycleEventType::kObjectMoved:
            return "ObjectMoved";
        case CanvasLifecycleEventType::kObjectResized:
            return "ObjectResized";
        case CanvasLifecycleEventType::kObjectLocked:
            return "ObjectLocked";
        case CanvasLifecycleEventType::kObjectUnlocked:
            return "ObjectUnlocked";
        case CanvasLifecycleEventType::kObjectGrouped:
            return "ObjectGrouped";
        case CanvasLifecycleEventType::kObjectUngrouped:
            return "ObjectUngrouped";
        case CanvasLifecycleEventType::kObjectRenamed:
            return "ObjectRenamed";
        case CanvasLifecycleEventType::kObjectVisibilityChanged:
            return "ObjectVisibilityChanged";
        case CanvasLifecycleEventType::kObjectZOrderChanged:
            return "ObjectZOrderChanged";
    }
    return "Unknown";
}

} // namespace markamp::canvas
