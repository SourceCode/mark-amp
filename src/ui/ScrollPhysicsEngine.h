#pragma once

#include <chrono>
#include <cstdint>

namespace markamp::ui
{

/// Phase 42 Task 05: Scroll mode enum.
enum class ScrollMode : uint8_t
{
    kImmediate, ///< Jump to position (no animation)
    kSmooth,    ///< Smooth ease-out scroll
    kMomentum,  ///< Momentum decay after fling
};

/// Phase 42 Task 05: Testable model for scroll physics.
///
/// Pure state machine with no wxWidgets dependency. Manages velocity,
/// deceleration, overscroll bounce, and smooth scrolling animations.
class ScrollPhysicsModel
{
public:
    /// Update physics state for one frame tick.
    /// @param dt_ms Time since last tick in milliseconds.
    /// @return true if animation is still active and needs more frames.
    [[nodiscard]] auto tick(double dt_ms) -> bool;

    /// Begin smooth scroll to a target position.
    void scroll_to(double target, ScrollMode mode = ScrollMode::kSmooth);

    /// Apply momentum from a fling gesture.
    void fling(double velocity_px_per_sec);

    /// Immediately stop all physics.
    void stop();

    /// Get current scroll position (pixels).
    [[nodiscard]] auto position() const -> double;

    /// Set current position without animation.
    void set_position(double pos);

    /// Set valid scroll range [0, max].
    void set_content_range(double visible_height, double content_height);

    [[nodiscard]] auto max_scroll() const -> double;
    [[nodiscard]] auto visible_height() const -> double;
    [[nodiscard]] auto content_height() const -> double;

    /// Is physics currently animating?
    [[nodiscard]] auto is_animating() const -> bool;

    /// Is position beyond content bounds (overscroll)?
    [[nodiscard]] auto is_overscrolling() const -> bool;

    /// Get overscroll distance (negative = past top, positive = past bottom).
    [[nodiscard]] auto overscroll_distance() const -> double;

    // ── Parameters ─────────────────────────────────────────────

    void set_friction(double friction);
    [[nodiscard]] auto friction() const -> double;

    void set_bounce_enabled(bool enabled);
    [[nodiscard]] auto bounce_enabled() const -> bool;

    void set_spring_stiffness(double stiffness);
    [[nodiscard]] auto spring_stiffness() const -> double;

    static constexpr double kDefaultFriction = 0.95;
    static constexpr double kDefaultSpringStiffness = 300.0;
    static constexpr double kMinVelocity = 0.5;    ///< Below this, stop animating
    static constexpr double kOverscrollMax = 80.0; ///< Max overscroll distance

private:
    double position_{0.0};
    double velocity_{0.0};
    double target_{0.0};
    double visible_height_{0.0};
    double content_height_{0.0};
    double friction_{kDefaultFriction};
    double spring_stiffness_{kDefaultSpringStiffness};
    bool bounce_enabled_{true};
    bool animating_{false};
    ScrollMode mode_{ScrollMode::kImmediate};

    void apply_bounds();
    void apply_spring(double dt_sec);
};

} // namespace markamp::ui
