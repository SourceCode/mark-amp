/// @file test_grammar_engine.cpp
/// @brief V16 Phase 15 — Comprehensive tests for GrammarEngine, LanguagePack,
///        SemanticTokenEngine, ScopeStack, and expanded TokenType.

#include "core/GrammarEngine.h"
#include "core/LanguagePack.h"
#include "core/SemanticTokenEngine.h"
#include "core/SyntaxHighlighter.h"

#include <catch2/catch_test_macros.hpp>
using namespace markamp::core;

// ============================================================================
// GrammarEngine: Construction and default state
// ============================================================================

TEST_CASE("GrammarEngine: default construction", "[grammar_engine][positive]")
{
    const GrammarEngine engine;
    CHECK(engine.grammar_count() == 0);
    CHECK(engine.grammars().empty());
}

TEST_CASE("GrammarEngine: load_grammar with empty path", "[grammar_engine][negative]")
{
    GrammarEngine engine;
    REQUIRE_FALSE(engine.load_grammar(""));
}

TEST_CASE("GrammarEngine: load_grammar returns false for nonexistent file",
          "[grammar_engine][negative]")
{
    GrammarEngine engine;
    REQUIRE_FALSE(engine.load_grammar("/nonexistent/path.tmLanguage.json"));
}

TEST_CASE("GrammarEngine: load_grammar_from_string with empty input", "[grammar_engine][negative]")
{
    GrammarEngine engine;
    REQUIRE_FALSE(engine.load_grammar_from_string("", "source.test"));
    REQUIRE_FALSE(engine.load_grammar_from_string("{}", ""));
}

// ============================================================================
// GrammarEngine: Grammar loading from string
// ============================================================================

TEST_CASE("GrammarEngine: load valid grammar from string", "[grammar_engine][positive]")
{
    GrammarEngine engine;
    const std::string grammar_json = R"({
        "scopeName": "source.test",
        "name": "TestLang",
        "fileTypes": ["tst", "test"],
        "patterns": [
            {
                "match": "\\b(if|else|while|for|return)\\b",
                "name": "keyword.control.test"
            },
            {
                "match": "\\b[0-9]+\\b",
                "name": "constant.numeric.test"
            }
        ]
    })";

    REQUIRE(engine.load_grammar_from_string(grammar_json, "source.test"));
    CHECK(engine.grammar_count() == 1);

    const auto* grammar = engine.get_grammar("source.test");
    REQUIRE(grammar != nullptr);
    CHECK(grammar->scope_name == "source.test");
    CHECK(grammar->name == "TestLang");
}

TEST_CASE("GrammarEngine: grammar definition introspection", "[grammar_engine][positive]")
{
    GrammarEngine engine;
    const std::string grammar_json = R"({
        "scopeName": "source.demo",
        "name": "DemoLang",
        "fileTypes": ["dm"],
        "patterns": [
            {
                "match": "//.*$",
                "name": "comment.line.double-slash.demo"
            }
        ],
        "repository": {
            "strings": {
                "match": "\"[^\"]*\"",
                "name": "string.quoted.double.demo"
            }
        }
    })";

    REQUIRE(engine.load_grammar_from_string(grammar_json, "source.demo"));

    const auto* definition = engine.get_definition("source.demo");
    REQUIRE(definition != nullptr);
    CHECK(definition->patterns.size() == 1);
    CHECK(definition->repository.size() == 1);
    CHECK(definition->repository.count("strings") == 1);
}

// ============================================================================
// GrammarEngine: Tokenization
// ============================================================================

