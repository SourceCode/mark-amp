#pragma once

// V11 Phase 48: Documentation Agent Playbooks And Developer Tooling
// Developer tools: introspection and documentation utilities for node graphs.

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::node_editor
{

/// Severity level for lint issues.
enum class LintSeverity : uint8_t
{
    kInfo,
    kWarning,
    kError
};

/// A lint issue found in a graph.
struct LintIssue
{
    LintSeverity severity{LintSeverity::kWarning};
    std::string message;
    NodeId node_id;
    std::string suggestion;
};

/// Summary of a graph.
struct GraphSummary
{
    std::size_t node_count{0};
    std::size_t link_count{0};
    std::unordered_map<std::string, std::size_t> type_histogram;
    std::size_t orphan_count{0};
    std::size_t max_depth{0};
};

/// Statistics for a graph.
struct GraphStats
{
    float density{0.0F};
    float avg_degree{0.0F};
    bool connected{true};
};

/// Node type documentation entry.
struct NodeTypeDoc
{
    std::string name;
    std::string description;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<std::string> properties;
};

/// Developer tools for node graph introspection and documentation.
class NodeDevTools
{
public:
    NodeDevTools() = default;

    // --- Graph summary ---
    [[nodiscard]] static auto graph_summary(const NodeGraph& graph) -> GraphSummary;

    // --- Node type documentation ---
    void register_type_doc(NodeTypeDoc doc);
    [[nodiscard]] auto document_node_type(const std::string& type_name) const -> const NodeTypeDoc*;
    [[nodiscard]] auto documented_type_count() const -> std::size_t;

    // --- Lint ---
    [[nodiscard]] static auto lint_graph(const NodeGraph& graph) -> std::vector<LintIssue>;
    [[nodiscard]] static auto lint_issue_count(const NodeGraph& graph) -> std::size_t;

    // --- Schema export ---
    [[nodiscard]] static auto export_schema(const NodeGraph& graph) -> std::string;

    // --- Playbook generation ---
    [[nodiscard]] static auto generate_playbook(const NodeGraph& graph) -> std::string;

    // --- Statistics ---
    [[nodiscard]] static auto stats(const NodeGraph& graph) -> GraphStats;

    // --- Bulk ---
    void clear();

private:
    std::unordered_map<std::string, NodeTypeDoc> type_docs_;
};

} // namespace markamp::node_editor
