/// @file test_phase16_knowledge_graph.cpp
/// @brief V9 Phase 16 – Comprehensive test suite for Knowledge Graph & Backlinks.

#include "core/Events.h"
#include "core/GraphAccessibility.h"
#include "core/GraphAnalytics.h"
#include "core/GraphAnnotations.h"
#include "core/GraphEvolution.h"
#include "core/GraphExporter.h"
#include "core/GraphLayoutAlgorithms.h"
#include "core/GraphSearch.h"
#include "core/OrphanManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// Build a simple test graph: A ─ B ─ C ─ D, E (isolated).
auto build_simple_graph() -> GraphData
{
    GraphData graph;

    GraphNode node_a;
    node_a.id = "A";
    node_a.label = "Alpha";
    node_a.type = GraphNodeType::Document;
    GraphNode node_b;
    node_b.id = "B";
    node_b.label = "Beta";
    node_b.type = GraphNodeType::Document;
    GraphNode node_c;
    node_c.id = "C";
    node_c.label = "Charlie";
    node_c.type = GraphNodeType::Document;
    GraphNode node_d;
    node_d.id = "D";
    node_d.label = "Delta";
    node_d.type = GraphNodeType::Document;
    GraphNode node_e;
    node_e.id = "E";
    node_e.label = "Echo";
    node_e.type = GraphNodeType::Document;

    graph.nodes = {node_a, node_b, node_c, node_d, node_e};

    GraphLink link_ab;
    link_ab.source_id = "A";
    link_ab.target_id = "B";
    link_ab.ref_type = GraphRefType::DocLink;
    GraphLink link_bc;
    link_bc.source_id = "B";
    link_bc.target_id = "C";
    link_bc.ref_type = GraphRefType::DocLink;
    GraphLink link_cd;
    link_cd.source_id = "C";
    link_cd.target_id = "D";
    link_cd.ref_type = GraphRefType::DocLink;

    graph.links = {link_ab, link_bc, link_cd};

    return graph;
}

/// Build a graph with tags: Doc1 and Doc2 share Tag1, Doc2 and Doc3 share Tag2.
auto build_tagged_graph() -> GraphData
{
    GraphData graph;

    GraphNode doc1;
    doc1.id = "doc1";
    doc1.label = "Document One";
    doc1.type = GraphNodeType::Document;
    GraphNode doc2;
    doc2.id = "doc2";
    doc2.label = "Document Two";
    doc2.type = GraphNodeType::Document;
    GraphNode doc3;
    doc3.id = "doc3";
    doc3.label = "Document Three";
    doc3.type = GraphNodeType::Document;
    GraphNode tag1;
    tag1.id = "tag1";
    tag1.label = "physics";
    tag1.type = GraphNodeType::Tag;
    GraphNode tag2;
    tag2.id = "tag2";
    tag2.label = "math";
    tag2.type = GraphNodeType::Tag;

    graph.nodes = {doc1, doc2, doc3, tag1, tag2};

    GraphLink l1;
    l1.source_id = "doc1";
    l1.target_id = "tag1";
    l1.ref_type = GraphRefType::Tag;
    GraphLink l2;
    l2.source_id = "doc2";
    l2.target_id = "tag1";
    l2.ref_type = GraphRefType::Tag;
    GraphLink l3;
    l3.source_id = "doc2";
    l3.target_id = "tag2";
    l3.ref_type = GraphRefType::Tag;
    GraphLink l4;
    l4.source_id = "doc3";
    l4.target_id = "tag2";
    l4.ref_type = GraphRefType::Tag;

    graph.links = {l1, l2, l3, l4};

    return graph;
}

/// Build an empty graph.
auto build_empty_graph() -> GraphData
{
    return {};
}

/// Build a single-node graph.
auto build_single_node_graph() -> GraphData
{
    GraphData graph;
    GraphNode node;
    node.id = "solo";
    node.label = "Solo Node";
    node.type = GraphNodeType::Document;
    graph.nodes = {node};
    return graph;
}

/// Build a fully connected graph of 4 nodes (complete K4).
auto build_complete_graph() -> GraphData
{
    GraphData graph;
    for (int i = 0; i < 4; ++i)
    {
        GraphNode node;
        node.id = "n" + std::to_string(i);
        node.label = "Node " + std::to_string(i);
        node.type = GraphNodeType::Document;
        graph.nodes.push_back(node);
    }
    // 6 edges for K4
    for (int i = 0; i < 4; ++i)
    {
        for (int j = i + 1; j < 4; ++j)
        {
            GraphLink link;
            link.source_id = "n" + std::to_string(i);
            link.target_id = "n" + std::to_string(j);
            link.ref_type = GraphRefType::DocLink;
            graph.links.push_back(link);
        }
    }
    return graph;
}

} // namespace

