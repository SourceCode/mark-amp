#include "Search.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════
// FtsQueryBuilder
// ════════════════════════════════════════════════════════════

auto FtsQueryBuilder::build_keyword_query(const std::string& input) const -> std::string
{
    // Split input into terms and join with AND.
    std::istringstream stream(input);
    std::string term;
    std::string result;

    while (stream >> term)
    {
        if (!result.empty())
        {
            result += " AND ";
        }
        result += escape_fts(term);
    }

    return result;
}

auto FtsQueryBuilder::build_phrase_query(const std::string& input) const -> std::string
{
    // Wrap the entire input in double quotes for exact phrase match.
    return "\"" + input + "\"";
}

auto FtsQueryBuilder::validate_regex(const std::string& pattern) const -> bool
{
    try
    {
        std::regex test_regex(pattern);
        return true;
    }
    catch (const std::regex_error&)
    {
        return false;
    }
}

auto FtsQueryBuilder::escape_fts(const std::string& input) const -> std::string
{
    std::string result;
    result.reserve(input.size());
    for (char ch : input)
    {
        // Escape FTS5 special characters: * " ( ) + ^
        if (ch == '*' || ch == '"' || ch == '(' || ch == ')' || ch == '+' || ch == '^')
        {
            result += '\\';
        }
        result += ch;
    }
    return result;
}

// ════════════════════════════════════════════════════════════
// SnippetExtractor
// ════════════════════════════════════════════════════════════

auto SnippetExtractor::extract(std::string_view content,
                               const std::vector<HighlightRange>& matches,
                               int context_chars) const -> std::string
{
    if (matches.empty() || content.empty())
    {
        // Return the first context_chars*2 characters if no matches.
        auto len = std::min(static_cast<std::size_t>(context_chars * 2), content.size());
        return std::string(content.substr(0, len));
    }

    // Center around the first match.
    const auto& first_match = matches[0];
    auto match_center = first_match.start + (first_match.end - first_match.start) / 2;

    auto start = (match_center > static_cast<std::size_t>(context_chars))
                     ? match_center - static_cast<std::size_t>(context_chars)
                     : std::size_t{0};
    auto end = std::min(match_center + static_cast<std::size_t>(context_chars), content.size());

    std::string snippet;
    if (start > 0)
    {
        snippet = "...";
    }
    snippet += std::string(content.substr(start, end - start));
    if (end < content.size())
    {
        snippet += "...";
    }
    return snippet;
}

auto SnippetExtractor::highlight_html(std::string_view content,
                                      const std::vector<HighlightRange>& matches) const
    -> std::string
{
    if (matches.empty())
    {
        return std::string(content);
    }

    // Sort matches by start position (they should already be sorted but ensure it).
    auto sorted_matches = matches;
    std::sort(sorted_matches.begin(),
              sorted_matches.end(),
              [](const HighlightRange& range_a, const HighlightRange& range_b)
              { return range_a.start < range_b.start; });

    std::string result;
    std::size_t pos = 0;

    for (const auto& match : sorted_matches)
    {
        if (match.start > pos)
        {
            result += std::string(content.substr(pos, match.start - pos));
        }
        result += "<mark>";
        result += std::string(content.substr(match.start, match.end - match.start));
        result += "</mark>";
        pos = match.end;
    }

    // Append remaining content
    if (pos < content.size())
    {
        result += std::string(content.substr(pos));
    }

    return result;
}

} // namespace markamp::core
