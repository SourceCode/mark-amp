# Phase 27 -- Knowledge Graph Data Computation

## Objective

Implement the knowledge graph data model and computation engine that represents documents and blocks as nodes and inter-document references as edges. SiYuan's graph view is a central knowledge management feature, visualizing how notes interconnect through block references, embeds, and mentions. The graph supports two scopes: a global graph showing all documents in the workspace and a local graph showing the neighborhood of a specific block or document within a configurable traversal depth. This phase builds the data computation layer -- Phase 28 handles the visual rendering.

The graph data computation pipeline works as follows: (1) scan all Markdown files in the workspace to extract block-level references (links to other blocks via `((block-id))` syntax, standard Markdown links to local documents, and embed references), (2) build an adjacency list representation of the reference graph, (3) compute node attributes such as reference count (used for sizing nodes) and type classification, (4) apply user-configured filters (minimum reference threshold, block type inclusion, daily note visibility), and (5) for local graphs, perform a breadth-first traversal from the center node to the specified depth. The result is a GraphData structure containing nodes and links arrays ready for visualization.

Performance is critical since the global graph may contain thousands of documents. The GraphService employs lazy computation with caching: the full graph is computed once and cached, then incrementally invalidated when documents are added, removed, or modified (detected via EventBus subscriptions to FileContentChangedEvent and FileOpenedEvent). The local graph is computed on demand from the cached global adjacency list. For large workspaces, the graph computation runs on a background thread with the result delivered via EventBus queue to the UI thread.

## Prerequisites

- No strict phase dependencies. Uses existing MarkAmp infrastructure: EventBus, Config, workspace file scanning (FileSystem/WorkspaceService).

## SiYuan Source Reference

- `kernel/model/graph.go` -- `GetGraph()` (global graph), `GetLocalGraph()` (local graph around a block), `graphNodeStyle()` (node styling by type), `graphRelationLinks()` (edge construction from refs)
- `kernel/sql/block_ref.go` -- Block reference queries: `QueryRefsByDefID()`, `QueryRefsByRefID()`, `QueryDefRootBlocksByRefRootID()`
- `kernel/model/blockinfo.go` -- Block metadata retrieval for node labeling
- SiYuan's graph returns `{ nodes: [{id, box, label, type, size, ...}], links: [{source, target, ref}] }`

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|------|-----------|---------|
| `src/core/GraphTypes.h` | `markamp::core` | GraphNode, GraphLink, GraphData, GraphConfig structs |
| `src/core/GraphService.h` | `markamp::core` | GraphService class declaration |
| `src/core/GraphService.cpp` | `markamp::core` | GraphService implementation |
| `src/core/ReferenceScanner.h` | `markamp::core` | ReferenceScanner for extracting refs from Markdown |
| `src/core/ReferenceScanner.cpp` | `markamp::core` | ReferenceScanner implementation |
| `tests/unit/test_graph_service.cpp` | (test) | Catch2 test suite for graph computation |

### Existing Files to Modify

| File | Change |
|------|--------|
| `src/core/Events.h` | Add graph computation events |
| `src/core/PluginContext.h` | Add `GraphService*` pointer |
| `src/CMakeLists.txt` | Add new source files |
| `tests/CMakeLists.txt` | Add test_graph_service target |
| `resources/config_defaults.json` | Add graph config defaults |

## Data Structures to Implement

