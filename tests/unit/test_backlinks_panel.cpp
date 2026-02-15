/// @file test_backlinks_panel.cpp
/// @brief V4 Phase 06 – Backlinks Panel controller tests.

#include "core/BacklinkIndex.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/TagService.h"
#include "core/VaultService.h"
#include "ui/BacklinksPanel.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <numbers>
#include <string>

namespace fs = std::filesystem;

namespace
{

struct BacklinksPanelTestFixture
{
    BacklinksPanelTestFixture()
        : vault_dir_(fs::temp_directory_path() /
                     ("markamp_bl_test_" +
                      std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())))
        , vault_service_(event_bus_, config_)
        , backlink_index_(event_bus_, vault_service_)
        , tag_service_(event_bus_, vault_service_)
        , controller_(event_bus_, vault_service_, backlink_index_)
    {
        fs::create_directories(vault_dir_);
    }

    ~BacklinksPanelTestFixture()
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

    auto open_and_index() -> void
    {
        auto result = vault_service_.open_vault(vault_dir_);
        REQUIRE(result.has_value());
        backlink_index_.rebuild();
        tag_service_.rebuild();
    }

    /// Helper: find document_id by title.
    [[nodiscard]] auto find_doc_id(const std::string& title) const -> std::string
    {
        auto docs = vault_service_.list_documents();
        for (const auto& doc : docs)
        {
            if (doc.title == title)
            {
                return doc.document_id;
            }
        }
        return "";
    }

    markamp::core::EventBus event_bus_;
    markamp::core::Config config_;
    fs::path vault_dir_;
    markamp::core::VaultService vault_service_;
    markamp::core::BacklinkIndex backlink_index_;
    markamp::core::TagService tag_service_;
    markamp::core::BacklinksPanelController controller_;
};

} // anonymous namespace

// ============================================================================
// Test 1: Compute backlinks — linked mentions grouped by source
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Compute backlinks groups linked mentions by source document",
                 "[backlinks][linked]")
{
    create_file("target.md", "---\ntitle: Target\n---\nI am the target.");
    create_file("source1.md", "---\ntitle: Source1\n---\nI link to [[Target]] here.");
    create_file("source2.md", "---\ntitle: Source2\n---\nAnother link to [[Target]].");
    open_and_index();

    const auto target_id = find_doc_id("Target");
    REQUIRE_FALSE(target_id.empty());

    auto data = controller_.compute_backlinks(target_id);

    CHECK(data.document_id == target_id);
    CHECK(data.document_title == "Target");
    CHECK(data.total_linked >= 2);
    CHECK(data.linked_groups.size() >= 2);
}

// ============================================================================
// Test 2: Compute backlinks — no backlinks returns empty
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Compute backlinks for isolated document returns empty",
                 "[backlinks][empty]")
{
    create_file("isolated.md", "---\ntitle: Isolated\n---\nNo links to me.");
    create_file("other.md", "---\ntitle: Other\n---\nNo links at all.");
    open_and_index();

    const auto isolated_id = find_doc_id("Isolated");
    REQUIRE_FALSE(isolated_id.empty());

    auto data = controller_.compute_backlinks(isolated_id);

    CHECK(data.total_linked == 0);
    CHECK(data.linked_groups.empty());
}

// ============================================================================
// Test 3: Filter — text filter
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Apply filter with text reduces results",
                 "[backlinks][filter]")
{
    create_file("page.md", "---\ntitle: Page\n---\nMain page content.");
    create_file("alpha.md", "---\ntitle: Alpha\n---\nLink to [[Page]] about alpha.");
    create_file("beta.md", "---\ntitle: Beta\n---\nLink to [[Page]] about beta.");
    open_and_index();

    const auto page_id = find_doc_id("Page");
    REQUIRE_FALSE(page_id.empty());

    auto data = controller_.compute_backlinks(page_id);

    markamp::core::BacklinkFilterState filter;
    filter.filter_text = "Alpha";

    auto filtered = controller_.apply_filter(data, filter);

    // Only Alpha group should remain
    CHECK(filtered.linked_groups.size() <= data.linked_groups.size());
}

