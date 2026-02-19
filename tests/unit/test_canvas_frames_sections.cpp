// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/FrameContainerModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Child containment CRUD", "[frame][container]")
{
    FrameContainerModel model;
    model.add_child({"obj-1", 10.0, 20.0});
    model.add_child({"obj-2", 30.0, 40.0});
    REQUIRE(model.child_count() == 2);
    REQUIRE(model.contains("obj-1"));
    model.remove_child("obj-1");
    REQUIRE(model.child_count() == 1);
    REQUIRE_FALSE(model.contains("obj-1"));
}

TEST_CASE("Frame styling", "[frame][style]")
{
    FrameContainerModel model;
    model.set_style({"My Frame", "#AABBCC", "dashed", 2.0});
    REQUIRE(model.style().title == "My Frame");
    REQUIRE(model.style().tint_color == "#AABBCC");
}

TEST_CASE("Frame navigation wraps", "[frame][nav]")
{
    FrameContainerModel model;
    model.set_frame_ids({"f1", "f2", "f3"});
    model.set_active_frame("f3");
    REQUIRE(model.next_frame() == "f1"); // wrap
    model.set_active_frame("f1");
    REQUIRE(model.prev_frame() == "f3"); // wrap
}

TEST_CASE("Frame navigation sequential", "[frame][nav]")
{
    FrameContainerModel model;
    model.set_frame_ids({"f1", "f2", "f3"});
    model.set_active_frame("f1");
    REQUIRE(model.next_frame() == "f2");
    model.set_active_frame("f3");
    REQUIRE(model.prev_frame() == "f2");
}

TEST_CASE("Selection mode toggle", "[frame][select]")
{
    FrameContainerModel model;
    REQUIRE(model.select_mode() == ContainerSelectMode::kContainerOnly);
    model.set_select_mode(ContainerSelectMode::kSelectChildren);
    REQUIRE(model.select_mode() == ContainerSelectMode::kSelectChildren);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
