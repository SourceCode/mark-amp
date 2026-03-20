// ============================================================================
// File: src/canvas/PrecisionNudgeModel.h
// Phase 02 W14: Precision Nudge — step sizes, coordinate editing
// ============================================================================
#pragma once

#include "canvas/KeyboardCommandModel.h"

#include <string>

namespace markamp::canvas
{

/// Testable model for Precision Nudge (Phase 02 W14).
///
/// Encapsulates:
/// - Small and big step sizes
/// - Coordinate input parsing
/// - Direction mapping
class PrecisionNudgeModel
{
public:
    // ── Step Sizes ──────────────────────────────────────────────────

    void set_small_step(double pixels);
    [[nodiscard]] auto small_step() const -> double;

    void set_big_step(double pixels);
    [[nodiscard]] auto big_step() const -> double;

    /// Return step size for the given mode.
    [[nodiscard]] auto step_for(bool big) const -> double;

    // ── Direction Helpers ────────────────────────────────────────────

    /// Compute delta-x for a nudge direction.
    [[nodiscard]] auto delta_x(NudgeDirection dir, bool big) const -> double;

    /// Compute delta-y for a nudge direction.
    [[nodiscard]] auto delta_y(NudgeDirection dir, bool big) const -> double;

    /// Human-readable label for a direction.
    [[nodiscard]] static auto direction_label(NudgeDirection dir) -> std::string;

    // ── Convenience ─────────────────────────────────────────────────

    [[nodiscard]] auto has_custom_steps() const noexcept -> bool
    {
        return small_step_ != 1.0 || big_step_ != 10.0;
    }

private:
    double small_step_{1.0};
    double big_step_{10.0};
};

} // namespace markamp::canvas
