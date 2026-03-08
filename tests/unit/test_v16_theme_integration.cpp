/// @file test_v16_theme_integration.cpp
/// @brief V16 — Comprehensive tests for theme integration: VsCodeThemeAdapter scope
///        mapping, ThemeColorToken enum completeness via ScopedTokenMap resolve,
///        ThemeSyntaxColors defaults, and round-trip resolution for all V16 tokens.

#include "core/ScopedTokenMap.h"
#include "core/Theme.h"
#include "core/ThemeTokens.h"
#include "core/VsCodeThemeAdapter.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// VsCodeThemeAdapter::map_vscode_scope — original 8 syntax scopes
// ============================================================================

TEST_CASE("map_vscode_scope maps keyword scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("keyword") == "syntax_keyword");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("keyword.control") == "syntax_keyword");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("keyword.operator") == "syntax_operator");
}

TEST_CASE("map_vscode_scope maps string scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("string") == "syntax_string");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("string.quoted") == "syntax_string");
}

TEST_CASE("map_vscode_scope maps comment scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("comment") == "syntax_comment");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("comment.line") == "syntax_comment");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("comment.block") == "syntax_comment");
}

TEST_CASE("map_vscode_scope maps type/function/preprocessor scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("constant.numeric") == "syntax_number");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.type") == "syntax_type");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.function") == "syntax_function");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("storage.type") == "syntax_type");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("meta.preprocessor") == "syntax_preprocessor");
}

// ============================================================================
// VsCodeThemeAdapter::map_vscode_scope — V16 Phase 17 fine-grained scopes
// ============================================================================

TEST_CASE("map_vscode_scope maps V16 variable scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("variable") == "syntax_variable");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("variable.other") == "syntax_variable");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("variable.parameter") == "syntax_parameter");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("variable.other.property") == "syntax_property");
}

TEST_CASE("map_vscode_scope maps V16 constant/escape scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("constant") == "syntax_constant");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("constant.language") == "syntax_constant");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("constant.character.escape") == "syntax_escape");
}

TEST_CASE("map_vscode_scope maps V16 entity scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.tag") == "syntax_tag");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.other.attribute-name") ==
          "syntax_attribute");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.namespace") == "syntax_namespace");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.type.enum") == "syntax_enum");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.type.interface") == "syntax_interface");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.type.struct") == "syntax_struct");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("entity.name.function.macro") == "syntax_macro");
}

TEST_CASE("map_vscode_scope maps V16 decorator/regex/doc scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("meta.decorator") == "syntax_decorator");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("punctuation.definition.decorator") ==
          "syntax_decorator");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("string.regexp") == "syntax_regex");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("comment.block.documentation") ==
          "syntax_doc_comment");
}

TEST_CASE("map_vscode_scope maps LSP semantic token fallbacks", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("namespace") == "syntax_namespace");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("type") == "syntax_type");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("class") == "syntax_type");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("enum") == "syntax_enum");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("interface") == "syntax_interface");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("struct") == "syntax_struct");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("typeParameter") == "syntax_type");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("parameter") == "syntax_parameter");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("function") == "syntax_function");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("method") == "syntax_function");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("macro") == "syntax_macro");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("decorator") == "syntax_decorator");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("regexp") == "syntax_regex");
    CHECK(VsCodeThemeAdapter::map_vscode_scope("property") == "syntax_property");
}

TEST_CASE("map_vscode_scope returns empty for unknown scopes", "[v16][vscode_scope]")
{
    CHECK(VsCodeThemeAdapter::map_vscode_scope("unknown.scope.xyz").empty());
    CHECK(VsCodeThemeAdapter::map_vscode_scope("").empty());
    CHECK(VsCodeThemeAdapter::map_vscode_scope("meta.unknown.block").empty());
}

// ============================================================================
// ThemeColorToken V16 enum — resolve via ScopedTokenMap public API
// ============================================================================

TEST_CASE("ScopedTokenMap resolves all V16 enum tokens when populated", "[v16][token_map]")
{
    ScopedTokenMap map;
    const wxColour marker(42, 42, 42);

    // V16 enum tokens map to specific string keys via the enum mapping.
    // We populate the known VSCode-compatible keys and verify enum resolution works.
    map.set("variable", marker);
    map.set("constant", marker);
    map.set("entity.name.tag", marker);
    map.set("entity.other.attribute-name", marker);
    map.set("variable.other.property", marker);
    map.set("entity.name.namespace", marker);
    map.set("entity.name.type.enum", marker);
    map.set("entity.name.type.interface", marker);
    map.set("entity.name.type.struct", marker);
    map.set("variable.parameter", marker);
    map.set("meta.decorator", marker);
    map.set("string.regexp", marker);
    map.set("constant.character.escape", marker);
    map.set("comment.block.documentation", marker);
    map.set("entity.name.function.macro", marker);

    CHECK(map.resolve(ThemeColorToken::SyntaxVariable).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxConstant).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxTag).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxAttribute).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxProperty).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxNamespace).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxEnum).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxInterface).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxStruct).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxParameter).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxDecorator).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxRegex).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxEscape).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxDocComment).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxMacro).has_value());
}

