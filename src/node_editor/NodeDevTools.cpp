#include "NodeDevTools.h"

#include <sstream>

namespace markamp::node_editor
{

auto NodeDevTools::graph_summary(const NodeGraph& graph) -> GraphSummary
{
    GraphSummary summary;
    summary.node_count = graph.node_count();
    summary.link_count = graph.link_count();

    // Count orphan nodes (nodes with no connections)
    auto node_ids = graph.all_node_ids();
    auto link_ids = graph.all_link_ids();

    for (auto nid : node_ids)
    {
        const auto* node = graph.find_node(nid);
        if (node == nullptr)
        {
            continue;
        }

        // Type histogram
        summary.type_histogram[node->type_name]++;

        // Check if this node has any connections
        bool has_connection = false;
        for (auto lid : link_ids)
        {
            const auto* link = graph.find_link(lid);
            if (link == nullptr)
            {
                continue;
            }

            // Check inputs
            for (const auto& sock : node->inputs)
            {
                if (sock == link->target)
                {
                    has_connection = true;
                    break;
                }
            }
            if (has_connection)
            {
                break;
            }

            // Check outputs
            for (const auto& sock : node->outputs)
            {
                if (sock == link->source)
                {
                    has_connection = true;
                    break;
                }
            }
            if (has_connection)
            {
                break;
            }
        }

        if (!has_connection)
        {
            ++summary.orphan_count;
        }
    }

    return summary;
}

void NodeDevTools::register_type_doc(NodeTypeDoc doc)
{
    type_docs_[doc.name] = std::move(doc);
}

auto NodeDevTools::document_node_type(const std::string& type_name) const -> const NodeTypeDoc*
{
    auto iter = type_docs_.find(type_name);
    if (iter == type_docs_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto NodeDevTools::documented_type_count() const -> std::size_t
{
    return type_docs_.size();
}

auto NodeDevTools::lint_graph(const NodeGraph& graph) -> std::vector<LintIssue>
{
    std::vector<LintIssue> issues;

    auto node_ids = graph.all_node_ids();
    auto link_ids = graph.all_link_ids();

    for (auto nid : node_ids)
    {
        const auto* node = graph.find_node(nid);
        if (node == nullptr)
        {
            continue;
        }

        // Check for empty label
        if (node->label.empty())
        {
            LintIssue issue;
            issue.severity = LintSeverity::kWarning;
            issue.message = "Node has empty label";
            issue.node_id = nid;
            issue.suggestion = "Set a descriptive label";
            issues.push_back(std::move(issue));
        }

        // Check for unconnected input sockets (info)
        for (const auto& input_socket : node->inputs)
        {
            bool connected = false;
            for (auto lid : link_ids)
            {
                const auto* link = graph.find_link(lid);
                if (link != nullptr && link->target == input_socket)
                {
                    connected = true;
                    break;
                }
            }
            if (!connected)
            {
                LintIssue issue;
                issue.severity = LintSeverity::kInfo;
                issue.message = "Input socket is unconnected";
                issue.node_id = nid;
                issue.suggestion = "Connect this input or remove it if unused";
                issues.push_back(std::move(issue));
            }
        }
    }

    return issues;
}

auto NodeDevTools::lint_issue_count(const NodeGraph& graph) -> std::size_t
{
    return lint_graph(graph).size();
}

auto NodeDevTools::export_schema(const NodeGraph& graph) -> std::string
{
    std::ostringstream oss;
    oss << R"({"schema":{"node_count":)" << graph.node_count() << R"(,"link_count":)"
        << graph.link_count() << "}}";
    return oss.str();
}

auto NodeDevTools::generate_playbook(const NodeGraph& graph) -> std::string
{
    std::ostringstream oss;
    oss << "# Node Graph Playbook\n\n";
    oss << "## Overview\n";
    oss << "- Nodes: " << graph.node_count() << "\n";
    oss << "- Links: " << graph.link_count() << "\n\n";

    auto summary = graph_summary(graph);
    if (!summary.type_histogram.empty())
    {
        oss << "## Node Types\n";
        for (const auto& [type, count] : summary.type_histogram)
        {
            oss << "- " << type << ": " << count << "\n";
        }
        oss << "\n";
    }

    if (summary.orphan_count > 0)
    {
        oss << "## Warnings\n";
        oss << "- Orphan nodes: " << summary.orphan_count << "\n";
    }

    return oss.str();
}

auto NodeDevTools::stats(const NodeGraph& graph) -> GraphStats
{
    GraphStats graph_stats;
    auto node_cnt = graph.node_count();
    auto link_cnt = graph.link_count();

    if (node_cnt > 1)
    {
        // Density = edges / possible edges
        auto max_edges = node_cnt * (node_cnt - 1);
        graph_stats.density = static_cast<float>(link_cnt) / static_cast<float>(max_edges);
    }

    if (node_cnt > 0)
    {
        graph_stats.avg_degree =
            (2.0F * static_cast<float>(link_cnt)) / static_cast<float>(node_cnt);
    }

    // Simple connectivity check: connected if no orphans
    auto summary = graph_summary(graph);
    graph_stats.connected = summary.orphan_count == 0 || node_cnt <= 1;

    return graph_stats;
}

void NodeDevTools::clear()
{
    type_docs_.clear();
}

} // namespace markamp::node_editor
