// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/FindReplaceModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Status text shows N of M", "[findreplace][status]")
{
    FindReplaceModel model;
    model.set_match_count(5);
    REQUIRE(model.status_text() == "1 of 5");
}

TEST_CASE("Status text shows no results", "[findreplace][status]")
{
    FindReplaceModel model;
    REQUIRE(model.status_text() == "No results");
}

TEST_CASE("Next match wraps around", "[findreplace][nav]")
{
    FindReplaceModel model;
    model.set_match_count(3);
    model.next_match(); // -> 1
    model.next_match(); // -> 2
    model.next_match(); // -> 0 (wrap)
    REQUIRE(model.current_index() == 0);
    REQUIRE(model.status_text() == "1 of 3");
}

TEST_CASE("Prev match wraps around", "[findreplace][nav]")
{
    FindReplaceModel model;
    model.set_match_count(3);
    model.prev_match(); // -> 2 (wrap from 0)
    REQUIRE(model.current_index() == 2);
    REQUIRE(model.status_text() == "3 of 3");
}

TEST_CASE("Scope toggles", "[findreplace][scope]")
{
    FindReplaceModel model;
    model.set_scope(FindScope::kSelection);
    REQUIRE(model.scope() == FindScope::kSelection);
    model.set_scope(FindScope::kAllOpenFiles);
    REQUIRE(model.scope() == FindScope::kAllOpenFiles);
}

TEST_CASE("Replace all needs confirmation", "[findreplace][safety]")
{
    REQUIRE(FindReplaceModel::needs_confirmation(ReplaceAction::kReplaceAll));
    REQUIRE_FALSE(FindReplaceModel::needs_confirmation(ReplaceAction::kReplaceOne));
    REQUIRE_FALSE(FindReplaceModel::needs_confirmation(ReplaceAction::kReplaceInSelection));
}

TEST_CASE("Options round-trip", "[findreplace][options]")
{
    FindReplaceModel model;
    model.set_case_sensitive(true);
    model.set_regex(true);
    model.set_whole_word(true);
    REQUIRE(model.case_sensitive());
    REQUIRE(model.is_regex());
    REQUIRE(model.whole_word());
}

TEST_CASE("Match count reset clamps index", "[findreplace][match]")
{
    FindReplaceModel model;
    model.set_match_count(5);
    model.next_match();       // 1
    model.next_match();       // 2
    model.set_match_count(2); // index 2 >= count 2, reset to 0
    REQUIRE(model.current_index() == 0);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
