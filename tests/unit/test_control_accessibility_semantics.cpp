// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/AccessibilityModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_controls() -> AccessibilityModel
{
    AccessibilityModel model;
    model.set_controls({
        {"btn-save", "Save", ControlRole::kButton, "", true},
        {"chk-wrap", "Word Wrap", ControlRole::kCheckbox, "unchecked", true},
        {"icon-status", "", ControlRole::kStatus, "", false},
        {"tab-editor", "Editor", ControlRole::kTab, "selected", true},
        {"sep-1", "", ControlRole::kToolbar, "", false},
    });
    return model;
}

TEST_CASE("Control lookup by ID", "[a11y][control]")
{
    auto model = make_test_controls();
    const auto* ctrl = model.control_by_id("btn-save");
    REQUIRE(ctrl != nullptr);
    REQUIRE(ctrl->accessible_name == "Save");
    REQUIRE(ctrl->role == ControlRole::kButton);
}

TEST_CASE("Unnamed controls audit", "[a11y][audit]")
{
    auto model = make_test_controls();
    const auto unnamed = model.unnamed_controls();
    REQUIRE(unnamed.size() == 2);
}

TEST_CASE("Non-focusable controls audit", "[a11y][audit]")
{
    auto model = make_test_controls();
    const auto non_focus = model.non_focusable();
    REQUIRE(non_focus.size() == 2);
}

TEST_CASE("Polite and assertive announcements", "[a11y][announce]")
{
    AccessibilityModel model;
    model.announce("File saved", false);
    model.announce("Error: disk full", true);

    REQUIRE(model.pending_announcements().size() == 2);
    REQUIRE_FALSE(model.pending_announcements()[0].is_assertive);
    REQUIRE(model.pending_announcements()[1].is_assertive);

    model.clear_announcements();
    REQUIRE(model.pending_announcements().empty());
}

TEST_CASE("High contrast mode", "[a11y][mode]")
{
    AccessibilityModel model;
    REQUIRE_FALSE(model.high_contrast());
    model.set_high_contrast(true);
    REQUIRE(model.high_contrast());
}

TEST_CASE("Reduced complexity mode", "[a11y][mode]")
{
    AccessibilityModel model;
    REQUIRE_FALSE(model.reduced_complexity());
    model.set_reduced_complexity(true);
    REQUIRE(model.reduced_complexity());
}

TEST_CASE("Missing control returns nullptr", "[a11y][control]")
{
    AccessibilityModel model;
    model.set_controls({});
    REQUIRE(model.control_by_id("nonexistent") == nullptr);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
