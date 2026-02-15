/// @file LocalGraphEngine.h
/// @brief V4 Phase 36 – Local Backlink Graph Engine.

#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data structures
// ============================================================================

/// A node in the local graph.
struct LocalGraphNode
{
    std::string note_id;
    std::string title;
    std::vector<std::string> tags;
    int link_count{0};    ///< Total links (in + out).
    int depth{0};         ///< Distance from the focus node.
    bool is_focus{false}; ///< True if this is the root node.
};

/// An edge in the local graph.
struct LocalGraphEdge
{
    std::string source_id;
    std::string target_id;
    std::string label; ///< Optional edge label (e.g. link text).
};

/// The complete local graph result.
struct LocalGraphData
{
    std::vector<LocalGraphNode> nodes;
    std::vector<LocalGraphEdge> edges;
    std::string focus_id;
};

/// Configuration for local graph building.
struct LocalGraphConfig
{
    int max_depth{2};
    int max_nodes{50};
    bool include_orphans{false};
    bool include_tags{true};
};

/// Information about a link between notes (used as input).
struct NoteLink
{
    std::string source_id;
    std::string target_id;
    std::string label;
};

/// Information about a note (used as input).
struct NoteInfo
{
    std::string note_id;
    std::string title;
    std::vector<std::string> tags;
};

// ============================================================================
// LocalGraphEngine
// ============================================================================

class LocalGraphEngine
{
public:
    explicit LocalGraphEngine(EventBus& event_bus);

    /// Load notes and their links into the engine.
    auto load_notes(const std::vector<NoteInfo>& notes, const std::vector<NoteLink>& links) -> void;

    /// Build a local neighborhood graph around a focus note.
    [[nodiscard]] auto build_local_graph(const std::string& note_id, int depth = -1)
        -> LocalGraphData;

    /// Configure graph building parameters.
    auto set_config(const LocalGraphConfig& config) -> void;

    /// Get direct neighbors of a note (incoming + outgoing).
    [[nodiscard]] auto get_neighbors(const std::string& note_id) const -> std::vector<std::string>;

    /// Identify connected component clusters in the local graph.
    [[nodiscard]] auto get_clusters(const LocalGraphData& graph) const
        -> std::vector<std::vector<std::string>>;

    /// Filter graph to nodes matching a tag.
    [[nodiscard]] auto filter_by_tag(const LocalGraphData& graph, const std::string& tag) const
        -> LocalGraphData;

    /// Find shortest path between two nodes.
    [[nodiscard]] auto highlight_path(const std::string& from, const std::string& to) const
        -> std::vector<std::string>;

    /// Accessors.
    [[nodiscard]] auto node_count() const -> int;
    [[nodiscard]] auto edge_count() const -> int;

private:
    EventBus& event_bus_;
    LocalGraphConfig config_;
    std::vector<NoteInfo> notes_;
    std::unordered_map<std::string, std::vector<std::string>> outgoing_;
    std::unordered_map<std::string, std::vector<std::string>> incoming_;
    std::unordered_map<std::string, std::string> link_labels_;
};

} // namespace markamp::core
