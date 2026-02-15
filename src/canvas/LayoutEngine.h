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
    Circular
};

struct LayoutOptions
{
    LayoutType type{LayoutType::Grid};
    double spacing{40.0};
    int columns{0};       // 0 = auto-compute for grid
    double radius{200.0}; // For radial/circular layout
    bool animate{true};
    double animation_duration{0.5}; // Seconds
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
};

} // namespace markamp::canvas
