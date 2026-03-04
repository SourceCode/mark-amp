#include "../../src/core/PreserveCaseReplacer.h"
#include "../../src/core/ReplaceUndoBuffer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <string>
#include <vector>

// Inline core models to avoid wxWidgets dependency
namespace markamp::ui
{

enum class FindScope : std::uint8_t
{
    kDocument = 0,
    kSelection
};

struct FindBarState
{
    std::string search_query;
    std::string replace_text;
    bool case_sensitive{false};
    bool whole_word{false};
    bool use_regex{false};
    bool preserve_case{false};
    bool replace_visible{false};
    FindScope scope{FindScope::kDocument};
    int total_matches{0};
    int current_match{0};
    [[nodiscard]] auto match_counter_text() const -> std::string
    {
        if (search_query.empty())
        {
            return "";
        }
        if (total_matches == 0)
        {
            return "No results";
        }
        return std::to_string(current_match) + " of " + std::to_string(total_matches);
    }
    void next_match()
    {
        if (total_matches == 0)
        {
            return;
        }
        current_match = (current_match % total_matches) + 1;
    }
    void prev_match()
    {
        if (total_matches == 0)
        {
            return;
        }
        current_match = current_match <= 1 ? total_matches : current_match - 1;
    }
    void clear_matches()
    {
        total_matches = 0;
        current_match = 0;
    }
};

struct SearchMatchResult
{
    std::string file_path;
    int line_number{0};
    int column{0};
    std::string match_text;
    std::string context_before;
    std::string context_after;
    std::string replace_preview;
    bool included{true};
};

struct SearchResultFileGroup
{
    std::string file_path;
    std::string display_name;
    std::vector<SearchMatchResult> matches;
    bool collapsed{false};
    [[nodiscard]] auto match_count() const -> std::size_t
    {
        return matches.size();
    }
    [[nodiscard]] auto included_count() const -> std::size_t
    {
        std::size_t cnt = 0;
        for (const auto& mtch : matches)
        {
            if (mtch.included)
            {
                ++cnt;
            }
        }
        return cnt;
    }
};

class SearchResultsModel
{
public:
    void set_results(std::vector<SearchResultFileGroup> grps)
    {
        groups_ = std::move(grps);
    }
    [[nodiscard]] auto groups() const -> const std::vector<SearchResultFileGroup>&
    {
        return groups_;
    }
    [[nodiscard]] auto total_matches() const -> std::size_t
    {
        std::size_t total = 0;
        for (const auto& grp : groups_)
        {
            total += grp.match_count();
        }
        return total;
    }
    [[nodiscard]] auto file_count() const -> std::size_t
    {
        return groups_.size();
    }
    [[nodiscard]] auto summary_text() const -> std::string
    {
        if (groups_.empty())
        {
            return "No results";
        }
        return std::to_string(total_matches()) + " results in " + std::to_string(file_count()) +
               " files";
    }
    void toggle_match_inclusion(std::size_t grp_idx, std::size_t mtch_idx)
    {
        if (grp_idx < groups_.size() && mtch_idx < groups_[grp_idx].matches.size())
        {
            groups_[grp_idx].matches[mtch_idx].included =
                !groups_[grp_idx].matches[mtch_idx].included;
        }
    }
    void toggle_group_collapse(std::size_t grp_idx)
    {
        if (grp_idx < groups_.size())
        {
            groups_[grp_idx].collapsed = !groups_[grp_idx].collapsed;
        }
    }
    void clear()
    {
        groups_.clear();
    }
    [[nodiscard]] auto included_matches() const -> std::vector<SearchMatchResult>
    {
        std::vector<SearchMatchResult> result;
        for (const auto& grp : groups_)
        {
            for (const auto& mtch : grp.matches)
            {
                if (mtch.included)
                {
                    result.push_back(mtch);
                }
            }
        }
        return result;
    }

private:
    std::vector<SearchResultFileGroup> groups_;
};

} // namespace markamp::ui

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// PreserveCaseReplacer: Case Pattern Detection Tests
// ============================================================================

TEST_CASE("detect_case_pattern identifies ALL_UPPER", "[find_replace][case]")
{
    REQUIRE(detect_case_pattern("HELLO") == CasePattern::kAllUpper);
    REQUIRE(detect_case_pattern("ABC") == CasePattern::kAllUpper);
    REQUIRE(detect_case_pattern("X") == CasePattern::kAllUpper);
}

TEST_CASE("detect_case_pattern identifies all_lower", "[find_replace][case]")
{
    REQUIRE(detect_case_pattern("hello") == CasePattern::kAllLower);
    REQUIRE(detect_case_pattern("abc") == CasePattern::kAllLower);
    REQUIRE(detect_case_pattern("x") == CasePattern::kAllLower);
}

TEST_CASE("detect_case_pattern identifies TitleCase", "[find_replace][case]")
{
    REQUIRE(detect_case_pattern("Hello") == CasePattern::kTitleCase);
    REQUIRE(detect_case_pattern("World") == CasePattern::kTitleCase);
}

