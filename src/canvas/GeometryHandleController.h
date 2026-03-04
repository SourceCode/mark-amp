#pragma once

/**
 * @file GeometryHandleController.h
 * @brief Phase 43 Task 2-3: Geometry handles and constrained resize.
 *
 * Handle types (corner/edge/rotation/radius), constrained resize
 * (Shift=proportional, Alt=center-anchored), snap-aware movement.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Handle type for geometry editing.
enum class HandleType : uint8_t
{
    kCorner,
    kEdge,
    kRotation,
    kRadius,
};

/// Handle position identifier.
enum class HandlePosition : uint8_t
{
    kTopLeft,
    kTopRight,
    kBottomLeft,
    kBottomRight,
    kTop,
    kBottom,
    kLeft,
    kRight,
    kRotate,
    kRadiusControl,
};

/// A geometry handle.
struct GeometryHandle
{
    HandleType type{HandleType::kCorner};
    HandlePosition position{HandlePosition::kTopLeft};
    double pos_x{0.0};
    double pos_y{0.0};

    /// Get type as string.
    [[nodiscard]] auto type_name() const -> std::string;

    /// Get position as string.
    [[nodiscard]] auto position_name() const -> std::string;
};

/// Constraint mode for resize.
enum class ConstraintMode : uint8_t
{
    kNone,
    kProportional,   ///< Shift: maintain aspect ratio
    kCenterAnchored, ///< Alt: resize from center
    kBoth,           ///< Shift+Alt
};

/**
 * @brief Manages geometry handles for shape editing.
 */
class GeometryHandleController
{
public:
    GeometryHandleController() = default;

    // ── Handle generation ──────────────────────────────────────────

    /// Generate handles for a bounding box.
    void generate_handles(double pos_x, double pos_y, double width, double height);

    /// Get handle count.
    [[nodiscard]] auto handle_count() const -> int;

    /// Get all handles.
    [[nodiscard]] auto handles() const -> const std::vector<GeometryHandle>&;

    /// Find handle near a point (within threshold).
    [[nodiscard]] auto hit_handle(double pos_x, double pos_y, double threshold) const
        -> const GeometryHandle*;

    // ── Constrained resize ─────────────────────────────────────────

    /// Set the constraint mode.
    void set_constraint(ConstraintMode mode);

    /// Get the constraint mode.
    [[nodiscard]] auto constraint() const -> ConstraintMode;

    /// Apply constrained resize. Returns new bounds (x, y, w, h).
    struct Bounds
    {
        double pos_x{0.0};
        double pos_y{0.0};
        double width{0.0};
        double height{0.0};
    };

    [[nodiscard]] auto constrained_resize(HandlePosition handle,
                                          double delta_x,
                                          double delta_y,
                                          const Bounds& original) const -> Bounds;

    // ── Snap ───────────────────────────────────────────────────────

    /// Set snap grid size (0 = no snap).
    void set_snap_grid(double grid_size);

    /// Get snap grid size.
    [[nodiscard]] auto snap_grid() const -> double;

    /// Snap a value to grid.
    [[nodiscard]] auto snap_value(double value) const -> double;

private:
    std::vector<GeometryHandle> handles_;
    ConstraintMode constraint_{ConstraintMode::kNone};
    double snap_grid_{0.0};
};

} // namespace markamp::canvas
