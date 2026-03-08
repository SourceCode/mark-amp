/// test_backlink.cpp — Unit tests for Backlink types
#include "core/Backlink.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// =============================================================================
// BacklinkItem struct tests
// =============================================================================

TEST_CASE("BacklinkItem: default type is DirectRef", "[backlink]")
{
    BacklinkItem item;
    REQUIRE(item.type == BacklinkType::DirectRef);
}

TEST_CASE("BacklinkItem: fields can be set", "[backlink]")
{
    BacklinkItem item;
    item.ref_block_id = {"20240101120000-aaaaaaa"};
    item.def_block_id = {"20240101120000-bbbbbbb"};
    item.ref_root_id = "root1";
    item.ref_doc_title = "My Document";
    item.ref_block_content = "Some content";
    item.context_before = "before";
    item.context_after = "after";
    item.type = BacklinkType::Mention;

    REQUIRE(item.ref_block_id.value == "20240101120000-aaaaaaa");
    REQUIRE(item.def_block_id.value == "20240101120000-bbbbbbb");
    REQUIRE(item.type == BacklinkType::Mention);
}

// =============================================================================
// MentionItem struct tests
// =============================================================================

TEST_CASE("MentionItem: default values", "[backlink]")
{
    MentionItem item;
    REQUIRE(item.source_block_id.value.empty());
    REQUIRE(item.matched_name.empty());
    REQUIRE(item.match_offset == 0);
    REQUIRE(item.match_length == 0);
}

// =============================================================================
// BacklinkTreeNode struct tests
// =============================================================================

TEST_CASE("BacklinkTreeNode: count returns number of backlinks", "[backlink]")
{
    BacklinkTreeNode node;
    node.doc_id = "doc1";
    node.doc_title = "Document 1";
    REQUIRE(node.count() == 0);

    BacklinkItem item;
    node.backlinks.push_back(item);
    REQUIRE(node.count() == 1);
}

// =============================================================================
// GraphNode struct tests
// =============================================================================

TEST_CASE("GraphNode: default values", "[backlink][graph]")
{
    GraphNode node;
    REQUIRE(node.block_id.value.empty());
    REQUIRE(node.label.empty());
    REQUIRE(node.node_type.empty());
    REQUIRE_FALSE(node.is_focal);
    REQUIRE(node.ref_count == 0);
}

// =============================================================================
// GraphEdge struct tests
// =============================================================================

TEST_CASE("GraphEdge: default ref type is BlockRef", "[backlink][graph]")
{
    GraphEdge edge;
    REQUIRE(edge.ref_type == RefType::BlockRef);
}

// =============================================================================
// GraphData struct tests
// =============================================================================

TEST_CASE("GraphData: default is empty", "[backlink][graph]")
{
    GraphData data;
    REQUIRE(data.nodes.empty());
    REQUIRE(data.edges.empty());
}

// =============================================================================
// BacklinkIndex tests
// =============================================================================

TEST_CASE("BacklinkIndex: empty index returns no backlinks", "[backlink][index]")
{
    BacklinkIndex index;
    BlockId bid{"20240101120000-abcdefg"};
    auto backlinks = index.get_backlinks(bid);
    REQUIRE(backlinks.empty());
    REQUIRE(index.backlink_count(bid) == 0);
}

TEST_CASE("BacklinkIndex: on_ref_added registers backlink", "[backlink][index]")
{
    BacklinkIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};
    index.on_ref_added(source, def, RefType::BlockRef);

    REQUIRE(index.backlink_count(def) == 1);
    auto backlinks = index.get_backlinks(def);
    REQUIRE(backlinks.size() == 1);
}

TEST_CASE("BacklinkIndex: on_ref_removed removes backlink", "[backlink][index]")
{
    BacklinkIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};
    index.on_ref_added(source, def, RefType::BlockRef);
    index.on_ref_removed(source, def);

    REQUIRE(index.backlink_count(def) == 0);
}

TEST_CASE("BacklinkIndex: clear empties all entries", "[backlink][index]")
{
    BacklinkIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};
    index.on_ref_added(source, def, RefType::BlockRef);
    index.clear();

    REQUIRE(index.backlink_count(def) == 0);
}

TEST_CASE("BacklinkIndex: get_tree_backlinks groups by source doc", "[backlink][index]")
{
    BacklinkIndex index;
    BlockId source{"20240101120000-aaaaaaa"};
    BlockId def{"20240101120000-bbbbbbb"};
    index.on_ref_added(source, def, RefType::BlockRef);

    auto tree = index.get_tree_backlinks(def);
    REQUIRE_FALSE(tree.empty());
}
