// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/TableModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Grid setup and cell access", "[table][grid]")
{
    TableModel model;
    model.set_grid(3, 4);
    REQUIRE(model.rows() == 3);
    REQUIRE(model.cols() == 4);
    model.set_cell(1, 2, "Hello");
    REQUIRE(model.cell(1, 2) == "Hello");
}

TEST_CASE("Insert and delete row", "[table][grid]")
{
    TableModel model;
    model.set_grid(2, 2);
    model.set_cell(0, 0, "A");
    model.insert_row(1);
    REQUIRE(model.rows() == 3);
    model.delete_row(1);
    REQUIRE(model.rows() == 2);
}

TEST_CASE("Tab navigation wraps", "[table][nav]")
{
    TableModel model;
    model.set_grid(2, 2);
    model.set_active_cell(0, 1);
    model.tab_next(); // wraps to row 1, col 0
    REQUIRE(model.active_row() == 1);
    REQUIRE(model.active_col() == 0);
}

TEST_CASE("CSV round-trip", "[table][csv]")
{
    TableModel model;
    model.set_grid(2, 3);
    model.set_cell(0, 0, "A");
    model.set_cell(0, 1, "B");
    model.set_cell(0, 2, "C");
    model.set_cell(1, 0, "1");
    model.set_cell(1, 1, "2");
    model.set_cell(1, 2, "3");

    const auto csv = model.to_csv();
    TableModel loaded;
    loaded.from_csv(csv);
    REQUIRE(loaded.rows() == 2);
    REQUIRE(loaded.cols() == 3);
    REQUIRE(loaded.cell(0, 0) == "A");
    REQUIRE(loaded.cell(1, 2) == "3");
}

TEST_CASE("Header and alternate formatting", "[table][format]")
{
    TableModel model;
    REQUIRE(model.has_header_row());
    model.set_alternate_rows(true);
    REQUIRE(model.alternate_rows());
}

TEST_CASE("Sort state", "[table][sort]")
{
    TableModel model;
    model.set_sort({1, SortDirection::kAscending});
    REQUIRE(model.sort().column == 1);
    REQUIRE(model.sort().direction == SortDirection::kAscending);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