TEST_CASE("detect_case_pattern identifies camelCase", "[find_replace][case]")
{
    REQUIRE(detect_case_pattern("helloWorld") == CasePattern::kCamelCase);
    REQUIRE(detect_case_pattern("myVariable") == CasePattern::kCamelCase);
}

TEST_CASE("detect_case_pattern identifies PascalCase", "[find_replace][case]")
{
    REQUIRE(detect_case_pattern("HelloWorld") == CasePattern::kPascalCase);
    REQUIRE(detect_case_pattern("MyClass") == CasePattern::kPascalCase);
}

TEST_CASE("detect_case_pattern handles mixed and empty", "[find_replace][case]")
{
    REQUIRE(detect_case_pattern("") == CasePattern::kMixed);
    REQUIRE(detect_case_pattern("123") == CasePattern::kMixed);
}

// ============================================================================
// PreserveCaseReplacer: Apply Pattern Tests
// ============================================================================

TEST_CASE("apply_case_pattern transforms correctly", "[find_replace][case]")
{
    SECTION("ALL_UPPER")
    {
        REQUIRE(apply_case_pattern("world", CasePattern::kAllUpper) == "WORLD");
        REQUIRE(apply_case_pattern("hello world", CasePattern::kAllUpper) == "HELLO WORLD");
    }

    SECTION("all_lower")
    {
        REQUIRE(apply_case_pattern("WORLD", CasePattern::kAllLower) == "world");
        REQUIRE(apply_case_pattern("Hello", CasePattern::kAllLower) == "hello");
    }

    SECTION("TitleCase")
    {
        REQUIRE(apply_case_pattern("world", CasePattern::kTitleCase) == "World");
        REQUIRE(apply_case_pattern("WORLD", CasePattern::kTitleCase) == "World");
    }

    SECTION("camelCase")
    {
        REQUIRE(apply_case_pattern("Hello", CasePattern::kCamelCase) == "hello");
    }

    SECTION("PascalCase")
    {
        REQUIRE(apply_case_pattern("hello", CasePattern::kPascalCase) == "Hello");
    }

    SECTION("Mixed (no change)")
    {
        REQUIRE(apply_case_pattern("test", CasePattern::kMixed) == "test");
    }

    SECTION("Empty string")
    {
        REQUIRE(apply_case_pattern("", CasePattern::kAllUpper) == "");
    }
}

// ============================================================================
// PreserveCaseReplacer: Integrated Replace Tests
// ============================================================================

TEST_CASE("preserve_case_replace preserves casing", "[find_replace][case]")
{
    REQUIRE(preserve_case_replace("HELLO", "world") == "WORLD");
    REQUIRE(preserve_case_replace("hello", "World") == "world");
    REQUIRE(preserve_case_replace("Hello", "world") == "World");
    REQUIRE(preserve_case_replace("", "world") == "world");
    REQUIRE(preserve_case_replace("HELLO", "") == "");
}

// ============================================================================
// ReplaceUndoBuffer Tests
// ============================================================================

TEST_CASE("ReplaceUndoBuffer empty state", "[find_replace][undo]")
{
    ReplaceUndoBuffer buffer;
    REQUIRE_FALSE(buffer.has_undo());
    REQUIRE(buffer.file_count() == 0);
}

TEST_CASE("ReplaceUndoBuffer snapshot and undo", "[find_replace][undo]")
{
    ReplaceUndoBuffer buffer;
    buffer.begin_batch();
    buffer.snapshot("/path/a.md", "original content A");
    buffer.snapshot("/path/b.md", "original content B");

    REQUIRE(buffer.has_undo());
    REQUIRE(buffer.file_count() == 2);

    auto snapshots = buffer.undo_all();
    REQUIRE(snapshots.size() == 2);
    REQUIRE_FALSE(buffer.has_undo());
}

TEST_CASE("ReplaceUndoBuffer first snapshot wins", "[find_replace][undo]")
{
    ReplaceUndoBuffer buffer;
    buffer.begin_batch();
    buffer.snapshot("/path/a.md", "first snapshot");
    buffer.snapshot("/path/a.md", "second snapshot");

    REQUIRE(buffer.file_count() == 1);
    auto snapshots = buffer.undo_all();
    REQUIRE(snapshots.size() == 1);
    REQUIRE(snapshots[0].original_content == "first snapshot");
}

TEST_CASE("ReplaceUndoBuffer begin_batch clears previous", "[find_replace][undo]")
{
    ReplaceUndoBuffer buffer;
    buffer.begin_batch();
    buffer.snapshot("/path/a.md", "content");
    REQUIRE(buffer.file_count() == 1);

    buffer.begin_batch();
    REQUIRE(buffer.file_count() == 0);
}

TEST_CASE("ReplaceUndoBuffer clear", "[find_replace][undo]")
{
    ReplaceUndoBuffer buffer;
    buffer.begin_batch();
    buffer.snapshot("/path/a.md", "content");
    buffer.clear();
    REQUIRE_FALSE(buffer.has_undo());
    REQUIRE(buffer.file_count() == 0);
}

