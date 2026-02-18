// ============================================================================
// File: src/core/AITagSuggester.cpp
// Phase 26: AI Integration — tag suggestion implementation
// ============================================================================

#include "AITagSuggester.h"

#include "AIService.h"

#include <algorithm>
#include <ranges>
#include <sstream>

namespace markamp::core
{

AITagSuggester::AITagSuggester(AIService& ai_service)
    : ai_service_(ai_service)
{
}

auto AITagSuggester::suggest_tags(const std::string& document_content,
                                  const std::vector<std::string>& existing_tags)
    -> std::vector<TagSuggestion>
{
    // Build context with existing workspace tags.
    std::string context;
    if (!existing_tags.empty())
    {
        context = "Existing workspace tags: ";
        for (size_t idx = 0; idx < existing_tags.size(); ++idx)
        {
            if (idx > 0)
            {
                context += ", ";
            }
            context += existing_tags[idx];
        }
        context += "\nPrefer suggesting existing tags when appropriate.";
    }

    const auto response =
        ai_service_.execute_action(AIAction::ExtractKeywords, document_content, context);

    if (!response.success || response.content.empty())
    {
        return {};
    }

    auto suggestions = parse_suggestions(response.content, existing_tags);

    // Filter out rejected tags.
    std::erase_if(suggestions,
                  [this](const TagSuggestion& suggestion)
                  { return std::ranges::find(rejected_, suggestion.tag) != rejected_.end(); });

    // Boost confidence for previously accepted tags.
    for (auto& suggestion : suggestions)
    {
        if (std::ranges::find(accepted_, suggestion.tag) != accepted_.end())
        {
            suggestion.confidence = std::min(1.0, suggestion.confidence + 0.2);
        }
    }

    // Sort by confidence descending.
    std::ranges::sort(suggestions,
                      [](const TagSuggestion& lhs, const TagSuggestion& rhs)
                      { return lhs.confidence > rhs.confidence; });

    return suggestions;
}

auto AITagSuggester::accept_suggestion(const std::string& tag) -> void
{
    if (std::ranges::find(accepted_, tag) == accepted_.end())
    {
        accepted_.push_back(tag);
    }
    // Remove from rejected if previously rejected.
    std::erase(rejected_, tag);
}

auto AITagSuggester::reject_suggestion(const std::string& tag) -> void
{
    if (std::ranges::find(rejected_, tag) == rejected_.end())
    {
        rejected_.push_back(tag);
    }
    // Remove from accepted if previously accepted.
    std::erase(accepted_, tag);
}

auto AITagSuggester::accepted_history() const -> std::vector<std::string>
{
    return accepted_;
}

auto AITagSuggester::rejected_history() const -> std::vector<std::string>
{
    return rejected_;
}

auto AITagSuggester::parse_suggestions(const std::string& response,
                                       const std::vector<std::string>& existing_tags)
    -> std::vector<TagSuggestion>
{
    std::vector<TagSuggestion> suggestions;
    std::istringstream stream(response);
    std::string line;

    while (std::getline(stream, line))
    {
        // Trim whitespace.
        while (!line.empty() && std::isspace(line.front()) != 0)
        {
            line.erase(line.begin());
        }
        while (!line.empty() && std::isspace(line.back()) != 0)
        {
            line.pop_back();
        }

        // Skip empty lines and bullet markers.
        if (line.empty())
        {
            continue;
        }
        if (line.front() == '-' || line.front() == '*')
        {
            line = line.substr(1);
            while (!line.empty() && line.front() == ' ')
            {
                line.erase(line.begin());
            }
        }

        if (line.empty())
        {
            continue;
        }

        TagSuggestion suggestion;
        suggestion.tag = line;
        suggestion.confidence = 0.7; // Default confidence

        // Check if this is an existing tag.
        suggestion.is_existing = std::ranges::find(existing_tags, line) != existing_tags.end();
        if (suggestion.is_existing)
        {
            suggestion.confidence = 0.9;
            suggestion.reason = "Matches existing workspace tag";
        }
        else
        {
            suggestion.reason = "Suggested based on content analysis";
        }

        suggestions.push_back(std::move(suggestion));
    }

    return suggestions;
}

} // namespace markamp::core
