/// @file fuzz_markdown_parser.cpp
/// Fuzz testing stub for MarkdownParser (Phase 34D, PRD Task 17).
///
/// Ensures the markdown parser never crashes on arbitrary input.
/// In a CI environment with libFuzzer, this file would be compiled
/// with -fsanitize=fuzzer. For now, it runs as a Catch2 test with
/// pre-defined edge-case inputs (seed corpus).

#include "core/MarkdownParser.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::core;

namespace
{

/// Seed corpus of edge-case markdown inputs.
auto seed_corpus() -> const std::vector<std::string>&
{
    static const std::vector<std::string> corpus = {
        "",
        "\n",
        "# ",
        "# Heading\n",
        "## Very long " + std::string(1000, 'x') + "\n",
        "```\n```\n",
        "```cpp\nint x;\n```\n",
        "```\n" + std::string(10000, 'a') + "\n```\n",
        "- item\n  - nested\n    - deep\n      - deeper\n",
        "> quote\n>> nested\n>>> deeper\n",
        "| a | b |\n|---|---|\n| 1 | 2 |\n",
        "[link](url)\n",
        "![image](path)\n",
        "*italic* **bold** ***both***\n",
        "---\n",
        "***\n",
        "___\n",
        "text[^1]\n\n[^1]: footnote\n",
        "~~strikethrough~~\n",
        "- [ ] task\n- [x] done\n",
        std::string(500, '#') + " broken heading\n",
        std::string(500, '>') + " deeply quoted\n",
        std::string(500, '-') + "\n",
        "```\n" + std::string(500, '`') + "\n```\n",
        "[]()\n",
        "![]()\n",
        "\\# escaped heading\n",
        "\\*not italic\\*\n",
        "<div>html block</div>\n",
        "<!-- comment -->\n",
        "text\n\n\n\n\n\n\ntext\n",
        "\t\ttabbed content\n",
        "a" + std::string(50000, ' ') + "b\n",
    };
    return corpus;
}

} // namespace

TEST_CASE("Fuzz: MarkdownParser does not crash on seed corpus", "[fuzz][parser]")
{
    MarkdownParser parser;

    for (const auto& input : seed_corpus())
    {
        // Must not crash — result validity is secondary
        auto result = parser.parse(input);
        // We only check that parse returns (no segfault/abort)
        (void)result;
    }
    REQUIRE(true); // Reached — no crash
}

TEST_CASE("Fuzz: MarkdownParser handles binary-like input", "[fuzz][parser]")
{
    MarkdownParser parser;

    // Create input with non-printable characters
    std::string binary_input;
    for (int idx = 0; idx < 256; ++idx)
    {
        binary_input.push_back(static_cast<char>(idx));
    }

    auto result = parser.parse(binary_input);
    (void)result;
    REQUIRE(true); // Reached without crash
}

TEST_CASE("Fuzz: MarkdownParser handles repeated special sequences", "[fuzz][parser]")
{
    MarkdownParser parser;

    const std::vector<std::string> patterns = {
        std::string(100, '[') + std::string(100, ']'),
        std::string(100, '(') + std::string(100, ')'),
        std::string(100, '*'),
        std::string(100, '_'),
        std::string(100, '~'),
        std::string(100, '`'),
    };

    for (const auto& pattern : patterns)
    {
        auto result = parser.parse(pattern);
        (void)result;
    }
    REQUIRE(true);
}
