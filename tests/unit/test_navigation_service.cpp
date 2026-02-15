/// @file test_navigation_service.cpp
/// @brief V4 Phase 20 – NavigationService tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/VaultService.h"
#include "ui/NavigationService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

namespace
{
struct TestFixture
{
    EventBus bus;
    Config config;
    VaultService vault{bus, config};
    NavigationService nav{bus, vault};
};
} // namespace

// ============================================================================
// Test 1: Navigate same pane
// ============================================================================
TEST_CASE("Navigate to document records history", "[nav][navigate]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(1, "doc_b");

    auto& hist = f.nav.history_for(1);
    CHECK(hist.size() == 2);
    CHECK(hist.current()->document_id == "doc_b");
}

// ============================================================================
// Test 2: Go back
// ============================================================================
TEST_CASE("Go back returns previous entry", "[nav][back]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(1, "doc_b");

    auto entry = f.nav.go_back(1);
    REQUIRE(entry != nullptr);
    CHECK(entry->document_id == "doc_a");
}

// ============================================================================
// Test 3: Go forward
// ============================================================================
TEST_CASE("Go forward after back", "[nav][forward]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(1, "doc_b");
    f.nav.go_back(1);

    auto entry = f.nav.go_forward(1);
    REQUIRE(entry != nullptr);
    CHECK(entry->document_id == "doc_b");
}

// ============================================================================
// Test 4: History truncation on new navigate
// ============================================================================
TEST_CASE("New navigation truncates forward history", "[nav][truncate]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(1, "doc_b");
    f.nav.navigate_to(1, "doc_c");
    f.nav.go_back(1); // Now at doc_b
    f.nav.go_back(1); // Now at doc_a

    // Navigate somewhere new — should truncate doc_b and doc_c
    f.nav.navigate_to(1, "doc_d");

    auto& hist = f.nav.history_for(1);
    CHECK(hist.size() == 2); // doc_a + doc_d
    CHECK_FALSE(hist.can_go_forward());
}

// ============================================================================
// Test 5: Wikilink follow (unresolved)
// ============================================================================
TEST_CASE("Follow unresolved wikilink returns false", "[nav][wikilink]")
{
    TestFixture f;
    auto resolved = f.nav.follow_wikilink(1, "NonExistent");
    CHECK_FALSE(resolved);
}

// ============================================================================
// Test 6: Can resolve
// ============================================================================
TEST_CASE("Can resolve checks vault", "[nav][resolve]")
{
    TestFixture f;
    // No vault open, so nothing should resolve
    CHECK_FALSE(f.nav.can_resolve("SomeNote"));
}

// ============================================================================
// Test 7: Per-pane isolation
// ============================================================================
TEST_CASE("Per-pane history is isolated", "[nav][perpane]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(2, "doc_b");
    f.nav.navigate_to(1, "doc_c");

    auto& hist1 = f.nav.history_for(1);
    auto& hist2 = f.nav.history_for(2);

    CHECK(hist1.size() == 2); // doc_a, doc_c
    CHECK(hist2.size() == 1); // doc_b
}

// ============================================================================
// Test 8: Breadcrumb trail
// ============================================================================
TEST_CASE("Breadcrumb trail returns document IDs", "[nav][breadcrumb]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(1, "doc_b");
    f.nav.navigate_to(1, "doc_c");

    auto trail = f.nav.breadcrumb_trail(1);
    REQUIRE(trail.size() == 3);
    // Since no vault is open, breadcrumbs fall back to document_id
    CHECK(trail[0] == "doc_a");
    CHECK(trail[1] == "doc_b");
    CHECK(trail[2] == "doc_c");
}

// ============================================================================
// Test 9: Cannot go back from start
// ============================================================================
TEST_CASE("Cannot go back from first entry", "[nav][backstart]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");

    auto& hist = f.nav.history_for(1);
    CHECK_FALSE(hist.can_go_back());

    auto entry = f.nav.go_back(1);
    CHECK(entry == nullptr);
}

// ============================================================================
// Test 10: Cannot go forward from end
// ============================================================================
TEST_CASE("Cannot go forward from last entry", "[nav][forwardend]")
{
    TestFixture f;
    f.nav.navigate_to(1, "doc_a");
    f.nav.navigate_to(1, "doc_b");

    auto& hist = f.nav.history_for(1);
    CHECK_FALSE(hist.can_go_forward());

    auto entry = f.nav.go_forward(1);
    CHECK(entry == nullptr);
}
