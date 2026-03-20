#pragma once

/**
 * @file SnapPreferenceModel.h
 * @brief V17 Phase 01 W10: User-level snap preference model.
 *
 * Wraps snap toggles and threshold as user preferences.
 * Complements SnapEngine (which does the actual snapping).
 * Use apply_to(SnapConfig&) to push preferences into a SnapEngine config.
 */

#include "canvas/SnapEngine.h"

#include <cstdint>

namespace markamp::canvas
{

/// Testable model for snap preferences.
class SnapPreferenceModel
{
public:
    // ── Toggle modes ────────────────────────────────────────────────

    auto set_grid_snap(bool enabled) -> void { grid_snap_ = enabled; }
    [[nodiscard]] auto grid_snap() const noexcept -> bool { return grid_snap_; }

    auto set_object_snap(bool enabled) -> void { object_snap_ = enabled; }
    [[nodiscard]] auto object_snap() const noexcept -> bool { return object_snap_; }

    auto set_angle_snap(bool enabled) -> void { angle_snap_ = enabled; }
    [[nodiscard]] auto angle_snap() const noexcept -> bool { return angle_snap_; }

    // ── Threshold ───────────────────────────────────────────────────

    auto set_threshold(double pixels) -> void { threshold_ = pixels; }
    [[nodiscard]] auto threshold() const noexcept -> double { return threshold_; }

    // ── Apply ───────────────────────────────────────────────────────

    /// Push preferences into a SnapConfig struct.
    auto apply_to(SnapConfig& config) const -> void
    {
        config.grid_enabled = grid_snap_;
        config.object_snap_enabled = object_snap_;
        config.snap_angle_enabled = angle_snap_;
        config.snap_threshold = threshold_;
    }

    // ── Convenience ─────────────────────────────────────────────────

    /// Whether any snap mode is active.
    [[nodiscard]] auto is_any_enabled() const noexcept -> bool
    {
        return grid_snap_ || object_snap_ || angle_snap_;
    }

private:
    bool grid_snap_{true};
    bool object_snap_{true};
    bool angle_snap_{false};
    double threshold_{8.0};
};

} // namespace markamp::canvas
