// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/DialogModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Button ordering: Primary first, Cancel last", "[dialog][order]")
{
    DialogModel model;
    model.set_buttons({
        {"Cancel", DialogButtonRole::kCancel, false, false},
        {"Save", DialogButtonRole::kPrimary, true, false},
        {"Don't Save", DialogButtonRole::kSecondary, false, false},
    });

    const auto ordered = model.ordered_buttons();
    REQUIRE(ordered[0].label == "Save");
    REQUIRE(ordered[2].label == "Cancel");
}

TEST_CASE("Default button index", "[dialog][default]")
{
    DialogModel model;
    model.set_buttons({
        {"Cancel", DialogButtonRole::kCancel, false, false},
        {"OK", DialogButtonRole::kPrimary, true, false},
    });

    REQUIRE(model.default_button_index() == 0); // OK is primary, sorted first
}

TEST_CASE("Danger severity for destructive actions", "[dialog][severity]")
{
    DialogModel model;
    model.set_severity(DialogSeverity::kDanger);
    REQUIRE(model.severity() == DialogSeverity::kDanger);
}

TEST_CASE("Prefer undo for info-level only", "[dialog][undo]")
{
    REQUIRE(DialogModel::prefer_undo(DialogSeverity::kInfo));
    REQUIRE_FALSE(DialogModel::prefer_undo(DialogSeverity::kWarning));
    REQUIRE_FALSE(DialogModel::prefer_undo(DialogSeverity::kDanger));
}

TEST_CASE("Remember and retrieve choice", "[dialog][remember]")
{
    DialogModel model;
    model.remember_choice("delete_confirm", 1);

    REQUIRE(model.has_remembered("delete_confirm"));
    REQUIRE(model.remembered_choice("delete_confirm") == 1);
}

TEST_CASE("Unknown dialog key returns -1", "[dialog][remember]")
{
    DialogModel model;
    REQUIRE(model.remembered_choice("unknown") == -1);
    REQUIRE_FALSE(model.has_remembered("unknown"));
}

TEST_CASE("Reset specific remembered choice", "[dialog][remember]")
{
    DialogModel model;
    model.remember_choice("delete_confirm", 1);
    model.remember_choice("overwrite_confirm", 0);

    model.reset_remembered("delete_confirm");
    REQUIRE_FALSE(model.has_remembered("delete_confirm"));
    REQUIRE(model.has_remembered("overwrite_confirm"));
}

TEST_CASE("Reset all remembered choices", "[dialog][remember]")
{
    DialogModel model;
    model.remember_choice("delete_confirm", 1);
    model.remember_choice("overwrite_confirm", 0);

    model.reset_all_remembered();
    REQUIRE_FALSE(model.has_remembered("delete_confirm"));
    REQUIRE_FALSE(model.has_remembered("overwrite_confirm"));
}

TEST_CASE("Dialog content getters", "[dialog][content]")
{
    DialogModel model;
    model.set_content("Delete File?", "This action cannot be undone.");
    REQUIRE(model.title() == "Delete File?");
    REQUIRE(model.message() == "This action cannot be undone.");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
