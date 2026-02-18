/// @file HelpArticle.h
/// @brief V9 Phase 35 — Help content data model and article registry.
///
/// Defines `HelpArticle`, `HelpCategory`, and `HelpArticleRegistry`
/// which provide the data layer for the in-app help system.
/// The registry supports registration, lookup by ID/category/tag,
/// and full-text search across article titles and bodies.

#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// HelpCategory — categories for help content
// ============================================================================

/// Classification of help articles for filtering and display.
enum class HelpCategory
{
    Syntax,   ///< Markdown syntax reference articles
    Command,  ///< Command palette command descriptions
    Setting,  ///< Settings panel documentation
    Error,    ///< Error explanations and troubleshooting
    Tutorial, ///< Interactive tutorials and walkthroughs
    General,  ///< General product documentation
};

/// Convert HelpCategory to human-readable string.
[[nodiscard]] inline auto help_category_name(HelpCategory category) -> std::string
{
    switch (category)
    {
        case HelpCategory::Syntax:
            return "Syntax";
        case HelpCategory::Command:
            return "Commands";
        case HelpCategory::Setting:
            return "Settings";
        case HelpCategory::Error:
            return "Errors";
        case HelpCategory::Tutorial:
            return "Tutorials";
        case HelpCategory::General:
            return "General";
    }
    return "Unknown";
}

// ============================================================================
// HelpArticle — a single help article
// ============================================================================

/// A self-contained help article with Markdown body content.
struct HelpArticle
{
    std::string id;    ///< Unique identifier, e.g. "syntax.headings"
    std::string title; ///< Display title
    std::string body;  ///< Markdown content
    HelpCategory category{HelpCategory::General};
    std::vector<std::string> tags; ///< Searchable tags

    /// Check if this article matches a search query (case-insensitive).
    [[nodiscard]] auto matches(const std::string& query) const -> bool
    {
        if (query.empty())
        {
            return true;
        }
        auto lower_query = to_lower(query);
        if (to_lower(title).find(lower_query) != std::string::npos)
        {
            return true;
        }
        if (to_lower(body).find(lower_query) != std::string::npos)
        {
            return true;
        }
        for (const auto& tag : tags)
        {
            if (to_lower(tag).find(lower_query) != std::string::npos)
            {
                return true;
            }
        }
        return false;
    }

private:
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return result;
    }
};

// ============================================================================
// HelpSearchResult — ranked search result
// ============================================================================

/// A search result with relevance ranking.
struct HelpSearchResult
{
    std::string article_id;
    std::string title;
    HelpCategory category{HelpCategory::General};
    int relevance_score{0}; ///< Higher = more relevant (title > tag > body)
};

// ============================================================================
// HelpArticleRegistry — article storage and search
// ============================================================================

/// Registry of help articles with search and lookup capabilities.
///
/// Usage:
/// ```cpp
/// HelpArticleRegistry registry;
/// registry.register_article({.id = "syntax.headings", .title = "Headings",
///                            .body = "# Heading 1\n## Heading 2", .category =
///                            HelpCategory::Syntax});
/// auto results = registry.search("heading");
/// auto article = registry.get_article("syntax.headings");
/// ```
class HelpArticleRegistry
{
public:
    HelpArticleRegistry() = default;

    /// Register a help article. Overwrites existing article with the same ID.
    void register_article(HelpArticle article)
    {
        auto article_id = article.id;
        articles_[article_id] = std::move(article);
    }

    /// Retrieve an article by ID. Returns nullptr if not found.
    [[nodiscard]] auto get_article(const std::string& article_id) const -> const HelpArticle*
    {
        auto iter = articles_.find(article_id);
        if (iter != articles_.end())
        {
            return &iter->second;
        }
        return nullptr;
    }

    /// Get all articles in a category.
    [[nodiscard]] auto get_articles_by_category(HelpCategory category) const
        -> std::vector<const HelpArticle*>
    {
        std::vector<const HelpArticle*> results;
        for (const auto& [aid, article] : articles_)
        {
            if (article.category == category)
            {
                results.push_back(&article);
            }
        }
        return results;
    }

    /// Get all articles matching a tag.
    [[nodiscard]] auto get_articles_by_tag(const std::string& tag) const
        -> std::vector<const HelpArticle*>
    {
        std::vector<const HelpArticle*> results;
        for (const auto& [aid, article] : articles_)
        {
            for (const auto& article_tag : article.tags)
            {
                if (article_tag == tag)
                {
                    results.push_back(&article);
                    break;
                }
            }
        }
        return results;
    }

    /// Full-text search across articles. Returns results ranked by relevance.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<HelpSearchResult>
    {
        std::vector<HelpSearchResult> results;
        if (query.empty())
        {
            return results;
        }

        auto lower_query = to_lower(query);

        for (const auto& [aid, article] : articles_)
        {
            int score = 0;
            auto lower_title = to_lower(article.title);

            // Title exact match = highest score
            if (lower_title == lower_query)
            {
                score = 100;
            }
            else if (lower_title.find(lower_query) != std::string::npos)
            {
                score = 75;
            }
            else
            {
                // Tag match
                for (const auto& tag : article.tags)
                {
                    if (to_lower(tag).find(lower_query) != std::string::npos)
                    {
                        score = 50;
                        break;
                    }
                }
                // Body match
                if (score == 0 && to_lower(article.body).find(lower_query) != std::string::npos)
                {
                    score = 25;
                }
            }

            if (score > 0)
            {
                results.push_back({.article_id = article.id,
                                   .title = article.title,
                                   .category = article.category,
                                   .relevance_score = score});
            }
        }

        // Sort by relevance descending
        std::sort(results.begin(),
                  results.end(),
                  [](const HelpSearchResult& lhs, const HelpSearchResult& rhs)
                  { return lhs.relevance_score > rhs.relevance_score; });

        return results;
    }

    /// Get total number of registered articles.
    [[nodiscard]] auto article_count() const -> std::size_t
    {
        return articles_.size();
    }

    /// Get all article IDs.
    [[nodiscard]] auto all_article_ids() const -> std::vector<std::string>
    {
        std::vector<std::string> ids;
        ids.reserve(articles_.size());
        for (const auto& [aid, article] : articles_)
        {
            ids.push_back(aid);
        }
        return ids;
    }

private:
    std::unordered_map<std::string, HelpArticle> articles_;

    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });
        return result;
    }
};

} // namespace markamp::core
