/// test_sql_query.cpp — Unit tests
#include "core/SqlQuery.h"
#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

TEST_CASE("ColumnInfo: default construction", "[sql_query]")
{
    ColumnInfo col;
    REQUIRE(col.name.empty());
}

TEST_CASE("TableInfo: default construction", "[sql_query]")
{
    TableInfo table;
    REQUIRE(table.name.empty());
    REQUIRE(table.columns.empty());
}

TEST_CASE("QueryResult: default is empty", "[sql_query]")
{
    QueryResult qr;
    REQUIRE(qr.rows.empty());
    REQUIRE(qr.columns.empty());
}

TEST_CASE("SqlValidator: compiles", "[sql_query]")
{
    static_assert(sizeof(SqlValidator) > 0);
}
