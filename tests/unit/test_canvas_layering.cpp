// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/LayeringModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

static auto make_layers() -> LayeringModel
{
    LayeringModel model;
    model.set_layers({
        {"obj-a", "Shape A", 0, LayerState::kNormal},
        {"obj-b", "Shape B", 1, LayerState::kNormal},
        {"obj-c", "Shape C", 2, LayerState::kNormal},
    });
    return model;
}

TEST_CASE("Bring forward swaps with next", "[layer][zorder]")
{
    auto model = make_layers();
    model.bring_forward("obj-a");
    REQUIRE(model.layers()[0].object_id == "obj-b");
    REQUIRE(model.layers()[1].object_id == "obj-a");
}

TEST_CASE("Send backward swaps with prev", "[layer][zorder]")
{
    auto model = make_layers();
    model.send_backward("obj-c");
    REQUIRE(model.layers()[1].object_id == "obj-c");
    REQUIRE(model.layers()[2].object_id == "obj-b");
}

TEST_CASE("Bring to front", "[layer][zorder]")
{
    auto model = make_layers();
    model.bring_to_front("obj-a");
    REQUIRE(model.layers().back().object_id == "obj-a");
}

TEST_CASE("Send to back", "[layer][zorder]")
{
    auto model = make_layers();
    model.send_to_back("obj-c");
    REQUIRE(model.layers().front().object_id == "obj-c");
}

TEST_CASE("Lock and hidden states", "[layer][state]")
{
    auto model = make_layers();
    model.set_state("obj-a", LayerState::kLocked);
    model.set_state("obj-b", LayerState::kHidden);
    REQUIRE(model.is_locked("obj-a"));
    REQUIRE(model.is_hidden("obj-b"));
    REQUIRE_FALSE(model.is_locked("obj-c"));
}

TEST_CASE("Selectable filters hidden", "[layer][filter]")
{
    auto model = make_layers();
    model.set_state("obj-b", LayerState::kHidden);
    const auto selectable = model.selectable();
    REQUIRE(selectable.size() == 2);
}

TEST_CASE("Selectable optionally includes locked", "[layer][filter]")
{
    auto model = make_layers();
    model.set_state("obj-a", LayerState::kLocked);
    REQUIRE(model.selectable(false).size() == 2);
    REQUIRE(model.selectable(true).size() == 3);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
