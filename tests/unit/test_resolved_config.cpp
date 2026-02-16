/// test_resolved_config.cpp — Phase 17: ResolvedConfig strategy tests
///
/// Validates that rebuild() correctly resolves strategy function pointers
/// based on Config::CachedValues settings.

#include "core/ResolvedConfig.h"

#include <catch2/catch_test_macros.hpp>

#include <string_view>

using namespace markamp::core;

// ══════════════════════════════════════════
// Wrap Strategy Tests
// ══════════════════════════════════════════

TEST_CASE("ResolvedConfig: word_wrap enabled selects wrap_at_column", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.word_wrap = true;
    cached.word_wrap_column = 40;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.wrap_strategy == strategy::wrap_at_column);
    REQUIRE(resolved.wrap_column == 40);
}

TEST_CASE("ResolvedConfig: word_wrap disabled selects no_wrap", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.word_wrap = false;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.wrap_strategy == strategy::no_wrap);
}

TEST_CASE("ResolvedConfig: no_wrap always returns 0", "[resolved_config]")
{
    auto result = strategy::no_wrap("This is a very long line of text", 10);
    REQUIRE(result == 0);
}

TEST_CASE("ResolvedConfig: wrap_at_column breaks at space before column", "[resolved_config]")
{
    std::string_view line = "Hello World Example Text Here";
    auto result = strategy::wrap_at_column(line, 12);

    // Should find space at position 11 ("Hello World " is 12 chars)
    REQUIRE(result > 0);
    REQUIRE(result <= 12);
    REQUIRE(line[result] == ' ');
}

TEST_CASE("ResolvedConfig: wrap_at_column returns 0 for short lines", "[resolved_config]")
{
    auto result = strategy::wrap_at_column("short", 80);
    REQUIRE(result == 0);
}

// ══════════════════════════════════════════
// Indent Strategy Tests
// ══════════════════════════════════════════

TEST_CASE("ResolvedConfig: auto_indent enabled selects match_indent", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.auto_indent = true;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.indent_strategy == strategy::match_indent);
}

TEST_CASE("ResolvedConfig: auto_indent disabled selects no_indent", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.auto_indent = false;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.indent_strategy == strategy::no_indent);
}

TEST_CASE("ResolvedConfig: match_indent extracts leading whitespace", "[resolved_config]")
{
    REQUIRE(strategy::match_indent("    code here") == 4);
    REQUIRE(strategy::match_indent("\t\tindented") == 2);
    REQUIRE(strategy::match_indent("no indent") == 0);
    REQUIRE(strategy::match_indent("") == 0);
}

// ══════════════════════════════════════════
// Whitespace Mode Tests
// ══════════════════════════════════════════

TEST_CASE("ResolvedConfig: whitespace hidden when show_whitespace false", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.show_whitespace = false;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.whitespace_mode == strategy::kWhitespaceHidden);
}

TEST_CASE("ResolvedConfig: whitespace visible when show_whitespace true", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.show_whitespace = true;
    cached.dim_whitespace = false;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.whitespace_mode == strategy::kWhitespaceVisible);
}

TEST_CASE("ResolvedConfig: whitespace dimmed when both flags true", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.show_whitespace = true;
    cached.dim_whitespace = true;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.whitespace_mode == strategy::kWhitespaceDimmed);
}

// ══════════════════════════════════════════
// Rebuild Correctness
// ══════════════════════════════════════════

TEST_CASE("ResolvedConfig: rebuild updates all fields correctly", "[resolved_config]")
{
    Config::CachedValues cached;
    cached.word_wrap = true;
    cached.word_wrap_column = 100;
    cached.auto_indent = true;
    cached.tab_size = 8;
    cached.font_size = 16;
    cached.show_line_numbers = false;
    cached.show_minimap = true;
    cached.highlight_current_line = false;
    cached.show_whitespace = true;
    cached.dim_whitespace = true;

    ResolvedConfig resolved;
    resolved.rebuild(cached);

    REQUIRE(resolved.wrap_column == 100);
    REQUIRE(resolved.tab_size == 8);
    REQUIRE(resolved.font_size == 16);
    REQUIRE(resolved.show_line_numbers == false);
    REQUIRE(resolved.show_minimap == true);
    REQUIRE(resolved.highlight_current_line == false);
    REQUIRE(resolved.whitespace_mode == strategy::kWhitespaceDimmed);
}

TEST_CASE("ResolvedConfig: rebuild changes strategy on config change", "[resolved_config]")
{
    ResolvedConfig resolved;

    // First config: wrap on
    Config::CachedValues cached;
    cached.word_wrap = true;
    cached.word_wrap_column = 80;
    resolved.rebuild(cached);
    REQUIRE(resolved.wrap_strategy == strategy::wrap_at_column);

    // Second config: wrap off
    cached.word_wrap = false;
    resolved.rebuild(cached);
    REQUIRE(resolved.wrap_strategy == strategy::no_wrap);
}
