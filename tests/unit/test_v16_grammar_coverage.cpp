/// @file test_v16_grammar_coverage.cpp
/// @brief V16 — Extended grammar engine tests covering custom grammar
///        registration, nested scope tokenization, SemanticTokenEngine
///        delta encoding, overlay priority, and V16 TokenType mappings.

#include "core/GrammarEngine.h"
#include "core/LanguagePack.h"
#include "core/SemanticTokenEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// LanguagePack: custom grammar registration and overrides
// ============================================================================

TEST_CASE("LanguagePack custom grammar registration adds language", "[v16][language_pack]")
{
    LanguagePack pack;
    const auto initial_count = pack.language_count();

    LanguageInfo custom;
    custom.language_id = "custom_dsl";
    custom.display_name = "Custom DSL";
    custom.scope_name = "source.custom";
    custom.extensions = {"cdsl"};

    pack.register_language(custom);
    CHECK(pack.language_count() == initial_count + 1);

    const auto* retrieved = pack.get_language("custom_dsl");
    REQUIRE(retrieved != nullptr);
    CHECK(retrieved->display_name == "Custom DSL");
    CHECK(retrieved->scope_name == "source.custom");
}

TEST_CASE("LanguagePack retrieves by extension after registration", "[v16][language_pack]")
{
    LanguagePack pack;

    LanguageInfo custom;
    custom.language_id = "mytest_lang";
    custom.display_name = "My Test Lang";
    custom.scope_name = "source.mytest";
    custom.extensions = {"mtl", "mtest"};

    pack.register_language(custom);

    const auto* by_ext = pack.language_for_extension("mtl");
    REQUIRE(by_ext != nullptr);
    CHECK(by_ext->language_id == "mytest_lang");

    const auto* by_ext2 = pack.language_for_extension("mtest");
    REQUIRE(by_ext2 != nullptr);
    CHECK(by_ext2->language_id == "mytest_lang");
}

TEST_CASE("LanguagePack unknown extension returns nullptr", "[v16][language_pack]")
{
    const LanguagePack pack;
    CHECK(pack.language_for_extension("definitely_not_real") == nullptr);
}

// ============================================================================
// GrammarEngine: nested scope tokenization
// ============================================================================

TEST_CASE("GrammarEngine nested begin/end scopes produce tokens", "[v16][grammar_nested]")
{
    GrammarEngine engine;
    const std::string grammar_json = R"({
        "scopeName": "source.nested",
        "name": "NestedLang",
        "patterns": [
            {
                "begin": "\\{",
                "end": "\\}",
                "name": "meta.block.nested",
                "patterns": [
                    {
                        "match": "\\b(if|while)\\b",
                        "name": "keyword.control.nested"
                    },
                    {
                        "match": "\\b[0-9]+\\b",
                        "name": "constant.numeric.nested"
                    }
                ]
            }
        ]
    })";

    REQUIRE(engine.load_grammar_from_string(grammar_json, "source.nested"));
    auto tokens = engine.tokenize_line("source.nested", "{ if 42 }");
    CHECK(tokens.size() >= 2); // at least keyword and number tokens

    bool found_keyword = false;
    bool found_number = false;
    for (const auto& token : tokens)
    {
        if (token.scope.find("keyword.control") != std::string::npos)
        {
            found_keyword = true;
        }
        if (token.scope.find("constant.numeric") != std::string::npos)
        {
            found_number = true;
        }
    }
    CHECK(found_keyword);
    CHECK(found_number);
}

TEST_CASE("ScopeStack is correct after nested block operations", "[v16][grammar_nested]")
{
    ScopeStack stack;
    CHECK(stack.empty());

    stack.push("source.cpp");
    stack.push("meta.function.body");
    stack.push("meta.block.if");
    CHECK(stack.depth() == 3);
    CHECK(stack.top() == "meta.block.if");

    stack.pop();
    CHECK(stack.depth() == 2);
    CHECK(stack.top() == "meta.function.body");

    stack.pop();
    stack.pop();
    CHECK(stack.empty());
}

TEST_CASE("ScopeStack to_string builds full scope path", "[v16][grammar_nested]")
{
    ScopeStack stack;
    stack.push("source.cpp");
    stack.push("meta.function");
    stack.push("variable.parameter");

    const auto full = stack.to_string();
    CHECK(full.find("source.cpp") != std::string::npos);
    CHECK(full.find("meta.function") != std::string::npos);
    CHECK(full.find("variable.parameter") != std::string::npos);
}

