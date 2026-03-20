// ============================================================================
// File: src/canvas/DragAutoscrollModel.h
// Phase 02 W16: Drag Autoscroll — speed, margins, active state
// ============================================================================
#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Autoscroll direction.
enum class AutoscrollDirection : uint8_t
{
    kNone,
    kUp,
    kDown,
    kLeft,
    kRight,
    kUpLeft,
    kUpRight,
    kDownLeft,
    kDownRight,
};

/// Testable model for Drag Autoscroll (Phase 02 W16).
///
/// Encapsulates:
/// - Edge margin thresholds
/// - Scroll speed
/// - Active state and current direction
class DragAutoscrollModel
{
public:
    // ── Edge Margins ────────────────────────────────────────────────

    void set_edge_margin(double pixels);
    [[nodiscard]] auto edge_margin() const -> double;

    // ── Scroll Speed ────────────────────────────────────────────────

    void set_speed(double pixels_per_tick);
    [[nodiscard]] auto speed() const -> double;

    // ── State ───────────────────────────────────────────────────────

    void start(AutoscrollDirection dir);
    void stop();

    [[nodiscard]] auto direction() const -> AutoscrollDirection;

    [[nodiscard]] auto is_active() const noexcept -> bool
    {
        return direction_ != AutoscrollDirection::kNone;
    }

    /// Human-readable label for a direction.
    [[nodiscard]] static auto direction_label(AutoscrollDirection dir) -> std::string;

private:
    double edge_margin_{40.0};
    double speed_{8.0};
    AutoscrollDirection direction_{AutoscrollDirection::kNone};
};

} // namespace markamp::canvas
