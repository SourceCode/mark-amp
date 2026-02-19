// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/SearchPanelModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_search() -> SearchPanelModel
{
    SearchPanelModel model;
    model.set_query({"TODO", false, false, false, "*.cpp", ""});
    model.set_matches({
        {"src/main.cpp", 10, "// TODO: fix this", 3, 4, true},
        {"src/main.cpp", 25, "// TODO: refactor", 3, 4, true},
        {"src/utils.cpp", 5, "// TODO: optimize", 3, 4, true},
        {"tests/test.cpp", 1, "// TODO: add tests", 3, 4, true},
    });
    return model;
}

TEST_CASE("Match count and file count", "[search][count]")
{
    auto model = make_test_search();
    REQUIRE(model.match_count() == 4);
    REQUIRE(model.file_count() == 3);
}

TEST_CASE("Results grouped by file", "[search][group]")
{
    auto model = make_test_search();
    const auto groups = model.grouped_results();
    REQUIRE(groups.size() == 3);
    REQUIRE(groups[0].file_path == "src/main.cpp");
    REQUIRE(groups[0].matches.size() == 2);
}

TEST_CASE("Toggle collapse on file group", "[search][collapse]")
{
    auto model = make_test_search();
    model.toggle_collapse("src/main.cpp");
    const auto groups = model.grouped_results();
    REQUIRE(groups[0].is_collapsed);
    model.toggle_collapse("src/main.cpp");
    const auto groups2 = model.grouped_results();
    REQUIRE_FALSE(groups2[0].is_collapsed);
}

TEST_CASE("Replace inclusion toggle", "[search][replace]")
{
    auto model = make_test_search();
    REQUIRE(model.included_count() == 4);
    model.toggle_match_inclusion("src/main.cpp", 10);
    REQUIRE(model.included_count() == 3);
}

TEST_CASE("Search history push and recall", "[search][history]")
{
    SearchPanelModel model;
    model.set_query({"first", false, false, false, "", ""});
    model.push_history();
    model.set_query({"second", true, false, false, "", ""});
    model.push_history();

    REQUIRE(model.history().size() == 2);
    REQUIRE(model.history()[0].text == "second"); // most recent first
}

TEST_CASE("Clear history", "[search][history]")
{
    SearchPanelModel model;
    model.set_query({"test", false, false, false, "", ""});
    model.push_history();
    model.clear_history();
    REQUIRE(model.history().empty());
}

TEST_CASE("Query options round-trip", "[search][query]")
{
    SearchPanelModel model;
    model.set_query({"pattern", true, true, true, "*.h", "vendor/*"});
    const auto& query = model.query();
    REQUIRE(query.is_regex);
    REQUIRE(query.case_sensitive);
    REQUIRE(query.whole_word);
    REQUIRE(query.include_paths == "*.h");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
