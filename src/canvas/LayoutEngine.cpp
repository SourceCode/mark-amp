#include "LayoutEngine.h"

#include <algorithm>
#include <cmath>

namespace markamp::canvas
{

auto LayoutEngine::compute_layout(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                  const LayoutOptions& options) const -> std::vector<LayoutResult>
{
    if (objects.empty())
    {
        return {};
    }

    switch (options.type)
    {
        case LayoutType::Grid:
            return layout_grid(objects, options);
        case LayoutType::Radial:
            return layout_radial(objects, options);
        case LayoutType::Tree:
            return layout_tree(objects, options);
        case LayoutType::ForceDirected:
            return layout_force_directed(objects, options);
        case LayoutType::Horizontal:
            return layout_linear(objects, true, options.spacing);
        case LayoutType::Vertical:
            return layout_linear(objects, false, options.spacing);
        case LayoutType::Circular:
            return layout_circular(objects, options.radius);
        case LayoutType::Spiral:
            return layout_spiral(objects, options);
    }
    return {};
}

auto LayoutEngine::layout_grid(const std::vector<std::pair<ObjectId, AABB>>& objects,
                               const LayoutOptions& options) const -> std::vector<LayoutResult>
{
    const auto count = static_cast<int>(objects.size());
    const int cols =
        options.columns > 0
            ? options.columns
            : std::max(1, static_cast<int>(std::ceil(std::sqrt(static_cast<double>(count)))));

    // Determine max cell size for uniform grid.
    double max_w = 0.0;
    double max_h = 0.0;
    for (const auto& [id, bounds] : objects)
    {
        max_w = std::max(max_w, bounds.max_x - bounds.min_x);
        max_h = std::max(max_h, bounds.max_y - bounds.min_y);
    }

    const double cell_w = max_w + options.spacing;
    const double cell_h = max_h + options.spacing;

    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    for (int idx = 0; idx < count; ++idx)
    {
        const int row = idx / cols;
        const int col = idx % cols;
        results.push_back({objects[static_cast<size_t>(idx)].first, {col * cell_w, row * cell_h}});
    }
    return results;
}

auto LayoutEngine::layout_radial(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                 const LayoutOptions& options) const -> std::vector<LayoutResult>
{
    // Center = centroid of current object positions.
    double cx = 0.0;
    double cy = 0.0;
    for (const auto& [id, bounds] : objects)
    {
        cx += (bounds.min_x + bounds.max_x) / 2.0;
        cy += (bounds.min_y + bounds.max_y) / 2.0;
    }
    cx /= static_cast<double>(objects.size());
    cy /= static_cast<double>(objects.size());

    const double angle_step = 2.0 * M_PI / static_cast<double>(objects.size());

    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    for (size_t idx = 0; idx < objects.size(); ++idx)
    {
        const double angle = angle_step * static_cast<double>(idx);
        results.push_back(
            {objects[idx].first,
             {cx + options.radius * std::cos(angle), cy + options.radius * std::sin(angle)}});
    }
    return results;
}

auto LayoutEngine::layout_tree(const std::vector<std::pair<ObjectId, AABB>>& objects,
                               const LayoutOptions& options) const -> std::vector<LayoutResult>
{
    // Simple top-down tree: first object is root, children spread horizontally.
    if (objects.empty())
    {
        return {};
    }

    double max_w = 0.0;
    double max_h = 0.0;
    for (const auto& [id, bounds] : objects)
    {
        max_w = std::max(max_w, bounds.max_x - bounds.min_x);
        max_h = std::max(max_h, bounds.max_y - bounds.min_y);
    }

    const double h_spacing = max_w + options.spacing;
    const double v_spacing = max_h + options.spacing * 2.0;

    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    // Root at top center.
    const double total_width = static_cast<double>(objects.size() - 1) * h_spacing;
    results.push_back({objects[0].first, {total_width / 2.0, 0.0}});

    // Children on the next row, spread horizontally.
    for (size_t idx = 1; idx < objects.size(); ++idx)
    {
        const double pos_x = static_cast<double>(idx - 1) * h_spacing;
        results.push_back({objects[idx].first, {pos_x, v_spacing}});
    }
    return results;
}

auto LayoutEngine::layout_force_directed(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                         const LayoutOptions& options) const
    -> std::vector<LayoutResult>
{
    // Simple force-directed simulation: repulsion between all pairs.
    struct NodePos
    {
        double pos_x;
        double pos_y;
    };
    std::vector<NodePos> positions(objects.size());

    // Initialize positions in a circle.
    const double init_radius = options.radius;
    for (size_t idx = 0; idx < objects.size(); ++idx)
    {
        const double angle =
            2.0 * M_PI * static_cast<double>(idx) / static_cast<double>(objects.size());
        positions[idx] = {init_radius * std::cos(angle), init_radius * std::sin(angle)};
    }

    // Run iterations.
    constexpr int kIterations = 50;
    constexpr double kRepulsion = 5000.0;
    constexpr double kDamping = 0.9;

    for (int iter = 0; iter < kIterations; ++iter)
    {
        std::vector<NodePos> forces(objects.size(), {0.0, 0.0});

        // Repulsion between all pairs.
        for (size_t i = 0; i < objects.size(); ++i)
        {
            for (size_t j = i + 1; j < objects.size(); ++j)
            {
                const double dx = positions[i].pos_x - positions[j].pos_x;
                const double dy = positions[i].pos_y - positions[j].pos_y;
                const double dist_sq = dx * dx + dy * dy + 1.0;
                const double force = kRepulsion / dist_sq;
                const double dist = std::sqrt(dist_sq);
                const double fx = force * dx / dist;
                const double fy = force * dy / dist;
                forces[i].pos_x += fx;
                forces[i].pos_y += fy;
                forces[j].pos_x -= fx;
                forces[j].pos_y -= fy;
            }
        }

        // Apply forces with damping.
        const double damping = kDamping * (1.0 - static_cast<double>(iter) / kIterations);
        for (size_t idx = 0; idx < objects.size(); ++idx)
        {
            positions[idx].pos_x += forces[idx].pos_x * damping;
            positions[idx].pos_y += forces[idx].pos_y * damping;
        }
    }

    std::vector<LayoutResult> results;
    results.reserve(objects.size());
    for (size_t idx = 0; idx < objects.size(); ++idx)
    {
        results.push_back({objects[idx].first, {positions[idx].pos_x, positions[idx].pos_y}});
    }
    return results;
}

auto LayoutEngine::layout_linear(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                 bool horizontal,
                                 double spacing) const -> std::vector<LayoutResult>
{
    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    double offset = 0.0;
    for (const auto& [id, bounds] : objects)
    {
        if (horizontal)
        {
            results.push_back({id, {offset, 0.0}});
            offset += (bounds.max_x - bounds.min_x) + spacing;
        }
        else
        {
            results.push_back({id, {0.0, offset}});
            offset += (bounds.max_y - bounds.min_y) + spacing;
        }
    }
    return results;
}

auto LayoutEngine::layout_circular(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                   double radius) const -> std::vector<LayoutResult>
{
    const double angle_step = 2.0 * M_PI / static_cast<double>(objects.size());

    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    for (size_t idx = 0; idx < objects.size(); ++idx)
    {
        const double angle = angle_step * static_cast<double>(idx);
        results.push_back(
            {objects[idx].first, {radius * std::cos(angle), radius * std::sin(angle)}});
    }
    return results;
}

auto LayoutEngine::layout_spiral(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                 const LayoutOptions& options) const -> std::vector<LayoutResult>
{
    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    // Archimedean spiral: r = a + b*θ
    constexpr double kTurnsPerObject = 0.4;
    const double arm_spacing = options.spacing + options.padding;

    for (size_t idx = 0; idx < objects.size(); ++idx)
    {
        const double angle = 2.0 * M_PI * kTurnsPerObject * static_cast<double>(idx);
        const double radius = arm_spacing * angle / (2.0 * M_PI);
        results.push_back({objects[idx].first,
                           {options.center.x + radius * std::cos(angle),
                            options.center.y + radius * std::sin(angle)}});
    }
    return results;
}

// --- Batch 8 (#46-48) ---

auto LayoutEngine::layout_type_name(LayoutType type) -> std::string
{
    switch (type)
    {
        case LayoutType::Grid:
            return "Grid";
        case LayoutType::Radial:
            return "Radial";
        case LayoutType::Tree:
            return "Tree";
        case LayoutType::ForceDirected:
            return "Force Directed";
        case LayoutType::Horizontal:
            return "Horizontal";
        case LayoutType::Vertical:
            return "Vertical";
        case LayoutType::Circular:
            return "Circular";
        case LayoutType::Spiral:
            return "Spiral";
    }
    return "Unknown";
}

auto LayoutEngine::available_layouts() -> std::vector<LayoutType>
{
    return {LayoutType::Grid,
            LayoutType::Radial,
            LayoutType::Tree,
            LayoutType::ForceDirected,
            LayoutType::Horizontal,
            LayoutType::Vertical,
            LayoutType::Circular,
            LayoutType::Spiral};
}

auto LayoutEngine::compute_compact_layout(const std::vector<std::pair<ObjectId, AABB>>& objects,
                                          double padding) const -> std::vector<LayoutResult>
{
    // Simple row-packing: place objects left-to-right, wrapping when
    // the row exceeds a adaptive width (sqrt(total area) * 2).
    double total_area = 0.0;
    for (const auto& obj_pair : objects)
    {
        const auto& bounds = obj_pair.second;
        total_area += (bounds.max_x - bounds.min_x) * (bounds.max_y - bounds.min_y);
    }

    const double row_width = std::max(200.0, std::sqrt(total_area) * 2.0);
    std::vector<LayoutResult> results;
    results.reserve(objects.size());

    double cursor_x = 0.0;
    double cursor_y = 0.0;
    double row_height = 0.0;

    for (const auto& obj_pair : objects)
    {
        const auto& bounds = obj_pair.second;
        const double obj_width = bounds.max_x - bounds.min_x;
        const double obj_height = bounds.max_y - bounds.min_y;

        if (cursor_x + obj_width > row_width && cursor_x > 0.0)
        {
            cursor_x = 0.0;
            cursor_y += row_height + padding;
            row_height = 0.0;
        }

        results.push_back({obj_pair.first, {cursor_x, cursor_y}});
        cursor_x += obj_width + padding;
        row_height = std::max(row_height, obj_height);
    }

    return results;
}

} // namespace markamp::canvas
