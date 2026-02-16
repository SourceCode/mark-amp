#pragma once

#include "canvas/CanvasTypes.h"

#include <cmath>
#include <functional>
#include <vector>

namespace markamp::canvas
{

/// A simplified representation of a board object for minimap rendering.
struct MinimapEntry
{
    AABB bounds;
    CanvasColor color{180, 180, 180, 255};
    CanvasObjectType type{CanvasObjectType::Shape};
};

/// Floating panel showing a scaled-down overview of the board with a
/// viewport indicator rectangle.  Click or drag to navigate the main canvas.
class MinimapPanel
{
public:
    using OnNavigateCallback = std::function<void(const Point2D& world_center)>;

    // ── Data setters ───────────────────────────────────────────────
    auto set_content_bounds(const AABB& bounds) -> void;
    auto set_viewport(const AABB& visible_region) -> void;
    auto set_entries(const std::vector<MinimapEntry>& entries) -> void;

    // ── Coordinate mapping (public for testability) ────────────────
    [[nodiscard]] auto world_to_minimap(const Point2D& world) const -> Point2D;
    [[nodiscard]] auto minimap_to_world(const Point2D& minimap) const -> Point2D;
    [[nodiscard]] auto compute_scale() const -> double;

    // ── Interaction ────────────────────────────────────────────────
    auto handle_click(double map_x, double map_y) -> bool;
    auto handle_drag(double map_x, double map_y) -> bool;

    // ── Navigation callback ────────────────────────────────────────
    auto set_on_navigate(OnNavigateCallback callback) -> void;

    // ── Panel geometry ─────────────────────────────────────────────
    auto set_panel_size(double width, double height) -> void;
    [[nodiscard]] auto panel_width() const -> double;
    [[nodiscard]] auto panel_height() const -> double;

    // ── Visibility ─────────────────────────────────────────────────
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    // ── Accessors ──────────────────────────────────────────────────
    [[nodiscard]] auto content_bounds() const -> const AABB&;
    [[nodiscard]] auto viewport_region() const -> const AABB&;
    [[nodiscard]] auto entries() const -> const std::vector<MinimapEntry>&;

    // ── Batch 7 (#41-42) ──────────────────────────────────────────

    /// User-adjustable minimap zoom factor.
    [[nodiscard]] auto zoom_factor() const -> double;
    auto set_zoom_factor(double factor) -> void;

    /// Highlight specific objects on the minimap.
    auto highlight_objects(const std::vector<ObjectId>& ids) -> void;
    [[nodiscard]] auto highlighted_ids() const -> const std::vector<ObjectId>&;

private:
    AABB content_bounds_{0.0, 0.0, 1000.0, 1000.0};
    AABB viewport_region_{0.0, 0.0, 500.0, 500.0};
    std::vector<MinimapEntry> entries_;
    bool visible_{true};
    double panel_width_{200.0};
    double panel_height_{150.0};
    OnNavigateCallback on_navigate_;
    bool dragging_{false};
    double zoom_factor_{1.0};
    std::vector<ObjectId> highlighted_ids_;

    static constexpr double kMarginFraction = 0.05;
};

} // namespace markamp::canvas
