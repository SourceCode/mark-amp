#include "GraphService.h"

#include "Events.h"
#include "ReferenceScanner.h"

#include <algorithm>
#include <queue>
#include <set>

namespace markamp::core
{

GraphService::GraphService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    subscribe_to_changes();
}

auto GraphService::get_global_graph(const GraphConfig& graph_config)
    -> std::expected<GraphData, std::string>
{
    std::lock_guard lock(graph_mutex_);

    if (!cache_valid_)
    {
        if (auto result = build_graph_data(); !result)
        {
            return std::unexpected(result.error());
        }
    }

    return apply_filters(graph_config);
}

auto GraphService::get_local_graph(const std::string& center_id,
                                   int depth,
                                   const GraphConfig& graph_config)
    -> std::expected<GraphData, std::string>
{
    std::lock_guard lock(graph_mutex_);

    if (!cache_valid_)
    {
        if (auto result = build_graph_data(); !result)
        {
            return std::unexpected(result.error());
        }
    }

    // BFS to find reachable nodes
    const auto reachable = bfs_local(center_id, depth);

    // Build filtered graph with only reachable nodes
    GraphData local_data;

    for (const auto& node_id : reachable)
    {
        auto iter = node_map_.find(node_id);
        if (iter != node_map_.end())
        {
            auto node = iter->second;

            // Apply type filter
            if (!graph_config.should_include_type(node.type))
            {
                continue;
            }

            // Apply daily note filter
            if (node.is_daily_note && !graph_config.show_daily_notes)
            {
                continue;
            }

            local_data.nodes.push_back(std::move(node));
        }
    }

    // Collect IDs of included nodes for link filtering
    std::set<std::string> included_ids;
    for (const auto& node : local_data.nodes)
    {
        included_ids.insert(node.id);
    }

    // Add links where both endpoints are in the result set
    for (const auto& [source_id, targets] : adjacency_.outgoing)
    {
        if (included_ids.find(source_id) == included_ids.end())
        {
            continue;
        }

        for (const auto& target_id : targets)
        {
            if (included_ids.find(target_id) != included_ids.end())
            {
                GraphLink link;
                link.source_id = source_id;
                link.target_id = target_id;
                link.ref_type = GraphRefType::BlockRef;
                local_data.links.push_back(std::move(link));
            }
        }
    }

    return local_data;
}

auto GraphService::get_notebook_graph(const std::string& notebook_id,
                                      const GraphConfig& graph_config)
    -> std::expected<GraphData, std::string>
{
    std::lock_guard lock(graph_mutex_);

    if (!cache_valid_)
    {
        if (auto result = build_graph_data(); !result)
        {
            return std::unexpected(result.error());
        }
    }

    // Filter to nodes in the specified notebook
    GraphData notebook_data;
    std::set<std::string> included_ids;

    for (const auto& [node_id, node] : node_map_)
    {
        if (node.notebook_id != notebook_id)
        {
            continue;
        }

        if (!graph_config.should_include_type(node.type))
        {
            continue;
        }

        if (node.ref_count < graph_config.min_refs)
        {
            continue;
        }

        if (node.is_daily_note && !graph_config.show_daily_notes)
        {
            continue;
        }

        included_ids.insert(node_id);
        notebook_data.nodes.push_back(node);
    }

    // Add links where both endpoints are in the notebook
    for (const auto& [source_id, targets] : adjacency_.outgoing)
    {
        if (included_ids.find(source_id) == included_ids.end())
        {
            continue;
        }

        for (const auto& target_id : targets)
        {
            if (included_ids.find(target_id) != included_ids.end())
            {
                GraphLink link;
                link.source_id = source_id;
                link.target_id = target_id;
                link.ref_type = GraphRefType::BlockRef;
                notebook_data.links.push_back(std::move(link));
            }
        }
    }

    return notebook_data;
}

void GraphService::invalidate_cache()
{
    std::lock_guard lock(graph_mutex_);
    cache_valid_ = false;
}

auto GraphService::recompute() -> std::expected<void, std::string>
{
    std::lock_guard lock(graph_mutex_);
    cache_valid_ = false;
    return build_graph_data();
}

auto GraphService::default_config() const -> GraphConfig
{
    GraphConfig graph_config;
    graph_config.min_refs = 0;
    graph_config.show_daily_notes = true;
    graph_config.show_tag_nodes = true;
    graph_config.show_orphan_nodes = true;
    graph_config.force_strength = -300.0;
    graph_config.link_distance = 100.0;
    graph_config.node_size_min = 4.0;
    graph_config.node_size_max = 24.0;
    graph_config.layout_algorithm = "force-directed";
    return graph_config;
}

void GraphService::register_document(const std::string& doc_id,
                                     const std::string& notebook_id,
                                     const std::string& title,
                                     const std::string& content,
                                     bool is_daily)
{
    std::lock_guard lock(graph_mutex_);
    documents_.push_back({doc_id, notebook_id, title, content, is_daily});
    cache_valid_ = false;
}

void GraphService::clear_documents()
{
    std::lock_guard lock(graph_mutex_);
    documents_.clear();
    node_map_.clear();
    adjacency_.clear();
    cache_valid_ = false;
}

