#pragma once

/// @file SearchQueryParser.h
/// @brief V4 Phase 15 – Advanced search query syntax parser.
///
/// Parses user search input supporting:
///   - Plain terms: `hello world` (AND semantics)
///   - Quoted phrases: `"exact match"`
///   - Scope prefixes: `tag:important`, `path:notes/`, `title:report`
///   - Negation: `-excluded`
///   - Boolean OR: `cats OR dogs`

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

struct SearchQuery;

/// Represents a single parsed token from a search query string.
struct ParsedQueryToken
{
    enum class Type : uint8_t
    {
        Term,        ///< Plain search term
        Phrase,      ///< Quoted phrase (exact match)
        ScopePrefix, ///< Scoped search (e.g., "tag:x")
        Operator,    ///< Boolean operator (OR)
        Negation     ///< Negated term (e.g., "-exclude")
    };

    Type type{Type::Term};
    std::string value; ///< The term/phrase/operator value
    std::string scope; ///< For ScopePrefix: the scope name (tag, path, title, etc.)

    /// Check if this is a negated term.
    [[nodiscard]] auto is_negated() const -> bool
    {
        return type == Type::Negation;
    }

    /// Check if this is a scope prefix.
    [[nodiscard]] auto is_scope() const -> bool
    {
        return type == Type::ScopePrefix;
    }
};

/// Parses search query strings into structured tokens, then builds SearchQuery objects.
class SearchQueryParser
{
public:
    SearchQueryParser() = default;

    /// Parse a raw search query string into tokens.
    [[nodiscard]] auto parse(const std::string& input) const -> std::vector<ParsedQueryToken>;

    /// Build a SearchQuery from parsed tokens.
    /// Assembles terms, scope filters, and search method from tokens.
    [[nodiscard]] auto build_query(const std::vector<ParsedQueryToken>& tokens) const
        -> SearchQuery;

    /// Convenience: parse + build in one step.
    [[nodiscard]] auto parse_and_build(const std::string& input) const -> SearchQuery;

private:
    [[nodiscard]] auto parse_quoted(const std::string& input, size_t& pos) const
        -> ParsedQueryToken;

    [[nodiscard]] auto parse_scope_or_term(const std::string& input, size_t& pos) const
        -> ParsedQueryToken;
};

} // namespace markamp::core