// ============================================================================
// Task 18: GraphAnalytics Unit Tests
// ============================================================================

TEST_CASE("GraphAnalytics: compute_statistics on simple graph", "[phase16][analytics]")
{
    const auto graph = build_simple_graph();
    const auto stats = GraphAnalytics::compute_statistics(graph);

    REQUIRE(stats.node_count == 5);
    REQUIRE(stats.edge_count == 3);
    REQUIRE(stats.isolated_count == 1);       // E is isolated
    REQUIRE(stats.connected_components == 2); // {A,B,C,D} and {E}
    REQUIRE(stats.max_degree == 2);           // B and C have degree 2
    REQUIRE(stats.avg_degree > 1.0);
    REQUIRE(stats.density > 0.0);
    REQUIRE(stats.density < 1.0);
}

TEST_CASE("GraphAnalytics: compute_statistics on empty graph", "[phase16][analytics]")
{
    const auto graph = build_empty_graph();
    const auto stats = GraphAnalytics::compute_statistics(graph);

    REQUIRE(stats.node_count == 0);
    REQUIRE(stats.edge_count == 0);
    REQUIRE(stats.density == 0.0);
    REQUIRE(stats.avg_degree == 0.0);
}

TEST_CASE("GraphAnalytics: compute_statistics on single node", "[phase16][analytics]")
{
    const auto graph = build_single_node_graph();
    const auto stats = GraphAnalytics::compute_statistics(graph);

    REQUIRE(stats.node_count == 1);
    REQUIRE(stats.edge_count == 0);
    REQUIRE(stats.isolated_count == 1);
    REQUIRE(stats.connected_components == 1);
}

TEST_CASE("GraphAnalytics: compute_statistics on complete graph", "[phase16][analytics]")
{
    const auto graph = build_complete_graph();
    const auto stats = GraphAnalytics::compute_statistics(graph);

    REQUIRE(stats.node_count == 4);
    REQUIRE(stats.edge_count == 6);
    REQUIRE(stats.density > 0.99); // K4 density = 1.0
    REQUIRE(stats.isolated_count == 0);
    REQUIRE(stats.connected_components == 1);
}

TEST_CASE("GraphAnalytics: compute_centrality", "[phase16][analytics]")
{
    const auto graph = build_simple_graph();
    const auto centrality = GraphAnalytics::compute_centrality(graph);

    REQUIRE(centrality.size() == 5);

    // Find centrality for node B (should be high — hub node)
    const auto* node_b_centrality = [&]()
    {
        for (const auto& nc : centrality)
        {
            if (nc.node_id == "B")
            {
                return &nc;
            }
        }
        return static_cast<const NodeCentrality*>(nullptr);
    }();

    REQUIRE(node_b_centrality != nullptr);
    REQUIRE(node_b_centrality->degree_centrality > 0.0);
    REQUIRE(node_b_centrality->closeness_centrality > 0.0);

    // Node E should have zero centrality (isolated)
    const auto* node_e_centrality = [&]()
    {
        for (const auto& nc : centrality)
        {
            if (nc.node_id == "E")
            {
                return &nc;
            }
        }
        return static_cast<const NodeCentrality*>(nullptr);
    }();

    REQUIRE(node_e_centrality != nullptr);
    REQUIRE(node_e_centrality->degree_centrality == 0.0);
    REQUIRE(node_e_centrality->closeness_centrality == 0.0);
}

TEST_CASE("GraphAnalytics: build_tag_graph", "[phase16][analytics]")
{
    const auto graph = build_tagged_graph();
    const auto tag_graph = GraphAnalytics::build_tag_graph(graph);

    REQUIRE(tag_graph.node_count() >= 2); // physics, math
    REQUIRE(tag_graph.edge_count() >= 1); // physics-math co-occur in doc2

    // physics and math should co-occur (doc2 has both)
    bool found_cooccurrence = false;
    for (const auto& edge : tag_graph.edges)
    {
        if ((edge.source_tag == "physics" && edge.target_tag == "math") ||
            (edge.source_tag == "math" && edge.target_tag == "physics"))
        {
            found_cooccurrence = true;
            REQUIRE(edge.co_occurrence_count >= 1);
        }
    }
    REQUIRE(found_cooccurrence);
}

TEST_CASE("GraphAnalytics: compute_dashboard", "[phase16][analytics]")
{
    const auto graph = build_simple_graph();
    const auto dashboard = GraphAnalytics::compute_dashboard(graph, 3);

    REQUIRE(dashboard.statistics.node_count == 5);
    REQUIRE(!dashboard.top_connected.empty());
    REQUIRE(dashboard.recent_orphans.size() == 1); // E
    REQUIRE(dashboard.health_score >= 0.0);
    REQUIRE(dashboard.health_score <= 1.0);
}