TEST_CASE("GrammarEngine: tokenize match rules", "[grammar_engine][positive]")
{
    GrammarEngine engine;
    const std::string grammar_json = R"({
        "scopeName": "source.tok",
        "name": "TokLang",
        "patterns": [
            {
                "match": "\\b(if|else|return)\\b",
                "name": "keyword.control.tok"
            },
            {
                "match": "\\b[0-9]+\\b",
                "name": "constant.numeric.tok"
            }
        ]
    })";

    REQUIRE(engine.load_grammar_from_string(grammar_json, "source.tok"));

    auto tokens = engine.tokenize_line("source.tok", "if x return 42");
    REQUIRE(tokens.size() >= 3);

    // Find keyword tokens
    bool found_if = false;
    bool found_return = false;
    bool found_number = false;
    for (const auto& token : tokens)
    {
        if (token.scope == "keyword.control.tok")
        {
            const std::string matched = "if x return 42";
            const std::string sub =
                matched.substr(static_cast<size_t>(token.start_index),
                               static_cast<size_t>(token.end_index - token.start_index));
            if (sub == "if")
            {
                found_if = true;
            }
            if (sub == "return")
            {
                found_return = true;
            }
        }
        if (token.scope == "constant.numeric.tok")
        {
            found_number = true;
        }
    }
    CHECK(found_if);
    CHECK(found_return);
    CHECK(found_number);
}

TEST_CASE("GrammarEngine: tokenize begin/end rules", "[grammar_engine][positive]")
{
    GrammarEngine engine;
    const std::string grammar_json = R"({
        "scopeName": "source.be",
        "name": "BeginEndLang",
        "patterns": [
            {
                "begin": "\"",
                "end": "\"",
                "name": "string.quoted.double.be"
            }
        ]
    })";

    REQUIRE(engine.load_grammar_from_string(grammar_json, "source.be"));

    auto tokens = engine.tokenize_line("source.be", "x = \"hello world\"");
    REQUIRE_FALSE(tokens.empty());

    bool found_string = false;
    for (const auto& token : tokens)
    {
        if (token.scope == "string.quoted.double.be")
        {
            found_string = true;
        }
    }
    CHECK(found_string);
}

TEST_CASE("GrammarEngine: tokenize_line for nonexistent grammar returns empty",
          "[grammar_engine][negative]")
{
    const GrammarEngine engine;
    auto tokens = engine.tokenize_line("source.nonexistent", "hello world");
    CHECK(tokens.empty());
}

// ============================================================================
// GrammarEngine: Scope-to-TokenType mapping
// ============================================================================

TEST_CASE("GrammarEngine: scope_to_token_type maps core scopes", "[grammar_engine][positive]")
{
    CHECK(GrammarEngine::scope_to_token_type("keyword.control.cpp") == TokenType::Keyword);
    CHECK(GrammarEngine::scope_to_token_type("comment.line.double-slash") == TokenType::Comment);
    CHECK(GrammarEngine::scope_to_token_type("comment.block.documentation") ==
          TokenType::DocComment);
    CHECK(GrammarEngine::scope_to_token_type("string.quoted.double") == TokenType::String);
    CHECK(GrammarEngine::scope_to_token_type("constant.numeric.integer") == TokenType::Number);
    CHECK(GrammarEngine::scope_to_token_type("constant.language.boolean") == TokenType::Constant);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.function.cpp") == TokenType::Function);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.type.class") == TokenType::Type);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.type.interface") == TokenType::Interface);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.type.enum") == TokenType::Enum);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.namespace") == TokenType::Namespace);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.tag.html") == TokenType::Tag);
    CHECK(GrammarEngine::scope_to_token_type("entity.other.attribute-name") ==
          TokenType::Attribute);
    CHECK(GrammarEngine::scope_to_token_type("variable.parameter") == TokenType::Parameter);
    CHECK(GrammarEngine::scope_to_token_type("variable.other.property") == TokenType::Property);
    CHECK(GrammarEngine::scope_to_token_type("variable.other") == TokenType::Variable);
    CHECK(GrammarEngine::scope_to_token_type("keyword.operator.assignment") == TokenType::Operator);
    CHECK(GrammarEngine::scope_to_token_type("punctuation.definition") == TokenType::Punctuation);
    CHECK(GrammarEngine::scope_to_token_type("meta.preprocessor") == TokenType::Preprocessor);
    CHECK(GrammarEngine::scope_to_token_type("meta.decorator") == TokenType::Decorator);
    CHECK(GrammarEngine::scope_to_token_type("meta.embedded") == TokenType::Embedded);
    CHECK(GrammarEngine::scope_to_token_type("string.regexp") == TokenType::Regex);
    CHECK(GrammarEngine::scope_to_token_type("constant.character.escape") == TokenType::Escape);
}