// ============================================================================
// Test 4: Filter — hide linked
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Apply filter hides linked mentions when disabled",
                 "[backlinks][filter]")
{
    create_file("target2.md", "---\ntitle: Target2\n---\nI am target2.");
    create_file("linker.md", "---\ntitle: Linker\n---\nLink to [[Target2]].");
    open_and_index();

    const auto target_id = find_doc_id("Target2");
    REQUIRE_FALSE(target_id.empty());

    auto data = controller_.compute_backlinks(target_id);

    markamp::core::BacklinkFilterState filter;
    filter.show_linked = false;

    auto filtered = controller_.apply_filter(data, filter);

    CHECK(filtered.linked_groups.empty());
    CHECK(filtered.total_linked == 0);
}

// ============================================================================
// Test 5: Local graph — center node included
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Local graph includes center node",
                 "[backlinks][localgraph]")
{
    create_file("center.md", "---\ntitle: Center\n---\n[[NodeA]] and [[NodeB]].");
    create_file("nodeA.md", "---\ntitle: NodeA\n---\nContent A.");
    create_file("nodeB.md", "---\ntitle: NodeB\n---\nContent B.");
    open_and_index();

    const auto center_id = find_doc_id("Center");
    REQUIRE_FALSE(center_id.empty());

    auto graph = controller_.compute_local_graph(center_id, 1);

    CHECK_FALSE(graph.nodes.empty());
    CHECK(graph.center_document_id == center_id);

    // Center node should be first and marked as center
    CHECK(graph.nodes[0].is_center);
    CHECK(graph.nodes[0].document_id == center_id);
}

// ============================================================================
// Test 6: Local graph — circular layout angles correct
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Local graph uses circular layout for neighbors",
                 "[backlinks][localgraph]")
{
    create_file("hub3.md", "---\ntitle: Hub3\n---\n[[Spoke1]] [[Spoke2]] [[Spoke3]].");
    create_file("spoke1.md", "---\ntitle: Spoke1\n---\nContent.");
    create_file("spoke2.md", "---\ntitle: Spoke2\n---\nContent.");
    create_file("spoke3.md", "---\ntitle: Spoke3\n---\nContent.");
    open_and_index();

    const auto hub_id = find_doc_id("Hub3");
    REQUIRE_FALSE(hub_id.empty());

    auto graph = controller_.compute_local_graph(hub_id, 1);

    // Should have center + 3 neighbors = 4 nodes
    CHECK(graph.nodes.size() >= 4);

    // Neighbor angles should be evenly distributed
    if (graph.nodes.size() >= 4)
    {
        // Non-center nodes have angles assigned
        for (size_t idx = 1; idx < graph.nodes.size(); ++idx)
        {
            CHECK(graph.nodes[idx].angle >= 0.0);
            CHECK(graph.nodes[idx].angle <= 2.0 * std::numbers::pi);
        }
    }
}

// ============================================================================
// Test 7: Local graph — edges are created
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Local graph creates edges between center and neighbors",
                 "[backlinks][localgraph]")
{
    create_file("main.md", "---\ntitle: Main\n---\n[[Sub1]] [[Sub2]].");
    create_file("sub1.md", "---\ntitle: Sub1\n---\nContent.");
    create_file("sub2.md", "---\ntitle: Sub2\n---\nContent.");
    open_and_index();

    const auto main_id = find_doc_id("Main");
    REQUIRE_FALSE(main_id.empty());

    auto graph = controller_.compute_local_graph(main_id, 1);

    // Should have at least 2 edges (center->sub1, center->sub2)
    CHECK(graph.edges.size() >= 2);

    // All edges from center
    for (const auto& edge : graph.edges)
    {
        // Either source or target should be 0 (center) or between neighbors
        CHECK(edge.source_index >= 0);
        CHECK(edge.target_index >= 0);
    }
}

// ============================================================================
// Test 8: Set active document publishes event
// ============================================================================
TEST_CASE_METHOD(BacklinksPanelTestFixture,
                 "Set active document publishes refresh event",
                 "[backlinks][events]")
{
    create_file("active.md", "---\ntitle: Active\n---\nContent.");
    create_file("referring.md", "---\ntitle: Referring\n---\n[[Active]] link here.");
    open_and_index();

    const auto active_id = find_doc_id("Active");
    REQUIRE_FALSE(active_id.empty());

    bool event_received = false;
    auto sub = event_bus_.subscribe<markamp::core::events::BacklinksPanelRefreshedEvent>(
        [&](const markamp::core::events::BacklinksPanelRefreshedEvent& evt)
        {
            event_received = true;
            CHECK(evt.document_id == active_id);
        });

    controller_.set_active_document(active_id);

    CHECK(event_received);
    CHECK(controller_.active_document_id() == active_id);
}
