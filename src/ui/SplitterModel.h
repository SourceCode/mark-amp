#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Visual state of the splitter (Phase 15 Task 1).
enum class SplitterState : uint8_t
{
    kIdle,
    kHovered,
    kDragging,
};

/// A snap point definition.
struct SplitterSnapPoint
{
    int position{0};  ///< Position in pixels where the splitter should snap
    int tolerance{5}; ///< How close (px) to trigger snap
};

/// Testable model for the Pane Splitter (Phase 15).
///
/// Encapsulates:
/// - Drag position clamping (min/max constraints)
/// - Snap point magnetism
/// - Double-click reset to default
/// - Keyboard resize increments
/// - Position persistence
class SplitterModel
{
public:
    /// Configure constraints.
    void set_constraints(int min_position, int max_position, int default_position);

    /// Set snap points.
    void set_snap_points(std::vector<SplitterSnapPoint> snap_points);

    /// Set the keyboard resize step (pixels per keypress).
    void set_keyboard_step(int step);

    // ── Position ────────────────────────────────────────────────────

    /// Get the current position.
    [[nodiscard]] auto position() const -> int;

    /// Set position (clamped and snapped).
    void set_position(int raw_position);

    /// Reset to default position (double-click behavior).
    void reset_to_default();

    // ── Keyboard resize ─────────────────────────────────────────────

    /// Keyboard resize left/up (decrease position).
    void resize_decrease();

    /// Keyboard resize right/down (increase position).
    void resize_increase();

    // ── Drag state ──────────────────────────────────────────────────

    /// Begin dragging.
    void begin_drag();

    /// Update drag position.
    void update_drag(int raw_position);

    /// End drag.
    void end_drag();

    /// Current visual state.
    [[nodiscard]] auto state() const -> SplitterState;

    // ── Constraints ─────────────────────────────────────────────────

    [[nodiscard]] auto min_position() const -> int;
    [[nodiscard]] auto max_position() const -> int;
    [[nodiscard]] auto default_position() const -> int;

private:
    int position_{0};
    int min_position_{0};
    int max_position_{1000};
    int default_position_{500};
    int keyboard_step_{10};
    SplitterState state_{SplitterState::kIdle};
    std::vector<SplitterSnapPoint> snap_points_;

    /// Clamp a position to constraints.
    [[nodiscard]] auto clamp(int raw_position) const -> int;

    /// Apply snap point magnetism.
    [[nodiscard]] auto snap(int raw_position) const -> int;
};

} // namespace markamp::ui
