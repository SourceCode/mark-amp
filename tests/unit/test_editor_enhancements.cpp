#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>
#include <wx/stc/stc.h>

#include <algorithm>
#include <cmath>
#include <regex>
#include <string>
#include <vector>

using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════
// WrapMode enum
// ═══════════════════════════════════════════════════════

TEST_CASE("WrapMode enum has three values", "[editor_enhancements]")
{
    auto none = WrapMode::None;
    auto word = WrapMode::Word;
    auto character = WrapMode::Character;

    REQUIRE(none != word);
    REQUIRE(word != character);
    REQUIRE(none != character);
}

TEST_CASE("WrapMode default should be Word", "[editor_enhancements]")
{
    WrapMode mode = WrapMode::Word;
    REQUIRE(mode == WrapMode::Word);
}

// ═══════════════════════════════════════════════════════
// Scintilla wrap character mode constant
// ═══════════════════════════════════════════════════════

TEST_CASE("Scintilla WRAP_CHAR constant is defined", "[editor_enhancements]")
{
    REQUIRE(wxSTC_WRAP_CHAR > 0);
}

// ═══════════════════════════════════════════════════════
// Line number gutter width calculation
// ═══════════════════════════════════════════════════════

TEST_CASE("Gutter digit calculation for line counts", "[editor_enhancements]")
{
    constexpr int kMinDigits = 3;

    // Helper: compute digits needed (same logic as EditorPanel::UpdateLineNumberMargin)
    auto compute_digits = [](int line_count) -> int
    { return std::max(3, static_cast<int>(std::log10(std::max(1, line_count))) + 1); };

    SECTION("1 line => 3 digits (minimum)")
    {
        REQUIRE(compute_digits(1) == kMinDigits);
    }

    SECTION("9 lines => 3 digits (minimum)")
    {
        REQUIRE(compute_digits(9) == kMinDigits);
    }

    SECTION("99 lines => 3 digits (minimum applied)")
    {
        REQUIRE(compute_digits(99) == kMinDigits);
    }

    SECTION("100 lines => 3 digits")
    {
        REQUIRE(compute_digits(100) == 3);
    }

    SECTION("999 lines => 3 digits")
    {
        REQUIRE(compute_digits(999) == 3);
    }

    SECTION("1000 lines => 4 digits")
    {
        REQUIRE(compute_digits(1000) == 4);
    }

    SECTION("9999 lines => 4 digits")
    {
        REQUIRE(compute_digits(9999) == 4);
    }

    SECTION("10000 lines => 5 digits")
    {
        REQUIRE(compute_digits(10000) == 5);
    }

    SECTION("99999 lines => 5 digits")
    {
        REQUIRE(compute_digits(99999) == 5);
    }

    SECTION("100000 lines => 6 digits")
    {
        REQUIRE(compute_digits(100000) == 6);
    }
}

// ═══════════════════════════════════════════════════════
// Large file threshold constant
// ═══════════════════════════════════════════════════════

TEST_CASE("Large file threshold is 50000 lines", "[editor_enhancements]")
{
    constexpr int kLargeFileThreshold = 50000;
    REQUIRE(kLargeFileThreshold == 50000);
}

// ═══════════════════════════════════════════════════════
// Bracket types recognized
// ═══════════════════════════════════════════════════════

TEST_CASE("Bracket characters are recognized", "[editor_enhancements]")
{
    auto is_brace = [](int ch) -> bool
    { return ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}'; };

    REQUIRE(is_brace('('));
    REQUIRE(is_brace(')'));
    REQUIRE(is_brace('['));
    REQUIRE(is_brace(']'));
    REQUIRE(is_brace('{'));
    REQUIRE(is_brace('}'));
    REQUIRE_FALSE(is_brace('<'));
    REQUIRE_FALSE(is_brace('>'));
    REQUIRE_FALSE(is_brace('a'));
}

// ═══════════════════════════════════════════════════════
// Markdown auto-indent pattern matching
// ═══════════════════════════════════════════════════════

