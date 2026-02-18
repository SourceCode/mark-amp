/// test_line_index.cpp — Unit tests for LineIndex

#include "core/LineIndex.h"

#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace markamp::core;

TEST_CASE("LineIndex: default has one line", "[line_index]")
{
    LineIndex idx;
    REQUIRE(idx.line_count() == 1);
    REQUIRE(idx.newline_count() == 0);
}

TEST_CASE("LineIndex: rebuild counts newlines", "[line_index]")
{
    LineIndex idx;
    idx.rebuild("line1\nline2\nline3\n");
    REQUIRE(idx.newline_count() == 3);
    REQUIRE(idx.line_count() == 4);
}

TEST_CASE("LineIndex: offset_to_line_col", "[line_index]")
{
    LineIndex idx;
    idx.rebuild("ab\ncd\nef");
    auto [line, col] = idx.offset_to_line_col(0);
    REQUIRE(line == 0);
    REQUIRE(col == 0);
    auto [l2, c2] = idx.offset_to_line_col(3);
    REQUIRE(l2 == 1);
    REQUIRE(c2 == 0);
}

TEST_CASE("LineIndex: line_col_to_offset", "[line_index]")
{
    LineIndex idx;
    idx.rebuild("ab\ncd\nef");
    REQUIRE(idx.line_col_to_offset(0, 0) == 0);
    REQUIRE(idx.line_col_to_offset(1, 0) == 3);
    REQUIRE(idx.line_col_to_offset(1, 1) == 4);
}

TEST_CASE("LineIndex: line_start", "[line_index]")
{
    LineIndex idx;
    idx.rebuild("ab\ncd\nef");
    REQUIRE(idx.line_start(0) == 0);
    REQUIRE(idx.line_start(1) == 3);
    REQUIRE(idx.line_start(2) == 6);
}

TEST_CASE("LineIndex: on_insert updates offsets", "[line_index]")
{
    LineIndex idx;
    idx.rebuild("ab\ncd");
    // Insert "XX" at offset 1 (no newlines inserted)
    idx.on_insert(1, 2, {});
    REQUIRE(idx.line_count() == 2);
}

TEST_CASE("LineIndex: on_erase updates offsets", "[line_index]")
{
    LineIndex idx;
    idx.rebuild("ab\ncd\nef");
    // Erase the first newline (at offset 2)  
    idx.on_erase(2, 1);
    REQUIRE(idx.newline_count() == 1);
}