// ============================================================================
// OrphanManager Unit Tests
// ============================================================================

TEST_CASE("OrphanManager: find_orphans", "[phase16][orphan]")
{
    const auto graph = build_simple_graph();
    const auto orphans = OrphanManager::find_orphans(graph);

    REQUIRE(orphans.size() == 1);
    REQUIRE(orphans[0].node_id == "E");
    REQUIRE(orphans[0].label == "Echo");
}

TEST_CASE("OrphanManager: find_orphans empty graph", "[phase16][orphan]")
{
    const auto graph = build_empty_graph();
    const auto orphans = OrphanManager::find_orphans(graph);
    REQUIRE(orphans.empty());
}

TEST_CASE("OrphanManager: find_orphans complete graph", "[phase16][orphan]")
{
    const auto graph = build_complete_graph();
    const auto orphans = OrphanManager::find_orphans(graph);
    REQUIRE(orphans.empty());
}

TEST_CASE("OrphanManager: suggest_connections via title mention", "[phase16][orphan]")
{
    const auto graph = build_simple_graph();
    // Content of node E mentions "Alpha" (label of node A)
    const auto suggestions = OrphanManager::suggest_connections(
        "E", graph, "This document references Alpha and some other topics");

    REQUIRE(!suggestions.empty());
    // Should find at least the "Alpha" title mention
    bool found_alpha = false;
    for (const auto& suggestion : suggestions)
    {
        if (suggestion.target_id == "A" && suggestion.reason == LinkSuggestionReason::kTitleMention)
        {
            found_alpha = true;
            REQUIRE(suggestion.confidence > 0.0);
        }
    }
    REQUIRE(found_alpha);
}

TEST_CASE("OrphanManager: orphan_ratio", "[phase16][orphan]")
{
    const auto graph = build_simple_graph();
    const auto ratio = OrphanManager::orphan_ratio(graph);
    REQUIRE(ratio > 0.15); // 1/5 = 0.2
    REQUIRE(ratio < 0.25);
}

TEST_CASE("OrphanManager: suggest_for_orphans", "[phase16][orphan]")
{
    const auto graph = build_simple_graph();
    std::unordered_map<std::string, std::string> contents;
    contents["E"] = "This talks about Alpha and Beta topics";

    const auto orphans = OrphanManager::suggest_for_orphans(graph, contents);
    REQUIRE(orphans.size() == 1);
    REQUIRE(!orphans[0].suggested_links.empty());
}

// ============================================================================
// GraphLayoutAlgorithms Unit Tests
// ============================================================================

TEST_CASE("CircularLayout: places nodes on circle", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    const auto layout = CircularLayout::apply(graph, 0.0, 0.0, 100.0);

    REQUIRE(layout.positions.size() == 5);
    // All nodes should be at distance ~100 from origin
    for (const auto& pos : layout.positions)
    {
        const double dist = std::sqrt(pos.pos_x * pos.pos_x + pos.pos_y * pos.pos_y);
        REQUIRE(dist > 99.0);
        REQUIRE(dist < 101.0);
    }
}

TEST_CASE("GridLayout: places nodes in grid", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    const auto layout = GridLayout::apply(graph, 3, 50.0);

    REQUIRE(layout.positions.size() == 5);
    // First node should be at (0, 0)
    REQUIRE(layout.positions[0].pos_x == 0.0);
    REQUIRE(layout.positions[0].pos_y == 0.0);

    // Node at index 3 should be at (0, 50) — col 0, row 1
    REQUIRE(layout.positions[3].pos_x == 0.0);
    REQUIRE(layout.positions[3].pos_y == 50.0);
}

TEST_CASE("RadialLayout: places center at origin", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    const auto layout = RadialLayout::apply(graph, "A", 80.0);

    // Node A should be at center
    const auto* pos_a = layout.find("A");
    REQUIRE(pos_a != nullptr);
    REQUIRE(pos_a->pos_x == 0.0);
    REQUIRE(pos_a->pos_y == 0.0);

    // Node B (depth 1) should be at ring 1
    const auto* pos_b = layout.find("B");
    REQUIRE(pos_b != nullptr);
    const double dist_b = std::sqrt(pos_b->pos_x * pos_b->pos_x + pos_b->pos_y * pos_b->pos_y);
    REQUIRE(dist_b > 79.0);
    REQUIRE(dist_b < 81.0);
}

TEST_CASE("HierarchicalLayout: top-down tree", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    const auto layout = HierarchicalLayout::apply(graph, "A", 100.0, 60.0);

    // Node A should be at top (y=0)
    const auto* pos_a = layout.find("A");
    REQUIRE(pos_a != nullptr);
    REQUIRE(pos_a->pos_y == 0.0);

    // Node B should be at depth 1
    const auto* pos_b = layout.find("B");
    REQUIRE(pos_b != nullptr);
    REQUIRE(pos_b->pos_y == 100.0);
}