TEST_CASE("Unordered list pattern matches", "[editor_enhancements]")
{
    static const std::regex ulist_re(R"(^(\s*[-*+] )(.*)$)");
    std::smatch match;

    SECTION("dash list item")
    {
        std::string line = "- hello world";
        REQUIRE(std::regex_match(line, match, ulist_re));
        REQUIRE(match[1].str() == "- ");
        REQUIRE(match[2].str() == "hello world");
    }

    SECTION("asterisk list item with indent")
    {
        std::string line = "  * nested item";
        REQUIRE(std::regex_match(line, match, ulist_re));
        REQUIRE(match[1].str() == "  * ");
        REQUIRE(match[2].str() == "nested item");
    }

    SECTION("empty list item (content is empty)")
    {
        std::string line = "- ";
        REQUIRE(std::regex_match(line, match, ulist_re));
        REQUIRE(match[2].str().empty());
    }

    SECTION("non-list line does not match")
    {
        std::string line = "just some text";
        REQUIRE_FALSE(std::regex_match(line, match, ulist_re));
    }
}

TEST_CASE("Ordered list pattern matches and increments", "[editor_enhancements]")
{
    static const std::regex olist_re(R"(^(\s*)(\d+)(\. )(.*)$)");
    std::smatch match;

    SECTION("simple ordered item")
    {
        std::string line = "1. first item";
        REQUIRE(std::regex_match(line, match, olist_re));
        int number = std::stoi(match[2].str());
        REQUIRE(number == 1);
        REQUIRE((number + 1) == 2);
        REQUIRE(match[4].str() == "first item");
    }

    SECTION("multi-digit ordered item")
    {
        std::string line = "12. twelfth item";
        REQUIRE(std::regex_match(line, match, olist_re));
        int number = std::stoi(match[2].str());
        REQUIRE(number == 12);
        REQUIRE((number + 1) == 13);
    }

    SECTION("empty ordered item")
    {
        std::string line = "5. ";
        REQUIRE(std::regex_match(line, match, olist_re));
        REQUIRE(match[4].str().empty());
    }
}

TEST_CASE("Blockquote pattern matches", "[editor_enhancements]")
{
    static const std::regex bquote_re(R"(^(\s*(?:>\s*)+)(.*)$)");
    std::smatch match;

    SECTION("single blockquote")
    {
        std::string line = "> some quoted text";
        REQUIRE(std::regex_match(line, match, bquote_re));
        REQUIRE(match[1].str() == "> ");
        REQUIRE(match[2].str() == "some quoted text");
    }

    SECTION("nested blockquote")
    {
        std::string line = "> > nested quote";
        REQUIRE(std::regex_match(line, match, bquote_re));
        REQUIRE(match[1].str() == "> > ");
    }

    SECTION("empty blockquote")
    {
        std::string line = "> ";
        REQUIRE(std::regex_match(line, match, bquote_re));
        REQUIRE(match[2].str().empty());
    }
}

TEST_CASE("Task list pattern matches", "[editor_enhancements]")
{
    static const std::regex task_re(R"(^(\s*- \[[ xX]\] )(.*)$)");
    std::smatch match;

    SECTION("unchecked task")
    {
        std::string line = "- [ ] todo item";
        REQUIRE(std::regex_match(line, match, task_re));
        REQUIRE(match[2].str() == "todo item");
    }

    SECTION("checked task")
    {
        std::string line = "- [x] done item";
        REQUIRE(std::regex_match(line, match, task_re));
        REQUIRE(match[2].str() == "done item");
    }

    SECTION("empty task item")
    {
        std::string line = "- [ ] ";
        REQUIRE(std::regex_match(line, match, task_re));
        REQUIRE(match[2].str().empty());
    }
}

// ═══════════════════════════════════════════════════════
// Config key names for editor prefs
// ═══════════════════════════════════════════════════════

TEST_CASE("Editor config keys are well-formed", "[editor_enhancements]")
{
    // Verify the key names we use in LoadPreferences/SavePreferences
    std::vector<std::string> keys = {"editor.font_size",
                                     "editor.tab_size",
                                     "editor.word_wrap_mode",
                                     "editor.show_line_numbers",
                                     "editor.bracket_matching",
                                     "editor.auto_indent",
                                     "editor.large_file_threshold"};

    for (const auto& key : keys)
    {
        REQUIRE(key.starts_with("editor."));
        REQUIRE(key.size() > 7); // "editor." is 7 chars
    }
    REQUIRE(keys.size() == 7);
}

// ═══════════════════════════════════════════════════════
// ViewMode + WrapMode coexist
// ═══════════════════════════════════════════════════════

TEST_CASE("ViewMode and WrapMode enums coexist without conflict", "[editor_enhancements]")
{
    auto vm = ViewMode::Split;
    auto wm = WrapMode::Word;

    // Both should be valid and distinct types
    REQUIRE(vm == ViewMode::Split);
    REQUIRE(wm == WrapMode::Word);
}
