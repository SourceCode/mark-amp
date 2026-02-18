/// @file HeadingJumpProtocol.h
/// @brief V9 Phase 17 Tasks 3–4 — Cross-document heading navigation with fuzzy matching.
///
/// Header-only. Resolves heading fragments (#heading-text) across documents
/// with slug normalization and Levenshtein-distance fuzzy fallback.

#pragma once

#include <algorithm>
#include <cctype>
#include <cmath>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// HeadingTarget
// ============================================================================

/// A heading location within a document.
struct HeadingTarget
{
    std::string document_id;  ///< Document containing this heading
    std::string heading_text; ///< Original heading text (e.g., "My Section")
    std::string heading_slug; ///< URL-slugified version (e.g., "my-section")
    int heading_level{1};     ///< 1–6 for h1–h6
    int line_number{0};       ///< 0-based line number
};

// ============================================================================
// HeadingIndex — cross-document heading registry
// ============================================================================

/// Cross-document heading index for fast lookup.
class HeadingIndex
{
public:
    HeadingIndex() = default;

    /// Add headings from a document.
    void add_document(const std::string& document_id, const std::vector<HeadingTarget>& headings)
    {
        // Remove old entries for this document
        auto iter = headings_.begin();
        while (iter != headings_.end())
        {
            if (iter->document_id == document_id)
            {
                iter = headings_.erase(iter);
            }
            else
            {
                ++iter;
            }
        }

        for (auto heading : headings)
        {
            heading.document_id = document_id;
            headings_.push_back(std::move(heading));
        }
    }

    /// Find a heading by document and slug.
    [[nodiscard]] auto find(const std::string& document_id, const std::string& slug) const
        -> std::optional<HeadingTarget>
    {
        for (const auto& heading : headings_)
        {
            if (heading.document_id == document_id && heading.heading_slug == slug)
            {
                return heading;
            }
        }
        return std::nullopt;
    }

    /// Find all headings in a specific document.
    [[nodiscard]] auto headings_in(const std::string& document_id) const
        -> std::vector<HeadingTarget>
    {
        std::vector<HeadingTarget> results;
        for (const auto& heading : headings_)
        {
            if (heading.document_id == document_id)
            {
                results.push_back(heading);
            }
        }
        return results;
    }

    /// Search headings across all documents by slug prefix.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<HeadingTarget>
    {
        const auto lower_query = to_lower(query);
        std::vector<HeadingTarget> results;
        for (const auto& heading : headings_)
        {
            if (heading.heading_slug.find(lower_query) != std::string::npos ||
                to_lower(heading.heading_text).find(lower_query) != std::string::npos)
            {
                results.push_back(heading);
            }
        }
        return results;
    }

    /// Total number of indexed headings.
    [[nodiscard]] auto size() const -> std::size_t
    {
        return headings_.size();
    }

    /// All indexed headings.
    [[nodiscard]] auto all() const -> const std::vector<HeadingTarget>&
    {
        return headings_;
    }

    /// Clear all indexed data.
    void clear()
    {
        headings_.clear();
    }

private:
    std::vector<HeadingTarget> headings_;

    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return result;
    }
};

// ============================================================================
// HeadingJumpProtocol — heading resolution with fuzzy fallback
// ============================================================================

/// Resolves heading fragment references across documents.
///
/// Supports exact slug matching first, then falls back to
/// Levenshtein-distance fuzzy matching if no exact match is found.
///
/// Usage:
/// ```cpp
/// HeadingJumpProtocol protocol;
/// auto target = protocol.resolve_heading("doc1", "my-section", content);
/// ```
class HeadingJumpProtocol
{
public:
    HeadingJumpProtocol() = default;

    /// Resolve a heading fragment within a document's content.
    /// Tries exact slug match first, then fuzzy match.
    [[nodiscard]] auto resolve_heading(const std::string& document_id,
                                       const std::string& heading_fragment,
                                       const std::string& content) const
        -> std::optional<HeadingTarget>
    {
        const auto headings = find_all_headings(content);
        const auto slug = slugify(heading_fragment);

        // Exact slug match
        for (const auto& heading : headings)
        {
            if (heading.heading_slug == slug)
            {
                HeadingTarget result = heading;
                result.document_id = document_id;
                return result;
            }
        }

        // Fuzzy fallback
        auto closest = closest_heading_match(slug, headings);
        if (closest.has_value())
        {
            closest->document_id = document_id;
        }
        return closest;
    }

    /// Find all headings in markdown content.
    [[nodiscard]] static auto find_all_headings(const std::string& content)
        -> std::vector<HeadingTarget>
    {
        std::vector<HeadingTarget> results;
        if (content.empty())
        {
            return results;
        }

        int line_num = 0;
        std::size_t pos = 0;
        bool in_code_fence = false;

        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }

            const std::string line = content.substr(pos, eol - pos);
            const auto trimmed = trim_left(line);

            // Toggle code fence tracking
            if (trimmed.rfind("```", 0) == 0 || trimmed.rfind("~~~", 0) == 0)
            {
                in_code_fence = !in_code_fence;
            }

