#include "StickyAggregator.h"

#include "canvas/StickyNote.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <sstream>

namespace markamp::canvas
{

auto StickyAggregator::find_duplicates(
    const std::vector<std::pair<ObjectId, const StickyNote*>>& notes, double threshold) const
    -> std::vector<AggregationSuggestion>
{
    std::vector<AggregationSuggestion> suggestions;

    for (size_t i = 0; i < notes.size(); ++i)
    {
        for (size_t j = i + 1; j < notes.size(); ++j)
        {
            const auto& text_a = notes[i].second->text();
            const auto& text_b = notes[j].second->text();
            const double sim = text_similarity(text_a, text_b);

            if (sim >= threshold)
            {
                AggregationSuggestion suggestion;
                suggestion.note_a = notes[i].first;
                suggestion.note_b = notes[j].first;
                suggestion.similarity = sim;
                suggestion.reason = (sim >= 1.0) ? "Identical text" : "Similar content";
                suggestions.push_back(std::move(suggestion));
            }
        }
    }

    // Sort by descending similarity.
    std::sort(suggestions.begin(),
              suggestions.end(),
              [](const AggregationSuggestion& lhs, const AggregationSuggestion& rhs)
              { return lhs.similarity > rhs.similarity; });

    return suggestions;
}

auto StickyAggregator::text_similarity(const std::string& text_a, const std::string& text_b) const
    -> double
{
    if (text_a.empty() && text_b.empty())
    {
        return 1.0;
    }
    if (text_a.empty() || text_b.empty())
    {
        return 0.0;
    }

    // Build word sets.
    auto to_words = [](const std::string& text) -> std::set<std::string>
    {
        std::set<std::string> words;
        std::istringstream stream(text);
        std::string word;
        while (stream >> word)
        {
            // Lowercase for case-insensitive comparison.
            std::transform(word.begin(),
                           word.end(),
                           word.begin(),
                           [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
            words.insert(word);
        }
        return words;
    };

    const auto words_a = to_words(text_a);
    const auto words_b = to_words(text_b);

    // Jaccard index = |intersection| / |union|
    std::set<std::string> intersection;
    std::set_intersection(words_a.begin(),
                          words_a.end(),
                          words_b.begin(),
                          words_b.end(),
                          std::inserter(intersection, intersection.begin()));

    std::set<std::string> union_set;
    std::set_union(words_a.begin(),
                   words_a.end(),
                   words_b.begin(),
                   words_b.end(),
                   std::inserter(union_set, union_set.begin()));

    if (union_set.empty())
    {
        return 1.0;
    }
    return static_cast<double>(intersection.size()) / static_cast<double>(union_set.size());
}

} // namespace markamp::canvas
