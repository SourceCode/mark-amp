// ============================================================================
// File: src/canvas/CanvasObjectLifecycleEvents.h
// Phase 12: Canvas Advanced Objects — object lifecycle event emission
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Types of canvas object lifecycle events.
enum class CanvasLifecycleEventType : uint8_t
{
    kObjectCreated,
    kObjectDeleted,
    kObjectModified,
    kObjectMoved,
    kObjectResized,
    kObjectLocked,
    kObjectUnlocked,
    kObjectGrouped,
    kObjectUngrouped,
    kObjectRenamed,
    kObjectVisibilityChanged,
    kObjectZOrderChanged
};

/// Payload for a canvas object lifecycle event.
struct CanvasLifecycleEvent
{
    CanvasLifecycleEventType type;
    ObjectId object_id{kInvalidObjectId};
    CanvasObjectType object_type{CanvasObjectType::StickyNote};
    std::string object_name;

    // ── Move/Resize details ──
    Point2D old_position{0.0, 0.0};
    Point2D new_position{0.0, 0.0};
    double old_width{0.0};
    double old_height{0.0};
    double new_width{0.0};
    double new_height{0.0};

    // ── Group details ──
    ObjectId group_id{kInvalidObjectId};
    std::vector<ObjectId> member_ids;

    // ── Visibility/Lock ──
    bool new_state{false};

    // ── Z-order ──
    int old_z_index{0};
    int new_z_index{0};
};

/// Callback type for lifecycle event listeners.
using CanvasLifecycleCallback = std::function<void(const CanvasLifecycleEvent&)>;

/// Subscription handle for unsubscribing from lifecycle events.
using LifecycleSubscriptionId = uint64_t;

/// Emits lifecycle events for canvas object mutations.
///
/// Provides:
///   - Event emission for all major object operations
///   - Subscription-based listener pattern
///   - Batch event support for multi-object operations
///   - Event filtering by type
class CanvasObjectEventEmitter
{
public:
    CanvasObjectEventEmitter();

    // ── Event Emission ────────────────────────────────────────────

    /// Emit a single lifecycle event.
    auto emit(const CanvasLifecycleEvent& event) -> void;

    /// Emit a batch of events (e.g. for multi-select move).
    auto emit_batch(const std::vector<CanvasLifecycleEvent>& events) -> void;

    // ── Convenience Emitters ──────────────────────────────────────

    /// Emit an object created event.
    auto emit_created(ObjectId obj_id, CanvasObjectType obj_type, const std::string& obj_name)
        -> void;

    /// Emit an object deleted event.
    auto emit_deleted(ObjectId obj_id, CanvasObjectType obj_type, const std::string& obj_name)
        -> void;

    /// Emit an object moved event.
    auto emit_moved(ObjectId obj_id, const Point2D& old_pos, const Point2D& new_pos) -> void;

    /// Emit an object resized event.
    auto emit_resized(ObjectId obj_id, double old_w, double old_h, double new_w, double new_h)
        -> void;

    /// Emit an object locked/unlocked event.
    auto emit_lock_changed(ObjectId obj_id, bool locked) -> void;

    /// Emit an object visibility changed event.
    auto emit_visibility_changed(ObjectId obj_id, bool visible) -> void;

    // ── Subscription ──────────────────────────────────────────────

    /// Subscribe to all lifecycle events.
    [[nodiscard]] auto subscribe(CanvasLifecycleCallback callback) -> LifecycleSubscriptionId;

    /// Subscribe to a specific event type only.
    [[nodiscard]] auto subscribe_to(CanvasLifecycleEventType type, CanvasLifecycleCallback callback)
        -> LifecycleSubscriptionId;

    /// Unsubscribe a listener.
    auto unsubscribe(LifecycleSubscriptionId subscription_id) -> void;

    /// Get the number of active subscriptions.
    [[nodiscard]] auto subscription_count() const -> size_t;

    // ── Event History ─────────────────────────────────────────────

    /// Get recent events (for debugging/testing).
    [[nodiscard]] auto recent_events() const -> const std::vector<CanvasLifecycleEvent>&;

    /// Clear event history.
    auto clear_history() -> void;

    /// Maximum events to keep in history.
    static constexpr size_t kMaxEventHistory = 100;

    // ── Event Name ────────────────────────────────────────────────

    /// Get a human-readable name for an event type.
    [[nodiscard]] static auto event_type_name(CanvasLifecycleEventType type) -> std::string;

private:
    struct Subscription
    {
        LifecycleSubscriptionId sub_id;
        CanvasLifecycleCallback callback;
        bool filter_by_type{false};
        CanvasLifecycleEventType type_filter{CanvasLifecycleEventType::kObjectCreated};
    };

    std::vector<Subscription> subscriptions_;
    std::vector<CanvasLifecycleEvent> event_history_;
    LifecycleSubscriptionId next_id_{1};
};

} // namespace markamp::canvas
