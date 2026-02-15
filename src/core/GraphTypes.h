#pragma once

#include <cmath>
#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// The type of block a graph node represents.
enum class GraphNodeType : uint8_t
{
    Document,
    Heading,
    Paragraph,
    List,
    CodeBlock,
    Table,
    Blockquote,
    MathBlock,
    Tag
};

/// The type of reference a graph link represents.
enum class GraphRefType : uint8_t
{
    BlockRef, // ((block-id)) reference
    DocLink,  // Standard [text](file.md) link
    Embed,    // !((block-id)) or transclusion
    Mention,  // Text mention (backlink without explicit ref)
    Tag       // Shared tag connection
};

/// A node in the knowledge graph.
struct GraphNode
{
    std::string id;          // Block or document ID
    std::string notebook_id; // Notebook/folder containing this node
    std::string label;       // Display label (doc title, heading text, etc.)
    GraphNodeType type{GraphNodeType::Document};
    int ref_count{0};          // Number of incoming references (determines visual size)
    bool is_daily_note{false}; // Whether this is a daily note/journal entry
    std::string icon;          // Optional icon identifier

    [[nodiscard]] auto size_factor() const -> double
    {
        // Logarithmic scaling: min size 1.0, grows with ref_count
        if (ref_count <= 0)
        {
            return 1.0;
        }
        return 1.0 + std::log2(1.0 + static_cast<double>(ref_count));
    }
};

/// An edge in the knowledge graph.
struct GraphLink
{
    std::string source_id; // Node ID of the referencing block
    std::string target_id; // Node ID of the referenced block
    GraphRefType ref_type{GraphRefType::BlockRef};
    std::string label; // Optional label (e.g., alias text)

    [[nodiscard]] auto is_bidirectional() const -> bool
    {
        return ref_type == GraphRefType::Tag || ref_type == GraphRefType::Mention;
    }
};

/// The complete graph data structure returned by graph queries.
struct GraphData
{
    std::vector<GraphNode> nodes;
    std::vector<GraphLink> links;

    [[nodiscard]] auto node_count() const -> int
    {
        return static_cast<int>(nodes.size());
    }

    [[nodiscard]] auto link_count() const -> int
    {
        return static_cast<int>(links.size());
    }

    [[nodiscard]] auto is_empty() const -> bool
    {
        return nodes.empty();
    }

    [[nodiscard]] auto find_node(const std::string& node_id) const -> const GraphNode*
    {
        for (const auto& node : nodes)
        {
            if (node.id == node_id)
            {
                return &node;
            }
        }
        return nullptr;
    }
};

/// Scope for graph computation requests.
enum class GraphScope : uint8_t
{
    Global,  // All documents in workspace
    Local,   // Neighborhood of a specific node
    Notebook // All documents in a specific notebook
};

/// Configuration for graph computation and filtering.
struct GraphConfig
{
    int min_refs{0};                       // Filter out nodes with fewer refs
    bool show_daily_notes{true};           // Include daily note documents
    bool show_tag_nodes{true};             // Include tag-based nodes
    bool show_orphan_nodes{true};          // Include nodes with zero connections
    std::set<GraphNodeType> include_types; // Which block types to include (empty = all)

    // Layout parameters (used by Phase 28 visualization)
    double force_strength{-300.0}; // Repulsion force between nodes
    double link_distance{100.0};   // Preferred edge length
    double node_size_min{4.0};     // Minimum node radius in pixels
    double node_size_max{24.0};    // Maximum node radius in pixels
    std::string layout_algorithm{"force-directed"};

    [[nodiscard]] auto should_include_type(GraphNodeType type) const -> bool
    {
        return include_types.empty() || include_types.contains(type);
    }
};

/// A reference extracted from a Markdown document.
struct ExtractedReference
{
    std::string source_block_id; // The block containing the reference
    std::string target_id;       // The referenced block or document ID
    GraphRefType ref_type{GraphRefType::DocLink};
    std::string alias;  // Display text for the link
    int line_number{0}; // Line in the source document
};

/// Adjacency list representation for efficient graph traversal.
struct AdjacencyList
{
    /// Map from node ID to set of adjacent node IDs (outgoing edges).
    std::unordered_map<std::string, std::vector<std::string>> outgoing;

    /// Map from node ID to set of adjacent node IDs (incoming edges).
    std::unordered_map<std::string, std::vector<std::string>> incoming;

    /// All node IDs in the graph.
    std::set<std::string> all_node_ids;

    void add_edge(const std::string& from, const std::string& to_node)
    {
        outgoing[from].push_back(to_node);
        incoming[to_node].push_back(from);
        all_node_ids.insert(from);
        all_node_ids.insert(to_node);
    }

    [[nodiscard]] auto in_degree(const std::string& node_id) const -> int
    {
        auto iter = incoming.find(node_id);
        if (iter == incoming.end())
        {
            return 0;
        }
        return static_cast<int>(iter->second.size());
    }

    [[nodiscard]] auto out_degree(const std::string& node_id) const -> int
    {
        auto iter = outgoing.find(node_id);
        if (iter == outgoing.end())
        {
            return 0;
        }
        return static_cast<int>(iter->second.size());
    }

    void clear()
    {
        outgoing.clear();
        incoming.clear();
        all_node_ids.clear();
    }
};

} // namespace markamp::core
