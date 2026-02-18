/// @file GraphExporter.h
/// @brief V9 Phase 16 – Export graph data to JSON, CSV, and Markdown formats.

#pragma once

#include "GraphTypes.h"

#include <cstdint>
#include <sstream>
#include <string>

namespace markamp::core
{

// ============================================================================
// Export Format
// ============================================================================

/// Supported graph export formats.
enum class GraphExportFormat : uint8_t
{
    kDot,     ///< Graphviz DOT (handled by GraphvizRenderer)
    kJson,    ///< JSON format
    kCsv,     ///< CSV format (nodes + edges)
    kMarkdown ///< Human-readable Markdown summary
};

// ============================================================================
// GraphExporter
// ============================================================================

/// Exports graph data to various text-based formats.
class GraphExporter
{
public:
    /// Export graph data to JSON format.
    [[nodiscard]] static auto export_to_json(const GraphData& graph) -> std::string
    {
        std::ostringstream oss;
        oss << "{\n";

        // Nodes
        oss << "  \"nodes\": [\n";
        for (size_t i = 0; i < graph.nodes.size(); ++i)
        {
            const auto& node = graph.nodes[i];
            oss << "    {"
                << "\"id\":\"" << escape_json(node.id) << "\","
                << "\"label\":\"" << escape_json(node.label) << "\","
                << "\"type\":" << static_cast<int>(node.type) << ","
                << "\"ref_count\":" << node.ref_count << ","
                << "\"notebook_id\":\"" << escape_json(node.notebook_id) << "\""
                << "}";
            if (i + 1 < graph.nodes.size())
            {
                oss << ",";
            }
            oss << "\n";
        }
        oss << "  ],\n";

        // Links
        oss << "  \"links\": [\n";
        for (size_t i = 0; i < graph.links.size(); ++i)
        {
            const auto& link = graph.links[i];
            oss << "    {"
                << "\"source\":\"" << escape_json(link.source_id) << "\","
                << "\"target\":\"" << escape_json(link.target_id) << "\","
                << "\"type\":" << static_cast<int>(link.ref_type) << ","
                << "\"label\":\"" << escape_json(link.label) << "\""
                << "}";
            if (i + 1 < graph.links.size())
            {
                oss << ",";
            }
            oss << "\n";
        }
        oss << "  ],\n";

        // Summary
        oss << "  \"summary\": {"
            << "\"node_count\":" << graph.node_count() << ","
            << "\"link_count\":" << graph.link_count() << "}\n";

        oss << "}\n";
        return oss.str();
    }

    /// Export graph data to CSV format.
    /// Format: nodes CSV, then "---", then edges CSV.
    [[nodiscard]] static auto export_to_csv(const GraphData& graph) -> std::string
    {
        std::ostringstream oss;

        // Node header
        oss << "id,label,type,ref_count,notebook_id\n";
        for (const auto& node : graph.nodes)
        {
            oss << escape_csv(node.id) << "," << escape_csv(node.label) << ","
                << static_cast<int>(node.type) << "," << node.ref_count << ","
                << escape_csv(node.notebook_id) << "\n";
        }

        oss << "---\n";

        // Edge header
        oss << "source,target,type,label\n";
        for (const auto& link : graph.links)
        {
            oss << escape_csv(link.source_id) << "," << escape_csv(link.target_id) << ","
                << static_cast<int>(link.ref_type) << "," << escape_csv(link.label) << "\n";
        }

        return oss.str();
    }

