/// test_string_utils.cpp — Unit tests for StringUtils

#include "core/StringUtils.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("escape_html: ampersand is escaped", "[string_utils]")
{
    REQUIRE(escape_html("a&b") == "a&amp;b");
}

TEST_CASE("escape_html: less-than is escaped", "[string_utils]")
{
    REQUIRE(escape_html("a<b") == "a&lt;b");
}

TEST_CASE("escape_html: greater-than is escaped", "[string_utils]")
{
    REQUIRE(escape_html("a>b") == "a&gt;b");
}

TEST_CASE("escape_html: double-quote is escaped", "[string_utils]")
{
    REQUIRE(escape_html("a\"b") == "a&quot;b");
}

TEST_CASE("escape_html: single-quote is escaped", "[string_utils]")
{
    REQUIRE(escape_html("a'b") == "a&#39;b");
}

TEST_CASE("escape_html: normal text passes through", "[string_utils]")
{
    REQUIRE(escape_html("Hello World 123") == "Hello World 123");
}

TEST_CASE("escape_html: empty string returns empty", "[string_utils]")
{
    REQUIRE(escape_html("").empty());
}

TEST_CASE("escape_html: multiple special chars", "[string_utils]")
{
    REQUIRE(escape_html("<div class=\"x\">&</div>") ==
            "&lt;div class=&quot;x&quot;&gt;&amp;&lt;/div&gt;");
}
