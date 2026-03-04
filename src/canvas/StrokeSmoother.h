#pragma once

/**
 * @file StrokeSmoother.h
 * @brief Phase 42 Task 1-2: Speed-adaptive stroke smoothing with pressure.
 *
 * StrokePoint with x/y/pressure/timestamp, speed-adaptive smoothing
 * (slow=precise, fast=smooth), point reduction, and pressure-aware width.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A point in a stroke with pressure and timestamp.
struct StrokePoint
{
    double pos_x{0.0};
    double pos_y{0.0};
    double pressure{1.0}; ///< 0.0-1.0, 1.0 = max pressure
    int timestamp_ms{0};

    /// Distance to another point.
    [[nodiscard]] auto distance_to(const StrokePoint& other) const -> double;

    /// Speed between this and another point (pixels/ms).
    [[nodiscard]] auto speed_to(const StrokePoint& other) const -> double;
};

/**
 * @brief Speed-adaptive stroke smoothing and point reduction.
 */
class StrokeSmoother
{
public:
    StrokeSmoother() = default;

    // ── Input ──────────────────────────────────────────────────────

    /// Add a raw point.
    void add_point(const StrokePoint& point);

    /// Get raw point count.
    [[nodiscard]] auto raw_count() const -> int;

    /// Get raw points.
    [[nodiscard]] auto raw_points() const -> const std::vector<StrokePoint>&;

    // ── Smoothing ──────────────────────────────────────────────────

    /// Smooth the current stroke. Returns smoothed points.
    [[nodiscard]] auto smooth() const -> std::vector<StrokePoint>;

    /// Set smoothing factor (0.0=none, 1.0=max).
    void set_smoothing_factor(double factor);

    /// Get smoothing factor.
    [[nodiscard]] auto smoothing_factor() const -> double;

    // ── Point reduction ────────────────────────────────────────────

    /// Reduce points while preserving shape. Returns reduced set.
    [[nodiscard]] auto reduce(double tolerance) const -> std::vector<StrokePoint>;

    // ── Pressure ───────────────────────────────────────────────────

    /// Calculate width at a point based on pressure and base width.
    [[nodiscard]] static auto pressure_width(double pressure, double base_width) -> double;

    // ── Speed threshold ────────────────────────────────────────────

    /// Set speed threshold for adaptive smoothing (px/ms).
    void set_speed_threshold(double threshold);

    /// Get speed threshold.
    [[nodiscard]] auto speed_threshold() const -> double;

    /// Clear all points.
    void clear();

private:
    std::vector<StrokePoint> points_;
    double smoothing_factor_{0.5};
    double speed_threshold_{2.0}; ///< px/ms: above this = fast
};

} // namespace markamp::canvas