TEST_CASE("ScopedTokenMap resolves V16 enums to correct colors", "[v16][token_map]")
{
    ScopedTokenMap map;

    map.set("variable", wxColour(10, 20, 30));
    map.set("entity.name.tag", wxColour(100, 110, 120));
    map.set("entity.name.function.macro", wxColour(200, 210, 220));

    auto var_color = map.resolve(ThemeColorToken::SyntaxVariable);
    REQUIRE(var_color.has_value());
    CHECK(var_color->Red() == 10);
    CHECK(var_color->Green() == 20);
    CHECK(var_color->Blue() == 30);

    auto tag_color = map.resolve(ThemeColorToken::SyntaxTag);
    REQUIRE(tag_color.has_value());
    CHECK(tag_color->Red() == 100);

    auto macro_color = map.resolve(ThemeColorToken::SyntaxMacro);
    REQUIRE(macro_color.has_value());
    CHECK(macro_color->Red() == 200);
}

TEST_CASE("ScopedTokenMap returns nullopt for unpopulated V16 tokens", "[v16][token_map]")
{
    const ScopedTokenMap map;
    CHECK_FALSE(map.resolve(ThemeColorToken::SyntaxVariable).has_value());
    CHECK_FALSE(map.resolve(ThemeColorToken::SyntaxMacro).has_value());
}

TEST_CASE("ScopedTokenMap also resolves original 8 syntax tokens", "[v16][token_map]")
{
    ScopedTokenMap map;
    const wxColour color(50, 60, 70);

    map.set("keyword", color);
    map.set("string", color);
    map.set("comment", color);
    map.set("number", color);
    map.set("type", color);
    map.set("function", color);
    map.set("operator", color);
    map.set("preprocessor", color);

    CHECK(map.resolve(ThemeColorToken::SyntaxKeyword).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxString).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxComment).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxNumber).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxType).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxFunction).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxOperator).has_value());
    CHECK(map.resolve(ThemeColorToken::SyntaxPreprocessor).has_value());
}

// ============================================================================
// ThemeSyntaxColors V16 defaults — Color fields are non-zero
// ============================================================================

TEST_CASE("ThemeSyntaxColors V16 fields have non-zero default colors", "[v16][syntax_colors]")
{
    const ThemeSyntaxColors syntax;

    auto is_nonzero = [](const Color& color) -> bool
    { return color.r > 0 || color.g > 0 || color.b > 0; };

    CHECK(is_nonzero(syntax.variable));
    CHECK(is_nonzero(syntax.constant));
    CHECK(is_nonzero(syntax.tag));
    CHECK(is_nonzero(syntax.attribute));
    CHECK(is_nonzero(syntax.property));
    CHECK(is_nonzero(syntax.namespace_name));
    CHECK(is_nonzero(syntax.enum_name));
    CHECK(is_nonzero(syntax.interface_name));
    CHECK(is_nonzero(syntax.struct_name));
    CHECK(is_nonzero(syntax.parameter));
    CHECK(is_nonzero(syntax.decorator));
    CHECK(is_nonzero(syntax.regex));
    CHECK(is_nonzero(syntax.escape));
    CHECK(is_nonzero(syntax.doc_comment));
    CHECK(is_nonzero(syntax.macro));
}

TEST_CASE("ThemeSyntaxColors V16 defaults match expected palette", "[v16][syntax_colors]")
{
    const ThemeSyntaxColors syntax;

    // Spot-check key values against their documented defaults
    CHECK(syntax.variable.r == 200);
    CHECK(syntax.variable.g == 200);
    CHECK(syntax.variable.b == 220);

    CHECK(syntax.tag.r == 240);
    CHECK(syntax.tag.g == 120);
    CHECK(syntax.tag.b == 100);

    CHECK(syntax.macro.r == 200);
    CHECK(syntax.macro.g == 140);
    CHECK(syntax.macro.b == 220);
}

TEST_CASE("ThemeSyntaxColors is copy-constructible and comparable", "[v16][syntax_colors]")
{
    const ThemeSyntaxColors original;
    const ThemeSyntaxColors copy{original};
    CHECK(original == copy);
}

