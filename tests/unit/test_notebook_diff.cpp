/// @file test_notebook_diff.cpp
/// @brief V4 Phase 40 – NotebookDiffEngine tests.

#include "core/EventBus.h"
#include "core/NotebookDiffEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

struct DiffFixture
{
    EventBus event_bus;
    NotebookDiffEngine engine{event_bus};
};

TEST_CASE("NotebookDiffEngine: identical notebooks", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> cells = {
        {"c1", "code", "x = 1", 1},
        {"c2", "markdown", "# Title", 0},
    };

    auto result = fixture.engine.diff_notebooks(cells, cells);
    REQUIRE(result.cells_unchanged == 2);
    REQUIRE(result.cells_added == 0);
    REQUIRE(result.cells_removed == 0);
    REQUIRE(result.cells_modified == 0);
}

TEST_CASE("NotebookDiffEngine: cell added", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> cells_a = {
        {"c1", "code", "x = 1", 1},
    };

    std::vector<DiffCell> cells_b = {
        {"c1", "code", "x = 1", 1},
        {"c2", "code", "y = 2", 2},
    };

    auto result = fixture.engine.diff_notebooks(cells_a, cells_b);
    REQUIRE(result.cells_unchanged == 1);
    REQUIRE(result.cells_added == 1);
}

TEST_CASE("NotebookDiffEngine: cell removed", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> cells_a = {
        {"c1", "code", "x = 1", 1},
        {"c2", "code", "y = 2", 2},
    };

    std::vector<DiffCell> cells_b = {
        {"c1", "code", "x = 1", 1},
    };

    auto result = fixture.engine.diff_notebooks(cells_a, cells_b);
    REQUIRE(result.cells_unchanged == 1);
    REQUIRE(result.cells_removed == 1);
}

TEST_CASE("NotebookDiffEngine: cell similarity", "[notebook_diff]")
{
    DiffFixture fixture;

    DiffCell cell_a{"c1", "code", "x = 1 + 2", 1};
    DiffCell cell_b{"c2", "code", "x = 1 + 3", 2};
    DiffCell cell_c{"c3", "code", "completely different code here", 3};

    double sim_ab = NotebookDiffEngine::cell_similarity(cell_a, cell_b);
    double sim_ac = NotebookDiffEngine::cell_similarity(cell_a, cell_c);

    // a and b share tokens, should be more similar.
    REQUIRE(sim_ab > sim_ac);
    REQUIRE(sim_ab > 0.3);
}

TEST_CASE("NotebookDiffEngine: diff cell content", "[notebook_diff]")
{
    DiffFixture fixture;
    auto diff = fixture.engine.diff_cell_content("x = 1\ny = 2", "x = 1\ny = 3");

    REQUIRE(diff.find("-y = 2") != std::string::npos);
    REQUIRE(diff.find("+y = 3") != std::string::npos);
    REQUIRE(diff.find(" x = 1") != std::string::npos);
}

TEST_CASE("NotebookDiffEngine: summarize diff", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> cells_a = {{"c1", "code", "x = 1", 1}};
    std::vector<DiffCell> cells_b = {
        {"c1", "code", "x = 1", 1},
        {"c2", "code", "y = 2", 2},
    };

    auto result = fixture.engine.diff_notebooks(cells_a, cells_b);
    auto summary = NotebookDiffEngine::summarize_diff(result);

    REQUIRE(summary.find("1 cell(s) added") != std::string::npos);
    REQUIRE(summary.find("1 cell(s) unchanged") != std::string::npos);
}

TEST_CASE("NotebookDiffEngine: format diff text", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> cells_a = {{"c1", "code", "x = 1", 1}};
    std::vector<DiffCell> cells_b = {
        {"c1", "code", "x = 1", 1},
        {"c2", "code", "y = 2", 2},
    };

    auto result = fixture.engine.diff_notebooks(cells_a, cells_b);
    auto text = fixture.engine.format_diff_text(result);

    REQUIRE(text.find("unchanged") != std::string::npos);
    REQUIRE(text.find("added") != std::string::npos);
}

TEST_CASE("NotebookDiffEngine: format diff html", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> cells_a = {{"c1", "code", "x = 1", 1}};
    std::vector<DiffCell> cells_b = {{"c1", "code", "x = 1", 1}};

    auto result = fixture.engine.diff_notebooks(cells_a, cells_b);
    auto html = fixture.engine.format_diff_html(result);

    REQUIRE(html.find("notebook-diff") != std::string::npos);
    REQUIRE(html.find("unchanged") != std::string::npos);
}

TEST_CASE("NotebookDiffEngine: three-way merge no conflicts", "[notebook_diff]")
{
    DiffFixture fixture;

    std::vector<DiffCell> base = {
        {"c1", "code", "x = 1", 1},
        {"c2", "code", "y = 2", 2},
    };

    std::vector<DiffCell> local = base; // No changes.
    std::vector<DiffCell> remote = base;
    remote.push_back({"c3", "code", "z = 3", 3}); // Remote adds cell.

    auto merge = fixture.engine.three_way_merge(base, local, remote);
    REQUIRE(!merge.has_conflicts);
    REQUIRE(merge.merged_cells.size() == 3); // base 2 + remote 1
}

TEST_CASE("NotebookDiffEngine: has_conflicts static method", "[notebook_diff]")
{
    MergeResult result;
    result.has_conflicts = false;
    REQUIRE(!NotebookDiffEngine::has_conflicts(result));

    result.has_conflicts = true;
    REQUIRE(NotebookDiffEngine::has_conflicts(result));
}