TEST_CASE("GrammarEngine: scope_to_token_type defaults to Text", "[grammar_engine][positive]")
{
    CHECK(GrammarEngine::scope_to_token_type("source.unknown") == TokenType::Text);
    CHECK(GrammarEngine::scope_to_token_type("") == TokenType::Text);
}

// ============================================================================
// GrammarEngine: Extension resolution
// ============================================================================

TEST_CASE("GrammarEngine: scope_for_extension with loaded grammar", "[grammar_engine][positive]")
{
    GrammarEngine engine;
    const std::string grammar_json = R"({
        "scopeName": "source.ext",
        "name": "ExtLang",
        "fileTypes": ["ext", "ex"]
    })";
    REQUIRE(engine.load_grammar_from_string(grammar_json, "source.ext"));

    auto scope = engine.scope_for_extension("ext");
    REQUIRE(scope.has_value());
    CHECK(scope.value() == "source.ext");

    auto missing = engine.scope_for_extension("xyz");
    CHECK_FALSE(missing.has_value());
}

// ============================================================================
// ScopeStack tests
// ============================================================================

TEST_CASE("ScopeStack: default state", "[scope_stack][positive]")
{
    const ScopeStack stack;
    CHECK(stack.empty());
    CHECK(stack.depth() == 0);
    CHECK(stack.top().empty());
}

TEST_CASE("ScopeStack: push and pop", "[scope_stack][positive]")
{
    ScopeStack stack;
    stack.push("source.cpp");
    CHECK_FALSE(stack.empty());
    CHECK(stack.depth() == 1);
    CHECK(stack.top() == "source.cpp");

    stack.push("meta.function");
    CHECK(stack.depth() == 2);
    CHECK(stack.top() == "meta.function");

    stack.pop();
    CHECK(stack.depth() == 1);
    CHECK(stack.top() == "source.cpp");
}

TEST_CASE("ScopeStack: to_string concatenates scopes", "[scope_stack][positive]")
{
    ScopeStack stack;
    stack.push("source.cpp");
    stack.push("meta.function");
    stack.push("string.quoted.double");

    CHECK(stack.to_string() == "source.cpp meta.function string.quoted.double");
}

TEST_CASE("ScopeStack: contains checks all levels", "[scope_stack][positive]")
{
    ScopeStack stack;
    stack.push("source.cpp");
    stack.push("meta.function");

    CHECK(stack.contains("source.cpp"));
    CHECK(stack.contains("meta.function"));
    CHECK_FALSE(stack.contains("string.quoted"));
}

TEST_CASE("ScopeStack: pop on empty is safe", "[scope_stack][edge]")
{
    ScopeStack stack;
    REQUIRE_NOTHROW(stack.pop());
    CHECK(stack.empty());
}

// ============================================================================
// LanguagePack tests
// ============================================================================

TEST_CASE("LanguagePack: default construction has builtin languages", "[language_pack][positive]")
{
    const LanguagePack pack;
    CHECK(pack.language_count() >= 30);
}

TEST_CASE("LanguagePack: get_language by ID", "[language_pack][positive]")
{
    const LanguagePack pack;

    const auto* cpp = pack.get_language("cpp");
    REQUIRE(cpp != nullptr);
    CHECK(cpp->display_name == "C++");
    CHECK(cpp->scope_name == "source.cpp");
    CHECK(cpp->category == LanguageCategory::kCore);

    const auto* python = pack.get_language("python");
    REQUIRE(python != nullptr);
    CHECK(python->display_name == "Python");
}

