/// @file test_pane_manager.cpp
/// @brief V4 Phase 19 – PaneManager tests (logic only).

#include "core/Config.h"
#include "core/EventBus.h"
#include "ui/PaneManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

namespace
{
struct TestFixture
{
    EventBus bus;
    Config config;
    PaneManager mgr{bus, config};
};
} // namespace

// ============================================================================
// Test 1: Initial state
// ============================================================================
TEST_CASE("Initial single pane", "[pane][init]")
{
    TestFixture f;
    CHECK(f.mgr.pane_count() == 1);
    CHECK(f.mgr.root()->type == SplitNode::Type::kLeaf);
    CHECK(f.mgr.active_pane().id > 0);
}

// ============================================================================
// Test 2: Horizontal split
// ============================================================================
TEST_CASE("Horizontal split creates two panes", "[pane][hsplit]")
{
    TestFixture f;
    auto original = f.mgr.active_pane();
    auto new_pane = f.mgr.split_active(SplitDirection::kHorizontal);

    CHECK(f.mgr.pane_count() == 2);
    CHECK(new_pane != original);
    CHECK(f.mgr.root()->type == SplitNode::Type::kSplit);
    CHECK(f.mgr.root()->direction == SplitDirection::kHorizontal);
}

// ============================================================================
// Test 3: Vertical split
// ============================================================================
TEST_CASE("Vertical split creates two panes", "[pane][vsplit]")
{
    TestFixture f;
    auto new_pane = f.mgr.split_active(SplitDirection::kVertical);

    CHECK(f.mgr.pane_count() == 2);
    CHECK(f.mgr.root()->direction == SplitDirection::kVertical);
    (void)new_pane; // Used implicitly
}

// ============================================================================
// Test 4: Close pane
// ============================================================================
TEST_CASE("Close pane merges sibling up", "[pane][close]")
{
    TestFixture f;
    auto new_pane = f.mgr.split_active(SplitDirection::kHorizontal);
    CHECK(f.mgr.pane_count() == 2);

    auto closed = f.mgr.close_pane(new_pane);
    CHECK(closed);
    CHECK(f.mgr.pane_count() == 1);
    CHECK(f.mgr.root()->type == SplitNode::Type::kLeaf);
}

// ============================================================================
// Test 5: Cannot close last pane
// ============================================================================
TEST_CASE("Cannot close last pane", "[pane][closelast]")
{
    TestFixture f;
    auto original = f.mgr.active_pane();
    auto closed = f.mgr.close_pane(original);
    CHECK_FALSE(closed);
    CHECK(f.mgr.pane_count() == 1);
}

// ============================================================================
// Test 6: Nested split
// ============================================================================
TEST_CASE("Nested split creates 3 panes", "[pane][nested]")
{
    TestFixture f;
    f.mgr.split_active(SplitDirection::kHorizontal);
    CHECK(f.mgr.pane_count() == 2);

    // Split the active pane again (it's still the original)
    f.mgr.split_active(SplitDirection::kVertical);
    CHECK(f.mgr.pane_count() == 3);
}

// ============================================================================
// Test 7: Focus cycling
// ============================================================================
TEST_CASE("Focus cycles through panes", "[pane][focus]")
{
    TestFixture f;
    auto original = f.mgr.active_pane();
    f.mgr.split_active(SplitDirection::kHorizontal);

    auto next = f.mgr.focus_next_pane();
    CHECK(next != original);

    auto back = f.mgr.focus_prev_pane();
    CHECK(back == original);
}

// ============================================================================
// Test 8: Open document in pane
// ============================================================================
TEST_CASE("Open document in specific pane", "[pane][open]")
{
    TestFixture f;
    auto pane = f.mgr.active_pane();
    f.mgr.open_in_pane(pane, "doc1");
    f.mgr.open_in_pane(pane, "doc2");

    auto states = f.mgr.pane_states();
    REQUIRE(states.size() == 1);
    CHECK(states[0].open_document_ids.size() == 2);
    CHECK(states[0].active_document_id == "doc2");
}

// ============================================================================
// Test 9: Move document between panes
// ============================================================================
TEST_CASE("Move document between panes", "[pane][move]")
{
    TestFixture f;
    auto pane1 = f.mgr.active_pane();
    f.mgr.open_in_pane(pane1, "doc1");

    auto pane2 = f.mgr.split_active(SplitDirection::kHorizontal);

    auto moved = f.mgr.move_document("doc1", pane1, pane2);
    CHECK(moved);

    auto states = f.mgr.pane_states();
    // Find pane2 state
    for (const auto& state : states)
    {
        if (state.pane_id == pane2)
        {
            CHECK(state.active_document_id == "doc1");
        }
    }
}

// ============================================================================
// Test 10: Serialization roundtrip
// ============================================================================
TEST_CASE("Serialize and restore layout", "[pane][serialize]")
{
    TestFixture f;
    f.mgr.split_active(SplitDirection::kHorizontal);
    CHECK(f.mgr.pane_count() == 2);

    auto json = f.mgr.serialize_layout();
    CHECK(!json.empty());
    CHECK(json.find("pane_count") != std::string::npos);

    // Restore validates basic structure
    CHECK(f.mgr.restore_layout(json));
    CHECK_FALSE(f.mgr.restore_layout(""));
}
