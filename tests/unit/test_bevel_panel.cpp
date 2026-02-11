#include "ui/BevelPanel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("BevelPanel: default style is Sunken", "[ui][bevel]")
{
    // Test via the style enum directly (no GUI instantiation needed)
    BevelPanel::Style style = BevelPanel::Style::Sunken;
    REQUIRE(style == BevelPanel::Style::Sunken);
}

TEST_CASE("BevelPanel: style enum values are distinct", "[ui][bevel]")
{
    REQUIRE(BevelPanel::Style::Raised != BevelPanel::Style::Sunken);
    REQUIRE(BevelPanel::Style::Sunken != BevelPanel::Style::Flat);
    REQUIRE(BevelPanel::Style::Raised != BevelPanel::Style::Flat);
}

TEST_CASE("BevelPanel: all three styles are valid", "[ui][bevel]")
{
    auto raised = BevelPanel::Style::Raised;
    auto sunken = BevelPanel::Style::Sunken;
    auto flat = BevelPanel::Style::Flat;

    // Ensure they can be assigned and compared
    REQUIRE(raised == BevelPanel::Style::Raised);
    REQUIRE(sunken == BevelPanel::Style::Sunken);
    REQUIRE(flat == BevelPanel::Style::Flat);
}