TEST_CASE("RandomLayout: all nodes within bounds", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    const auto layout = RandomLayout::apply(graph, 200.0, 200.0, 42);

    for (const auto& pos : layout.positions)
    {
        REQUIRE(pos.pos_x >= -100.0);
        REQUIRE(pos.pos_x <= 100.0);
        REQUIRE(pos.pos_y >= -100.0);
        REQUIRE(pos.pos_y <= 100.0);
    }
}

TEST_CASE("RandomLayout: deterministic with same seed", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    const auto layout1 = RandomLayout::apply(graph, 200.0, 200.0, 42);
    const auto layout2 = RandomLayout::apply(graph, 200.0, 200.0, 42);

    for (size_t i = 0; i < layout1.positions.size(); ++i)
    {
        REQUIRE(layout1.positions[i].pos_x == layout2.positions[i].pos_x);
        REQUIRE(layout1.positions[i].pos_y == layout2.positions[i].pos_y);
    }
}

TEST_CASE("apply_layout factory dispatches correctly", "[phase16][layout]")
{
    const auto graph = build_simple_graph();
    LayoutParams params;
    params.center_x = 0.0;
    params.center_y = 0.0;
    params.radius = 100.0;

    const auto layout = apply_layout(graph, LayoutAlgorithm::kCircular, params);

    // Verify positions were generated
    REQUIRE(!layout.positions.empty());
    bool any_moved = false;
    for (const auto& pos : layout.positions)
    {
        if (pos.pos_x != 0.0 || pos.pos_y != 0.0)
        {
            any_moved = true;
            break;
        }
    }
    REQUIRE(any_moved);
}

TEST_CASE("CircularLayout: empty graph is noop", "[phase16][layout]")
{
    const auto graph = build_empty_graph();
    const auto layout = CircularLayout::apply(graph, 0.0, 0.0, 100.0);
    REQUIRE(layout.positions.empty());
}

// ============================================================================
// GraphAnnotations Unit Tests
// ============================================================================

TEST_CASE("GraphAnnotationManager: add and retrieve", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    const auto ann_id = manager.add_annotation("nodeA", "Important note", "yellow");

    REQUIRE(!ann_id.empty());
    REQUIRE(manager.annotation_count() == 1);

    const auto annotations = manager.get_annotations("nodeA");
    REQUIRE(annotations.size() == 1);
    REQUIRE(annotations[0].text == "Important note");
    REQUIRE(annotations[0].color == "yellow");
}

TEST_CASE("GraphAnnotationManager: update annotation", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    const auto ann_id = manager.add_annotation("nodeA", "Original text");
    REQUIRE(manager.update_annotation(ann_id, "Updated text"));

    const auto annotations = manager.get_annotations("nodeA");
    REQUIRE(annotations[0].text == "Updated text");
}

TEST_CASE("GraphAnnotationManager: remove annotation", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    const auto ann_id = manager.add_annotation("nodeA", "Test");
    REQUIRE(manager.remove_annotation(ann_id));
    REQUIRE(manager.annotation_count() == 0);
    REQUIRE(!manager.remove_annotation("nonexistent"));
}

TEST_CASE("GraphAnnotationManager: search annotations", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    manager.add_annotation("nodeA", "Physics notes");
    manager.add_annotation("nodeB", "Math homework");
    manager.add_annotation("nodeC", "Physics lab report");

    const auto results = manager.search_annotations("physics");
    REQUIRE(results.size() == 2);
}

TEST_CASE("GraphAnnotationManager: serialize and deserialize", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    manager.add_annotation("nodeA", "Note Alpha", "red");
    manager.add_annotation("nodeB", "Note Beta", "blue");

    const auto serialized = manager.serialize();
    REQUIRE(!serialized.empty());

    GraphAnnotationManager restored;
    restored.deserialize(serialized);
    REQUIRE(restored.annotation_count() == 2);

    const auto ann_a = restored.get_annotations("nodeA");
    REQUIRE(ann_a.size() == 1);
    REQUIRE(ann_a[0].text == "Note Alpha");
}

TEST_CASE("GraphAnnotationManager: clear_all", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    manager.add_annotation("nodeA", "Test1");
    manager.add_annotation("nodeB", "Test2");
    manager.clear_all();
    REQUIRE(manager.annotation_count() == 0);
}

TEST_CASE("GraphAnnotationManager: get_all_annotations", "[phase16][annotations]")
{
    GraphAnnotationManager manager;
    manager.add_annotation("nodeA", "Test1");
    manager.add_annotation("nodeB", "Test2");
    manager.add_annotation("nodeA", "Test3");

    const auto all = manager.get_all_annotations();
    REQUIRE(all.size() == 3);
}

