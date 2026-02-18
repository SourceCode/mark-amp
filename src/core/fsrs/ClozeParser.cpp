#include "ClozeParser.h"

#include <algorithm>
#include <regex>
#include <set>
#include <sstream>

namespace markamp::core::fsrs
{

// ── Public API ──────────────────────────────────────────────────────────────

auto ClozeParser::parse(const std::string& text) const -> ClozeCard
{
    ClozeCard result;
    result.original_text = text;

    // Pattern: {{cN::text}} or {{cN::text::hint}} or {{text}} (shorthand c1)
    static const std::regex kClozePattern(R"(\{\{(?:c(\d+)::)?(.*?)(?:::(.*?))?\}\})",
                                          std::regex::ECMAScript);

    auto it = std::sregex_iterator(text.begin(), text.end(), kClozePattern);
    const auto end = std::sregex_iterator();

    std::set<int32_t> seen_indices;

    for (; it != end; ++it)
    {
        const std::smatch& match = *it;
        ClozeField field;

        // If no explicit index, default to 1
        if (match[1].matched && !match[1].str().empty())
        {
            field.index = std::stoi(match[1].str());
        }
        else
        {
            field.index = 1;
        }

        field.text = match[2].str();

        if (match[3].matched)
        {
            field.hint = match[3].str();
        }

        seen_indices.insert(field.index);
        result.fields.push_back(std::move(field));
    }

    result.card_count = static_cast<int32_t>(seen_indices.size());
    return result;
}

auto ClozeParser::render_front(const ClozeCard& card, int32_t active_index) const -> std::string
{
    std::string rendered = card.original_text;

    // Process in reverse order to preserve positions
    static const std::regex kClozePattern(R"(\{\{(?:c(\d+)::)?(.*?)(?:::(.*?))?\}\})",
                                          std::regex::ECMAScript);

    // Collect all matches with positions
    struct MatchInfo
    {
        size_t pos;
        size_t length;
        int32_t index;
        std::string text;
        std::string hint;
    };

    std::vector<MatchInfo> matches;
    auto it = std::sregex_iterator(rendered.begin(), rendered.end(), kClozePattern);
    const auto end = std::sregex_iterator();

    for (; it != end; ++it)
    {
        const std::smatch& match = *it;
        MatchInfo info;
        info.pos = static_cast<size_t>(match.position());
        info.length = static_cast<size_t>(match.length());

        if (match[1].matched && !match[1].str().empty())
        {
            info.index = std::stoi(match[1].str());
        }
        else
        {
            info.index = 1;
        }

        info.text = match[2].str();
        if (match[3].matched)
        {
            info.hint = match[3].str();
        }

        matches.push_back(std::move(info));
    }

    // Process in reverse to preserve positions
    std::sort(matches.begin(),
              matches.end(),
              [](const MatchInfo& lhs, const MatchInfo& rhs) { return lhs.pos > rhs.pos; });

    for (const auto& match_info : matches)
    {
        std::string replacement;
        if (match_info.index == active_index)
        {
            // Active cloze: show blank
            if (!match_info.hint.empty())
            {
                replacement = "[..." + match_info.hint + "...]";
            }
            else
            {
                replacement = "[...]";
            }
        }
        else
        {
            // Inactive cloze: show plain text
            replacement = match_info.text;
        }
        rendered.replace(match_info.pos, match_info.length, replacement);
    }

    return rendered;
}

auto ClozeParser::render_back(const ClozeCard& card, int32_t active_index) const -> std::string
{
    std::string rendered = card.original_text;

    static const std::regex kClozePattern(R"(\{\{(?:c(\d+)::)?(.*?)(?:::(.*?))?\}\})",
                                          std::regex::ECMAScript);

    struct MatchInfo
    {
        size_t pos;
        size_t length;
        int32_t index;
        std::string text;
    };

    std::vector<MatchInfo> matches;
    auto it = std::sregex_iterator(rendered.begin(), rendered.end(), kClozePattern);
    const auto end = std::sregex_iterator();

    for (; it != end; ++it)
    {
        const std::smatch& match = *it;
        MatchInfo info;
        info.pos = static_cast<size_t>(match.position());
        info.length = static_cast<size_t>(match.length());

        if (match[1].matched && !match[1].str().empty())
        {
            info.index = std::stoi(match[1].str());
        }
        else
        {
            info.index = 1;
        }

        info.text = match[2].str();
        matches.push_back(std::move(info));
    }

    std::sort(matches.begin(),
              matches.end(),
              [](const MatchInfo& lhs, const MatchInfo& rhs) { return lhs.pos > rhs.pos; });

    for (const auto& match_info : matches)
    {
        std::string replacement;
        if (match_info.index == active_index)
        {
            // Active cloze: highlight the answer
            replacement = "**" + match_info.text + "**";
        }
        else
        {
            // Inactive cloze: show plain text
            replacement = match_info.text;
        }
        rendered.replace(match_info.pos, match_info.length, replacement);
    }

    return rendered;
}

auto ClozeParser::generate_cards(const ClozeCard& card) const
    -> std::vector<std::pair<std::string, std::string>>
{
    const auto indices = unique_indices(card);

    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(static_cast<size_t>(card.card_count));

    for (const int32_t index : indices)
    {
        result.emplace_back(render_front(card, index), render_back(card, index));
    }

    return result;
}

auto ClozeParser::is_cloze(const std::string& text) -> bool
{
    // Quick check for {{ presence before running regex
    return text.find("{{") != std::string::npos && text.find("}}") != std::string::npos;
}

auto ClozeParser::unique_indices(const ClozeCard& card) -> std::vector<int32_t>
{
    std::set<int32_t> seen;
    for (const auto& field : card.fields)
    {
        seen.insert(field.index);
    }
    return {seen.begin(), seen.end()};
}

} // namespace markamp::core::fsrs
