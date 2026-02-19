// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/RegressionHarnessModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Snapshot hash match", "[regression][snapshot]")
{
    RegressionHarnessModel model;
    model.set_baselines({
        {"btn-save", ControlState::kDefault, "abc123", true},
        {"btn-save", ControlState::kHover, "def456", true},
    });

    REQUIRE(model.is_snapshot_current("btn-save", ControlState::kDefault, "abc123"));
    REQUIRE_FALSE(model.is_snapshot_current("btn-save", ControlState::kDefault, "changed"));
}

TEST_CASE("Missing baseline returns false", "[regression][snapshot]")
{
    RegressionHarnessModel model;
    model.set_baselines({});
    REQUIRE_FALSE(model.is_snapshot_current("missing", ControlState::kDefault, "abc"));
}

TEST_CASE("Stale baselines detection", "[regression][snapshot]")
{
    RegressionHarnessModel model;
    model.set_baselines({
        {"btn-a", ControlState::kDefault, "aaa", true},
        {"btn-b", ControlState::kDefault, "bbb", false},
        {"btn-c", ControlState::kHover, "ccc", false},
    });
    REQUIRE(model.stale_baselines().size() == 2);
}

TEST_CASE("Keyboard workflow tracking", "[regression][keyboard]")
{
    RegressionHarnessModel model;
    model.set_keyboard_steps({
        {"s1", "Open file", "Ctrl+O", false},
        {"s2", "Save file", "Ctrl+S", false},
        {"s3", "Close tab", "Ctrl+W", false},
    });

    REQUIRE_FALSE(model.all_keyboard_steps_passed());
    model.mark_step_passed("s1");
    model.mark_step_passed("s2");
    model.mark_step_passed("s3");
    REQUIRE(model.all_keyboard_steps_passed());
}

TEST_CASE("Keyboard pass rate", "[regression][keyboard]")
{
    RegressionHarnessModel model;
    model.set_keyboard_steps({
        {"s1", "Step 1", "Tab", false},
        {"s2", "Step 2", "Enter", false},
    });
    model.mark_step_passed("s1");
    REQUIRE(model.keyboard_pass_rate() == 0.5);
}

TEST_CASE("CI gate status", "[regression][ci]")
{
    RegressionHarnessModel model;
    REQUIRE_FALSE(model.ci_gate_passing());
    model.set_ci_gate_passing(true);
    REQUIRE(model.ci_gate_passing());
}

TEST_CASE("Empty keyboard steps all pass", "[regression][keyboard]")
{
    RegressionHarnessModel model;
    model.set_keyboard_steps({});
    REQUIRE(model.all_keyboard_steps_passed());
    REQUIRE(model.keyboard_pass_rate() == 1.0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
