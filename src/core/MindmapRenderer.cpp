// ============================================================================
// File: src/core/MindmapRenderer.cpp
// Phase 30: Diagram Rendering Extended — Mindmap renderer implementation
// ============================================================================
#include "MindmapRenderer.h"

#include <chrono>
#include <cmath>
#include <sstream>
#include <stack>

namespace markamp::core
{

auto MindmapRenderer::render(const std::string& source) const
    -> std::expected<DiagramResult, std::string>
{
    const auto start = std::chrono::steady_clock::now();

    auto nodes = parse_tree(source);
    if (nodes.empty())
    {
        return std::unexpected("Empty mindmap source");
    }

    layout_tree(nodes);

    DiagramResult result;
    result.svg = render_svg(nodes);

    const auto end = std::chrono::steady_clock::now();
    result.render_time_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    return result;
}

auto MindmapRenderer::is_available() const -> bool
{
    return true; // Native renderer, always available
}

auto MindmapRenderer::language_id() const -> std::string
{
    return "mindmap";
}

auto MindmapRenderer::display_name() const -> std::string
{
    return "Mindmap";
}

auto MindmapRenderer::parse_tree(const std::string& source) const -> std::vector<MindmapNode>
{
    std::vector<MindmapNode> nodes;
    std::istringstream stream(source);
    std::string line;

    // Stack of (depth, node_index) for parent tracking
    std::stack<std::pair<int, int>> parent_stack;

    while (std::getline(stream, line))
    {
        if (line.empty())
        {
            continue;
        }

        // Calculate indentation depth (tabs or 2 spaces = 1 level)
        int depth = 0;
        std::size_t pos = 0;
        while (pos < line.size())
        {
            if (line[pos] == '\t')
            {
                ++depth;
                ++pos;
            }
            else if (pos + 1 < line.size() && line[pos] == ' ' && line[pos + 1] == ' ')
            {
                ++depth;
                pos += 2;
            }
            else
            {
                break;
            }
        }

        const auto label = line.substr(pos);
        if (label.empty())
        {
            continue;
        }

        MindmapNode node;
        node.label = label;
        node.depth = depth;
        node.width = std::max(80.0, static_cast<double>(label.size()) * 8.0);

        const int node_idx = static_cast<int>(nodes.size());

        // Pop stack until parent is at depth - 1
        while (!parent_stack.empty() && parent_stack.top().first >= depth)
        {
            parent_stack.pop();
        }

        // Link to parent
        if (!parent_stack.empty())
        {
            nodes[parent_stack.top().second].children_indices.push_back(node_idx);
        }

        parent_stack.push({depth, node_idx});
        nodes.push_back(std::move(node));
    }

    return nodes;
}

void MindmapRenderer::layout_tree(std::vector<MindmapNode>& nodes) const
{
    if (nodes.empty())
    {
        return;
    }

    constexpr double kHorizontalSpacing = 180.0;
    constexpr double kVerticalSpacing = 50.0;

    double next_y = 20.0;

    // Simple layout: position by depth (x) and sequential order (y)
    for (auto& node : nodes)
    {
        node.pos_x = 20.0 + static_cast<double>(node.depth) * kHorizontalSpacing;
        node.pos_y = next_y;
        next_y += kVerticalSpacing;
    }
}

auto MindmapRenderer::render_svg(const std::vector<MindmapNode>& nodes) const -> std::string
{
    if (nodes.empty())
    {
        return {};
    }

    // Calculate SVG dimensions
    double max_x = 0.0;
    double max_y = 0.0;
    for (const auto& node : nodes)
    {
        max_x = std::max(max_x, node.pos_x + node.width + 20.0);
        max_y = std::max(max_y, node.pos_y + node.height + 20.0);
    }

    std::ostringstream svg;
    svg << R"(<svg xmlns="http://www.w3.org/2000/svg" width=")" << static_cast<int>(max_x)
        << R"(" height=")" << static_cast<int>(max_y) << R"(">)"
        << "\n";

    // Draw edges first
    for (const auto& node : nodes)
    {
        for (int child_idx : node.children_indices)
        {
            const auto& child = nodes[child_idx];
            svg << R"(  <line x1=")" << (node.pos_x + node.width / 2.0) << R"(" y1=")"
                << (node.pos_y + node.height) << R"(" x2=")" << (child.pos_x + child.width / 2.0)
                << R"(" y2=")" << child.pos_y << R"(" stroke="#666" stroke-width="1.5"/>)"
                << "\n";
        }
    }

    // Draw nodes
    for (const auto& node : nodes)
    {
        const auto rx = 8.0;
        svg << R"(  <rect x=")" << node.pos_x << R"(" y=")" << node.pos_y << R"(" width=")"
            << node.width << R"(" height=")" << node.height << R"(" rx=")" << rx
            << R"(" fill="#e8f0fe" stroke="#4285f4" stroke-width="1.5"/>)"
            << "\n";

        svg << R"(  <text x=")" << (node.pos_x + node.width / 2.0) << R"(" y=")"
            << (node.pos_y + node.height / 2.0 + 4.0)
            << R"(" text-anchor="middle" font-family="sans-serif" font-size="12">)" << node.label
            << "</text>\n";
    }

    svg << "</svg>\n";
    return svg.str();
}

} // namespace markamp::core