// ============================================================================
// Task 19: GraphExporter Integration Tests
// ============================================================================

TEST_CASE("GraphExporter: export_to_json", "[phase16][exporter]")
{
    const auto graph = build_simple_graph();
    const auto json = GraphExporter::export_to_json(graph);

    REQUIRE(json.find("\"nodes\"") != std::string::npos);
    REQUIRE(json.find("\"links\"") != std::string::npos);
    REQUIRE(json.find("Alpha") != std::string::npos);
    REQUIRE(json.find("\"summary\"") != std::string::npos);
}

TEST_CASE("GraphExporter: export_to_csv", "[phase16][exporter]")
{
    const auto graph = build_simple_graph();
    const auto csv = GraphExporter::export_to_csv(graph);

    REQUIRE(csv.find("id,label,type,ref_count,notebook_id") != std::string::npos);
    REQUIRE(csv.find("---") != std::string::npos);
    REQUIRE(csv.find("source,target,type,label") != std::string::npos);
}

TEST_CASE("GraphExporter: export_to_markdown", "[phase16][exporter]")
{
    const auto graph = build_simple_graph();
    const auto markdown = GraphExporter::export_to_markdown(graph);

    REQUIRE(markdown.find("# Knowledge Graph Export") != std::string::npos);
    REQUIRE(markdown.find("**Nodes:**") != std::string::npos);
    REQUIRE(markdown.find("**Links:**") != std::string::npos);
    REQUIRE(markdown.find("Alpha") != std::string::npos);
}

TEST_CASE("GraphExporter: export_graph factory", "[phase16][exporter]")
{
    const auto graph = build_simple_graph();
    const auto json = GraphExporter::export_graph(graph, GraphExportFormat::kJson);
    const auto csv = GraphExporter::export_graph(graph, GraphExportFormat::kCsv);
    const auto markdown = GraphExporter::export_graph(graph, GraphExportFormat::kMarkdown);

    REQUIRE(!json.empty());
    REQUIRE(!csv.empty());
    REQUIRE(!markdown.empty());
}

TEST_CASE("GraphExporter: export empty graph", "[phase16][exporter]")
{
    const auto graph = build_empty_graph();
    const auto json = GraphExporter::export_to_json(graph);

    REQUIRE(json.find("\"nodes\": [") != std::string::npos);
    REQUIRE(json.find("\"node_count\":0") != std::string::npos);
}

// ============================================================================
// GraphSearch Integration Tests
// ============================================================================

TEST_CASE("GraphSearchEngine: search_nodes exact match", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "Alpha");

    REQUIRE(!results.empty());
    REQUIRE(results[0].node_id == "A");
    REQUIRE(results[0].match_type == GraphSearchMatchType::kLabelExact);
    REQUIRE(results[0].relevance == 1.0);
}

TEST_CASE("GraphSearchEngine: search_nodes prefix match", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "Alph");

    REQUIRE(!results.empty());
    REQUIRE(results[0].match_type == GraphSearchMatchType::kLabelPrefix);
}

TEST_CASE("GraphSearchEngine: search_nodes contains match", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "lph");

    REQUIRE(!results.empty());
    REQUIRE(results[0].match_type == GraphSearchMatchType::kLabelContains);
}

TEST_CASE("GraphSearchEngine: search_nodes case insensitive", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "alpha");

    REQUIRE(!results.empty());
    REQUIRE(results[0].node_id == "A");
}

TEST_CASE("GraphSearchEngine: search_nodes no results", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "Nonexistent");
    REQUIRE(results.empty());
}

TEST_CASE("GraphSearchEngine: search_nodes empty query", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "");
    REQUIRE(results.empty());
}

TEST_CASE("GraphSearchEngine: find_path shortest path", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto path = GraphSearchEngine::find_path(graph, "A", "D");

    REQUIRE(path.size() == 4); // A -> B -> C -> D
    REQUIRE(path[0] == "A");
    REQUIRE(path[1] == "B");
    REQUIRE(path[2] == "C");
    REQUIRE(path[3] == "D");
}

TEST_CASE("GraphSearchEngine: find_path same node", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto path = GraphSearchEngine::find_path(graph, "A", "A");

    REQUIRE(path.size() == 1);
    REQUIRE(path[0] == "A");
}

TEST_CASE("GraphSearchEngine: find_path no path (isolated)", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto path = GraphSearchEngine::find_path(graph, "A", "E");

    REQUIRE(path.empty());
}

TEST_CASE("GraphSearchEngine: find_all_paths", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto paths = GraphSearchEngine::find_all_paths(graph, "A", "D", 5);

    REQUIRE(!paths.empty());
    REQUIRE(paths[0].front() == "A");
    REQUIRE(paths[0].back() == "D");
}