auto GraphService::build_graph_data() -> std::expected<void, std::string>
{
    // Clear existing data
    adjacency_.clear();
    node_map_.clear();

    ReferenceScanner scanner;

    // Build nodes and scan for references
    for (const auto& doc : documents_)
    {
        // Create a node for each document
        GraphNode node;
        node.id = doc.doc_id;
        node.notebook_id = doc.notebook_id;
        node.label = doc.title;
        node.type = GraphNodeType::Document;
        node.is_daily_note = doc.is_daily;
        node_map_[doc.doc_id] = std::move(node);

        // Scan content for references
        auto refs = scanner.scan(doc.doc_id, doc.content);

        for (const auto& ref : refs)
        {
            adjacency_.add_edge(ref.source_block_id, ref.target_id);

            // Ensure target node exists in node_map
            if (node_map_.find(ref.target_id) == node_map_.end())
            {
                GraphNode target_node;
                target_node.id = ref.target_id;

                if (ref.ref_type == GraphRefType::Tag)
                {
                    target_node.type = GraphNodeType::Tag;
                    target_node.label = "#" + ref.target_id;
                }
                else
                {
                    // Referenced block/doc may be an external reference
                    target_node.label = ref.alias.empty() ? ref.target_id : ref.alias;
                }

                node_map_[ref.target_id] = std::move(target_node);
            }
        }
    }

    // Compute ref_count for each node from incoming edges
    for (auto& [node_id, node] : node_map_)
    {
        node.ref_count = adjacency_.in_degree(node_id);
    }

    cache_valid_ = true;
    return {};
}

auto GraphService::apply_filters(const GraphConfig& graph_config) const -> GraphData
{
    GraphData result;
    std::set<std::string> included_ids;

    for (const auto& [node_id, node] : node_map_)
    {
        // Filter by minimum reference count
        if (node.ref_count < graph_config.min_refs)
        {
            continue;
        }

        // Filter daily notes
        if (node.is_daily_note && !graph_config.show_daily_notes)
        {
            continue;
        }

        // Filter tag nodes
        if (node.type == GraphNodeType::Tag && !graph_config.show_tag_nodes)
        {
            continue;
        }

        // Filter by included types
        if (!graph_config.should_include_type(node.type))
        {
            continue;
        }

        // Filter orphan nodes (nodes with zero connections)
        if (!graph_config.show_orphan_nodes)
        {
            const auto total_degree =
                adjacency_.in_degree(node_id) + adjacency_.out_degree(node_id);
            if (total_degree == 0)
            {
                continue;
            }
        }

        included_ids.insert(node_id);
        result.nodes.push_back(node);
    }

    // Add links where both endpoints pass the filter
    for (const auto& [source_id, targets] : adjacency_.outgoing)
    {
        if (included_ids.find(source_id) == included_ids.end())
        {
            continue;
        }

        for (const auto& target_id : targets)
        {
            if (included_ids.find(target_id) != included_ids.end())
            {
                GraphLink link;
                link.source_id = source_id;
                link.target_id = target_id;
                link.ref_type = GraphRefType::BlockRef;
                result.links.push_back(std::move(link));
            }
        }
    }

    return result;
}

auto GraphService::bfs_local(const std::string& center_id, int depth) const -> std::set<std::string>
{
    std::set<std::string> visited;
    std::queue<std::pair<std::string, int>> frontier;

    visited.insert(center_id);
    frontier.push({center_id, 0});

    while (!frontier.empty())
    {
        const auto [current_id, current_depth] = frontier.front();
        frontier.pop();

        if (current_depth >= depth)
        {
            continue;
        }

        // Traverse outgoing edges
        auto out_iter = adjacency_.outgoing.find(current_id);
        if (out_iter != adjacency_.outgoing.end())
        {
            for (const auto& neighbor_id : out_iter->second)
            {
                if (visited.find(neighbor_id) == visited.end())
                {
                    visited.insert(neighbor_id);
                    frontier.push({neighbor_id, current_depth + 1});
                }
            }
        }

        // Traverse incoming edges (bidirectional traversal)
        auto in_iter = adjacency_.incoming.find(current_id);
        if (in_iter != adjacency_.incoming.end())
        {
            for (const auto& neighbor_id : in_iter->second)
            {
                if (visited.find(neighbor_id) == visited.end())
                {
                    visited.insert(neighbor_id);
                    frontier.push({neighbor_id, current_depth + 1});
                }
            }
        }
    }

    return visited;
}

void GraphService::subscribe_to_changes()
{
    // Subscribe to file change events for real-time graph cache invalidation.
    [[maybe_unused]] auto sub1 = event_bus_.subscribe<events::FileChangedEvent>(
        [this](const events::FileChangedEvent& /*evt*/)
        {
            invalidate_cache();
        });

    [[maybe_unused]] auto sub2 = event_bus_.subscribe<events::FileContentChangedEvent>(
        [this](const events::FileContentChangedEvent& /*evt*/)
        {
            invalidate_cache();
        });

    [[maybe_unused]] auto sub3 = event_bus_.subscribe<events::FilesTrashedEvent>(
        [this](const events::FilesTrashedEvent& /*evt*/)
        {
            invalidate_cache();
        });
}

} // namespace markamp::core
