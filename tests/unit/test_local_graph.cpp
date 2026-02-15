/// @file test_local_graph.cpp
/// @brief V4 Phase 36 – LocalGraphEngine tests.

#include "core/EventBus.h"
#include "core/LocalGraphEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct GraphFixture
{
    EventBus event_bus;
    LocalGraphEngine engine{event_bus};

    GraphFixture()
    {
        // Build a small graph:
        // A -> B -> C -> D
        // A -> E
        // F (orphan)
        std::vector<NoteInfo> notes = {
            {"a", "Note A", {"project"}},
            {"b", "Note B", {"project", "code"}},
            {"c", "Note C", {"code"}},
            {"d", "Note D", {}},
            {"e", "Note E", {"project"}},
            {"f", "Note F", {}},
        };

        std::vector<NoteLink> links = {
            {"a", "b", "see also"},
            {"b", "c", "depends"},
            {"c", "d", "links to"},
            {"a", "e", "related"},
        };

        engine.load_notes(notes, links);
    }
};

TEST_CASE("LocalGraphEngine: build local graph depth 1", "[local_graph]")
{
    GraphFixture fixture;
    auto graph = fixture.engine.build_local_graph("a", 1);

    REQUIRE(graph.focus_id == "a");
    // Depth 1 from A: {A, B, E}
    REQUIRE(graph.nodes.size() == 3);
    REQUIRE(graph.edges.size() >= 2); // a->b, a->e
}

TEST_CASE("LocalGraphEngine: build local graph depth 2", "[local_graph]")
{
    GraphFixture fixture;
    auto graph = fixture.engine.build_local_graph("a", 2);

    // Depth 2 from A: {A, B, E, C}
    REQUIRE(graph.nodes.size() == 4);
}

TEST_CASE("LocalGraphEngine: focus node is marked", "[local_graph]")
{
    GraphFixture fixture;
    auto graph = fixture.engine.build_local_graph("a", 1);

    bool found_focus = false;
    for (const auto& node : graph.nodes)
    {
        if (node.note_id == "a")
        {
            REQUIRE(node.is_focus);
            REQUIRE(node.depth == 0);
            found_focus = true;
        }
    }
    REQUIRE(found_focus);
}

TEST_CASE("LocalGraphEngine: get neighbors", "[local_graph]")
{
    GraphFixture fixture;
    auto neighbors = fixture.engine.get_neighbors("a");

    // A -> B, A -> E (outgoing); no incoming
    REQUIRE(neighbors.size() == 2);
}

TEST_CASE("LocalGraphEngine: get neighbors bidirectional", "[local_graph]")
{
    GraphFixture fixture;
    auto neighbors = fixture.engine.get_neighbors("b");

    // B -> C (outgoing); A -> B (incoming)
    REQUIRE(neighbors.size() == 2);
}

TEST_CASE("LocalGraphEngine: get clusters", "[local_graph]")
{
    GraphFixture fixture;
    // Full graph: A-B-C-D-E connected, F isolated
    auto full_graph = fixture.engine.build_local_graph("a", 10);
    auto clusters = fixture.engine.get_clusters(full_graph);

    // F is not reachable from A, so full_graph won't include F
    // All nodes in the graph should be one cluster
    REQUIRE(clusters.size() >= 1);
}

TEST_CASE("LocalGraphEngine: filter by tag", "[local_graph]")
{
    GraphFixture fixture;
    auto graph = fixture.engine.build_local_graph("a", 2);
    auto filtered = fixture.engine.filter_by_tag(graph, "code");

    // Nodes with "code" tag: B, C
    REQUIRE(filtered.nodes.size() == 2);
}

TEST_CASE("LocalGraphEngine: highlight path", "[local_graph]")
{
    GraphFixture fixture;
    auto path = fixture.engine.highlight_path("a", "d");

    // Path: A -> B -> C -> D
    REQUIRE(path.size() == 4);
    REQUIRE(path.front() == "a");
    REQUIRE(path.back() == "d");
}

TEST_CASE("LocalGraphEngine: no path returns empty", "[local_graph]")
{
    GraphFixture fixture;
    auto path = fixture.engine.highlight_path("a", "f");

    // F is isolated, no path from A
    REQUIRE(path.empty());
}

TEST_CASE("LocalGraphEngine: node and edge counts", "[local_graph]")
{
    GraphFixture fixture;

    REQUIRE(fixture.engine.node_count() == 6);
    REQUIRE(fixture.engine.edge_count() == 4);
}