TEST_CASE("ReplaceUndoBuffer auto-begins batch on snapshot", "[find_replace][undo]")
{
    ReplaceUndoBuffer buffer;
    buffer.snapshot("/path/a.md", "content");
    REQUIRE(buffer.has_undo());
    REQUIRE(buffer.file_count() == 1);
}

// ============================================================================
// FindBarState Tests
// ============================================================================

TEST_CASE("FindBarState match counter text", "[find_replace][state]")
{
    FindBarState state;
    REQUIRE(state.match_counter_text() == "");

    state.search_query = "test";
    REQUIRE(state.match_counter_text() == "No results");

    state.total_matches = 10;
    state.current_match = 3;
    REQUIRE(state.match_counter_text() == "3 of 10");
}

TEST_CASE("FindBarState next/prev match navigation", "[find_replace][state]")
{
    FindBarState state;
    state.search_query = "test";
    state.total_matches = 5;
    state.current_match = 1;

    state.next_match();
    REQUIRE(state.current_match == 2);

    state.next_match();
    REQUIRE(state.current_match == 3);

    // Navigate to end
    state.current_match = 5;
    state.next_match();
    REQUIRE(state.current_match == 1); // Wraps around

    // Previous from 1 wraps to 5
    state.current_match = 1;
    state.prev_match();
    REQUIRE(state.current_match == 5);
}

TEST_CASE("FindBarState next/prev with zero matches", "[find_replace][state]")
{
    FindBarState state;
    state.search_query = "test";
    state.total_matches = 0;
    state.current_match = 0;

    state.next_match(); // Should not crash
    REQUIRE(state.current_match == 0);

    state.prev_match();
    REQUIRE(state.current_match == 0);
}

TEST_CASE("FindBarState clear_matches", "[find_replace][state]")
{
    FindBarState state;
    state.total_matches = 10;
    state.current_match = 5;
    state.clear_matches();
    REQUIRE(state.total_matches == 0);
    REQUIRE(state.current_match == 0);
}

// ============================================================================
// SearchResultsModel Tests
// ============================================================================

TEST_CASE("SearchResultsModel empty state", "[find_replace][results]")
{
    SearchResultsModel model;
    REQUIRE(model.total_matches() == 0);
    REQUIRE(model.file_count() == 0);
    REQUIRE(model.summary_text() == "No results");
}

TEST_CASE("SearchResultsModel with results", "[find_replace][results]")
{
    SearchResultsModel model;
    SearchResultFileGroup group;
    group.file_path = "/path/test.md";
    group.display_name = "test.md";
    group.matches.push_back({"", 10, 0, "match1", "", "", "", true});
    group.matches.push_back({"", 20, 0, "match2", "", "", "", true});

    model.set_results({group});
    REQUIRE(model.total_matches() == 2);
    REQUIRE(model.file_count() == 1);
    REQUIRE(model.summary_text() == "2 results in 1 files");
}

TEST_CASE("SearchResultsModel toggle match inclusion", "[find_replace][results]")
{
    SearchResultsModel model;
    SearchResultFileGroup group;
    group.file_path = "/test.md";
    group.display_name = "test.md";
    group.matches.push_back({"", 1, 0, "m1", "", "", "", true});
    group.matches.push_back({"", 2, 0, "m2", "", "", "", true});

    model.set_results({group});
    model.toggle_match_inclusion(0, 1);

    auto included = model.included_matches();
    REQUIRE(included.size() == 1);
    REQUIRE(included[0].match_text == "m1");
}

TEST_CASE("SearchResultsModel toggle group collapse", "[find_replace][results]")
{
    SearchResultsModel model;
    SearchResultFileGroup group;
    group.file_path = "/test.md";
    group.display_name = "test.md";
    group.matches.push_back({"", 1, 0, "m1", "", "", "", true});

    model.set_results({group});
    REQUIRE_FALSE(model.groups()[0].collapsed);

    model.toggle_group_collapse(0);
    REQUIRE(model.groups()[0].collapsed);

    model.toggle_group_collapse(0);
    REQUIRE_FALSE(model.groups()[0].collapsed);
}

TEST_CASE("SearchResultsModel clear", "[find_replace][results]")
{
    SearchResultsModel model;
    SearchResultFileGroup group;
    group.file_path = "/test.md";
    group.display_name = "test.md";
    group.matches.push_back({"", 1, 0, "m1", "", "", "", true});
    model.set_results({group});

    model.clear();
    REQUIRE(model.total_matches() == 0);
    REQUIRE(model.file_count() == 0);
}

TEST_CASE("SearchResultFileGroup included_count", "[find_replace][results]")
{
    SearchResultFileGroup group;
    group.matches.push_back({"", 1, 0, "m1", "", "", "", true});
    group.matches.push_back({"", 2, 0, "m2", "", "", "", false});
    group.matches.push_back({"", 3, 0, "m3", "", "", "", true});

    REQUIRE(group.match_count() == 3);
    REQUIRE(group.included_count() == 2);
}
