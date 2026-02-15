#include "canvas/TableObject.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::canvas;

TEST_CASE("TableObject — default construction", "[canvas][table]")
{
    TableObject table;
    REQUIRE(table.type() == CanvasObjectType::Table);
    REQUIRE(table.column_count() == 3);
    REQUIRE(table.row_count() == 3);
    REQUIRE(table.column(0).header == "Column A");
    REQUIRE(table.column(1).header == "Column B");
    REQUIRE(table.column(2).header == "Column C");
}

TEST_CASE("TableObject — add and remove columns", "[canvas][table]")
{
    TableObject table;
    table.add_column("Column D", 150.0);
    REQUIRE(table.column_count() == 4);
    REQUIRE(table.column(3).header == "Column D");
    REQUIRE_THAT(table.column(3).width, Catch::Matchers::WithinAbs(150.0, 0.01));

    // Each row should have gained a 4th cell.
    for (size_t ri = 0; ri < table.row_count(); ++ri)
    {
        REQUIRE(table.get_cell(ri, 3).text.empty());
    }

    table.remove_column(1);
    REQUIRE(table.column_count() == 3);
    REQUIRE(table.column(0).header == "Column A");
    REQUIRE(table.column(1).header == "Column C"); // B was removed.
}

TEST_CASE("TableObject — cannot remove last column", "[canvas][table]")
{
    TableObject table;
    table.remove_column(0);
    table.remove_column(0); // Now 1 left.
    REQUIRE(table.column_count() == 1);
    table.remove_column(0); // Should not remove last.
    REQUIRE(table.column_count() == 1);
}

TEST_CASE("TableObject — add and remove rows", "[canvas][table]")
{
    TableObject table;
    REQUIRE(table.row_count() == 3);

    table.add_row();
    REQUIRE(table.row_count() == 4);

    table.insert_row(1);
    REQUIRE(table.row_count() == 5);

    table.remove_row(0);
    REQUIRE(table.row_count() == 4);
}

TEST_CASE("TableObject — cell get/set", "[canvas][table]")
{
    TableObject table;
    table.set_cell(0, 0, "Hello");
    table.set_cell(1, 2, "World");

    REQUIRE(table.get_cell(0, 0).text == "Hello");
    REQUIRE(table.get_cell(1, 2).text == "World");
    REQUIRE(table.get_cell(2, 0).text.empty());
}

TEST_CASE("TableObject — cell styling", "[canvas][table]")
{
    TableObject table;
    const CanvasColor red{255, 0, 0, 255};
    const CanvasColor white{255, 255, 255, 255};

    table.set_cell_style(0, 0, red, white, true);
    const auto& cell = table.get_cell(0, 0);
    REQUIRE(cell.fill_color.r == 255);
    REQUIRE(cell.fill_color.g == 0);
    REQUIRE(cell.bold);
}

TEST_CASE("TableObject — out-of-bounds cell set is no-op", "[canvas][table]")
{
    TableObject table;
    table.set_cell(999, 0, "nope"); // Should not crash.
    table.set_cell(0, 999, "nope"); // Should not crash.
}

TEST_CASE("TableObject — resize column", "[canvas][table]")
{
    TableObject table;
    table.resize_column(0, 200.0);
    REQUIRE_THAT(table.column(0).width, Catch::Matchers::WithinAbs(200.0, 0.01));

    // Below minimum clamps.
    table.resize_column(0, 10.0);
    REQUIRE(table.column(0).width >= 40.0);
}

TEST_CASE("TableObject — computed dimensions", "[canvas][table]")
{
    TableObject table; // 3 cols × 120 = 360 wide, header 36 + 3 rows × 32 = 132 tall.
    REQUIRE_THAT(table.total_width(), Catch::Matchers::WithinAbs(360.0, 0.01));
    REQUIRE_THAT(table.total_height(), Catch::Matchers::WithinAbs(132.0, 0.01));

    const auto bounds = table.local_bounds();
    REQUIRE_THAT(bounds.min_x, Catch::Matchers::WithinAbs(0.0, 0.01));
    REQUIRE_THAT(bounds.min_y, Catch::Matchers::WithinAbs(0.0, 0.01));
    REQUIRE_THAT(bounds.max_x, Catch::Matchers::WithinAbs(360.0, 0.01));
    REQUIRE_THAT(bounds.max_y, Catch::Matchers::WithinAbs(132.0, 0.01));
}

TEST_CASE("TableObject — clone", "[canvas][table]")
{
    TableObject table;
    table.set_name("TestTable");
    table.set_cell(0, 0, "A1");
    table.set_row_height(40.0);

    auto cloned = table.clone();
    REQUIRE(cloned->type() == CanvasObjectType::Table);
    REQUIRE(cloned->name() == "TestTable");

    const auto& ct = static_cast<const TableObject&>(*cloned);
    REQUIRE(ct.column_count() == 3);
    REQUIRE(ct.row_count() == 3);
    REQUIRE(ct.get_cell(0, 0).text == "A1");
    REQUIRE_THAT(ct.row_height(), Catch::Matchers::WithinAbs(40.0, 0.01));
}

TEST_CASE("TableObject — JSON serialization", "[canvas][table]")
{
    TableObject table;
    table.set_cell(0, 0, "Cell00");

    const auto json = table.to_json();
    REQUIRE(json.find("\"type\":\"Table\"") != std::string::npos);
    REQUIRE(json.find("\"columns\":[") != std::string::npos);
    REQUIRE(json.find("\"rows\":[") != std::string::npos);
    REQUIRE(json.find("Cell00") != std::string::npos);
}

TEST_CASE("TableObject — visual settings", "[canvas][table]")
{
    TableObject table;
    table.set_header_height(50.0);
    REQUIRE_THAT(table.header_height(), Catch::Matchers::WithinAbs(50.0, 0.01));

    const CanvasColor blue{0, 0, 255, 255};
    table.set_border_color(blue);
    REQUIRE(table.border_color().b == 255);

    table.set_header_bg_color(blue);
    REQUIRE(table.header_bg_color().b == 255);
}
