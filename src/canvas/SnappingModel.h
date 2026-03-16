#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Snap target type.
enum class SnapTarget : uint8_t
{
    kGrid,
    kObjectEdge,
    kObjectCenter,
    kGuide,
};

/// Grid style.
enum class GridStyle : uint8_t
{
    kDot,
    kLine,
    kCross,
};

/// Snap result from engine.
struct SnapResult
{
    double snapped_x{0.0};
    double snapped_y{0.0};
    bool did_snap_x{false};
    bool did_snap_y{false};
    SnapTarget target_x{SnapTarget::kGrid};
    SnapTarget target_y{SnapTarget::kGrid};

    // ── Round 5 Batch 5 (#47-48) ────────────────────────────────

    /// (#47) Whether any axis snapped.
    [[nodiscard]] auto did_snap() const noexcept -> bool
    {
        return did_snap_x || did_snap_y;
    }

    /// (#48) Whether both axes snapped.
    [[nodiscard]] auto snapped_both() const noexcept -> bool
    {
        return did_snap_x && did_snap_y;
    }
};

/// Testable model for Snapping, Grid & Guide Precision (Phase 49).
///
/// Encapsulates:
/// - Grid settings (spacing, style, major interval, visibility)
/// - Snap configuration (targets, tolerance)
/// - Snap computation
/// - Smart spacing hints
class SnappingModel
{
public:
    // ── Grid ────────────────────────────────────────────────────────

    void set_grid_spacing(double spacing);
    [[nodiscard]] auto grid_spacing() const -> double;

    void set_grid_style(GridStyle style);
    [[nodiscard]] auto grid_style() const -> GridStyle;

    void set_major_interval(int interval);
    [[nodiscard]] auto major_interval() const -> int;

    void set_grid_visible(bool visible);
    [[nodiscard]] auto grid_visible() const -> bool;

    // ── Snap config ─────────────────────────────────────────────────

    void set_snap_enabled(bool enabled);
    [[nodiscard]] auto snap_enabled() const -> bool;

    void set_snap_tolerance(double pixels);
    [[nodiscard]] auto snap_tolerance() const -> double;

    void enable_target(SnapTarget target);
    void disable_target(SnapTarget target);
    [[nodiscard]] auto is_target_enabled(SnapTarget target) const -> bool;

    // ── Snap computation ────────────────────────────────────────────

    /// Snap a point to the nearest grid intersection.
    [[nodiscard]] auto snap_to_grid(double x_val, double y_val) const -> SnapResult;

private:
    double grid_spacing_{20.0};
    GridStyle grid_style_{GridStyle::kDot};
    int major_interval_{5};
    bool grid_visible_{true};
    bool snap_enabled_{true};
    double snap_tolerance_{8.0};
    std::vector<SnapTarget> enabled_targets_{SnapTarget::kGrid, SnapTarget::kObjectEdge};

    // ── Round 5 Batch 5 (#49-50) ────────────────────────────────

    /// (#49) Whether grid style is dot.
    [[nodiscard]] auto is_dot_grid() const noexcept -> bool
    {
        return grid_style_ == GridStyle::kDot;
    }

    /// (#50) Whether grid style is line.
    [[nodiscard]] auto is_line_grid() const noexcept -> bool
    {
        return grid_style_ == GridStyle::kLine;
    }
};

} // namespace markamp::canvas
