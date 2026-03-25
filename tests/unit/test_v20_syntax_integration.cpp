/// @file test_v20_syntax_integration.cpp
/// @brief V20 Phase 07 – Syntax/highlighting integration tests.

#include "core/HighlightDegradation.h"
#include "core/LanguageService.h"
#include "core/TokenThemeMapper.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("SyntaxInteg: Language → Tokens → Theme pipeline", "[v20][syntax-integration]")
{
    LanguageService lang;
    TokenThemeMapper theme;

    // Register theme scopes
    ScopeMapping keyword;
    keyword.scope = "keyword";
    keyword.token_class = TokenClass::kKeyword;
    keyword.style.foreground_color = "#C678DD";
    keyword.style.is_bold = true;
    theme.register_scope(keyword);

    ScopeMapping comment;
    comment.scope = "comment";
    comment.token_class = TokenClass::kComment;
    comment.style.foreground_color = "#5C6370";
    comment.style.is_italic = true;
    theme.register_scope(comment);

    ScopeMapping str;
    str.scope = "string";
    str.token_class = TokenClass::kString;
    str.style.foreground_color = "#98C379";
    theme.register_scope(str);

    // Resolve language from file
    auto resolution = lang.resolve_from_path("/project/main.cpp");
    REQUIRE(resolution.ok());
    REQUIRE(resolution.language_id == "cpp");

    // Map tokens to theme styles
    auto kw_style = theme.resolve_style(TokenClass::kKeyword);
    REQUIRE(kw_style.foreground_color == "#C678DD");

    auto cm_style = theme.resolve_style(TokenClass::kComment);
    REQUIRE(cm_style.is_italic);
}

TEST_CASE("SyntaxInteg: Cross-surface consistency", "[v20][syntax-integration]")
{
    LanguageService lang;
    TokenThemeMapper theme;
    HighlightDegradationService degradation;

    // Same language across surfaces
    auto editor = lang.resolve_from_path("/code.py");
    auto notebook = lang.resolve_from_id("python");

    REQUIRE(editor.language_id == notebook.language_id);

    // All surfaces should share tokens
    REQUIRE(degradation.should_share_tokens(HighlightSurface::kEditor));
    REQUIRE(degradation.should_share_tokens(HighlightSurface::kEditor));
    REQUIRE(degradation.should_share_tokens(HighlightSurface::kDiffPanel));
}

TEST_CASE("SyntaxInteg: Large file degradation pipeline", "[v20][syntax-integration]")
{
    LanguageService lang;
    HighlightDegradationService degradation;

    auto resolution = lang.resolve_from_path("/big/file.cpp");
    REQUIRE(resolution.ok());

    // Small file: full highlight
    REQUIRE(degradation.determine_mode(1000) == DegradationMode::kFull);

    // Large file: viewport mode
    REQUIRE(degradation.determine_mode(15000) == DegradationMode::kViewport);

    // Very large file: minimal
    REQUIRE(degradation.determine_mode(80000) == DegradationMode::kMinimal);
}

TEST_CASE("SyntaxInteg: Theme mapping completeness", "[v20][syntax-integration]")
{
    TokenThemeMapper theme;

    // Register common scopes
    auto reg = [&](const std::string& scope, TokenClass cls, const std::string& color) {
        ScopeMapping m;
        m.scope = scope;
        m.token_class = cls;
        m.style.foreground_color = color;
        theme.register_scope(m);
    };

    reg("keyword", TokenClass::kKeyword, "#C678DD");
    reg("string", TokenClass::kString, "#98C379");
    reg("comment", TokenClass::kComment, "#5C6370");
    reg("number", TokenClass::kNumber, "#D19A66");
    reg("operator", TokenClass::kOperator, "#56B6C2");
    reg("type", TokenClass::kType, "#E5C07B");
    reg("function", TokenClass::kFunction, "#61AFEF");

    REQUIRE(theme.mapping_count() == 7);
    REQUIRE(theme.unmapped_count() == 0);

    // Request unmapped class
    theme.resolve_style(TokenClass::kVariable);
    REQUIRE(theme.unmapped_count() == 1);
}

TEST_CASE("SyntaxInteg: Fenced code language support", "[v20][syntax-integration]")
{
    LanguageService lang;

    auto fenced = lang.fenced_block_languages();
    REQUIRE(fenced.size() > 10);

    // Markdown itself should not support fenced blocks
    auto* md = lang.find_language("markdown");
    REQUIRE(md != nullptr);
    REQUIRE_FALSE(md->supports_fenced_blocks);

    // Python should
    auto* py = lang.find_language("python");
    REQUIRE(py != nullptr);
    REQUIRE(py->supports_fenced_blocks);
}
