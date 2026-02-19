// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/SplitterModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ── Phase 15 Task 1: Drag clamping ──────────────────────────────────

TEST_CASE("Position clamps to min/max", "[splitter][clamp]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);

    model.set_position(50);
    REQUIRE(model.position() == 100);

    model.set_position(900);
    REQUIRE(model.position() == 800);
}

// ── Phase 15 Task 2: Snap points ────────────────────────────────────

TEST_CASE("Snap point magnetism", "[splitter][snap]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);
    model.set_snap_points({{300, 10}, {500, 10}});

    model.set_position(297);
    REQUIRE(model.position() == 300); // Snapped

    model.set_position(200);
    REQUIRE(model.position() == 200); // No snap
}

TEST_CASE("Double-click resets to default", "[splitter][reset]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);
    model.set_position(600);
    REQUIRE(model.position() == 600);

    model.reset_to_default();
    REQUIRE(model.position() == 400);
}

// ── Phase 15 Task 3: Keyboard resize ────────────────────────────────

TEST_CASE("Keyboard resize increments", "[splitter][keyboard]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);
    model.set_keyboard_step(20);

    model.resize_increase();
    REQUIRE(model.position() == 420);

    model.resize_decrease();
    REQUIRE(model.position() == 400);
}

TEST_CASE("Keyboard resize clamps at boundary", "[splitter][keyboard]")
{
    SplitterModel model;
    model.set_constraints(100, 110, 100);
    model.set_keyboard_step(20);

    model.resize_increase();
    REQUIRE(model.position() == 110);

    model.resize_decrease();
    model.resize_decrease();
    REQUIRE(model.position() == 100);
}

// ── Phase 15 Task 4: Drag lifecycle ─────────────────────────────────

TEST_CASE("Drag state lifecycle", "[splitter][drag]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);

    REQUIRE(model.state() == SplitterState::kIdle);

    model.begin_drag();
    REQUIRE(model.state() == SplitterState::kDragging);

    model.update_drag(300);
    REQUIRE(model.position() == 300);

    model.end_drag();
    REQUIRE(model.state() == SplitterState::kIdle);
}

TEST_CASE("Drag respects constraints", "[splitter][drag]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);

    model.begin_drag();
    model.update_drag(50);
    REQUIRE(model.position() == 100); // Clamped

    model.update_drag(900);
    REQUIRE(model.position() == 800); // Clamped
    model.end_drag();
}

// ── Constraint queries ──────────────────────────────────────────────

TEST_CASE("Constraint getters", "[splitter][constraints]")
{
    SplitterModel model;
    model.set_constraints(100, 800, 400);

    REQUIRE(model.min_position() == 100);
    REQUIRE(model.max_position() == 800);
    REQUIRE(model.default_position() == 400);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
