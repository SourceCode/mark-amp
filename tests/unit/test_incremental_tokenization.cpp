/// test_incremental_tokenization.cpp — Phase 28: IncrementalTokenizer tests

#include "core/IncrementalTokenizer.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

TEST_CASE("IncrementalTokenizer: set_content marks all dirty", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"int x = 0;", "return x;", "}"});

    REQUIRE(tokenizer.line_count() == 3);
    REQUIRE(tokenizer.dirty_count() == 3);
    REQUIRE_FALSE(tokenizer.is_valid(0));
}

TEST_CASE("IncrementalTokenizer: retokenize clears dirty flags", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"int x = 0;", "return x;"});
    auto count = tokenizer.retokenize();

    REQUIRE(count == 2);
    REQUIRE(tokenizer.dirty_count() == 0);
    REQUIRE(tokenizer.is_valid(0));
    REQUIRE(tokenizer.is_valid(1));
}

TEST_CASE("IncrementalTokenizer: invalidate_range marks subset dirty", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"line 0", "line 1", "line 2", "line 3"});
    tokenizer.retokenize(); // all clean

    tokenizer.invalidate_range(1, 3); // invalidate lines 1,2

    REQUIRE(tokenizer.dirty_count() == 2);
    REQUIRE(tokenizer.is_valid(0));
    REQUIRE_FALSE(tokenizer.is_valid(1));
    REQUIRE_FALSE(tokenizer.is_valid(2));
    REQUIRE(tokenizer.is_valid(3));
}

TEST_CASE("IncrementalTokenizer: partial retokenize", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"a", "b", "c", "d"});
    tokenizer.retokenize();

    tokenizer.invalidate_line(2);
    auto count = tokenizer.retokenize();

    REQUIRE(count == 1); // only line 2 re-tokenized
}

TEST_CASE("IncrementalTokenizer: generation increments on retokenize", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"test"});

    REQUIRE(tokenizer.generation() == 0);
    tokenizer.retokenize();
    REQUIRE(tokenizer.generation() == 1);
    tokenizer.retokenize();
    REQUIRE(tokenizer.generation() == 2);
}

TEST_CASE("IncrementalTokenizer: replace_line invalidates", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"old line"});
    tokenizer.retokenize();

    tokenizer.replace_line(0, "new line");

    REQUIRE(tokenizer.dirty_count() == 1);
    REQUIRE_FALSE(tokenizer.is_valid(0));
}

TEST_CASE("IncrementalTokenizer: insert_lines shifts data", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"a", "b"});
    tokenizer.retokenize();

    tokenizer.insert_lines(1, {"x", "y"});

    REQUIRE(tokenizer.line_count() == 4);
    REQUIRE(tokenizer.dirty_count() == 2); // newly inserted lines
}

TEST_CASE("IncrementalTokenizer: remove_lines shrinks data", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"a", "b", "c", "d"});
    tokenizer.retokenize();

    tokenizer.remove_lines(1, 2); // remove lines 1,2

    REQUIRE(tokenizer.line_count() == 2);
}

TEST_CASE("IncrementalTokenizer: out-of-range line returns empty", "[incremental_tokenizer]")
{
    SyntaxHighlighter highlighter;
    IncrementalTokenizer tokenizer(highlighter, "cpp");

    tokenizer.set_content({"test"});
    tokenizer.retokenize();

    REQUIRE(tokenizer.line(99).empty());
}
