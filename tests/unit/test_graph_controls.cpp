// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/GraphControlModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_graph() -> GraphControlModel
{
    GraphControlModel model;
    model.set_presets({
        {"all", "All Notes", 3, true, ""},
        {"tagged", "Tagged Only", 2, false, "project"},
    });
    model.set_nodes({
        {"n1", "Note A", 5, false},
        {"n2", "Note B", 3, false},
        {"n3", "Note C", 1, false},
    });
    model.set_backlinks({
        {"src/b.md", "links to A", 10},
        {"src/a.md", "links to B", 5},
        {"src/a.md", "also links", 2},
    });
    return model;
}

TEST_CASE("Filter preset lookup", "[graph][preset]")
{
    auto model = make_test_graph();
    const auto* preset = model.preset_by_id("tagged");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->name == "Tagged Only");
    REQUIRE(model.preset_by_id("unknown") == nullptr);
}

TEST_CASE("Add and remove preset", "[graph][preset]")
{
    auto model = make_test_graph();
    model.add_preset({"custom", "Custom", 1, false, ""});
    REQUIRE(model.presets().size() == 3);
    model.remove_preset("custom");
    REQUIRE(model.presets().size() == 2);
}

TEST_CASE("Layout mode", "[graph][layout]")
{
    auto model = make_test_graph();
    model.set_layout(GraphLayout::kRadial);
    REQUIRE(model.layout() == GraphLayout::kRadial);
}

TEST_CASE("Depth clamped to minimum 1", "[graph][depth]")
{
    GraphControlModel model;
    model.set_depth(0);
    REQUIRE(model.depth() == 1);
    model.set_depth(5);
    REQUIRE(model.depth() == 5);
}

TEST_CASE("Node selection by ID", "[graph][node]")
{
    auto model = make_test_graph();
    model.select_node("n2");
    const auto* selected = model.selected_node();
    REQUIRE(selected != nullptr);
    REQUIRE(selected->label == "Note B");
}

TEST_CASE("Keyboard node traversal", "[graph][node]")
{
    auto model = make_test_graph();
    model.select_next(); // -> n1 (index 0)
    REQUIRE(model.selected_node()->label == "Note A");
    model.select_next(); // -> n2
    model.select_next(); // -> n3
    model.select_next(); // -> n1 (wrap)
    REQUIRE(model.selected_node()->label == "Note A");
    model.select_prev(); // -> n3 (wrap back)
    REQUIRE(model.selected_node()->label == "Note C");
}

TEST_CASE("Backlinks sorted by file then line", "[graph][backlink]")
{
    auto model = make_test_graph();
    const auto sorted = model.sorted_backlinks();
    REQUIRE(sorted[0].source_file == "src/a.md");
    REQUIRE(sorted[0].line_number == 2);
    REQUIRE(sorted[1].line_number == 5);
    REQUIRE(sorted[2].source_file == "src/b.md");
}

TEST_CASE("Backlink count", "[graph][backlink]")
{
    auto model = make_test_graph();
    REQUIRE(model.backlink_count() == 3);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