TEST_CASE("GraphSearchEngine: find_connected_component", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto component = GraphSearchEngine::find_connected_component(graph, "A");

    REQUIRE(component.nodes.size() == 4); // A, B, C, D
    REQUIRE(component.links.size() == 3);
}

TEST_CASE("GraphSearchEngine: find_connected_component isolated node", "[phase16][search]")
{
    const auto graph = build_simple_graph();
    const auto component = GraphSearchEngine::find_connected_component(graph, "E");

    REQUIRE(component.nodes.size() == 1);
    REQUIRE(component.links.empty());
}

TEST_CASE("GraphSearchEngine: get_neighbors_at_depth", "[phase16][search]")
{
    const auto graph = build_simple_graph();

    const auto depth1 = GraphSearchEngine::get_neighbors_at_depth(graph, "A", 1);
    REQUIRE(depth1.size() == 1);
    REQUIRE(depth1[0] == "B");

    const auto depth2 = GraphSearchEngine::get_neighbors_at_depth(graph, "A", 2);
    REQUIRE(depth2.size() == 1);
    REQUIRE(depth2[0] == "C");

    const auto depth3 = GraphSearchEngine::get_neighbors_at_depth(graph, "A", 3);
    REQUIRE(depth3.size() == 1);
    REQUIRE(depth3[0] == "D");
}

// ============================================================================
// GraphEvolution Integration Tests
// ============================================================================

TEST_CASE("GraphEvolutionTracker: take_snapshot", "[phase16][evolution]")
{
    GraphEvolutionTracker tracker;
    const auto graph = build_simple_graph();
    const auto snapshot = tracker.take_snapshot(graph);

    REQUIRE(snapshot.node_count == 5);
    REQUIRE(snapshot.edge_count == 3);
    REQUIRE(snapshot.node_ids.size() == 5);
    REQUIRE(tracker.get_snapshot_count() == 1);
}

TEST_CASE("GraphEvolutionTracker: capacity enforcement", "[phase16][evolution]")
{
    GraphEvolutionTracker tracker;
    const auto graph = build_simple_graph();

    for (int i = 0; i < 60; ++i)
    {
        tracker.take_snapshot(graph);
    }

    REQUIRE(tracker.get_snapshot_count() == GraphEvolutionTracker::kMaxSnapshots);
}

TEST_CASE("GraphEvolutionTracker: compare_snapshots", "[phase16][evolution]")
{
    GraphEvolutionTracker tracker;

    // First snapshot: simple graph
    const auto graph1 = build_simple_graph();
    const auto snap1 = tracker.take_snapshot(graph1);

    // Second snapshot: add a node and edge
    auto graph2 = build_simple_graph();
    GraphNode new_node;
    new_node.id = "F";
    new_node.label = "Foxtrot";
    new_node.type = GraphNodeType::Document;
    graph2.nodes.push_back(new_node);

    GraphLink new_link;
    new_link.source_id = "E";
    new_link.target_id = "F";
    new_link.ref_type = GraphRefType::DocLink;
    graph2.links.push_back(new_link);

    const auto snap2 = tracker.take_snapshot(graph2);

    const auto diff = GraphEvolutionTracker::compare_snapshots(snap1, snap2);
    REQUIRE(diff.nodes_added.size() == 1);
    REQUIRE(diff.nodes_added[0] == "F");
    REQUIRE(diff.edges_added.size() == 1);
    REQUIRE(diff.node_count_delta == 1);
    REQUIRE(diff.edge_count_delta == 1);
}

TEST_CASE("GraphEvolutionTracker: generate_comparison_summary", "[phase16][evolution]")
{
    const auto graph1 = build_simple_graph();
    const auto graph2 = build_complete_graph();

    GraphEvolutionTracker tracker;
    const auto snap1 = tracker.take_snapshot(graph1);
    const auto snap2 = tracker.take_snapshot(graph2);

    const auto result = GraphEvolutionTracker::generate_comparison_summary(snap1, snap2);

    REQUIRE(!result.summary.empty());
    REQUIRE(result.summary.find("Added") != std::string::npos);
}

TEST_CASE("GraphEvolutionTracker: serialize and deserialize snapshot", "[phase16][evolution]")
{
    const auto graph = build_simple_graph();
    GraphEvolutionTracker tracker;
    const auto snapshot = tracker.take_snapshot(graph);

    const auto serialized = GraphEvolutionTracker::serialize_snapshot(snapshot);
    REQUIRE(!serialized.empty());
    REQUIRE(serialized.find("SNAPSHOT") != std::string::npos);

    const auto deserialized = GraphEvolutionTracker::deserialize_snapshot(serialized);
    REQUIRE(deserialized.node_count == snapshot.node_count);
    REQUIRE(deserialized.edge_count == snapshot.edge_count);
    REQUIRE(deserialized.node_ids.size() == snapshot.node_ids.size());
}

