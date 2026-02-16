#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <vector>

namespace markamp::canvas
{

/// A freehand-drawn polyline path with configurable stroke and smoothing.
/// Captures a sequence of points from pen/mouse input and supports
/// Chaikin corner-cutting smoothing and Ramer-Douglas-Peucker simplification.
class FreehandPath : public CanvasObject
{
public:
    FreehandPath();

    // ── Points ─────────────────────────────────────────────────

    /// Append a point to the path.
    auto add_point(Point2D point) -> void;

    /// Access the point sequence.
    [[nodiscard]] auto points() const -> const std::vector<Point2D>&;

    /// Number of points in the path.
    [[nodiscard]] auto point_count() const -> size_t;

    // ── Stroke ─────────────────────────────────────────────────

    [[nodiscard]] auto stroke_color() const -> CanvasColor;
    auto set_stroke_color(CanvasColor color) -> void;

    [[nodiscard]] auto stroke_width() const -> double;
    auto set_stroke_width(double width) -> void;

    // ── Smoothing & Simplification ─────────────────────────────

    /// Smoothing factor (0.0 = none, 1.0 = maximum). Controls how many
    /// Chaikin subdivision passes are applied.
    [[nodiscard]] auto smoothing_factor() const -> double;
    auto set_smoothing_factor(double factor) -> void;

    /// Apply Chaikin corner-cutting subdivision to smooth the path.
    /// The number of passes is derived from smoothing_factor_.
    auto smooth() -> void;

    /// Ramer-Douglas-Peucker polyline simplification.
    /// Removes points that deviate less than `tolerance` from the simplified line.
    auto simplify(double tolerance) -> void;

    // ── Path Shape (#13-14) ────────────────────────────────────

    [[nodiscard]] auto is_closed() const -> bool;
    auto set_closed(bool closed) -> void;

    [[nodiscard]] auto fill_color() const -> CanvasColor;
    auto set_fill_color(CanvasColor color) -> void;

    // ── Computed & Operations (#15-18) ─────────────────────────

    [[nodiscard]] auto total_length() const -> double;
    [[nodiscard]] auto bounding_center() const -> Point2D;
    auto reverse() -> void;
    auto erase_points_in(const AABB& region) -> void;

    // ── Batch 7 (#37-39) ──────────────────────────────────────────

    /// Returns true if the path has zero points.
    [[nodiscard]] auto is_empty() const -> bool;

    /// Remove all points from the path.
    auto clear_points() -> void;

    /// Downsample the path to at most max_points, evenly spaced.
    auto subsample(size_t max_points) -> void;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

private:
    std::vector<Point2D> points_;
    CanvasColor stroke_color_{0, 0, 0, 255}; // Black
    double stroke_width_{2.0};
    double smoothing_factor_{0.5};
    bool closed_{false};
    CanvasColor fill_color_{0, 0, 0, 0};

    /// Recursive RDP helper.
    static auto rdp_simplify(const std::vector<Point2D>& points,
                             double tolerance,
                             size_t start_idx,
                             size_t end_idx,
                             std::vector<bool>& keep) -> void;

    /// Perpendicular distance from point to line segment.
    static auto perpendicular_distance(const Point2D& point,
                                       const Point2D& line_start,
                                       const Point2D& line_end) -> double;
};

} // namespace markamp::canvas
