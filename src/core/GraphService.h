#pragma once

#include "GraphTypes.h"

#include <expected>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

namespace markamp::core
{

class EventBus;
class Config;

/// Service for computing knowledge graph data from workspace documents.
/// Caches the global graph and invalidates on document changes.
class GraphService
{
public:
    GraphService(EventBus& event_bus, Config& config);
    ~GraphService() = default;

    // Non-copyable, non-movable
    GraphService(const GraphService&) = delete;
    auto operator=(const GraphService&) -> GraphService& = delete;
    GraphService(GraphService&&) = delete;
    auto operator=(GraphService&&) -> GraphService& = delete;

    /// Compute the global graph for the entire workspace.
    [[nodiscard]] auto get_global_graph(const GraphConfig& graph_config)
        -> std::expected<GraphData, std::string>;

    /// Compute a local graph centered on a specific block/document.
    [[nodiscard]] auto
    get_local_graph(const std::string& center_id, int depth, const GraphConfig& graph_config)
        -> std::expected<GraphData, std::string>;

    /// Compute the graph for a specific notebook/folder.
    [[nodiscard]] auto get_notebook_graph(const std::string& notebook_id,
                                          const GraphConfig& graph_config)
        -> std::expected<GraphData, std::string>;

    /// Invalidate the cached graph (called when documents change).
    void invalidate_cache();

    /// Force a full recomputation of the graph data.
    [[nodiscard]] auto recompute() -> std::expected<void, std::string>;

    /// Get the default graph configuration from user settings.
    [[nodiscard]] auto default_config() const -> GraphConfig;

    /// Register a document's content for graph building.
    /// This allows unit testing without real filesystem access.
    void register_document(const std::string& doc_id,
                           const std::string& notebook_id,
                           const std::string& title,
                           const std::string& content,
                           bool is_daily = false);

    /// Clear all registered documents.
    void clear_documents();

private:
    [[maybe_unused]] EventBus& event_bus_;
    [[maybe_unused]] Config& config_;

    mutable std::mutex graph_mutex_;
    AdjacencyList adjacency_;
    std::unordered_map<std::string, GraphNode> node_map_;
    bool cache_valid_{false};

    /// Document content for scanning (registered via register_document).
    struct DocumentInfo
    {
        std::string doc_id;
        std::string notebook_id;
        std::string title;
        std::string content;
        bool is_daily{false};
    };
    std::vector<DocumentInfo> documents_;

    /// Scan all registered documents and build the adjacency list + node map.
    [[nodiscard]] auto build_graph_data() -> std::expected<void, std::string>;

    /// Apply filters from GraphConfig to produce the final GraphData.
    [[nodiscard]] auto apply_filters(const GraphConfig& graph_config) const -> GraphData;

    /// BFS from center node to specified depth, collecting reachable nodes.
    [[nodiscard]] auto bfs_local(const std::string& center_id, int depth) const
        -> std::set<std::string>;

    /// Subscribe to document change events for cache invalidation.
    void subscribe_to_changes();
};

} // namespace markamp::core
