/// @file GraphAccessibility.h
/// @brief V9 Phase 16 – Accessibility metadata and screen reader text for graph views.

#pragma once

#include "GraphAnalytics.h"
#include "GraphTypes.h"

#include <sstream>
#include <string>

namespace markamp::core
{

// ============================================================================
// Accessibility Info
// ============================================================================

/// Accessibility role and description for a graph UI element.
struct GraphAccessibilityInfo
{
    std::string role;        ///< ARIA role (e.g., "img", "list", "button")
    std::string label;       ///< Accessible name
    std::string description; ///< Accessible description
    std::string live_region; ///< ARIA live region type ("polite", "assertive", "")
};

// ============================================================================
// GraphAccessibility
// ============================================================================

/// Generates accessibility text for graph components.
class GraphAccessibility
{
public:
    /// Generate a screen reader summary of graph statistics.
    [[nodiscard]] static auto announce_graph_summary(const GraphStatistics& stats) -> std::string
    {
        std::ostringstream oss;
        oss << "Knowledge graph with " << stats.node_count << " nodes and " << stats.edge_count
            << " connections. ";
        oss << stats.connected_components << " connected component"
            << (stats.connected_components != 1 ? "s" : "") << ". ";
        if (stats.isolated_count > 0)
        {
            oss << stats.isolated_count << " isolated node"
                << (stats.isolated_count != 1 ? "s" : "") << ". ";
        }
        return oss.str();
    }

    /// Generate a screen reader description of a node.
    [[nodiscard]] static auto announce_node_info(const GraphNode& node) -> std::string
    {
        std::ostringstream oss;
        oss << node_type_label(node.type) << " \"" << node.label << "\"";
        if (node.ref_count > 0)
        {
            oss << " with " << node.ref_count << " reference" << (node.ref_count != 1 ? "s" : "");
        }
        if (node.ref_count == 0)
        {
            oss << ", orphan";
        }
        oss << ".";
        return oss.str();
    }

    /// Generate a screen reader announcement for navigation between nodes.
    [[nodiscard]] static auto announce_navigation(const std::string& from_label,
                                                  const std::string& to_label) -> std::string
    {
        std::ostringstream oss;
        oss << "Navigated from \"" << from_label << "\" to \"" << to_label << "\".";
        return oss.str();
    }

    /// Get accessibility info for the graph view container.
    [[nodiscard]] static auto get_graph_view_info() -> GraphAccessibilityInfo
    {
        GraphAccessibilityInfo info;
        info.role = "img";
        info.label = "Knowledge Graph Visualization";
        info.description =
            "Interactive graph showing connections between documents, tags, and references. "
            "Use arrow keys to navigate nodes, Enter to select, Escape to deselect.";
        info.live_region = "polite";
        return info;
    }

    /// Get accessibility info for the graph filter bar.
    [[nodiscard]] static auto get_filter_bar_info() -> GraphAccessibilityInfo
    {
        GraphAccessibilityInfo info;
        info.role = "toolbar";
        info.label = "Graph Filters";
        info.description = "Filter controls for the knowledge graph visualization.";
        info.live_region = "";
        return info;
    }

    /// Get accessibility info for the minimap.
    [[nodiscard]] static auto get_minimap_info() -> GraphAccessibilityInfo
    {
        GraphAccessibilityInfo info;
        info.role = "img";
        info.label = "Graph Minimap";
        info.description = "Overview of the full graph with viewport indicator.";
        info.live_region = "";
        return info;
    }

    /// Announce search results count.
    [[nodiscard]] static auto announce_search_results(const std::string& query, int result_count)
        -> std::string
    {
        std::ostringstream oss;
        oss << result_count << " result" << (result_count != 1 ? "s" : "") << " found for \""
            << query << "\".";
        return oss.str();
    }

private:
    [[nodiscard]] static auto node_type_label(GraphNodeType type) -> std::string
    {
        switch (type)
        {
            case GraphNodeType::Document:
                return "Document";
            case GraphNodeType::Heading:
                return "Heading";
            case GraphNodeType::Paragraph:
                return "Paragraph";
            case GraphNodeType::List:
                return "List";
            case GraphNodeType::CodeBlock:
                return "Code block";
            case GraphNodeType::Table:
                return "Table";
            case GraphNodeType::Blockquote:
                return "Blockquote";
            case GraphNodeType::MathBlock:
                return "Math block";
            case GraphNodeType::Tag:
                return "Tag";
        }
        return "Node";
    }
};

} // namespace markamp::core