```cpp
// ============================================================================
// File: src/core/GraphTypes.h
// ============================================================================
#pragma once

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
    BlockRef,    // ((block-id)) reference
    DocLink,     // Standard [text](file.md) link
    Embed,       // !((block-id)) or transclusion
    Mention,     // Text mention (backlink without explicit ref)
    Tag          // Shared tag connection
};

/// A node in the knowledge graph.
struct GraphNode
{
    std::string id;             // Block or document ID
    std::string notebook_id;    // Notebook/folder containing this node
    std::string label;          // Display label (doc title, heading text, etc.)
    GraphNodeType type{GraphNodeType::Document};
    int ref_count{0};           // Number of incoming references (determines visual size)
    bool is_daily_note{false};  // Whether this is a daily note/journal entry
    std::string icon;           // Optional icon identifier

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
    std::string label;     // Optional label (e.g., alias text)

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

    [[nodiscard]] auto find_node(const std::string& id) const -> const GraphNode*
    {
        for (const auto& node : nodes)
        {
            if (node.id == id)
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
    Global,   // All documents in workspace
    Local,    // Neighborhood of a specific node
    Notebook  // All documents in a specific notebook
};

/// Configuration for graph computation and filtering.
struct GraphConfig
{
    int min_refs{0};                  // Filter out nodes with fewer refs
    bool show_daily_notes{true};      // Include daily note documents
    bool show_tag_nodes{true};        // Include tag-based nodes
    bool show_orphan_nodes{true};     // Include nodes with zero connections
    std::set<GraphNodeType> include_types; // Which block types to include (empty = all)

    // Layout parameters (used by Phase 28 visualization)
    double force_strength{-300.0};    // Repulsion force between nodes
    double link_distance{100.0};      // Preferred edge length
    double node_size_min{4.0};        // Minimum node radius in pixels
    double node_size_max{24.0};       // Maximum node radius in pixels
    std::string layout_algorithm{"force-directed"}; // "force-directed", "radial", "hierarchical"

    [[nodiscard]] auto should_include_type(GraphNodeType type) const -> bool
    {
        return include_types.empty() || include_types.contains(type);
    }
};

/// A reference extracted from a Markdown document.
struct ExtractedReference
{
    std::string source_block_id;  // The block containing the reference
    std::string target_id;        // The referenced block or document ID
    GraphRefType ref_type{GraphRefType::DocLink};
    std::string alias;            // Display text for the link
    int line_number{0};           // Line in the source document
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

    void add_edge(const std::string& from, const std::string& to)
    {
        outgoing[from].push_back(to);
        incoming[to].push_back(from);
        all_node_ids.insert(from);
        all_node_ids.insert(to);
    }

    [[nodiscard]] auto in_degree(const std::string& id) const -> int
    {
        auto it = incoming.find(id);
        if (it == incoming.end())
        {
            return 0;
        }
        return static_cast<int>(it->second.size());
    }

    [[nodiscard]] auto out_degree(const std::string& id) const -> int
    {
        auto it = outgoing.find(id);
        if (it == outgoing.end())
        {
            return 0;
        }
        return static_cast<int>(it->second.size());
    }
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/GraphService.h
// ============================================================================
#pragma once

#include "GraphTypes.h"

#include <expected>
#include <memory>
#include <mutex>
#include <string>

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

    // Non-copyable
    GraphService(const GraphService&) = delete;
    auto operator=(const GraphService&) -> GraphService& = delete;

    /// Compute the global graph for the entire workspace.
    [[nodiscard]] auto get_global_graph(const GraphConfig& config)
        -> std::expected<GraphData, std::string>;

    /// Compute a local graph centered on a specific block/document.
    [[nodiscard]] auto get_local_graph(const std::string& center_id,
                                       int depth,
                                       const GraphConfig& config)
        -> std::expected<GraphData, std::string>;

    /// Compute the graph for a specific notebook/folder.
    [[nodiscard]] auto get_notebook_graph(const std::string& notebook_id,
                                          const GraphConfig& config)
        -> std::expected<GraphData, std::string>;

    /// Invalidate the cached graph (called when documents change).
    void invalidate_cache();

    /// Force a full recomputation of the graph data.
    [[nodiscard]] auto recompute() -> std::expected<void, std::string>;

    /// Get the default graph configuration from user settings.
    [[nodiscard]] auto default_config() const -> GraphConfig;

private:
    EventBus& event_bus_;
    Config& config_;

    mutable std::mutex graph_mutex_;
    AdjacencyList adjacency_;
    std::unordered_map<std::string, GraphNode> node_map_;
    bool cache_valid_{false};

    /// Scan all workspace documents and build the adjacency list + node map.
    [[nodiscard]] auto build_graph_data()
        -> std::expected<void, std::string>;

    /// Apply filters from GraphConfig to produce the final GraphData.
    [[nodiscard]] auto apply_filters(const GraphConfig& config) const -> GraphData;

    /// BFS from center node to specified depth, collecting reachable nodes.
    [[nodiscard]] auto bfs_local(const std::string& center_id, int depth) const
        -> std::set<std::string>;

    /// Subscribe to document change events for cache invalidation.
    void subscribe_to_changes();

    Subscription file_changed_sub_;
    Subscription file_opened_sub_;
};

} // namespace markamp::core
```