TEST_CASE("GraphEvolutionTracker: latest_snapshot", "[phase16][evolution]")
{
    GraphEvolutionTracker tracker;
    REQUIRE(tracker.latest_snapshot() == nullptr);

    const auto graph = build_simple_graph();
    tracker.take_snapshot(graph);
    REQUIRE(tracker.latest_snapshot() != nullptr);
    REQUIRE(tracker.latest_snapshot()->node_count == 5);
}

TEST_CASE("GraphEvolutionTracker: clear", "[phase16][evolution]")
{
    GraphEvolutionTracker tracker;
    const auto graph = build_simple_graph();
    tracker.take_snapshot(graph);
    tracker.clear();
    REQUIRE(tracker.get_snapshot_count() == 0);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST_CASE("Edge case: disconnected graph statistics", "[phase16][edge]")
{
    GraphData graph;
    for (int i = 0; i < 5; ++i)
    {
        GraphNode node;
        node.id = "iso_" + std::to_string(i);
        node.label = "Isolated " + std::to_string(i);
        node.type = GraphNodeType::Document;
        graph.nodes.push_back(node);
    }

    const auto stats = GraphAnalytics::compute_statistics(graph);
    REQUIRE(stats.isolated_count == 5);
    REQUIRE(stats.connected_components == 5);
    REQUIRE(stats.density == 0.0);
}

TEST_CASE("Edge case: unicode labels in search", "[phase16][edge]")
{
    GraphData graph;
    GraphNode node;
    node.id = "u1";
    node.label = "日本語ノート";
    node.type = GraphNodeType::Document;
    graph.nodes = {node};

    const auto results = GraphSearchEngine::search_nodes(graph, "日本語");
    REQUIRE(!results.empty());
}

TEST_CASE("Edge case: search in empty graph", "[phase16][edge]")
{
    const auto graph = build_empty_graph();
    const auto results = GraphSearchEngine::search_nodes(graph, "test");
    REQUIRE(results.empty());
}

TEST_CASE("Edge case: path in empty graph", "[phase16][edge]")
{
    const auto graph = build_empty_graph();
    const auto path = GraphSearchEngine::find_path(graph, "X", "Y");
    REQUIRE(path.empty());
}

TEST_CASE("Edge case: export single node graph", "[phase16][edge]")
{
    const auto graph = build_single_node_graph();
    const auto json = GraphExporter::export_to_json(graph);
    REQUIRE(json.find("Solo Node") != std::string::npos);
    REQUIRE(json.find("\"node_count\":1") != std::string::npos);
}

// ============================================================================
// GraphAccessibility Tests
// ============================================================================

TEST_CASE("GraphAccessibility: announce_graph_summary", "[phase16][accessibility]")
{
    GraphStatistics stats;
    stats.node_count = 42;
    stats.edge_count = 100;
    stats.connected_components = 3;
    stats.isolated_count = 5;

    const auto text = GraphAccessibility::announce_graph_summary(stats);
    REQUIRE(text.find("42 nodes") != std::string::npos);
    REQUIRE(text.find("100 connections") != std::string::npos);
    REQUIRE(text.find("3 connected components") != std::string::npos);
    REQUIRE(text.find("5 isolated nodes") != std::string::npos);
}

TEST_CASE("GraphAccessibility: announce_node_info", "[phase16][accessibility]")
{
    GraphNode node;
    node.id = "test1";
    node.label = "Test Document";
    node.type = GraphNodeType::Document;
    node.ref_count = 5;

    const auto text = GraphAccessibility::announce_node_info(node);
    REQUIRE(text.find("Document") != std::string::npos);
    REQUIRE(text.find("Test Document") != std::string::npos);
    REQUIRE(text.find("5 references") != std::string::npos);
}

TEST_CASE("GraphAccessibility: announce_navigation", "[phase16][accessibility]")
{
    const auto text = GraphAccessibility::announce_navigation("Alpha", "Beta");
    REQUIRE(text.find("Alpha") != std::string::npos);
    REQUIRE(text.find("Beta") != std::string::npos);
}

TEST_CASE("GraphAccessibility: get_graph_view_info", "[phase16][accessibility]")
{
    const auto info = GraphAccessibility::get_graph_view_info();
    REQUIRE(info.role == "img");
    REQUIRE(!info.label.empty());
    REQUIRE(!info.description.empty());
    REQUIRE(info.live_region == "polite");
}

TEST_CASE("GraphAccessibility: announce_search_results", "[phase16][accessibility]")
{
    REQUIRE(GraphAccessibility::announce_search_results("test", 0).find("0 results") !=
            std::string::npos);
    REQUIRE(GraphAccessibility::announce_search_results("test", 1).find("1 result") !=
            std::string::npos);
    REQUIRE(GraphAccessibility::announce_search_results("test", 5).find("5 results") !=
            std::string::npos);
}

// ============================================================================
// Task 20: Performance Tests
// ============================================================================

TEST_CASE("Performance: analytics on 1000-node graph", "[phase16][performance]")
{
    GraphData graph;
    for (int i = 0; i < 1000; ++i)
    {
        GraphNode node;
        node.id = "perf_" + std::to_string(i);
        node.label = "Performance Node " + std::to_string(i);
        node.type = GraphNodeType::Document;
        graph.nodes.push_back(node);
    }
    // Create ~2000 random-ish edges
    for (int i = 0; i < 2000; ++i)
    {
        GraphLink link;
        link.source_id = "perf_" + std::to_string(i % 1000);
        link.target_id = "perf_" + std::to_string((i * 7 + 13) % 1000);
        link.ref_type = GraphRefType::DocLink;
        graph.links.push_back(link);
    }

    const auto stats = GraphAnalytics::compute_statistics(graph);
    REQUIRE(stats.node_count == 1000);
    REQUIRE(stats.edge_count == 2000);
}

TEST_CASE("Performance: layout on 500-node graph", "[phase16][performance]")
{
    GraphData graph;
    for (int i = 0; i < 500; ++i)
    {
        GraphNode node;
        node.id = "layout_" + std::to_string(i);
        node.label = "Layout Node " + std::to_string(i);
        node.type = GraphNodeType::Document;
        graph.nodes.push_back(node);
    }

    const auto layout = CircularLayout::apply(graph, 0.0, 0.0, 500.0);

    // Verify all nodes placed
    REQUIRE(layout.positions.size() == 500);
    for (const auto& pos : layout.positions)
    {
        const double dist = std::sqrt(pos.pos_x * pos.pos_x + pos.pos_y * pos.pos_y);
        REQUIRE(dist > 499.0);
        REQUIRE(dist < 501.0);
    }
}

TEST_CASE("Performance: 100 snapshots with evolution tracking", "[phase16][performance]")
{
    GraphEvolutionTracker tracker;
    GraphData graph;

    for (int i = 0; i < 100; ++i)
    {
        GraphNode node;
        node.id = "evo_" + std::to_string(i);
        node.label = "Evo " + std::to_string(i);
        node.type = GraphNodeType::Document;
        graph.nodes.push_back(node);
        tracker.take_snapshot(graph);
    }

    REQUIRE(tracker.get_snapshot_count() == GraphEvolutionTracker::kMaxSnapshots);

    const auto snapshots = tracker.get_snapshots();
    const auto diff = GraphEvolutionTracker::compare_snapshots(snapshots.front(), snapshots.back());
    REQUIRE(diff.node_count_delta > 0);
}

TEST_CASE("Performance: search in 1000-node graph", "[phase16][performance]")
{
    GraphData graph;
    for (int i = 0; i < 1000; ++i)
    {
        GraphNode node;
        node.id = "search_" + std::to_string(i);
        node.label = "Searchable Document " + std::to_string(i);
        node.type = GraphNodeType::Document;
        graph.nodes.push_back(node);
    }

    const auto results = GraphSearchEngine::search_nodes(graph, "Searchable");
    REQUIRE(results.size() == 1000);
}

// ============================================================================
// Event Smoke Tests
// ============================================================================

TEST_CASE("Phase 16 events: construct and access fields", "[phase16][events]")
{
    GraphAnalyticsComputedEvent analytics_evt;
    analytics_evt.node_count = 42;
    analytics_evt.edge_count = 100;
    analytics_evt.density = 0.5;
    REQUIRE(analytics_evt.node_count == 42);

    OrphanDetectedEvent orphan_evt;
    orphan_evt.orphan_count = 3;
    orphan_evt.suggested_links_count = 10;
    REQUIRE(orphan_evt.orphan_count == 3);

    GraphAnnotationAddedEvent annotation_evt;
    annotation_evt.node_id = "nodeA";
    annotation_evt.annotation_id = "ann_1";
    REQUIRE(!annotation_evt.annotation_id.empty());

    GraphExportedEvent export_evt;
    export_evt.format = "json";
    export_evt.node_count = 50;
    REQUIRE(export_evt.format == "json");

    GraphSnapshotTakenEvent snapshot_evt;
    snapshot_evt.snapshot_index = 5;
    snapshot_evt.node_count = 100;
    REQUIRE(snapshot_evt.snapshot_index == 5);

    GraphSearchCompletedEvent search_evt;
    search_evt.query = "test";
    search_evt.result_count = 10;
    REQUIRE(search_evt.result_count == 10);
}
