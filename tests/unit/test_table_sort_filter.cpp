#include "canvas/TableObject.h"
#include "canvas/TableSortFilter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("TableSortFilter — sort ascending by text", "[canvas][table][sort]")
{
    TableObject table;
    table.set_cell(0, 0, "Banana");
    table.set_cell(1, 0, "Apple");
    table.set_cell(2, 0, "Cherry");

    TableSortFilter::sort_by_column(table, 0, SortDirection::kAscending);

    REQUIRE(table.get_cell(0, 0).text == "Apple");
    REQUIRE(table.get_cell(1, 0).text == "Banana");
    REQUIRE(table.get_cell(2, 0).text == "Cherry");
}

TEST_CASE("TableSortFilter — sort descending by text", "[canvas][table][sort]")
{
    TableObject table;
    table.set_cell(0, 0, "Banana");
    table.set_cell(1, 0, "Apple");
    table.set_cell(2, 0, "Cherry");

    TableSortFilter::sort_by_column(table, 0, SortDirection::kDescending);

    REQUIRE(table.get_cell(0, 0).text == "Cherry");
    REQUIRE(table.get_cell(1, 0).text == "Banana");
    REQUIRE(table.get_cell(2, 0).text == "Apple");
}

TEST_CASE("TableSortFilter — numeric sort ascending", "[canvas][table][sort]")
{
    TableObject table;
    table.set_cell(0, 0, "100");
    table.set_cell(1, 0, "5");
    table.set_cell(2, 0, "42");

    TableSortFilter::sort_numeric(table, 0, SortDirection::kAscending);

    REQUIRE(table.get_cell(0, 0).text == "5");
    REQUIRE(table.get_cell(1, 0).text == "42");
    REQUIRE(table.get_cell(2, 0).text == "100");
}

TEST_CASE("TableSortFilter — numeric sort with non-numeric values", "[canvas][table][sort]")
{
    TableObject table;
    table.set_cell(0, 0, "10");
    table.set_cell(1, 0, "abc");
    table.set_cell(2, 0, "3");

    TableSortFilter::sort_numeric(table, 0, SortDirection::kAscending);

    // Numeric values first, non-numeric at end.
    REQUIRE(table.get_cell(0, 0).text == "3");
    REQUIRE(table.get_cell(1, 0).text == "10");
    REQUIRE(table.get_cell(2, 0).text == "abc");
}

TEST_CASE("TableSortFilter — filter single predicate", "[canvas][table][filter]")
{
    TableObject table;
    table.set_cell(0, 0, "Hello World");
    table.set_cell(1, 0, "Goodbye");
    table.set_cell(2, 0, "Hello Again");

    const FilterPredicate pred{0, "Hello", false};
    const auto matches = TableSortFilter::filter_rows(table, pred);

    REQUIRE(matches.size() == 2);
    REQUIRE(matches[0] == 0);
    REQUIRE(matches[1] == 2);
}

TEST_CASE("TableSortFilter — filter case insensitive", "[canvas][table][filter]")
{
    TableObject table;
    table.set_cell(0, 0, "HELLO");
    table.set_cell(1, 0, "hello");
    table.set_cell(2, 0, "nope");

    const FilterPredicate pred{0, "hello", false}; // case-insensitive.
    const auto matches = TableSortFilter::filter_rows(table, pred);

    REQUIRE(matches.size() == 2);
}

TEST_CASE("TableSortFilter — filter case sensitive", "[canvas][table][filter]")
{
    TableObject table;
    table.set_cell(0, 0, "HELLO");
    table.set_cell(1, 0, "hello");
    table.set_cell(2, 0, "nope");

    const FilterPredicate pred{0, "hello", true}; // case-sensitive.
    const auto matches = TableSortFilter::filter_rows(table, pred);

    REQUIRE(matches.size() == 1);
    REQUIRE(matches[0] == 1);
}

TEST_CASE("TableSortFilter — filter multi-predicate AND", "[canvas][table][filter]")
{
    TableObject table;
    table.set_cell(0, 0, "Apple");
    table.set_cell(0, 1, "Red");
    table.set_cell(1, 0, "Banana");
    table.set_cell(1, 1, "Yellow");
    table.set_cell(2, 0, "Apple");
    table.set_cell(2, 1, "Green");

    const std::vector<FilterPredicate> preds = {{0, "Apple", false}, {1, "Green", false}};

    const auto matches = TableSortFilter::filter_rows_multi(table, preds);
    REQUIRE(matches.size() == 1);
    REQUIRE(matches[0] == 2);
}

TEST_CASE("TableSortFilter — empty predicate list matches all", "[canvas][table][filter]")
{
    TableObject table;
    const auto matches = TableSortFilter::filter_rows_multi(table, {});
    REQUIRE(matches.size() == table.row_count());
}

TEST_CASE("TableSortFilter — remove_filtered_rows", "[canvas][table][filter]")
{
    TableObject table;
    table.set_cell(0, 0, "Keep");
    table.set_cell(1, 0, "Remove");
    table.set_cell(2, 0, "Keep");

    // Keep only rows 0 and 2.
    TableSortFilter::remove_filtered_rows(table, {0, 2});

    REQUIRE(table.row_count() == 2);
    REQUIRE(table.get_cell(0, 0).text == "Keep");
    REQUIRE(table.get_cell(1, 0).text == "Keep");
}

TEST_CASE("TableSortFilter — remove_filtered_rows empty keeps", "[canvas][table][filter]")
{
    TableObject table;
    TableSortFilter::remove_filtered_rows(table, {});
    REQUIRE(table.row_count() == 0);
}

TEST_CASE("TableSortFilter — sort out-of-bounds column is no-op", "[canvas][table][sort]")
{
    TableObject table;
    table.set_cell(0, 0, "A");
    TableSortFilter::sort_by_column(table, 999, SortDirection::kAscending);
    REQUIRE(table.get_cell(0, 0).text == "A"); // Unchanged.
}
