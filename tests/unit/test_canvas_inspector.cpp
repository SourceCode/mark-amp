// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/InspectorModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Section visibility", "[inspector][sections]")
{
    InspectorModel model;
    model.set_visible_sections({InspectorSection::kTransform, InspectorSection::kAppearance});
    REQUIRE(model.is_section_visible(InspectorSection::kTransform));
    REQUIRE_FALSE(model.is_section_visible(InspectorSection::kConnector));
}

TEST_CASE("Multi-select detection", "[inspector][select]")
{
    InspectorModel model;
    model.set_selected_count(1);
    REQUIRE_FALSE(model.is_multi_select());
    model.set_selected_count(3);
    REQUIRE(model.is_multi_select());
}

TEST_CASE("Property get/set with mixed", "[inspector][property]")
{
    InspectorModel model;
    model.set_property("width", {"100", false});
    REQUIRE(model.property("width").value == "100");
    model.set_property("color", {"", true}); // mixed state
    REQUIRE(model.property("color").is_mixed);
}

TEST_CASE("Property validation", "[inspector][validate]")
{
    InspectorModel model;
    model.set_constraint("opacity", 0.0, 1.0);
    REQUIRE(model.validate("opacity", 0.5) == ValidationResult::kValid);
    REQUIRE(model.validate("opacity", 1.5) == ValidationResult::kOutOfRange);
    REQUIRE(model.validate("opacity", -0.1) == ValidationResult::kOutOfRange);
}

TEST_CASE("Unconstrained property always valid", "[inspector][validate]")
{
    InspectorModel model;
    REQUIRE(model.validate("anything", 9999.0) == ValidationResult::kValid);
}

TEST_CASE("Style copy flag", "[inspector][copy]")
{
    InspectorModel model;
    REQUIRE_FALSE(model.has_copied_style());
    model.copy_style();
    REQUIRE(model.has_copied_style());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
