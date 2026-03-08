/// test_snippet_engine.cpp — Comprehensive tests for SnippetEngine
#include "core/SnippetEngine.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ── Positive Tests ──

TEST_CASE("SnippetEngine: register and count", "[snippet_engine][positive]")
{
    SnippetEngine engine;
    REQUIRE(engine.count() == 0);
    engine.register_snippet(
        {"Callout", "!callout", "> [!NOTE]\n> $0", "Insert callout", "markdown"});
    REQUIRE(engine.count() == 1);
}

TEST_CASE("SnippetEngine: expand by prefix", "[snippet_engine][positive]")
{
    SnippetEngine engine;
    engine.register_snippet({"Bold", "!bold", "**$1**$0", "Bold text", ""});
    auto result = engine.expand("!bold");
    REQUIRE(result.has_value());
    REQUIRE_FALSE(result->empty());
}

TEST_CASE("SnippetEngine: get_snippets returns all", "[snippet_engine][positive]")
{
    SnippetEngine engine;
    engine.register_snippet({"A", "!a", "body_a", "", ""});
    engine.register_snippet({"B", "!b", "body_b", "", ""});
    REQUIRE(engine.get_snippets().size() == 2);
}

TEST_CASE("SnippetEngine: get_by_name finds exact name", "[snippet_engine][positive]")
{
    SnippetEngine engine;
    engine.register_snippet({"TestSnip", "!ts", "body", "", ""});
    auto* snip = engine.get_by_name("TestSnip");
    REQUIRE(snip != nullptr);
    REQUIRE(snip->prefix == "!ts");
}

TEST_CASE("SnippetEngine: get_for_prefix filters by prefix start", "[snippet_engine][positive]")
{
    SnippetEngine engine;
    engine.register_snippet({"Note", "!note", "> Note", "", ""});
    engine.register_snippet({"Notice", "!notice", "> Notice", "", ""});
    engine.register_snippet({"Bold", "!bold", "**bold**", "", ""});
    auto matches = engine.get_for_prefix("!no");
    REQUIRE(matches.size() == 2); // !note and !notice
}

TEST_CASE("SnippetEngine: clear removes all snippets", "[snippet_engine][positive]")
{
    SnippetEngine engine;
    engine.register_snippet({"A", "!a", "body", "", ""});
    engine.register_snippet({"B", "!b", "body", "", ""});
    engine.clear();
    REQUIRE(engine.count() == 0);
    REQUIRE(engine.get_snippets().empty());
}

TEST_CASE("SnippetEngine: expand_body resolves tab stops", "[snippet_engine][positive]")
{
    auto expanded = SnippetEngine::expand_body("Hello $1 World $0");
    REQUIRE_FALSE(expanded.empty());
}

// ── Negative Tests ──

TEST_CASE("SnippetEngine: expand unknown prefix returns nullopt", "[snippet_engine][negative]")
{
    SnippetEngine engine;
    auto result = engine.expand("!nonexistent");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("SnippetEngine: get_by_name unknown returns nullptr", "[snippet_engine][negative]")
{
    SnippetEngine engine;
    REQUIRE(engine.get_by_name("Nope") == nullptr);
}

TEST_CASE("SnippetEngine: get_for_prefix no matches returns empty", "[snippet_engine][negative]")
{
    SnippetEngine engine;
    engine.register_snippet({"A", "!a", "body", "", ""});
    auto matches = engine.get_for_prefix("!z");
    REQUIRE(matches.empty());
}

TEST_CASE("SnippetEngine: expand after clear returns nullopt", "[snippet_engine][negative]")
{
    SnippetEngine engine;
    engine.register_snippet({"A", "!a", "body", "", ""});
    engine.clear();
    REQUIRE_FALSE(engine.expand("!a").has_value());
}

// ── Edge Cases ──

TEST_CASE("SnippetEngine: snippet with empty body", "[snippet_engine][edge]")
{
    SnippetEngine engine;
    engine.register_snippet({"Empty", "!empty", "", "", ""});
    auto result = engine.expand("!empty");
    REQUIRE(result.has_value());
}

TEST_CASE("Snippet struct: default values", "[snippet_engine][edge]")
{
    Snippet s;
    REQUIRE(s.name.empty());
    REQUIRE(s.prefix.empty());
    REQUIRE(s.body.empty());
}

TEST_CASE("SnippetTabStop struct: default values", "[snippet_engine][edge]")
{
    SnippetTabStop ts;
    REQUIRE(ts.index == 0);
    REQUIRE(ts.placeholder.empty());
    REQUIRE(ts.choices.empty());
}
