#include "NodeSearchProvider.h"

#include <algorithm>
#include <cctype>
#include <map>

namespace markamp::node_editor
{

NodeSearchProvider::NodeSearchProvider(const NodeDefinitionRegistry& registry)
    : registry_(registry)
{
}

auto NodeSearchProvider::compute_score(const std::string& query,
                                       const std::string& display_name,
                                       const std::string& type_name,
                                       const std::string& category) -> float
{
    if (query.empty())
    {
        return 1.0F;
    }

    // Case-insensitive matching
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    auto to_lower = [](const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return result;
    };

    const std::string kLowerDisplay = to_lower(display_name);
    const std::string kLowerType = to_lower(type_name);
    const std::string kLowerCategory = to_lower(category);

    float score = 0.0F;

    // Exact prefix match on display name — highest priority
    if (kLowerDisplay.find(lower_query) == 0)
    {
        score += 100.0F;
    }
    // Substring match on display name
    else if (kLowerDisplay.find(lower_query) != std::string::npos)
    {
        score += 60.0F;
    }

    // Prefix match on type_name
    if (kLowerType.find(lower_query) == 0)
    {
        score += 40.0F;
    }
    else if (kLowerType.find(lower_query) != std::string::npos)
    {
        score += 20.0F;
    }

    // Category match (lower weight)
    if (kLowerCategory.find(lower_query) != std::string::npos)
    {
        score += 10.0F;
    }

    return score;
}

auto NodeSearchProvider::search(const std::string& query,
                                GraphMode /*mode_filter*/,
                                std::size_t max_results) const -> std::vector<NodeSearchResult>
{
    const auto kAllDefs = registry_.all();
    std::vector<NodeSearchResult> results;

    for (const auto* def : kAllDefs)
    {
        if (def == nullptr)
        {
            continue;
        }
        const float kScore = compute_score(query, def->display_name, def->type_name, def->category);
        if (kScore > 0.0F)
        {
            NodeSearchResult result;
            result.type_name = def->type_name;
            result.display_name = def->display_name;
            result.category = def->category;
            result.score = kScore;
            results.push_back(result);
        }
    }

    // Sort by score descending
    std::sort(results.begin(),
              results.end(),
              [](const NodeSearchResult& lhs, const NodeSearchResult& rhs)
              { return lhs.score > rhs.score; });

    // Limit results
    if (results.size() > max_results)
    {
        results.resize(max_results);
    }

    return results;
}

auto NodeSearchProvider::all_by_category() const
    -> std::vector<std::pair<std::string, std::vector<NodeSearchResult>>>
{
    const auto kAllDefs = registry_.all();
    std::map<std::string, std::vector<NodeSearchResult>> category_map;

    for (const auto* def : kAllDefs)
    {
        if (def == nullptr)
        {
            continue;
        }
        NodeSearchResult result;
        result.type_name = def->type_name;
        result.display_name = def->display_name;
        result.category = def->category;
        result.score = 1.0F;
        category_map[def->category].push_back(result);
    }

    return {category_map.begin(), category_map.end()};
}

auto NodeSearchProvider::total_count() const -> std::size_t
{
    return registry_.count();
}

} // namespace markamp::node_editor
