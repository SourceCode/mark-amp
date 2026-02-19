// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ToolWindowModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_tool_window() -> ToolWindowModel
{
    ToolWindowModel model;
    model.set_problems({
        {"p1", "Missing semicolon", "src/main.cpp", 10, ProblemSeverity::kError, "C++", true},
        {"p2", "Unused variable", "src/utils.cpp", 5, ProblemSeverity::kWarning, "C++", false},
        {"p3", "Possible typo", "src/main.cpp", 20, ProblemSeverity::kInfo, "Spell", false},
        {"p4", "Deprecated API", "src/api.cpp", 15, ProblemSeverity::kWarning, "Lint", true},
    });
    model.set_channels({
        {"output", "Output", true},
        {"debug", "Debug Console", false},
        {"tasks", "Tasks", false},
    });
    return model;
}

TEST_CASE("Error and warning counts", "[toolwindow][problems]")
{
    auto model = make_test_tool_window();
    REQUIRE(model.error_count() == 1);
    REQUIRE(model.warning_count() == 2);
}

TEST_CASE("Filter by severity", "[toolwindow][problems]")
{
    auto model = make_test_tool_window();
    REQUIRE(model.by_severity(ProblemSeverity::kError).size() == 1);
    REQUIRE(model.by_severity(ProblemSeverity::kWarning).size() == 2);
}

TEST_CASE("Filter by source", "[toolwindow][problems]")
{
    auto model = make_test_tool_window();
    REQUIRE(model.by_source("C++").size() == 2);
    REQUIRE(model.by_source("Lint").size() == 1);
}

TEST_CASE("Quick fix count", "[toolwindow][problems]")
{
    auto model = make_test_tool_window();
    REQUIRE(model.quick_fix_count() == 2);
}

TEST_CASE("Active channel", "[toolwindow][output]")
{
    auto model = make_test_tool_window();
    const auto* active = model.active_channel();
    REQUIRE(active != nullptr);
    REQUIRE(active->name == "Output");

    model.set_active_channel("debug");
    REQUIRE(model.active_channel()->name == "Debug Console");
}

TEST_CASE("Follow tail and word wrap", "[toolwindow][output]")
{
    ToolWindowModel model;
    REQUIRE(model.follow_tail());
    REQUIRE_FALSE(model.word_wrap());
    model.set_follow_tail(false);
    model.set_word_wrap(true);
    REQUIRE_FALSE(model.follow_tail());
    REQUIRE(model.word_wrap());
}

TEST_CASE("Panel visibility and reset", "[toolwindow][layout]")
{
    ToolWindowModel model;
    REQUIRE(model.is_panel_visible("output"));
    model.set_panel_visible("output", false);
    REQUIRE_FALSE(model.is_panel_visible("output"));
    model.reset_layout();
    REQUIRE(model.is_panel_visible("output"));
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