            if (!in_code_fence && !trimmed.empty() && trimmed[0] == '#')
            {
                auto heading = parse_heading_line(trimmed, line_num);
                if (heading.has_value())
                {
                    results.push_back(std::move(*heading));
                }
            }

            pos = eol + 1;
            ++line_num;
        }

        return results;
    }

    /// Build a cross-document heading index from a map of document_id → content.
    [[nodiscard]] static auto
    build_heading_index(const std::unordered_map<std::string, std::string>& documents)
        -> HeadingIndex
    {
        HeadingIndex index;
        for (const auto& [doc_id, content] : documents)
        {
            auto headings = find_all_headings(content);
            index.add_document(doc_id, headings);
        }
        return index;
    }

    /// Find the closest heading match using Levenshtein distance.
    /// Returns nullopt if no heading is close enough (threshold: 40% of slug length).
    [[nodiscard]] static auto closest_heading_match(const std::string& query_slug,
                                                    const std::vector<HeadingTarget>& headings)
        -> std::optional<HeadingTarget>
    {
        if (headings.empty() || query_slug.empty())
        {
            return std::nullopt;
        }

        const HeadingTarget* best_match = nullptr;
        int best_distance = std::numeric_limits<int>::max();
        const int max_distance =
            std::max(1, static_cast<int>(query_slug.size()) * 4 / 10); // 40% threshold

        for (const auto& heading : headings)
        {
            const int dist = levenshtein(query_slug, heading.heading_slug);
            if (dist < best_distance)
            {
                best_distance = dist;
                best_match = &heading;
            }
        }

        if (best_match != nullptr && best_distance <= max_distance)
        {
            return *best_match;
        }

        return std::nullopt;
    }

    /// Convert heading text to a URL slug.
    /// "My Heading (Title)" → "my-heading-title"
    [[nodiscard]] static auto slugify(const std::string& text) -> std::string
    {
        std::string slug;
        slug.reserve(text.size());

        bool prev_was_dash = true;
        for (const char ch : text)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) != 0)
            {
                slug += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
                prev_was_dash = false;
            }
            else if (!prev_was_dash && (ch == ' ' || ch == '-' || ch == '_'))
            {
                slug += '-';
                prev_was_dash = true;
            }
        }

        if (!slug.empty() && slug.back() == '-')
        {
            slug.pop_back();
        }

        return slug;
    }

private:
    /// Parse a heading line ("## Title") into a HeadingTarget.
    [[nodiscard]] static auto parse_heading_line(const std::string& trimmed_line, int line_num)
        -> std::optional<HeadingTarget>
    {
        int level = 0;
        std::size_t idx = 0;
        while (idx < trimmed_line.size() && trimmed_line[idx] == '#')
        {
            ++level;
            ++idx;
        }

        if (level < 1 || level > 6)
        {
            return std::nullopt;
        }

        // Must be followed by a space
        if (idx >= trimmed_line.size() || trimmed_line[idx] != ' ')
        {
            return std::nullopt;
        }
        ++idx;

        // Extract heading text
        auto heading_text = trimmed_line.substr(idx);
        // Remove trailing # decorations
        while (!heading_text.empty() && heading_text.back() == '#')
        {
            heading_text.pop_back();
        }
        // Trim trailing whitespace
        while (!heading_text.empty() && (heading_text.back() == ' ' || heading_text.back() == '\t'))
        {
            heading_text.pop_back();
        }

        if (heading_text.empty())
        {
            return std::nullopt;
        }

        HeadingTarget target;
        target.heading_text = heading_text;
        target.heading_slug = slugify(heading_text);
        target.heading_level = level;
        target.line_number = line_num;
        return target;
    }

    /// Compute Levenshtein edit distance between two strings.
    [[nodiscard]] static auto levenshtein(const std::string& str_a, const std::string& str_b) -> int
    {
        const auto len_a = str_a.size();
        const auto len_b = str_b.size();

        std::vector<std::vector<int>> matrix(len_a + 1, std::vector<int>(len_b + 1, 0));

        for (std::size_t idx = 0; idx <= len_a; ++idx)
        {
            matrix[idx][0] = static_cast<int>(idx);
        }
        for (std::size_t idx = 0; idx <= len_b; ++idx)
        {
            matrix[0][idx] = static_cast<int>(idx);
        }

        for (std::size_t row = 1; row <= len_a; ++row)
        {
            for (std::size_t col = 1; col <= len_b; ++col)
            {
                const int cost = (str_a[row - 1] == str_b[col - 1]) ? 0 : 1;
                matrix[row][col] = std::min({
                    matrix[row - 1][col] + 1,       // deletion
                    matrix[row][col - 1] + 1,       // insertion
                    matrix[row - 1][col - 1] + cost // substitution
                });
            }
        }

        return matrix[len_a][len_b];
    }

    /// Trim leading whitespace.
    [[nodiscard]] static auto trim_left(const std::string& str) -> std::string
    {
        auto start = str.find_first_not_of(" \t");
        if (start == std::string::npos)
        {
            return {};
        }
        return str.substr(start);
    }
};

} // namespace markamp::core
