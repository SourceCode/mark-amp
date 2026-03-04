#pragma once

/**
 * @file PointerEventRouter.h
 * @brief Phase 41 Task 2: Pointer event routing and capture.
 *
 * Unifies hit target resolution, capture semantics, and propagation
 * order with no-duplicate dispatch guarantee.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Pointer event type.
enum class PointerEventType : uint8_t
{
    kDown,
    kMove,
    kUp,
    kCancel,
};

/// A pointer event.
struct PointerEvent
{
    PointerEventType type{PointerEventType::kDown};
    double x{0.0};
    double y{0.0};
    int pointer_id{0};

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;
};

/// A hit target for pointer dispatch.
struct HitTarget
{
    std::string target_id;
    int z_order{0}; ///< Higher = on top
    double x{0.0};
    double y{0.0};
    double width{0.0};
    double height{0.0};

    /// Check if a point hits this target.
    [[nodiscard]] auto contains(double px, double py) const -> bool;
};

/**
 * @brief Routes pointer events to hit targets with capture semantics.
 */
class PointerEventRouter
{
public:
    PointerEventRouter() = default;

    // ── Target registration ────────────────────────────────────────

    /// Register a hit target.
    void register_target(const HitTarget& target);

    /// Remove a hit target.
    void remove_target(const std::string& target_id);

    /// Get target count.
    [[nodiscard]] auto target_count() const -> int;

    // ── Hit testing ────────────────────────────────────────────────

    /// Find the topmost target at a point.
    [[nodiscard]] auto hit_test(double x, double y) const -> const HitTarget*;

    // ── Capture ────────────────────────────────────────────────────

    /// Set pointer capture to a target (all events route here).
    void set_capture(const std::string& target_id);

    /// Release pointer capture.
    void release_capture();

    /// Check if pointer is captured.
    [[nodiscard]] auto has_capture() const -> bool;

    /// Get the capture target ID.
    [[nodiscard]] auto capture_target_id() const -> const std::string&;

    // ── Dispatch ───────────────────────────────────────────────────

    /// Route a pointer event. Returns the target ID it was dispatched to.
    [[nodiscard]] auto dispatch(const PointerEvent& event) -> std::string;

    /// Get total dispatch count.
    [[nodiscard]] auto dispatch_count() const -> int;

private:
    std::vector<HitTarget> targets_;
    std::string capture_target_id_;
    bool has_capture_{false};
    int dispatch_count_{0};
};

} // namespace markamp::canvas