TEST_CASE("LanguagePack: get_language by alias", "[language_pack][positive]")
{
    const LanguagePack pack;

    const auto* cpp = pack.get_language("c++");
    REQUIRE(cpp != nullptr);
    CHECK(cpp->language_id == "cpp");

    const auto* js = pack.get_language("js");
    REQUIRE(js != nullptr);
    CHECK(js->language_id == "javascript");
}

TEST_CASE("LanguagePack: language_for_extension", "[language_pack][positive]")
{
    const LanguagePack pack;

    const auto* cpp = pack.language_for_extension("cpp");
    REQUIRE(cpp != nullptr);
    CHECK(cpp->language_id == "cpp");

    const auto* ts = pack.language_for_extension("ts");
    REQUIRE(ts != nullptr);
    CHECK(ts->language_id == "typescript");

    const auto* unknown = pack.language_for_extension("xyz");
    CHECK(unknown == nullptr);
}

TEST_CASE("LanguagePack: language_for_filename", "[language_pack][positive]")
{
    const LanguagePack pack;

    const auto* docker = pack.language_for_filename("Dockerfile");
    REQUIRE(docker != nullptr);
    CHECK(docker->language_id == "dockerfile");

    const auto* makefile = pack.language_for_filename("Makefile");
    REQUIRE(makefile != nullptr);
    CHECK(makefile->language_id == "makefile");
}

TEST_CASE("LanguagePack: languages_in_category", "[language_pack][positive]")
{
    const LanguagePack pack;

    auto core_langs = pack.languages_in_category(LanguageCategory::kCore);
    CHECK(core_langs.size() >= 5); // cpp, csharp, java, javascript, typescript, jsx, tsx

    auto shell_langs = pack.languages_in_category(LanguageCategory::kShell);
    CHECK(shell_langs.size() >= 2); // bash, powershell
}

// ============================================================================
// SemanticTokenEngine tests
// ============================================================================

TEST_CASE("SemanticTokenEngine: default state", "[semantic_token][positive]")
{
    const SemanticTokenEngine engine;
    CHECK(engine.token_count() == 0);
    CHECK(engine.tokens().empty());
}

TEST_CASE("SemanticTokenEngine: add and retrieve tokens", "[semantic_token][positive]")
{
    SemanticTokenEngine engine;
    engine.add_token({0, 3, SemanticTokenType::kKeyword, 0, "keyword.control"});
    engine.add_token({4, 4, SemanticTokenType::kType, 0, "entity.name.type"});

    CHECK(engine.token_count() == 2);
    CHECK(engine.tokens()[0].type == SemanticTokenType::kKeyword);
    CHECK(engine.tokens()[1].type == SemanticTokenType::kType);
}

TEST_CASE("SemanticTokenEngine: clear removes all tokens", "[semantic_token][positive]")
{
    SemanticTokenEngine engine;
    engine.add_token({0, 3, SemanticTokenType::kKeyword, 0, ""});
    engine.clear();
    CHECK(engine.token_count() == 0);
}

TEST_CASE("SemanticTokenEngine: to_token_type conversion", "[semantic_token][positive]")
{
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kNamespace) ==
          TokenType::Namespace);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kFunction) == TokenType::Function);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kEnum) == TokenType::Enum);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kInterface) ==
          TokenType::Interface);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kParameter) ==
          TokenType::Parameter);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kDecorator) ==
          TokenType::Decorator);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kRegexp) == TokenType::Regex);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kMacro) == TokenType::Macro);
}

TEST_CASE("SemanticTokenEngine: to_scope mapping", "[semantic_token][positive]")
{
    CHECK(SemanticTokenEngine::to_scope(SemanticTokenType::kFunction) == "entity.name.function");
    CHECK(SemanticTokenEngine::to_scope(SemanticTokenType::kKeyword) == "keyword.control");
    CHECK(SemanticTokenEngine::to_scope(SemanticTokenType::kComment) == "comment");
}

