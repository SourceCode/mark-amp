/// @file test_graph_view_panel.cpp
/// @brief V4 Phase 08 – Graph View Panel controller tests.

#include "core/BacklinkIndex.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/GraphEngine.h"
#include "core/TagService.h"
#include "core/VaultService.h"
#include "ui/GraphViewPanel.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;

namespace
{

struct GraphViewTestFixture
{
    GraphViewTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_gv_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , backlink_index_(event_bus_, vault_service_)
        , tag_service_(event_bus_, vault_service_)
        , graph_engine_(event_bus_, vault_service_, backlink_index_, tag_service_)
        , controller_(event_bus_, graph_engine_, vault_service_)
    {
        fs::create_directories(vault_dir_);
    }

    ~GraphViewTestFixture()
    {
        fs::remove_all(vault_dir_);
    }

    auto create_file(const std::string& name, const std::string& content) const -> void
    {
        const fs::path file_path = vault_dir_ / name;
        fs::create_directories(file_path.parent_path());
        std::ofstream out(file_path);
        out << content;
    }

    auto open_and_build() -> void
    {
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
        backlink_index_.rebuild();
        tag_service_.rebuild();
        controller_.build_full_graph();
    }

    markamp::core::EventBus event_bus_;
    markamp::core::Config config_;
    fs::path vault_dir_;
    markamp::core::VaultService vault_service_;
    markamp::core::BacklinkIndex backlink_index_;
    markamp::core::TagService tag_service_;
    markamp::core::GraphEngine graph_engine_;
    markamp::core::GraphViewPanelController controller_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Graph builds with correct node count
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture, "Build full graph populates nodes", "[graphview][build]")
{
    create_file("doc1.md", "---\ntitle: Doc1\n---\n[[Doc2]] link.");
    create_file("doc2.md", "---\ntitle: Doc2\n---\n[[Doc3]] link.");
    create_file("doc3.md", "---\ntitle: Doc3\n---\nContent.");
    open_and_build();

    const auto& graph = controller_.graph_data();
    CHECK(graph.nodes.size() == 3);
    CHECK(graph.edges.size() >= 2);
}

// ============================================================================
// Test 2: Coordinate transform round-trip
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Graph-to-screen and screen-to-graph are inverse",
                 "[graphview][coords]")
{
    controller_.set_viewport_size(800, 600);

    // Place a point at graph (100, 200)
    auto [sx, sy] = controller_.graph_to_screen(100.0, 200.0);
    auto [gx, gy] = controller_.screen_to_graph(sx, sy);

    CHECK(std::abs(gx - 100.0) < 0.01);
    CHECK(std::abs(gy - 200.0) < 0.01);
}

// ============================================================================
// Test 3: Zoom changes viewport zoom level
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture, "Zoom at changes zoom level", "[graphview][zoom]")
{
    controller_.set_viewport_size(800, 600);
    const double initial_zoom = controller_.viewport().zoom;

    // Zoom in
    controller_.zoom_at(3.0, 400.0, 300.0);

    CHECK(controller_.viewport().zoom > initial_zoom);
}

// ============================================================================
// Test 4: Zoom is clamped to bounds
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture, "Zoom is clamped to min/max", "[graphview][zoom]")
{
    controller_.set_viewport_size(800, 600);

    // Zoom way out
    for (int step = 0; step < 100; ++step)
    {
        controller_.zoom_at(-5.0, 400.0, 300.0);
    }
    CHECK(controller_.viewport().zoom >= controller_.viewport().min_zoom);

    // Zoom way in
    for (int step = 0; step < 100; ++step)
    {
        controller_.zoom_at(5.0, 400.0, 300.0);
    }
    CHECK(controller_.viewport().zoom <= controller_.viewport().max_zoom);
}

// ============================================================================
// Test 5: Hit test on node
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Hit test finds node at its position",
                 "[graphview][hittest]")
{
    create_file("hit1.md", "---\ntitle: Hit1\n---\nContent.");
    open_and_build();

    const auto& graph = controller_.graph_data();
    REQUIRE_FALSE(graph.nodes.empty());

    // Place node at known position
    auto [screen_x, screen_y] = controller_.graph_to_screen(graph.nodes[0].x, graph.nodes[0].y);

    auto result = controller_.hit_test(screen_x, screen_y);
    CHECK(result.hit);
    CHECK(result.node_index == 0);
}

