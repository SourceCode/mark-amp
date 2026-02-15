// ============================================================================
// File: src/core/MindmapRenderer.h
// Phase 30: Diagram Rendering Extended — Mindmap renderer (native C++)
// ============================================================================
#pragma once

#include "IDiagramRenderer.h"

#include <string>

namespace markamp::core
{

/// Renders mindmap diagrams natively in C++ as SVG.
/// Uses indentation-based syntax: each level of indentation
/// represents a child node.
class MindmapRenderer : public IDiagramRenderer
{
public:
    MindmapRenderer() = default;

    // IDiagramRenderer interface
    [[nodiscard]] auto render(const std::string& source) const
        -> std::expected<DiagramResult, std::string> override;
    [[nodiscard]] auto is_available() const -> bool override;
    [[nodiscard]] auto language_id() const -> std::string override;
    [[nodiscard]] auto display_name() const -> std::string override;

private:
    /// Internal node for tree layout.
    struct MindmapNode
    {
        std::string label;
        double pos_x{0.0};
        double pos_y{0.0};
        double width{0.0};
        double height{30.0};
        int depth{0};
        std::vector<int> children_indices;
    };

    /// Parse indentation-based source into a tree.
    [[nodiscard]] auto parse_tree(const std::string& source) const -> std::vector<MindmapNode>;

    /// Layout the tree for SVG rendering.
    void layout_tree(std::vector<MindmapNode>& nodes) const;

    /// Render the tree as SVG.
    [[nodiscard]] auto render_svg(const std::vector<MindmapNode>& nodes) const -> std::string;
};

} // namespace markamp::core