```cpp
// ============================================================================
// File: src/core/ReferenceScanner.h
// ============================================================================
#pragma once

#include "GraphTypes.h"

#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Scans Markdown content for block references, document links, and embeds.
/// Extracts ExtractedReference objects for graph construction.
class ReferenceScanner
{
public:
    /// Scan a Markdown document for all references.
    [[nodiscard]] auto scan(const std::string& document_id,
                            std::string_view markdown_content) const
        -> std::vector<ExtractedReference>;

    /// Extract block references: ((block-id)) syntax.
    [[nodiscard]] auto extract_block_refs(const std::string& source_id,
                                          std::string_view content) const
        -> std::vector<ExtractedReference>;

    /// Extract standard Markdown links: [text](target.md) where target is local.
    [[nodiscard]] auto extract_doc_links(const std::string& source_id,
                                         std::string_view content) const
        -> std::vector<ExtractedReference>;

    /// Extract embed references: !((block-id)) transclusion syntax.
    [[nodiscard]] auto extract_embeds(const std::string& source_id,
                                      std::string_view content) const
        -> std::vector<ExtractedReference>;

    /// Extract tag references: #tag or #multi-word-tag syntax.
    [[nodiscard]] auto extract_tags(const std::string& source_id,
                                    std::string_view content) const
        -> std::vector<ExtractedReference>;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`GraphService::get_global_graph(config)`** -- Acquire `graph_mutex_`. If `cache_valid_` is false, call `build_graph_data()`. Apply filters from `config` via `apply_filters()`. Publish `GraphDataComputedEvent` with scope=Global. Return the filtered GraphData.

2. **`GraphService::get_local_graph(center_id, depth, config)`** -- Ensure cache is valid. Call `bfs_local(center_id, depth)` to get the set of reachable node IDs. Build a GraphData containing only those nodes and links between them. Apply filters. Publish `GraphDataComputedEvent` with scope=Local.

3. **`GraphService::build_graph_data()`** -- Enumerate all Markdown files in the workspace using FileSystem/WorkspaceService. For each file, read content and pass to `ReferenceScanner::scan()`. Build a GraphNode for each document (type=Document, label=title from first heading or filename). Build GraphNodes for each referenced block. Populate `adjacency_` and `node_map_`. Compute `ref_count` for each node from `adjacency_.in_degree()`. Set `cache_valid_ = true`.

4. **`GraphService::apply_filters(config)`** -- Iterate `node_map_`. Skip nodes where: `ref_count < config.min_refs`, or `is_daily_note && !config.show_daily_notes`, or type not in `config.include_types`, or `ref_count == 0 && !config.show_orphan_nodes`. Collect filtered node IDs. Build links vector by including only links where both source and target pass the filter.

5. **`GraphService::bfs_local(center_id, depth)`** -- Standard BFS from `center_id`, traversing both outgoing and incoming edges in `adjacency_`. Limit traversal to `depth` hops. Return the set of all visited node IDs. If `center_id` is not found in the graph, return a set containing only `center_id`.

6. **`GraphService::invalidate_cache()`** -- Set `cache_valid_ = false`. This is called by EventBus handlers when document content changes.

7. **`GraphService::subscribe_to_changes()`** -- Subscribe to `FileContentChangedEvent` and `FileOpenedEvent` on the EventBus. In each handler, call `invalidate_cache()`. Store the Subscription tokens in member fields.

8. **`ReferenceScanner::scan(document_id, content)`** -- Call all four extraction methods (block_refs, doc_links, embeds, tags) and concatenate the results into a single vector.

9. **`ReferenceScanner::extract_block_refs(source_id, content)`** -- Scan content for the pattern `((` followed by a block ID (alphanumeric + hyphens) followed by `))`. For each match, create an ExtractedReference with `ref_type = BlockRef`. Track line numbers by counting newlines before each match position.

10. **`ReferenceScanner::extract_doc_links(source_id, content)`** -- Scan for standard Markdown link syntax `[text](target)`. Filter to local links only (no http/https prefix, ends in .md or .sy or has no extension). Create ExtractedReference with `ref_type = DocLink` and alias = the link text.

11. **`ReferenceScanner::extract_tags(source_id, content)`** -- Scan for `#tagname` patterns (where tagname is alphanumeric + hyphens, not inside code blocks). Create a pseudo-node for each unique tag. Return references with `ref_type = Tag`.

12. **`GraphService::get_notebook_graph(notebook_id, config)`** -- Similar to global graph but filter `node_map_` to nodes whose `notebook_id` matches. Include links that cross notebook boundaries if both endpoints are in the result set.

## Events to Add

Add the following to `src/core/Events.h`:

```cpp
// ============================================================================
// Knowledge graph events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphDataComputedEvent)
GraphScope scope{GraphScope::Global};
int node_count{0};
int link_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphFilterChangedEvent)
GraphConfig config;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphCacheInvalidatedEvent);
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `knowledgebase.graph.min_refs` | int | 0 | Minimum reference count to display a node |
| `knowledgebase.graph.show_daily_notes` | bool | true | Show daily note documents in graph |
| `knowledgebase.graph.show_tag_nodes` | bool | true | Show tag-based nodes |
| `knowledgebase.graph.show_orphans` | bool | true | Show unconnected nodes |
| `knowledgebase.graph.local_depth` | int | 3 | BFS depth for local graph |
| `knowledgebase.graph.force_strength` | double | -300.0 | Repulsion force strength |
| `knowledgebase.graph.link_distance` | double | 100.0 | Preferred link distance in pixels |
| `knowledgebase.graph.layout_algorithm` | string | "force-directed" | Layout algorithm name |

## Test Cases

All tests in `tests/unit/test_graph_service.cpp` using Catch2.

1. **"ReferenceScanner extracts block refs from ((id)) syntax"** -- Input: `"See ((abc-123)) for details and ((def-456))."`. Verify 2 ExtractedReferences with target IDs "abc-123" and "def-456", both with `ref_type == BlockRef`.

2. **"ReferenceScanner extracts local doc links but ignores URLs"** -- Input: `"Link to [note](other.md) and [web](https://example.com)."`. Verify 1 ExtractedReference with target "other.md" and `ref_type == DocLink`. The HTTPS link is excluded.

3. **"ReferenceScanner extracts tags"** -- Input: `"Tagged with #project-alpha and #review."`. Verify 2 references with targets "project-alpha" and "review", both with `ref_type == Tag`.

4. **"ReferenceScanner ignores refs inside code blocks"** -- Input with a fenced code block containing `((fake-ref))`. Verify 0 ExtractedReferences (code block content is skipped).

5. **"GraphService builds global graph with correct node and link counts"** -- Set up a mock workspace with 3 documents, where doc1 links to doc2 and doc3, and doc2 links to doc3. Verify the global graph has 3 nodes and 3 links.

6. **"GraphService computes ref_count from incoming edges"** -- In the 3-document setup above, doc3 has 2 incoming refs. Verify `find_node("doc3")->ref_count == 2`.

7. **"get_local_graph returns only neighborhood nodes"** -- Set up 5 documents in a chain: A->B->C->D->E. Request local graph centered on C with depth=1. Verify nodes are {B, C, D} (immediate neighbors only). Depth=2 should include {A, B, C, D, E}.

8. **"apply_filters excludes nodes below min_refs threshold"** -- Global graph with nodes having ref_counts [0, 1, 3, 5]. Set `config.min_refs = 2`. Verify only nodes with ref_count >= 2 appear (2 nodes).

9. **"apply_filters excludes daily notes when configured"** -- Mark one node as `is_daily_note = true`. Set `config.show_daily_notes = false`. Verify that node is excluded from the result.

10. **"cache invalidation triggers recomputation on next query"** -- Compute global graph (cache valid). Call `invalidate_cache()`. Modify the workspace (add a link). Compute global graph again. Verify the new link appears in the result.

## Acceptance Criteria

- [ ] ReferenceScanner correctly extracts block refs, doc links, embeds, and tags from Markdown
- [ ] ReferenceScanner skips references inside fenced code blocks
- [ ] GraphService computes global graph with accurate node counts and ref_counts
- [ ] get_local_graph performs correct BFS to specified depth
- [ ] GraphConfig filters (min_refs, show_daily_notes, include_types) work correctly
- [ ] Graph cache invalidates on FileContentChangedEvent
- [ ] Graph computation is thread-safe (graph_mutex_ protects all shared state)
- [ ] AdjacencyList correctly tracks both incoming and outgoing edges
- [ ] Orphan nodes (zero connections) can be shown or hidden via config

## Files to Create/Modify

```
CREATE  src/core/GraphTypes.h
CREATE  src/core/GraphService.h
CREATE  src/core/GraphService.cpp
CREATE  src/core/ReferenceScanner.h
CREATE  src/core/ReferenceScanner.cpp
CREATE  tests/unit/test_graph_service.cpp
MODIFY  src/core/Events.h              -- add graph events
MODIFY  src/core/PluginContext.h        -- add GraphService* graph_service{nullptr};
MODIFY  src/CMakeLists.txt              -- add GraphService.cpp, ReferenceScanner.cpp
MODIFY  tests/CMakeLists.txt            -- add test_graph_service target
MODIFY  resources/config_defaults.json  -- add knowledgebase.graph.* defaults
```