    /// Export graph data to a human-readable Markdown summary.
    [[nodiscard]] static auto export_to_markdown(const GraphData& graph) -> std::string
    {
        std::ostringstream oss;

        oss << "# Knowledge Graph Export\n\n";
        oss << "## Summary\n\n";
        oss << "- **Nodes:** " << graph.node_count() << "\n";
        oss << "- **Links:** " << graph.link_count() << "\n\n";

        // Nodes list
        oss << "## Nodes\n\n";
        oss << "| ID | Label | Type | Refs |\n";
        oss << "|---|---|---|---|\n";
        for (const auto& node : graph.nodes)
        {
            oss << "| " << node.id << " | " << node.label << " | " << node_type_name(node.type)
                << " | " << node.ref_count << " |\n";
        }

        oss << "\n## Connections\n\n";
        if (graph.links.empty())
        {
            oss << "_No connections._\n";
        }
        else
        {
            oss << "| Source | Target | Type |\n";
            oss << "|---|---|---|\n";
            for (const auto& link : graph.links)
            {
                oss << "| " << link.source_id << " | " << link.target_id << " | "
                    << ref_type_name(link.ref_type) << " |\n";
            }
        }

        return oss.str();
    }

    /// Dispatch export by format.
    [[nodiscard]] static auto export_graph(const GraphData& graph, GraphExportFormat format)
        -> std::string
    {
        switch (format)
        {
            case GraphExportFormat::kJson:
                return export_to_json(graph);
            case GraphExportFormat::kCsv:
                return export_to_csv(graph);
            case GraphExportFormat::kMarkdown:
                return export_to_markdown(graph);
            case GraphExportFormat::kDot:
                // DOT handled by GraphvizRenderer; return basic DOT here
                return export_to_dot(graph);
        }
        return {};
    }

private:
    [[nodiscard]] static auto escape_json(const std::string& str) -> std::string
    {
        std::string result;
        result.reserve(str.size());
        for (const char ch : str)
        {
            switch (ch)
            {
                case '"':
                    result += "\\\"";
                    break;
                case '\\':
                    result += "\\\\";
                    break;
                case '\n':
                    result += "\\n";
                    break;
                case '\r':
                    result += "\\r";
                    break;
                case '\t':
                    result += "\\t";
                    break;
                default:
                    result += ch;
                    break;
            }
        }
        return result;
    }

    [[nodiscard]] static auto escape_csv(const std::string& str) -> std::string
    {
        if (str.find(',') != std::string::npos || str.find('"') != std::string::npos)
        {
            std::string escaped = "\"";
            for (const char ch : str)
            {
                if (ch == '"')
                {
                    escaped += "\"\"";
                }
                else
                {
                    escaped += ch;
                }
            }
            escaped += "\"";
            return escaped;
        }
        return str;
    }

    [[nodiscard]] static auto node_type_name(GraphNodeType type) -> std::string
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
                return "CodeBlock";
            case GraphNodeType::Table:
                return "Table";
            case GraphNodeType::Blockquote:
                return "Blockquote";
            case GraphNodeType::MathBlock:
                return "MathBlock";
            case GraphNodeType::Tag:
                return "Tag";
        }
        return "Unknown";
    }

    [[nodiscard]] static auto ref_type_name(GraphRefType type) -> std::string
    {
        switch (type)
        {
            case GraphRefType::BlockRef:
                return "BlockRef";
            case GraphRefType::DocLink:
                return "DocLink";
            case GraphRefType::Embed:
                return "Embed";
            case GraphRefType::Mention:
                return "Mention";
            case GraphRefType::Tag:
                return "Tag";
        }
        return "Unknown";
    }

    [[nodiscard]] static auto export_to_dot(const GraphData& graph) -> std::string
    {
        std::ostringstream oss;
        oss << "digraph knowledge_graph {\n";
        oss << "  rankdir=LR;\n";
        oss << "  node [shape=ellipse];\n\n";

        for (const auto& node : graph.nodes)
        {
            oss << "  \"" << node.id << "\" [label=\"" << node.label << "\"];\n";
        }
        oss << "\n";
        for (const auto& link : graph.links)
        {
            oss << "  \"" << link.source_id << "\" -> \"" << link.target_id << "\";\n";
        }

        oss << "}\n";
        return oss.str();
    }
};

} // namespace markamp::core