// ============================================================================
// ScopedTokenMap round-trip resolution for V16 tokens
// ============================================================================

TEST_CASE("ScopedTokenMap set/resolve round-trip for V16 string keys", "[v16][token_resolve]")
{
    ScopedTokenMap map;
    const wxColour test_color(0xAB, 0xCD, 0xEF);

    map.set("variable", test_color);
    auto resolved = map.resolve("variable");
    REQUIRE(resolved.has_value());
    CHECK(resolved->IsOk());
    CHECK(resolved->Red() == 0xAB);
    CHECK(resolved->Green() == 0xCD);
    CHECK(resolved->Blue() == 0xEF);
}

TEST_CASE("ScopedTokenMap resolves V16 enum tokens via mapping", "[v16][token_resolve]")
{
    ScopedTokenMap map;
    const wxColour test_color(100, 200, 150);

    // The enum SyntaxVariable maps to "variable" in get_enum_mapping
    map.set("variable", test_color);
    auto resolved = map.resolve(ThemeColorToken::SyntaxVariable);
    REQUIRE(resolved.has_value());
    CHECK(resolved->Red() == 100);
    CHECK(resolved->Green() == 200);
    CHECK(resolved->Blue() == 150);
}

TEST_CASE("ScopedTokenMap returns nullopt for unset V16 tokens", "[v16][token_resolve]")
{
    const ScopedTokenMap map;
    auto resolved = map.resolve("variable");
    CHECK_FALSE(resolved.has_value());

    auto enum_resolved = map.resolve(ThemeColorToken::SyntaxMacro);
    CHECK_FALSE(enum_resolved.has_value());
}

TEST_CASE("ScopedTokenMap clear removes V16 tokens", "[v16][token_resolve]")
{
    ScopedTokenMap map;
    map.set("variable", wxColour(10, 20, 30));
    REQUIRE(map.resolve("variable").has_value());

    map.clear();
    CHECK_FALSE(map.resolve("variable").has_value());
}

TEST_CASE("ScopedTokenMap is_explicit detects V16 tokens", "[v16][token_resolve]")
{
    ScopedTokenMap map;
    CHECK_FALSE(map.is_explicit("variable"));

    map.set("variable", wxColour(1, 2, 3));
    CHECK(map.is_explicit("variable"));
}

// ============================================================================
// VsCodeThemeAdapter compatibility_report with V16
// ============================================================================

TEST_CASE("compatibility_report counts V16 mapped scopes", "[v16][compat_report]")
{
    std::vector<VsCodeTokenRule> rules;
    rules.push_back({"keyword.control", "#C678DD", ""});
    rules.push_back({"variable.parameter", "#ABB2BF", ""});
    rules.push_back({"entity.name.tag", "#E06C75", ""});
    rules.push_back({"string.regexp", "#56B6C2", ""});

    auto report = VsCodeThemeAdapter::compatibility_report({}, rules);
    CHECK(report.supported_tokens >= 4);
    CHECK(report.total_tokens >= 4);
}

TEST_CASE("compatibility_report handles empty input", "[v16][compat_report]")
{
    auto report = VsCodeThemeAdapter::compatibility_report({}, {});
    CHECK(report.total_tokens == 0);
    CHECK(report.supported_tokens == 0);
}

TEST_CASE("compatibility_report treats all token rules as supported", "[v16][compat_report]")
{
    // The adapter treats all token rules as at least partially supported
    std::vector<VsCodeTokenRule> rules;
    rules.push_back({"unknown.scope.xyz", "#FFFFFF", ""});

    auto report = VsCodeThemeAdapter::compatibility_report({}, rules);
    CHECK(report.supported_tokens == 1);
    CHECK(report.total_tokens == 1);
}

TEST_CASE("compatibility_report flags unmapped color keys", "[v16][compat_report]")
{
    std::vector<VsCodeColorEntry> colors;
    colors.push_back({"custom.unknown.color", "#FF0000"});

    auto report = VsCodeThemeAdapter::compatibility_report(colors, {});
    CHECK(!report.unmapped_colors.empty());
}

TEST_CASE("compatibility_report handles mixed mapped and unmapped", "[v16][compat_report]")
{
    std::vector<VsCodeTokenRule> rules;
    rules.push_back({"keyword", "#C678DD", ""});      // mapped
    rules.push_back({"comment", "#5C6370", ""});      // mapped
    rules.push_back({"custom.fancy", "#AABBCC", ""}); // treated as supported

    auto report = VsCodeThemeAdapter::compatibility_report({}, rules);
    CHECK(report.supported_tokens == 3);
    CHECK(report.total_tokens == 3);
}
