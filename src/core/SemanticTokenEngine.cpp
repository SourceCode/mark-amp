/// @file SemanticTokenEngine.cpp
/// @brief V16 Phase 14 — Semantic token overlay engine implementation.

#include "SemanticTokenEngine.h"

#include <algorithm>

namespace markamp::core
{

void SemanticTokenEngine::add_token(SemanticTokenEntry entry)
{
    tokens_.push_back(std::move(entry));
}

void SemanticTokenEngine::clear()
{
    tokens_.clear();
}

auto SemanticTokenEngine::tokens() const -> const std::vector<SemanticTokenEntry>&
{
    return tokens_;
}

auto SemanticTokenEngine::token_count() const -> size_t
{
    return tokens_.size();
}

auto SemanticTokenEngine::merge_with_syntax(const std::vector<Token>& syntax_tokens) const
    -> std::vector<Token>
{
    if (tokens_.empty())
    {
        return syntax_tokens; // No semantic tokens to merge
    }

    std::vector<Token> result = syntax_tokens;

    // For each semantic token, override any syntax token that overlaps
    for (const auto& semantic : tokens_)
    {
        const TokenType merged_type = to_token_type(semantic.type);

        for (auto& syntax_tok : result)
        {
            // Check if semantic token overlaps this syntax token
            const size_t syn_end = syntax_tok.start + syntax_tok.length;
            const size_t sem_end = semantic.start + semantic.length;

            if (semantic.start < syn_end && sem_end > syntax_tok.start)
            {
                // Overlap detected — semantic wins if higher priority
                if (priority(semantic.type) > 0)
                {
                    syntax_tok.type = merged_type;
                }
            }
        }
    }

    return result;
}

auto SemanticTokenEngine::to_token_type(SemanticTokenType semantic_type) -> TokenType
{
    switch (semantic_type)
    {
        case SemanticTokenType::kNamespace:
            return TokenType::Namespace;
        case SemanticTokenType::kType:
            return TokenType::Type;
        case SemanticTokenType::kClass:
            return TokenType::Type;
        case SemanticTokenType::kEnum:
            return TokenType::Enum;
        case SemanticTokenType::kInterface:
            return TokenType::Interface;
        case SemanticTokenType::kStruct:
            return TokenType::Struct;
        case SemanticTokenType::kTypeParameter:
            return TokenType::TypeParameter;
        case SemanticTokenType::kParameter:
            return TokenType::Parameter;
        case SemanticTokenType::kVariable:
            return TokenType::Variable;
        case SemanticTokenType::kProperty:
            return TokenType::Property;
        case SemanticTokenType::kEnumMember:
            return TokenType::EnumMember;
        case SemanticTokenType::kFunction:
            return TokenType::Function;
        case SemanticTokenType::kMethod:
            return TokenType::Function;
        case SemanticTokenType::kMacro:
            return TokenType::Macro;
        case SemanticTokenType::kKeyword:
            return TokenType::Keyword;
        case SemanticTokenType::kModifier:
            return TokenType::Keyword;
        case SemanticTokenType::kComment:
            return TokenType::Comment;
        case SemanticTokenType::kString:
            return TokenType::String;
        case SemanticTokenType::kNumber:
            return TokenType::Number;
        case SemanticTokenType::kRegexp:
            return TokenType::Regex;
        case SemanticTokenType::kOperator:
            return TokenType::Operator;
        case SemanticTokenType::kDecorator:
            return TokenType::Decorator;
        case SemanticTokenType::kLabel:
            return TokenType::Label;
        case SemanticTokenType::kEvent:
            return TokenType::Function;
        case SemanticTokenType::kLifetime:
            return TokenType::TypeParameter;
    }
    return TokenType::Text;
}

auto SemanticTokenEngine::to_scope(SemanticTokenType semantic_type) -> std::string
{
    switch (semantic_type)
    {
        case SemanticTokenType::kNamespace:
            return "entity.name.namespace";
        case SemanticTokenType::kType:
            return "entity.name.type";
        case SemanticTokenType::kClass:
            return "entity.name.type.class";
        case SemanticTokenType::kEnum:
            return "entity.name.type.enum";
        case SemanticTokenType::kInterface:
            return "entity.name.type.interface";
        case SemanticTokenType::kStruct:
            return "entity.name.type.struct";
        case SemanticTokenType::kTypeParameter:
            return "entity.name.type.parameter";
        case SemanticTokenType::kParameter:
            return "variable.parameter";
        case SemanticTokenType::kVariable:
            return "variable.other";
        case SemanticTokenType::kProperty:
            return "variable.other.property";
        case SemanticTokenType::kEnumMember:
            return "variable.other.enummember";
        case SemanticTokenType::kFunction:
            return "entity.name.function";
        case SemanticTokenType::kMethod:
            return "entity.name.function.method";
        case SemanticTokenType::kMacro:
            return "entity.name.function.macro";
        case SemanticTokenType::kKeyword:
            return "keyword.control";
        case SemanticTokenType::kModifier:
            return "storage.modifier";
        case SemanticTokenType::kComment:
            return "comment";
        case SemanticTokenType::kString:
            return "string";
        case SemanticTokenType::kNumber:
            return "constant.numeric";
        case SemanticTokenType::kRegexp:
            return "string.regexp";
        case SemanticTokenType::kOperator:
            return "keyword.operator";
        case SemanticTokenType::kDecorator:
            return "meta.decorator";
        case SemanticTokenType::kLabel:
            return "entity.name.label";
        case SemanticTokenType::kEvent:
            return "variable.other.event";
        case SemanticTokenType::kLifetime:
            return "storage.modifier.lifetime";
    }
    return "source";
}

auto SemanticTokenEngine::priority(SemanticTokenType semantic_type) -> int
{
    // Semantic tokens generally take precedence over syntax tokens.
    // Higher priority = wins in overlap resolution.
    switch (semantic_type)
    {
        case SemanticTokenType::kNamespace:
        case SemanticTokenType::kType:
        case SemanticTokenType::kClass:
        case SemanticTokenType::kEnum:
        case SemanticTokenType::kInterface:
        case SemanticTokenType::kStruct:
        case SemanticTokenType::kTypeParameter:
            return 10; // Type information is highest priority

        case SemanticTokenType::kFunction:
        case SemanticTokenType::kMethod:
        case SemanticTokenType::kMacro:
            return 9;

        case SemanticTokenType::kParameter:
        case SemanticTokenType::kVariable:
        case SemanticTokenType::kProperty:
        case SemanticTokenType::kEnumMember:
            return 8;

        case SemanticTokenType::kDecorator:
        case SemanticTokenType::kLabel:
        case SemanticTokenType::kEvent:
        case SemanticTokenType::kLifetime:
            return 7;

        case SemanticTokenType::kKeyword:
        case SemanticTokenType::kModifier:
        case SemanticTokenType::kOperator:
            return 5; // Syntax-level tokens have lower priority

        case SemanticTokenType::kComment:
        case SemanticTokenType::kString:
        case SemanticTokenType::kNumber:
        case SemanticTokenType::kRegexp:
            return 3; // Literal tokens have lowest semantic priority
    }
    return 0;
}

} // namespace markamp::core
