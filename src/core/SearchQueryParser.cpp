/// @file SearchQueryParser.cpp
/// @brief V4 Phase 15 – Advanced search query syntax parser implementation.

#include "core/SearchQueryParser.h"

#include "core/SearchEngine.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// Public API
// ============================================================================

auto SearchQueryParser::parse(const std::string& input) const -> std::vector<ParsedQueryToken>
{
    std::vector<ParsedQueryToken> tokens;

    if (input.empty())
    {
        return tokens;
    }

    size_t pos = 0;
    const size_t len = input.size();

    while (pos < len)
    {
        // Skip whitespace
        while (pos < len && (input[pos] == ' ' || input[pos] == '\t'))
        {
            ++pos;
        }

        if (pos >= len)
        {
            break;
        }

        // Quoted phrase: "..."
        if (input[pos] == '"')
        {
            tokens.push_back(parse_quoted(input, pos));
            continue;
        }

        // Negation: -term
        if (input[pos] == '-' && pos + 1 < len && input[pos + 1] != ' ')
        {
            ++pos; // Skip the '-'
            auto token = parse_scope_or_term(input, pos);
            // Convert to negation
            ParsedQueryToken neg_token;
            neg_token.type = ParsedQueryToken::Type::Negation;
            neg_token.value = token.value;
            neg_token.scope = token.scope;
            tokens.push_back(std::move(neg_token));
            continue;
        }

        // Regular term or scope prefix
        auto token = parse_scope_or_term(input, pos);

        // Check if it's the OR operator
        if (token.value == "OR" && token.type == ParsedQueryToken::Type::Term)
        {
            token.type = ParsedQueryToken::Type::Operator;
        }

        tokens.push_back(std::move(token));
    }

    return tokens;
}

auto SearchQueryParser::build_query(const std::vector<ParsedQueryToken>& tokens) const
    -> SearchQuery
{
    SearchQuery query;

    std::vector<std::string> terms;
    bool has_phrase = false;
    bool has_or = false;

    for (const auto& token : tokens)
    {
        switch (token.type)
        {
            case ParsedQueryToken::Type::Term:
                terms.push_back(token.value);
                break;

            case ParsedQueryToken::Type::Phrase:
                terms.push_back("\"" + token.value + "\"");
                has_phrase = true;
                break;

            case ParsedQueryToken::Type::Operator:
                has_or = true;
                break;

            case ParsedQueryToken::Type::Negation:
                // Negated terms map to exclude folders for simplicity
                // or could be handled by the search engine
                terms.push_back("-" + token.value);
                break;

            case ParsedQueryToken::Type::ScopePrefix:
                // Map scope prefixes to SearchScope fields
                if (token.scope == "tag")
                {
                    query.scope.tags.push_back(token.value);
                }
                else if (token.scope == "path" || token.scope == "folder")
                {
                    query.scope.folders.push_back(token.value);
                }
                else if (token.scope == "title")
                {
                    // Title scope: add as a term with prefix for engine to handle
                    terms.push_back("title:" + token.value);
                }
                else if (token.scope == "frontmatter" || token.scope == "fm")
                {
                    // Parse key=value
                    auto eq_pos = token.value.find('=');
                    if (eq_pos != std::string::npos)
                    {
                        query.scope.frontmatter_field = token.value.substr(0, eq_pos);
                        query.scope.frontmatter_value = token.value.substr(eq_pos + 1);
                    }
                    else
                    {
                        query.scope.frontmatter_field = token.value;
                    }
                }
                break;
        }
    }

    // Build query string from terms
    if (!terms.empty())
    {
        std::string separator = has_or ? " OR " : " ";
        std::string query_str;
        for (size_t idx = 0; idx < terms.size(); ++idx)
        {
            if (idx > 0)
            {
                query_str += separator;
            }
            query_str += terms[idx];
        }
        query.query_string = query_str;
    }

    // Set search method based on content
    if (has_phrase && terms.size() == 1)
    {
        query.method = SearchMethod::Phrase;
    }

    return query;
}

auto SearchQueryParser::parse_and_build(const std::string& input) const -> SearchQuery
{
    return build_query(parse(input));
}

// ============================================================================
// Private: Parse quoted string
// ============================================================================

auto SearchQueryParser::parse_quoted(const std::string& input, size_t& pos) const
    -> ParsedQueryToken
{
    ParsedQueryToken token;
    token.type = ParsedQueryToken::Type::Phrase;

    ++pos; // Skip opening quote

    std::string value;
    while (pos < input.size())
    {
        if (input[pos] == '"')
        {
            ++pos; // Skip closing quote
            break;
        }
        // Handle escaped quotes
        if (input[pos] == '\\' && pos + 1 < input.size() && input[pos + 1] == '"')
        {
            value += '"';
            pos += 2;
            continue;
        }
        value += input[pos];
        ++pos;
    }

    token.value = value;
    return token;
}

// ============================================================================
// Private: Parse scope prefix or plain term
// ============================================================================

auto SearchQueryParser::parse_scope_or_term(const std::string& input, size_t& pos) const
    -> ParsedQueryToken
{
    ParsedQueryToken token;
    token.type = ParsedQueryToken::Type::Term;

    std::string word;
    while (pos < input.size() && input[pos] != ' ' && input[pos] != '\t')
    {
        word += input[pos];
        ++pos;
    }

    // Check for scope prefix: "scope:value"
    auto colon_pos = word.find(':');
    if (colon_pos != std::string::npos && colon_pos > 0 && colon_pos < word.size() - 1)
    {
        std::string scope = word.substr(0, colon_pos);
        std::string value = word.substr(colon_pos + 1);

        // Recognized scopes
        if (scope == "tag" || scope == "path" || scope == "folder" || scope == "title" ||
            scope == "frontmatter" || scope == "fm" || scope == "ext" || scope == "type")
        {
            token.type = ParsedQueryToken::Type::ScopePrefix;
            token.scope = scope;
            token.value = value;
            return token;
        }
    }

    token.value = word;
    return token;
}

} // namespace markamp::core
