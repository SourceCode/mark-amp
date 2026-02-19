// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/BreadcrumbModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_breadcrumb() -> BreadcrumbModel
{
    BreadcrumbModel model;
    model.set_path({
        {"workspace", "workspace", BreadcrumbSegmentState::kNormal, {"project-a", "project-b"}},
        {"src", "src", BreadcrumbSegmentState::kNormal, {"tests", "docs", "build"}},
        {"ui", "ui", BreadcrumbSegmentState::kNormal, {"core", "utils"}},
        {"components", "components", BreadcrumbSegmentState::kNormal, {"hooks", "helpers"}},
        {"Button.tsx", "Button.tsx", BreadcrumbSegmentState::kActive, {}},
    });
    return model;
}

// ── Phase 14 Task 1: Segment state ──────────────────────────────────

TEST_CASE("Active segment is marked correctly", "[breadcrumb][state]")
{
    auto model = make_test_breadcrumb();
    model.set_active(2);
    REQUIRE(model.segments()[2].state == BreadcrumbSegmentState::kActive);
    REQUIRE(model.segments()[4].state == BreadcrumbSegmentState::kNormal);
}

TEST_CASE("Hover state set and cleared", "[breadcrumb][state]")
{
    auto model = make_test_breadcrumb();
    model.set_hovered(1);
    REQUIRE(model.segments()[1].state == BreadcrumbSegmentState::kHovered);
    model.clear_hover();
    REQUIRE(model.segments()[1].state == BreadcrumbSegmentState::kNormal);
}

// ── Phase 14 Task 2: Overflow truncation ────────────────────────────

TEST_CASE("No truncation when segments fit", "[breadcrumb][overflow]")
{
    auto model = make_test_breadcrumb();
    REQUIRE_FALSE(model.needs_truncation(10));
    REQUIRE(model.visible_segments(10).size() == 5);
}

TEST_CASE("Middle truncation uses ellipsis", "[breadcrumb][overflow]")
{
    auto model = make_test_breadcrumb();
    REQUIRE(model.needs_truncation(3));
    const auto visible = model.visible_segments(3);
    // first + ellipsis + last 1 = 3
    REQUIRE(visible.size() == 3);
    REQUIRE(visible[0].label == "workspace");
    REQUIRE(visible[1].label == "…");
    REQUIRE(visible[2].label == "Button.tsx");
}

// ── Phase 14 Task 3: Segment siblings ───────────────────────────────

TEST_CASE("Segments expose sibling list for menus", "[breadcrumb][siblings]")
{
    auto model = make_test_breadcrumb();
    REQUIRE(model.segments()[1].siblings.size() == 3);
    REQUIRE(model.segments()[1].siblings[0] == "tests");
}

// ── Phase 14 Task 4: Navigation history ─────────────────────────────

TEST_CASE("Navigation creates history entries", "[breadcrumb][history]")
{
    auto model = make_test_breadcrumb();
    model.navigate_to(4);
    model.navigate_to(2);

    REQUIRE(model.can_go_back());
    REQUIRE_FALSE(model.can_go_forward());
}

TEST_CASE("Back and forward navigation", "[breadcrumb][history]")
{
    auto model = make_test_breadcrumb();
    model.navigate_to(0);
    model.navigate_to(2);
    model.navigate_to(4);

    REQUIRE(model.go_back());
    REQUIRE(model.segments()[2].state == BreadcrumbSegmentState::kActive);

    REQUIRE(model.go_forward());
    REQUIRE(model.segments()[4].state == BreadcrumbSegmentState::kActive);
}

TEST_CASE("Navigate from middle truncates forward history", "[breadcrumb][history]")
{
    auto model = make_test_breadcrumb();
    model.navigate_to(0);
    model.navigate_to(2);
    model.navigate_to(4);

    model.go_back();      // at index 2
    model.navigate_to(1); // should truncate forward (4)

    REQUIRE_FALSE(model.can_go_forward());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
