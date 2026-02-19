// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/NavigationModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Push and current location", "[navigation][history]")
{
    NavigationModel model;
    model.push_location({"file1", "main.cpp", "editor"});
    REQUIRE(model.current() != nullptr);
    REQUIRE(model.current()->label == "main.cpp");
}

TEST_CASE("Back and forward navigation", "[navigation][history]")
{
    NavigationModel model;
    model.push_location({"f1", "File A", "editor"});
    model.push_location({"f2", "File B", "editor"});
    model.push_location({"f3", "File C", "canvas"});

    REQUIRE(model.can_go_back());
    REQUIRE_FALSE(model.can_go_forward());

    const auto* back1 = model.go_back();
    REQUIRE(back1->label == "File B");

    const auto* back2 = model.go_back();
    REQUIRE(back2->label == "File A");

    REQUIRE_FALSE(model.can_go_back());
    REQUIRE(model.can_go_forward());

    const auto* fwd = model.go_forward();
    REQUIRE(fwd->label == "File B");
}

TEST_CASE("Push truncates forward history", "[navigation][history]")
{
    NavigationModel model;
    model.push_location({"f1", "A", "editor"});
    model.push_location({"f2", "B", "editor"});
    model.push_location({"f3", "C", "editor"});

    model.go_back();                            // -> B
    model.go_back();                            // -> A
    model.push_location({"f4", "D", "editor"}); // truncates B and C

    REQUIRE_FALSE(model.can_go_forward());
    REQUIRE(model.current()->label == "D");
}

TEST_CASE("Targets by type", "[navigation][jump]")
{
    NavigationModel model;
    model.set_targets({
        {"t1", "main.cpp", NavTargetType::kFile, "editor", 10},
        {"t2", "Introduction", NavTargetType::kHeading, "editor", 5},
        {"t3", "parseConfig", NavTargetType::kSymbol, "editor", 8},
        {"t4", "Ideas", NavTargetType::kGraphNode, "graph", 3},
    });

    REQUIRE(model.targets_by_type(NavTargetType::kFile).size() == 1);
    REQUIRE(model.targets_by_type(NavTargetType::kHeading).size() == 1);
}

TEST_CASE("MRU targets sorted by use count", "[navigation][jump]")
{
    NavigationModel model;
    model.set_targets({
        {"t1", "Low", NavTargetType::kFile, "editor", 2},
        {"t2", "High", NavTargetType::kFile, "editor", 10},
        {"t3", "Mid", NavTargetType::kFile, "editor", 5},
    });

    const auto mru = model.mru_targets();
    REQUIRE(mru[0].label == "High");
    REQUIRE(mru[1].label == "Mid");
    REQUIRE(mru[2].label == "Low");
}

TEST_CASE("Breadcrumb sync", "[navigation][breadcrumb]")
{
    NavigationModel model;
    model.set_breadcrumb({"Project", "src", "main.cpp"});
    REQUIRE(model.breadcrumb().size() == 3);
    REQUIRE(model.breadcrumb()[2] == "main.cpp");
}

TEST_CASE("Surface tracking", "[navigation][surface]")
{
    NavigationModel model;
    model.set_active_surface("editor");
    REQUIRE(model.active_surface() == "editor");
    model.set_active_surface("canvas");
    REQUIRE(model.active_surface() == "canvas");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
