// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/ContextMenuModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Scope based context", "[context][scope]")
{
    ContextMenuModel model;
    model.set_scope(ContextScope::kMultiSelect);
    REQUIRE(model.scope() == ContextScope::kMultiSelect);
}

TEST_CASE("Applicable action filtering", "[context][filter]")
{
    ContextMenuModel model;
    model.set_actions({
        {"copy", "Copy", "edit", true},
        {"crop", "Crop Image", "edit", false},
        {"delete", "Delete", "edit", true},
    });
    REQUIRE(model.applicable_actions().size() == 2);
}

TEST_CASE("Group based filtering", "[context][group]")
{
    ContextMenuModel model;
    model.set_actions({
        {"copy", "Copy", "edit", true},
        {"align", "Align", "arrange", true},
        {"delete", "Delete", "edit", true},
    });
    REQUIRE(model.actions_in_group("edit").size() == 2);
    REQUIRE(model.actions_in_group("arrange").size() == 1);
}

TEST_CASE("Object type tracking", "[context][type]")
{
    ContextMenuModel model;
    model.set_object_type("image");
    REQUIRE(model.object_type() == "image");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
