#pragma once

#include "canvas/CanvasTypes.h"
#include "canvas/ViewportTransform.h"

#include <vector>

namespace markamp::canvas
{

/// Configuration for snap behavior.
struct SnapConfig
{
    bool grid_enabled{true};
    double grid_spacing{20.0};
    double snap_threshold{8.0}; // Pixels in screen space
    bool object_snap_enabled{true};
    CanvasColor guide_color{255, 0, 200, 180}; // Magenta
    bool snap_angle_enabled{false};            // 15° angle snapping for connectors
};

/// Type of alignment guide line.
enum class GuideLineType : uint8_t
{
    kCenterH,
    kCenterV,
    kEdgeTop,
    kEdgeBottom,
    kEdgeLeft,
    kEdgeRight
};

/// A guide line segment for visual feedback.
struct GuideLine
{
    Point2D start;
    Point2D end;
    GuideLineType type{GuideLineType::kCenterH};
};

/// Result of a snap operation.
struct SnapResult
{
    double snapped_x{0.0};
    double snapped_y{0.0};
    bool snapped{false};
    std::vector<GuideLine> guide_lines;

    /// Number of guide lines produced by the snap.
    [[nodiscard]] auto guide_count() const noexcept -> std::size_t
    {
        return guide_lines.size();
    }
};

/// Provides snap-to-grid and snap-to-object alignment during drag/resize.
class SnapEngine
{
public:
    SnapEngine() = default;

    // ── Grid Snap ──────────────────────────────────────────────

    /// Snap a single point to the nearest grid intersection.
    [[nodiscard]] auto snap_to_grid(const Point2D& point) const -> Point2D;

    // ── Object Snap ────────────────────────────────────────────

    /// Snap a moving bounding box against other object bounding boxes.
    /// Returns the snapped position (top-left of the moving AABB) and guide lines.
    [[nodiscard]] auto snap_to_objects(const AABB& moving, const std::vector<AABB>& others) const
        -> SnapResult;

    // ── Combined Snap ──────────────────────────────────────────

    /// Snap a proposed point considering both grid and object snapping.
    /// `moving_bounds` is the AABB of the object being dragged (at the proposed position).
    /// `other_bounds` are the AABBs of all other objects on the board.
    [[nodiscard]] auto snap_point(const Point2D& proposed,
                                  const AABB& moving_bounds,
                                  const std::vector<AABB>& other_bounds,
                                  const ViewportTransform& viewport) const -> SnapResult;

    // ── Configuration ──────────────────────────────────────────

    [[nodiscard]] auto config() const -> const SnapConfig&;
    auto set_config(const SnapConfig& cfg) -> void;

    /// Snap an angle (radians) to the nearest 15° increment.
    [[nodiscard]] auto snap_angle(double angle_radians) const -> double;

    // ── Batch 4 (#23-24) ──────────────────────────────────────────

    /// Snap a point to the center of a given reference AABB (object center snap).
    [[nodiscard]] auto snap_to_center(const Point2D& point,
                                      const AABB& reference,
                                      double threshold = 8.0) const -> Point2D;

    /// User-defined custom guide lines (in world coordinates).
    [[nodiscard]] auto custom_guides() const -> const std::vector<GuideLine>&;
    auto add_custom_guide(const GuideLine& guide) -> void;
    auto clear_custom_guides() -> void;

    /// (#88) Check if any snap mode is currently active.
    [[nodiscard]] auto is_enabled() const -> bool;

    /// Number of user-defined custom guide lines.
    [[nodiscard]] auto custom_guide_count() const noexcept -> std::size_t
    {
        return custom_guides_.size();
    }

    /// Current grid spacing value.
    [[nodiscard]] auto grid_spacing() const noexcept -> double
    {
        return config_.grid_spacing;
    }

    /// Current snap threshold in screen pixels.
    [[nodiscard]] auto snap_threshold() const noexcept -> double
    {
        return config_.snap_threshold;
    }

    // ── Batch 5 (#41-45) ──────────────────────────────────────────

    /// (#41) Whether grid snapping is active.
    [[nodiscard]] auto is_grid_enabled() const noexcept -> bool
    {
        return config_.grid_enabled;
    }

    /// (#42) Whether object-to-object snapping is active.
    [[nodiscard]] auto is_object_snap_enabled() const noexcept -> bool
    {
        return config_.object_snap_enabled;
    }

    /// (#43) Whether angle snapping is active.
    [[nodiscard]] auto is_angle_snap_enabled() const noexcept -> bool
    {
        return config_.snap_angle_enabled;
    }

    /// (#44) Current guide line color.
    [[nodiscard]] auto guide_color() const noexcept -> const CanvasColor&
    {
        return config_.guide_color;
    }

    /// (#45) Update grid spacing.
    auto set_grid_spacing(double spacing) -> void
    {
        config_.grid_spacing = spacing;
    }

private:
    SnapConfig config_;
    std::vector<GuideLine> custom_guides_;
};

} // namespace markamp::canvas
