/// @file SemanticTokenEngine.h
/// @brief V16 Phase 14 — Semantic token types, modifiers, and scope precedence.
///
/// Provides a unified layer that overlays semantic information on top of
/// syntax tokens produced by the GrammarEngine. Semantic tokens take
/// precedence over syntactic tokens based on a priority system.
#pragma once

#include "SyntaxHighlighter.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Semantic token type — aligned with LSP SemanticTokenTypes.
enum class SemanticTokenType
{
    kNamespace,
    kType,
    kClass,
    kEnum,
    kInterface,
    kStruct,
    kTypeParameter,
    kParameter,
    kVariable,
    kProperty,
    kEnumMember,
    kFunction,
    kMethod,
    kMacro,
    kKeyword,
    kModifier,
    kComment,
    kString,
    kNumber,
    kRegexp,
    kOperator,
    kDecorator,
    kLabel,
    // Extension types not in standard LSP
    kEvent,
    kLifetime, // Rust lifetimes
};

/// Semantic token modifier — can be combined as bit flags.
enum class SemanticTokenModifier : uint32_t
{
    kNone = 0,
    kDeclaration = 1 << 0,
    kDefinition = 1 << 1,
    kReadonly = 1 << 2,
    kStatic = 1 << 3,
    kDeprecated = 1 << 4,
    kAbstract = 1 << 5,
    kAsync = 1 << 6,
    kModification = 1 << 7,
    kDocumentation = 1 << 8,
    kDefaultLibrary = 1 << 9,
};

/// A single semantic token with position, type, and modifiers.
struct SemanticTokenEntry
{
    size_t start{0};
    size_t length{0};
    SemanticTokenType type{SemanticTokenType::kVariable};
    uint32_t modifiers{0};
    std::string scope; ///< Optional TextMate scope for theme resolution
};

/// Engine for managing semantic token overlays.
///
/// Usage:
/// ```cpp
/// SemanticTokenEngine engine;
/// engine.add_token({0, 3, SemanticTokenType::kKeyword, 0, "keyword.control"});
/// engine.add_token({4, 4, SemanticTokenType::kType, 0, "entity.name.type"});
/// auto merged = engine.merge_with_syntax(syntax_tokens);
/// ```
class SemanticTokenEngine
{
public:
    SemanticTokenEngine() = default;

    /// Add a semantic token to the engine.
    void add_token(SemanticTokenEntry entry);

    /// Clear all semantic tokens.
    void clear();

    /// Get all registered semantic tokens.
    [[nodiscard]] auto tokens() const -> const std::vector<SemanticTokenEntry>&;

    /// Number of semantic tokens.
    [[nodiscard]] auto token_count() const -> size_t;

    /// Merge semantic tokens with syntax tokens.
    /// Semantic tokens take precedence where they overlap.
    [[nodiscard]] auto merge_with_syntax(const std::vector<Token>& syntax_tokens) const
        -> std::vector<Token>;

    /// Convert a SemanticTokenType to a TokenType.
    [[nodiscard]] static auto to_token_type(SemanticTokenType semantic_type) -> TokenType;

    /// Convert a SemanticTokenType to a TextMate scope string.
    [[nodiscard]] static auto to_scope(SemanticTokenType semantic_type) -> std::string;

    /// Priority for precedence resolution (higher = wins).
    [[nodiscard]] static auto priority(SemanticTokenType semantic_type) -> int;

private:
    std::vector<SemanticTokenEntry> tokens_;
};

} // namespace markamp::core
