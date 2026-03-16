#pragma once

#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace markamp::canvas
{

enum class LayoutType : uint8_t
{
    Grid,
    Radial,
    Tree,
    ForceDirected,
    Horizontal,
    Vertical,
    Circular,
    Spiral
};

struct LayoutOptions
{
    LayoutType type{LayoutType::Grid};
    double spacing{40.0};
    int columns{0};       // 0 = auto-compute for grid
    double radius{200.0}; // For radial/circular layout
    bool animate{true};
    double animation_duration{0.5}; // Seconds
    double padding{0.0};            // Extra padding around each object
    Point2D center{0.0, 0.0};       // Layout center point

    /// Whether layout animation is enabled.
    [[nodiscard]] auto is_animated() const noexcept -> bool
    {
        return animate;
    }

    /// Whether columns are auto-computed (grid layout).
    [[nodiscard]] auto is_auto_columns() const noexcept -> bool
    {
        return columns == 0;
    }

    /// Whether this is a grid layout.
    [[nodiscard]] auto is_grid() const noexcept -> bool
    {
        return type == LayoutType::Grid;
    }
};

struct LayoutResult
{
    ObjectId id;
    Point2D new_position;
};

/// Stateless engine that computes new positions for canvas objects
/// according to various layout algorithms.
class LayoutEngine
{
public:
    /// Compute new positions for the given objects using the specified layout.
    [[nodiscard]] auto compute_layout(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                      const LayoutOptions& options) const
        -> std::vector<LayoutResult>;

    // ── Batch 8 (#46-48) ──────────────────────────────────────────

    /// Return a display-friendly name for each LayoutType.
    [[nodiscard]] static auto layout_type_name(LayoutType type) -> std::string;

    /// Return a vector of all supported layout types.
    [[nodiscard]] static auto available_layouts() -> std::vector<LayoutType>;

    /// Compact layout that minimizes whitespace between objects.
    [[nodiscard]] auto compute_compact_layout(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                              double padding = 5.0) const
        -> std::vector<LayoutResult>;

    // ── Batch 16-18 (#113-115) ────────────────────────────────────

    /// (#113) Return the number of available layout types.
    [[nodiscard]] static auto layout_count() -> std::size_t;

    /// (#114) Check if a LayoutType value is valid.
    [[nodiscard]] static auto is_valid_type(LayoutType type) -> bool;

    /// (#115) Return default layout options.
    [[nodiscard]] static auto default_options() -> LayoutOptions;

private:
    [[nodiscard]] auto layout_grid(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                   const LayoutOptions& options) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_radial(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                     const LayoutOptions& options) const
        -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_tree(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                   const LayoutOptions& options) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_force_directed(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                             const LayoutOptions& options) const
        -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_linear(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                     bool horizontal,
                                     double spacing) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_circular(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                       double radius) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_spiral(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                     const LayoutOptions& options) const
        -> std::vector<LayoutResult>;
};

} // namespace markamp::canvas
