/// @file test_diff_editor.cpp
/// @brief Phase 46 — Unit tests for Diff Editor and Merge Tool.

#include "core/DiffEngine.h"
#include "core/MergeEngine.h"
#include "core/WordDiffEngine.h"
#include "ui/DiffGutter.h"
#include "ui/InlineDiffView.h"
#include "ui/MergeEditor.h"
#include "ui/SideBySideDiffView.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// WordDiffEngine — Word-level diffing
// ============================================================================

TEST_CASE("WordDiffEngine: identifies changed words", "[diff_editor]")
{
    WordDiffEngine engine;
    auto result = engine.diff_lines("The quick brown fox", "The slow brown dog");

    REQUIRE(result.has_changes);
    REQUIRE_FALSE(result.old_spans.empty());
    REQUIRE_FALSE(result.new_spans.empty());

    // "The" and "brown" should be Equal in both spans.
    bool found_equal_the = false;
    bool found_deleted_quick = false;
    for (const auto& span : result.old_spans)
    {
        if (span.text == "The" && span.type == WordChangeType::Equal)
        {
            found_equal_the = true;
        }
        if (span.text == "quick" && span.type == WordChangeType::Deleted)
        {
            found_deleted_quick = true;
        }
    }
    REQUIRE(found_equal_the);
    REQUIRE(found_deleted_quick);
}

TEST_CASE("WordDiffEngine: identical lines produce no changes", "[diff_editor]")
{
    WordDiffEngine engine;
    auto result = engine.diff_lines("Hello world", "Hello world");

    REQUIRE_FALSE(result.has_changes);
}

TEST_CASE("WordDiffEngine: tokenize splits on whitespace and punctuation", "[diff_editor]")
{
    auto tokens = WordDiffEngine::tokenize("hello, world!");
    // tokens: "hello", ",", " ", "world", "!"
    REQUIRE(tokens.size() == 5);
    REQUIRE(tokens[0] == "hello");
    REQUIRE(tokens[1] == ",");
    REQUIRE(tokens[3] == "world");
}

// ============================================================================
// MergeEngine — 3-way merge
// ============================================================================

TEST_CASE("MergeEngine: auto-merges non-overlapping changes", "[diff_editor]")
{
    MergeEngine engine;
    std::string base = "line1\nline2\nline3\nline4";
    std::string theirs = "line1\ntheirs2\nline3\nline4"; // Changed line 2
    std::string yours = "line1\nline2\nline3\nyours4";   // Changed line 4

    auto result = engine.merge(base, theirs, yours);

    REQUIRE_FALSE(result.has_conflicts);
    REQUIRE(result.auto_merged_regions >= 2);
}

TEST_CASE("MergeEngine: detects conflicts on overlapping changes", "[diff_editor]")
{
    MergeEngine engine;
    std::string base = "line1\nline2\nline3";
    std::string theirs = "line1\ntheirs2\nline3"; // Changed line 2
    std::string yours = "line1\nyours2\nline3";   // Also changed line 2

    auto result = engine.merge(base, theirs, yours);

    REQUIRE(result.has_conflicts);
    REQUIRE(result.conflicts.size() >= 1);
    REQUIRE(result.unresolved_count() >= 1);
}

TEST_CASE("MergeEngine: preserves base for unchanged regions", "[diff_editor]")
{
    MergeEngine engine;
    std::string base = "keep1\nkeep2\nkeep3";
    std::string theirs = "keep1\nkeep2\nkeep3"; // No changes
    std::string yours = "keep1\nkeep2\nkeep3";  // No changes

    auto result = engine.merge(base, theirs, yours);

    REQUIRE_FALSE(result.has_conflicts);
    // All lines should be from base.
    bool all_base = true;
    for (const auto& line : result.lines)
    {
        if (line.origin != MergeOrigin::Base)
        {
            all_base = false;
        }
    }
    REQUIRE(all_base);
}

// ============================================================================
// DiffGutter — Line indicator mapping
// ============================================================================

TEST_CASE("DiffGutter: maps lines to correct indicator types", "[diff_editor]")
{
    DiffEngine diff_engine;
    auto diff = diff_engine.compute_diff("line1\nline2\nline3", "line1\nmodified\nline3");

    DiffGutterModel gutter;
    gutter.build_from_diff(diff, 3, 3);

    auto& left = gutter.left_decorations();
    auto& right = gutter.right_decorations();
    REQUIRE(left.size() == 3);
    REQUIRE(right.size() == 3);

    // Line 0 should be unchanged on both sides.
    REQUIRE(left[0].indicator == GutterIndicator::Unchanged);
    REQUIRE(right[0].indicator == GutterIndicator::Unchanged);
}

// ============================================================================
// MergeEditor — Conflict resolution
// ============================================================================

TEST_CASE("MergeEditor: accept theirs resolves conflict", "[diff_editor]")
{
    MergeEngine engine;
    std::string base = "line1\nline2";
    std::string theirs = "line1\ntheirs";
    std::string yours = "line1\nyours";

    auto result = engine.merge(base, theirs, yours);
    REQUIRE(result.has_conflicts);

    MergeEditorModel editor;
    editor.set_merge_result(std::move(result));
    REQUIRE(editor.unresolved_count() >= 1);

    editor.accept_theirs(0);
    REQUIRE(editor.merge_result().conflicts[0].resolution == ConflictResolution::AcceptTheirs);
}

TEST_CASE("MergeEditor: accept all yours resolves all", "[diff_editor]")
{
    MergeEngine engine;
    std::string base = "a\nb\nc";
    std::string theirs = "x\ny\nz";
    std::string yours = "p\nq\nr";

    auto result = engine.merge(base, theirs, yours);
    MergeEditorModel editor;
    editor.set_merge_result(std::move(result));

    editor.accept_all_yours();

    for (const auto& c : editor.merge_result().conflicts)
    {
        REQUIRE(c.resolution == ConflictResolution::AcceptYours);
    }
}

// ============================================================================
// InlineDiffView — Interleaved diff
// ============================================================================

TEST_CASE("InlineDiffView: builds interleaved lines", "[diff_editor]")
{
    DiffEngine diff_engine;
    auto diff = diff_engine.compute_diff("line1\nline2\nline3", "line1\nnewline\nline3");

    auto old_lines = DiffEngine::split_lines("line1\nline2\nline3");
    auto new_lines = DiffEngine::split_lines("line1\nnewline\nline3");

    InlineDiffViewModel view;
    view.build(diff, old_lines, new_lines);

    REQUIRE(view.total_lines() >= 3);
    REQUIRE(view.change_count() >= 1);
}
