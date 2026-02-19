// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/IntegrityModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Integrity issues and error counting", "[integrity][issues]")
{
    IntegrityModel model;
    model.add_issue({"obj-1", "bounds", "Negative width", ValidationSeverity::kError});
    model.add_issue({"obj-2", "transform", "NaN rotation", ValidationSeverity::kWarning});
    model.add_issue({"obj-3", "fill", "Missing color", ValidationSeverity::kError});
    REQUIRE(model.issues().size() == 3);
    REQUIRE(model.error_count() == 2);
    REQUIRE(model.warning_count() == 1);
    REQUIRE_FALSE(model.is_valid());
}

TEST_CASE("Valid when no errors", "[integrity][valid]")
{
    IntegrityModel model;
    model.add_issue({"obj-1", "style", "Deprecated prop", ValidationSeverity::kWarning});
    REQUIRE(model.is_valid()); // warnings don't block
}

TEST_CASE("Command precondition check", "[integrity][precondition]")
{
    IntegrityModel model;
    model.set_precondition_result(false, "Selection is empty");
    REQUIRE_FALSE(model.precondition_passed());
    REQUIRE(model.precondition_reason() == "Selection is empty");
}

TEST_CASE("Board consistency scan counts", "[integrity][scan]")
{
    IntegrityModel model;
    model.set_orphan_count(3);
    model.set_invalid_link_count(1);
    REQUIRE(model.orphan_count() == 3);
    REQUIRE(model.invalid_link_count() == 1);
}

TEST_CASE("Clear issues resets state", "[integrity][clear]")
{
    IntegrityModel model;
    model.add_issue({"x", "y", "z", ValidationSeverity::kError});
    model.clear_issues();
    REQUIRE(model.is_valid());
    REQUIRE(model.issues().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