// ============================================================================
// SemanticTokenEngine: token management
// ============================================================================

TEST_CASE("SemanticTokenEngine add multiple tokens", "[v16][semantic_delta]")
{
    SemanticTokenEngine engine;

    // Add tokens at various positions
    engine.add_token({0, 3, SemanticTokenType::kKeyword, 0, "keyword"});
    engine.add_token({4, 5, SemanticTokenType::kFunction, 0, "function"});
    engine.add_token({10, 3, SemanticTokenType::kVariable, 0, "variable"});

    CHECK(engine.token_count() == 3);

    const auto& tokens = engine.tokens();
    CHECK(tokens[0].start == 0);
    CHECK(tokens[0].length == 3);
    CHECK(tokens[1].start == 4);
    CHECK(tokens[1].length == 5);
    CHECK(tokens[2].start == 10);
    CHECK(tokens[2].length == 3);
}

TEST_CASE("SemanticTokenEngine clear removes all tokens", "[v16][semantic_delta]")
{
    SemanticTokenEngine engine;
    engine.add_token({0, 3, SemanticTokenType::kKeyword, 0, ""});
    engine.add_token({5, 4, SemanticTokenType::kString, 0, ""});

    CHECK(engine.token_count() == 2);
    engine.clear();
    CHECK(engine.token_count() == 0);
    CHECK(engine.tokens().empty());
}

TEST_CASE("SemanticTokenEngine handles boundary conditions", "[v16][semantic_delta]")
{
    SemanticTokenEngine engine;

    // Zero-length token
    engine.add_token({0, 0, SemanticTokenType::kComment, 0, ""});
    CHECK(engine.token_count() == 1);

    // Large offset
    engine.add_token({100000, 10, SemanticTokenType::kVariable, 0, ""});
    CHECK(engine.token_count() == 2);
}

// ============================================================================
// SemanticTokenEngine: to_token_type mapping
// ============================================================================

TEST_CASE("SemanticTokenEngine to_token_type maps core types", "[v16][semantic_overlay]")
{
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kKeyword) == TokenType::Keyword);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kType) == TokenType::Type);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kFunction) == TokenType::Function);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kVariable) == TokenType::Variable);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kString) == TokenType::String);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kNumber) == TokenType::Number);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kComment) == TokenType::Comment);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kOperator) == TokenType::Operator);
}

TEST_CASE("SemanticTokenEngine to_token_type maps V16 extended types", "[v16][semantic_overlay]")
{
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kNamespace) ==
          TokenType::Namespace);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kEnum) == TokenType::Enum);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kInterface) ==
          TokenType::Interface);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kStruct) == TokenType::Struct);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kParameter) ==
          TokenType::Parameter);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kDecorator) ==
          TokenType::Decorator);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kRegexp) == TokenType::Regex);
    CHECK(SemanticTokenEngine::to_token_type(SemanticTokenType::kMacro) == TokenType::Macro);
}

// ============================================================================
// GrammarEngine: V16 fine-grained scope_to_token_type mappings
// ============================================================================

TEST_CASE("scope_to_token_type maps V16 variable scopes", "[v16][scope_mapping]")
{
    CHECK(GrammarEngine::scope_to_token_type("variable.other") == TokenType::Variable);
    CHECK(GrammarEngine::scope_to_token_type("variable.parameter") == TokenType::Parameter);
    CHECK(GrammarEngine::scope_to_token_type("variable.other.property") == TokenType::Property);
}

TEST_CASE("scope_to_token_type maps V16 entity scopes", "[v16][scope_mapping]")
{
    CHECK(GrammarEngine::scope_to_token_type("entity.name.tag") == TokenType::Tag);
    CHECK(GrammarEngine::scope_to_token_type("entity.other.attribute-name") ==
          TokenType::Attribute);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.namespace") == TokenType::Namespace);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.type.enum") == TokenType::Enum);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.type.interface") == TokenType::Interface);
    // These map to broader parent scopes via prefix matching:
    // entity.name.type.struct → entity.name.type → Type
    // entity.name.function.macro → entity.name.function → Function
    CHECK(GrammarEngine::scope_to_token_type("entity.name.type.struct") == TokenType::Type);
    CHECK(GrammarEngine::scope_to_token_type("entity.name.function.macro") == TokenType::Function);
}
