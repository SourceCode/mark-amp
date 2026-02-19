// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/MultiSelectModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_items() -> MultiSelectModel
{
    MultiSelectModel model;
    model.set_items({
        {"a", "File A", false},
        {"b", "File B", false},
        {"c", "File C", false},
        {"d", "File D", false},
        {"e", "File E", false},
    });
    return model;
}

TEST_CASE("Single select replaces selection", "[multiselect][select]")
{
    auto model = make_test_items();
    model.select("b");
    REQUIRE(model.selected_count() == 1);
    model.select("d");
    REQUIRE(model.selected_count() == 1);
    REQUIRE(model.selected_ids()[0] == "d");
}

TEST_CASE("Toggle select adds/removes", "[multiselect][select]")
{
    auto model = make_test_items();
    model.select("a", SelectionModifier::kToggle);
    model.select("c", SelectionModifier::kToggle);
    REQUIRE(model.selected_count() == 2);
    model.select("a", SelectionModifier::kToggle);
    REQUIRE(model.selected_count() == 1);
}

TEST_CASE("Extend select creates range", "[multiselect][select]")
{
    auto model = make_test_items();
    model.select("b");
    model.select("d", SelectionModifier::kExtend);
    REQUIRE(model.selected_count() == 3); // b, c, d
}

TEST_CASE("Select all and deselect all", "[multiselect][select]")
{
    auto model = make_test_items();
    model.select_all();
    REQUIRE(model.selected_count() == 5);
    model.deselect_all();
    REQUIRE(model.selected_count() == 0);
}

TEST_CASE("Bulk actions depend on count", "[multiselect][bulk]")
{
    auto model = make_test_items();
    REQUIRE(model.bulk_actions().empty());

    model.select("a");
    REQUIRE(model.bulk_actions().size() == 3);
    REQUIRE(model.bulk_actions()[0] == "Open");

    model.select("b", SelectionModifier::kToggle);
    REQUIRE(model.bulk_actions().size() == 3);
    REQUIRE(model.bulk_actions()[0] == "Move");
}

TEST_CASE("Status text formatting", "[multiselect][status]")
{
    auto model = make_test_items();
    REQUIRE(model.status_text() == "No items selected");
    model.select("a");
    REQUIRE(model.status_text() == "1 item selected");
    model.select_all();
    REQUIRE(model.status_text() == "5 items selected");
}

TEST_CASE("Safe destructive confirmation flow", "[multiselect][destructive]")
{
    auto model = make_test_items();
    REQUIRE_FALSE(model.has_pending_destructive());
    model.request_destructive("Delete");
    REQUIRE(model.has_pending_destructive());
    REQUIRE(model.pending_destructive() == "Delete");
    model.cancel_destructive();
    REQUIRE_FALSE(model.has_pending_destructive());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
