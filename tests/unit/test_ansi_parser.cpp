// test_ansi_parser.cpp — 10 tests for AnsiParser
#include "core/AnsiParser.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AnsiParser plain text produces TextOutput", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("Hello World");
    REQUIRE_FALSE(ops.empty());
    auto* text = std::get_if<TextOutput>(&ops[0]);
    REQUIRE(text != nullptr);
    CHECK(text->text == "Hello World");
}

TEST_CASE("AnsiParser bold SGR sets bold attribute", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("\033[1mBold");
    bool found_bold = false;
    for (const auto& op : ops)
    {
        if (const auto* text = std::get_if<TextOutput>(&op))
        {
            if (text->text == "Bold")
            {
                found_bold = text->attrs.bold;
            }
        }
    }
    CHECK(found_bold);
}

TEST_CASE("AnsiParser reset SGR clears attributes", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("\033[1m\033[0mNormal");
    for (const auto& op : ops)
    {
        if (const auto* text = std::get_if<TextOutput>(&op))
        {
            if (text->text == "Normal")
            {
                CHECK_FALSE(text->attrs.bold);
            }
        }
    }
}

TEST_CASE("AnsiParser cursor position CSI", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("\033[5;10H");
    bool found = false;
    for (const auto& op : ops)
    {
        if (const auto* pos = std::get_if<CursorPosition>(&op))
        {
            found = true;
            CHECK(pos->row >= 0);
            CHECK(pos->col >= 0);
        }
    }
    CHECK(found);
}

TEST_CASE("AnsiParser erase display CSI", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("\033[2J");
    bool found = false;
    for (const auto& op : ops)
    {
        if (std::get_if<EraseDisplay>(&op) != nullptr)
        {
            found = true;
        }
    }
    CHECK(found);
}

TEST_CASE("AnsiParser erase line CSI", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("\033[K");
    bool found = false;
    for (const auto& op : ops)
    {
        if (std::get_if<EraseLine>(&op) != nullptr)
        {
            found = true;
        }
    }
    CHECK(found);
}

TEST_CASE("AnsiParser OSC title change", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("\033]0;My Title\007");
    bool found = false;
    for (const auto& op : ops)
    {
        if (const auto* title = std::get_if<SetTitle>(&op))
        {
            found = true;
            CHECK(title->title == "My Title");
        }
    }
    CHECK(found);
}

TEST_CASE("AnsiParser newline produces LineFeed", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("a\nb");
    bool has_lf = false;
    for (const auto& op : ops)
    {
        if (std::get_if<LineFeed>(&op) != nullptr)
        {
            has_lf = true;
        }
    }
    CHECK(has_lf);
}

TEST_CASE("AnsiParser reset clears state", "[ansi]")
{
    AnsiParser parser;
    parser.parse("\033[1m");
    parser.reset();
    auto ops = parser.parse("After reset");
    for (const auto& op : ops)
    {
        if (const auto* text = std::get_if<TextOutput>(&op))
        {
            CHECK_FALSE(text->attrs.bold);
        }
    }
}

TEST_CASE("AnsiParser empty input returns empty ops", "[ansi]")
{
    AnsiParser parser;
    auto ops = parser.parse("");
    CHECK(ops.empty());
}
