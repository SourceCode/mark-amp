/// @file test_v20_token_theme.cpp
/// @brief V20 Phase 07 – TokenThemeMapper unit tests.

#include "core/TokenThemeMapper.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TokenThemeMapper: construction", "[v20][token-theme]")
{
    TokenThemeMapper mapper;
    REQUIRE(mapper.mapping_count() == 0);
    REQUIRE(mapper.unmapped_count() == 0);
}

TEST_CASE("TokenThemeMapper: register and resolve by class", "[v20][token-theme]")
{
    TokenThemeMapper mapper;

    ScopeMapping keyword;
    keyword.scope = "keyword";
    keyword.token_class = TokenClass::kKeyword;
    keyword.style.foreground_color = "#C678DD";
    keyword.style.is_bold = true;
    mapper.register_scope(keyword);

    auto style = mapper.resolve_style(TokenClass::kKeyword);
    REQUIRE(style.foreground_color == "#C678DD");
    REQUIRE(style.is_bold);
}

TEST_CASE("TokenThemeMapper: resolve by scope", "[v20][token-theme]")
{
    TokenThemeMapper mapper;

    ScopeMapping comment;
    comment.scope = "comment";
    comment.token_class = TokenClass::kComment;
    comment.style.foreground_color = "#5C6370";
    comment.style.is_italic = true;
    mapper.register_scope(comment);

    auto style = mapper.resolve_scope("comment");
    REQUIRE(style.foreground_color == "#5C6370");
    REQUIRE(style.is_italic);

    // Prefix match
    auto style2 = mapper.resolve_scope("comment.line");
    REQUIRE(style2.foreground_color == "#5C6370");
}

TEST_CASE("TokenThemeMapper: unmapped fallback", "[v20][token-theme]")
{
    TokenThemeMapper mapper;

    TokenStyle def_style;
    def_style.foreground_color = "#ABB2BF";
    mapper.set_default_style(def_style);

    auto style = mapper.resolve_style(TokenClass::kVariable);
    REQUIRE(style.foreground_color == "#ABB2BF");
    REQUIRE(mapper.unmapped_count() == 1);
}

TEST_CASE("TokenThemeMapper: all_scopes", "[v20][token-theme]")
{
    TokenThemeMapper mapper;

    ScopeMapping s1;
    s1.scope = "keyword";
    s1.token_class = TokenClass::kKeyword;
    mapper.register_scope(s1);

    ScopeMapping s2;
    s2.scope = "string";
    s2.token_class = TokenClass::kString;
    mapper.register_scope(s2);

    auto scopes = mapper.all_scopes();
    REQUIRE(scopes.size() == 2);
}

TEST_CASE("TokenThemeMapper: default_style", "[v20][token-theme]")
{
    TokenThemeMapper mapper;
    REQUIRE_FALSE(mapper.default_style().has_foreground());

    TokenStyle style;
    style.foreground_color = "#FFFFFF";
    mapper.set_default_style(style);
    REQUIRE(mapper.default_style().has_foreground());
}

TEST_CASE("Phase 07 events: TokenScopeRegisteredEvent", "[v20][token-events]")
{
    events::TokenScopeRegisteredEvent evt;
    evt.scope = "keyword.control";
    evt.foreground_color = "#C678DD";
    REQUIRE(evt.scope == "keyword.control");
}

TEST_CASE("Phase 07 events: ThemeTokenMappingChangedEvent", "[v20][token-events]")
{
    events::ThemeTokenMappingChangedEvent evt;
    evt.total_mappings = 42;
    evt.unmapped_count = 3;
    REQUIRE(evt.total_mappings == 42);
}