// ============================================================================
// Test 6: Hit test misses empty space
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Hit test returns no hit in empty space",
                 "[graphview][hittest]")
{
    create_file("miss1.md", "---\ntitle: Miss1\n---\nContent.");
    open_and_build();

    // Hit test at extreme coordinates
    auto result = controller_.hit_test(999999.0, 999999.0);
    CHECK_FALSE(result.hit);
}

// ============================================================================
// Test 7: Node selection
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Select node updates selection and publishes event",
                 "[graphview][selection]")
{
    create_file("sel1.md", "---\ntitle: Sel1\n---\nContent.");
    open_and_build();

    bool event_received = false;
    auto sub = event_bus_.subscribe<markamp::core::events::GraphNodeSelectedEvent>(
        [&](const markamp::core::events::GraphNodeSelectedEvent&) { event_received = true; });

    controller_.select_node(0);
    CHECK(controller_.selected_node_index() == 0);
    CHECK(event_received);

    controller_.clear_selection();
    CHECK(controller_.selected_node_index() == -1);
}

// ============================================================================
// Test 8: Node dragging moves position
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture, "Drag node moves its position", "[graphview][drag]")
{
    create_file("drag1.md", "---\ntitle: Drag1\n---\nContent.");
    open_and_build();

    const auto& graph = controller_.graph_data();
    REQUIRE_FALSE(graph.nodes.empty());

    auto [start_sx, start_sy] = controller_.graph_to_screen(graph.nodes[0].x, graph.nodes[0].y);

    const double original_x = graph.nodes[0].x;

    // Begin drag on the node
    controller_.begin_drag(start_sx, start_sy);
    CHECK(controller_.drag_state().is_dragging);

    // Move 50px to the right
    controller_.update_drag(start_sx + 50.0, start_sy);

    // Node should have moved in graph coords
    CHECK(graph.nodes[0].x != original_x);

    controller_.end_drag();
    CHECK_FALSE(controller_.drag_state().is_dragging);

    // Node velocity should be zeroed after drag
    CHECK(graph.nodes[0].vx == 0.0);
    CHECK(graph.nodes[0].vy == 0.0);
}

// ============================================================================
// Test 9: Pan drags viewport
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Pan on empty space moves viewport center",
                 "[graphview][pan]")
{
    controller_.set_viewport_size(800, 600);

    const double original_cx = controller_.viewport().center_x;

    // Begin drag in empty space (far from any node)
    controller_.begin_drag(999999.0, 999999.0);
    CHECK(controller_.drag_state().is_panning);

    controller_.update_drag(999999.0 + 100.0, 999999.0);

    CHECK(controller_.viewport().center_x != original_cx);

    controller_.end_drag();
}

// ============================================================================
// Test 10: Double click publishes events
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Double click on node publishes open event",
                 "[graphview][doubleclick]")
{
    create_file("dbl1.md", "---\ntitle: Dbl1\n---\nContent.");
    open_and_build();

    const auto& graph = controller_.graph_data();
    REQUIRE_FALSE(graph.nodes.empty());

    bool double_click_received = false;
    auto sub1 = event_bus_.subscribe<markamp::core::events::GraphNodeDoubleClickedEvent>(
        [&](const markamp::core::events::GraphNodeDoubleClickedEvent&)
        { double_click_received = true; });

    auto [sx, sy] = controller_.graph_to_screen(graph.nodes[0].x, graph.nodes[0].y);
    controller_.handle_double_click(sx, sy);

    CHECK(double_click_received);
}

// ============================================================================
// Test 11: Display config updates
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture, "Display config updates persist", "[graphview][config]")
{
    markamp::core::GraphDisplayConfig config;
    config.show_labels = false;
    config.curved_edges = true;
    config.edge_opacity = 0.8;

    controller_.set_display_config(config);

    CHECK_FALSE(controller_.display_config().show_labels);
    CHECK(controller_.display_config().curved_edges);
    CHECK(controller_.display_config().edge_opacity == 0.8);
}

// ============================================================================
// Test 12: Viewport resize
// ============================================================================
TEST_CASE_METHOD(GraphViewTestFixture,
                 "Set viewport size updates dimensions",
                 "[graphview][viewport]")
{
    controller_.set_viewport_size(1920, 1080);

    CHECK(controller_.viewport().viewport_width == 1920);
    CHECK(controller_.viewport().viewport_height == 1080);
}