TEST_CASE("SemanticTokenEngine: priority ordering", "[semantic_token][positive]")
{
    // Types > Functions > Variables > Keywords > Literals
    CHECK(SemanticTokenEngine::priority(SemanticTokenType::kType) >
          SemanticTokenEngine::priority(SemanticTokenType::kFunction));
    CHECK(SemanticTokenEngine::priority(SemanticTokenType::kFunction) >
          SemanticTokenEngine::priority(SemanticTokenType::kVariable));
    CHECK(SemanticTokenEngine::priority(SemanticTokenType::kVariable) >
          SemanticTokenEngine::priority(SemanticTokenType::kKeyword));
    CHECK(SemanticTokenEngine::priority(SemanticTokenType::kKeyword) >
          SemanticTokenEngine::priority(SemanticTokenType::kString));
}

TEST_CASE("SemanticTokenEngine: merge with empty semantic returns syntax unchanged",
          "[semantic_token][positive]")
{
    const SemanticTokenEngine engine;
    std::vector<Token> syntax = {{TokenType::Keyword, "if", 0, 2},
                                 {TokenType::Text, " ", 2, 1},
                                 {TokenType::Number, "42", 3, 2}};

    auto result = engine.merge_with_syntax(syntax);
    REQUIRE(result.size() == 3);
    CHECK(result[0].type == TokenType::Keyword);
    CHECK(result[1].type == TokenType::Text);
    CHECK(result[2].type == TokenType::Number);
}

// ============================================================================
// Expanded TokenType tests
// ============================================================================

TEST_CASE("TokenType: new types exist in token_class", "[syntax_highlighter][positive]")
{
    CHECK(SyntaxHighlighter::token_class(TokenType::Namespace) == "namespace");
    CHECK(SyntaxHighlighter::token_class(TokenType::Enum) == "enum");
    CHECK(SyntaxHighlighter::token_class(TokenType::EnumMember) == "enum-member");
    CHECK(SyntaxHighlighter::token_class(TokenType::Interface) == "interface");
    CHECK(SyntaxHighlighter::token_class(TokenType::Struct) == "struct");
    CHECK(SyntaxHighlighter::token_class(TokenType::Parameter) == "parameter");
    CHECK(SyntaxHighlighter::token_class(TokenType::Label) == "label");
    CHECK(SyntaxHighlighter::token_class(TokenType::Decorator) == "decorator");
    CHECK(SyntaxHighlighter::token_class(TokenType::Regex) == "regexp");
    CHECK(SyntaxHighlighter::token_class(TokenType::Escape) == "escape");
    CHECK(SyntaxHighlighter::token_class(TokenType::Embedded) == "embedded");
    CHECK(SyntaxHighlighter::token_class(TokenType::MetaTag) == "meta-tag");
    CHECK(SyntaxHighlighter::token_class(TokenType::DocComment) == "doc-comment");
    CHECK(SyntaxHighlighter::token_class(TokenType::Macro) == "macro");
    CHECK(SyntaxHighlighter::token_class(TokenType::TypeParameter) == "type-parameter");
}

TEST_CASE("TokenType: legacy types still work", "[syntax_highlighter][positive]")
{
    CHECK(SyntaxHighlighter::token_class(TokenType::Keyword) == "keyword");
    CHECK(SyntaxHighlighter::token_class(TokenType::String) == "string");
    CHECK(SyntaxHighlighter::token_class(TokenType::Comment) == "comment");
    CHECK(SyntaxHighlighter::token_class(TokenType::Function) == "function");
    CHECK(SyntaxHighlighter::token_class(TokenType::Type) == "type");
    CHECK(SyntaxHighlighter::token_class(TokenType::Text) == "text");
}

// ============================================================================
// GrammarEngine: type size validation
// ============================================================================

TEST_CASE("GrammarEngine: type size is valid", "[grammar_engine][edge]")
{
    static_assert(sizeof(GrammarEngine) > 0);
    static_assert(sizeof(GrammarToken) > 0);
    static_assert(sizeof(Grammar) > 0);
    static_assert(sizeof(GrammarRule) > 0);
    static_assert(sizeof(GrammarDefinition) > 0);
}
