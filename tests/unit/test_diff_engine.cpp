// test_diff_engine.cpp — 10 tests for DiffEngine
#include "core/DiffEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("DiffEngine identical texts produce no changes", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("hello\nworld\n", "hello\nworld\n");
    CHECK(result.hunks.empty());
}

TEST_CASE("DiffEngine detects insertion", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("line1\nline3\n", "line1\nline2\nline3\n");
    CHECK_FALSE(result.hunks.empty());
    CHECK(result.additions > 0);
}

TEST_CASE("DiffEngine detects deletion", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("line1\nline2\nline3\n", "line1\nline3\n");
    CHECK_FALSE(result.hunks.empty());
    CHECK(result.deletions > 0);
}

TEST_CASE("DiffEngine detects modification", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("line1\nold_line\nline3\n", "line1\nnew_line\nline3\n");
    CHECK_FALSE(result.hunks.empty());
}

TEST_CASE("DiffEngine split_lines works correctly", "[diff]")
{
    auto lines = DiffEngine::split_lines("a\nb\nc\n");
    CHECK(lines.size() >= 3);
    CHECK(lines[0] == "a");
    CHECK(lines[1] == "b");
    CHECK(lines[2] == "c");
}

TEST_CASE("DiffEngine render_unified produces text", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("a\n", "a\nb\n");
    auto unified = DiffEngine::render_unified(result);
    CHECK_FALSE(unified.empty());
}

TEST_CASE("DiffEngine render_side_by_side_html produces HTML", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("old\n", "new\n");
    auto html = DiffEngine::render_side_by_side_html(result);
    CHECK_FALSE(html.empty());
}

TEST_CASE("DiffEngine render_inline_html produces HTML", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("old\n", "new\n");
    auto html = DiffEngine::render_inline_html(result);
    CHECK_FALSE(html.empty());
}

TEST_CASE("DiffEngine empty to non-empty", "[diff]")
{
    DiffEngine engine;
    auto result = engine.compute_diff("", "new content\n");
    CHECK(result.additions > 0);
}

TEST_CASE("DiffEngine context lines parameter", "[diff]")
{
    DiffEngine engine;
    auto r0 = engine.compute_diff("a\nb\nc\nd\ne\n", "a\nB\nc\nd\ne\n", 0);
    auto r3 = engine.compute_diff("a\nb\nc\nd\ne\n", "a\nB\nc\nd\ne\n", 3);
    // Both should detect same changes but r3 may have more context lines in hunks
    CHECK_FALSE(r0.hunks.empty());
    CHECK_FALSE(r3.hunks.empty());
}
