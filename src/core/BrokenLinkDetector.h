/// @file BrokenLinkDetector.h
/// @brief V9 Phase 17 Tasks 6–7 — Broken link detection with repair suggestions.
///
/// Header-only. Scans markdown documents for links that no longer resolve,
/// categorizes them by type and failure reason, and provides fuzzy-matched
/// repair suggestions.

#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Broken link types
// ============================================================================

/// Type of link that is broken.
enum class BrokenLinkType : uint8_t
{
    kWikiLink,     ///< [[target]] that doesn't resolve
    kMarkdownLink, ///< [text](url) with broken relative path
    kHeadingRef,   ///< #heading-slug that doesn't exist
    kBlockRef,     ///< ^block-id that doesn't exist
    kFileLink      ///< File path link that doesn't exist
};

/// Why a link is broken.
enum class BrokenLinkReason : uint8_t
{
    kTargetNotFound, ///< No document matches the link target
    kHeadingMissing, ///< Document exists but heading not found
    kBlockIdMissing, ///< Document exists but ^block-id not found
    kFileDeleted,    ///< Referenced file no longer exists
    kAmbiguousTarget ///< Multiple documents match (shortest-path ambiguity)
};

/// A single broken link found in a document.
struct BrokenLink
{
    std::string source_document; ///< Document containing the broken link
    int source_line{0};          ///< Line number of the broken link (0-based)
    std::string link_text;       ///< Raw link text as written in markdown
    std::string link_target;     ///< The target the link was trying to reach
    BrokenLinkType link_type{BrokenLinkType::kWikiLink};
    BrokenLinkReason reason{BrokenLinkReason::kTargetNotFound};
};

/// A suggested repair for a broken link.
struct RepairSuggestion
{
    std::string suggested_target; ///< What the link should point to
    double confidence{0.0};       ///< Confidence score 0.0–1.0
    std::string explanation;      ///< Why this suggestion was made
};

/// Summary report of broken links across a document set.
struct BrokenLinkReport
{
    int total_broken{0};
    std::unordered_map<int, int> by_type_counts; ///< BrokenLinkType (as int) → count
    std::unordered_map<std::string, std::vector<BrokenLink>> by_document;
    std::string scan_timestamp;

    /// Get count for a specific type.
    [[nodiscard]] auto count_for_type(BrokenLinkType link_type) const -> int
    {
        auto iter = by_type_counts.find(static_cast<int>(link_type));
        if (iter != by_type_counts.end())
        {
            return iter->second;
        }
        return 0;
    }

    /// Get all broken links in a specific document.
    [[nodiscard]] auto broken_in(const std::string& document_id) const -> std::vector<BrokenLink>
    {
        auto iter = by_document.find(document_id);
        if (iter != by_document.end())
        {
            return iter->second;
        }
        return {};
    }

    /// Get documents with the most broken links.
    [[nodiscard]] auto worst_documents(int limit) const -> std::vector<std::pair<std::string, int>>
    {
        std::vector<std::pair<std::string, int>> doc_counts;
        for (const auto& [doc_id, links] : by_document)
        {
            doc_counts.emplace_back(doc_id, static_cast<int>(links.size()));
        }

        std::sort(doc_counts.begin(),
                  doc_counts.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.second > rhs.second; });

        if (static_cast<int>(doc_counts.size()) > limit)
        {
            doc_counts.resize(static_cast<std::size_t>(limit));
        }
        return doc_counts;
    }
};

// ============================================================================
// BrokenLinkDetector
// ============================================================================

/// Scans markdown documents for broken links and provides repair suggestions.
///
/// Usage:
/// ```cpp
/// BrokenLinkDetector detector;
/// auto broken = detector.scan_document("doc1", content, vault_paths);
/// auto report = detector.scan_all(documents, vault_paths);
/// ```
class BrokenLinkDetector
{
public:
    BrokenLinkDetector() = default;

    /// Scan a single document for broken links.
    [[nodiscard]] auto scan_document(const std::string& document_id,
                                     const std::string& content,
                                     const std::vector<std::string>& vault_paths) const
        -> std::vector<BrokenLink>
    {
        std::vector<BrokenLink> results;

        scan_wikilinks(document_id, content, vault_paths, results);
        scan_markdown_links(document_id, content, vault_paths, results);
        scan_heading_refs(document_id, content, results);
        scan_block_refs(document_id, content, results);

        return results;
    }

    /// Scan all documents and produce a comprehensive report.
    [[nodiscard]] auto scan_all(const std::unordered_map<std::string, std::string>& documents,
                                const std::vector<std::string>& vault_paths) const
        -> BrokenLinkReport
    {
        BrokenLinkReport report;

        for (const auto& [doc_id, content] : documents)
        {
            auto broken = scan_document(doc_id, content, vault_paths);
            if (!broken.empty())
            {
                report.by_document[doc_id] = broken;
                for (const auto& link : broken)
                {
                    ++report.total_broken;
                    ++report.by_type_counts[static_cast<int>(link.link_type)];
                }
            }
        }

        return report;
    }

    /// Suggest repairs for a broken link using fuzzy matching.
    [[nodiscard]] auto suggest_repair(const BrokenLink& broken_link,
                                      const std::vector<std::string>& vault_paths) const
        -> std::vector<RepairSuggestion>
    {
        std::vector<RepairSuggestion> suggestions;
        const auto target = to_lower(broken_link.link_target);

        for (const auto& vault_path : vault_paths)
        {
            const auto path_lower = to_lower(vault_path);

            // Extract filename without extension for comparison
            auto filename = extract_filename(vault_path);
            auto filename_lower = to_lower(filename);

            // Exact substring match
            if (path_lower.find(target) != std::string::npos)
            {
                RepairSuggestion suggestion;
                suggestion.suggested_target = vault_path;
                suggestion.confidence = 0.9;
                suggestion.explanation = "Path contains target text";
                suggestions.push_back(std::move(suggestion));
                continue;
            }

            // Filename starts with target
            if (filename_lower.find(target) != std::string::npos)
            {
                RepairSuggestion suggestion;
                suggestion.suggested_target = filename;
                suggestion.confidence = 0.8;
                suggestion.explanation = "Filename contains target text";
                suggestions.push_back(std::move(suggestion));
                continue;
            }

            // Levenshtein distance on filename
            const int distance = levenshtein(target, filename_lower);
            const int max_acceptable =
                std::max(1,
                         static_cast<int>(target.size()) * 3 / 10); // 30% threshold

            if (distance <= max_acceptable)
            {
                RepairSuggestion suggestion;
                suggestion.suggested_target = filename;
                suggestion.confidence =
                    1.0 - (static_cast<double>(distance) /
                           static_cast<double>(std::max(target.size(), filename_lower.size())));
                suggestion.explanation =
                    "Similar filename (Levenshtein distance: " + std::to_string(distance) + ")";
                suggestions.push_back(std::move(suggestion));
            }
        }

        // Sort by confidence descending
        std::sort(suggestions.begin(),
                  suggestions.end(),
                  [](const auto& lhs, const auto& rhs) { return lhs.confidence > rhs.confidence; });

        // Limit to top 5
        constexpr std::size_t kMaxSuggestions = 5;
        if (suggestions.size() > kMaxSuggestions)
        {
            suggestions.resize(kMaxSuggestions);
        }

        return suggestions;
    }

private:
    /// Scan for broken [[wikilinks]].
    void scan_wikilinks(const std::string& document_id,
                        const std::string& content,
                        const std::vector<std::string>& vault_paths,
                        std::vector<BrokenLink>& results) const
    {
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

            // Track code fences
            const auto trimmed = trim_left(line);
            if (trimmed.rfind("```", 0) == 0 || trimmed.rfind("~~~", 0) == 0)
            {
                in_code_fence = !in_code_fence;
            }

            if (!in_code_fence)
            {
                // Find [[...]] patterns
                std::size_t search_pos = 0;
                while (search_pos < line.size())
                {
                    auto open_bracket = line.find("[[", search_pos);
                    if (open_bracket == std::string::npos)
                    {
                        break;
                    }

                    auto close_bracket = line.find("]]", open_bracket + 2);
                    if (close_bracket == std::string::npos)
                    {
                        break;
                    }

                    auto link_target =
                        line.substr(open_bracket + 2, close_bracket - open_bracket - 2);

                    // Strip display text after |
                    auto pipe_pos = link_target.find('|');
                    if (pipe_pos != std::string::npos)
                    {
                        link_target = link_target.substr(0, pipe_pos);
                    }

                    // Strip heading after #
                    auto hash_pos = link_target.find('#');
                    std::string heading_part;
                    if (hash_pos != std::string::npos)
                    {
                        heading_part = link_target.substr(hash_pos + 1);
                        link_target = link_target.substr(0, hash_pos);
                    }

                    // Strip block ref after ^
                    auto caret_pos = link_target.find('^');
                    if (caret_pos != std::string::npos)
                    {
                        link_target = link_target.substr(0, caret_pos);
                    }

                    // Check if target resolves
                    if (!link_target.empty() && !resolves_in_vault(link_target, vault_paths))
                    {
                        BrokenLink broken;
                        broken.source_document = document_id;
                        broken.source_line = line_num;
                        broken.link_text =
                            line.substr(open_bracket, close_bracket - open_bracket + 2);
                        broken.link_target = link_target;
                        broken.link_type = BrokenLinkType::kWikiLink;
                        broken.reason = BrokenLinkReason::kTargetNotFound;
                        results.push_back(std::move(broken));
                    }

                    search_pos = close_bracket + 2;
                }
            }

            pos = eol + 1;
            ++line_num;
        }
    }

    /// Scan for broken [text](url) markdown links (relative paths only).
    void scan_markdown_links(const std::string& document_id,
                             const std::string& content,
                             const std::vector<std::string>& vault_paths,
                             std::vector<BrokenLink>& results) const
    {
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
            if (trimmed.rfind("```", 0) == 0 || trimmed.rfind("~~~", 0) == 0)
            {
                in_code_fence = !in_code_fence;
            }

            if (!in_code_fence)
            {
                std::size_t search_pos = 0;
                while (search_pos < line.size())
                {
                    // Find ]( pattern
                    auto bracket_close = line.find("](", search_pos);
                    if (bracket_close == std::string::npos)
                    {
                        break;
                    }

                    auto paren_close = line.find(')', bracket_close + 2);
                    if (paren_close == std::string::npos)
                    {
                        break;
                    }

                    auto url = line.substr(bracket_close + 2, paren_close - bracket_close - 2);

                    // Only check relative paths (not http://, mailto:, etc.)
                    if (!url.empty() && url.find("://") == std::string::npos &&
                        url.find("mailto:") == std::string::npos &&
                        url[0] != '#') // Not an anchor-only link
                    {
                        // Strip query/fragment
                        auto query_pos = url.find('?');
                        if (query_pos != std::string::npos)
                        {
                            url = url.substr(0, query_pos);
                        }
                        auto frag_pos = url.find('#');
                        if (frag_pos != std::string::npos)
                        {
                            url = url.substr(0, frag_pos);
                        }

                        if (!url.empty() && !resolves_in_vault(url, vault_paths))
                        {
                            BrokenLink broken;
                            broken.source_document = document_id;
                            broken.source_line = line_num;
                            broken.link_text =
                                line.substr(bracket_close, paren_close - bracket_close + 1);
                            broken.link_target = url;
                            broken.link_type = BrokenLinkType::kMarkdownLink;
                            broken.reason = BrokenLinkReason::kFileDeleted;
                            results.push_back(std::move(broken));
                        }
                    }

                    search_pos = paren_close + 1;
                }
            }

            pos = eol + 1;
            ++line_num;
        }
    }

    /// Scan for heading references that don't resolve within the same document.
    void scan_heading_refs(const std::string& document_id,
                           const std::string& content,
                           std::vector<BrokenLink>& results) const
    {
        // Collect all headings in this document
        std::vector<std::string> heading_slugs;
        int line_num = 0;
        std::size_t pos = 0;

        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }

            const std::string line = content.substr(pos, eol - pos);
            const auto trimmed = trim_left(line);

            if (!trimmed.empty() && trimmed[0] == '#')
            {
                auto slug = extract_heading_slug(trimmed);
                if (!slug.empty())
                {
                    heading_slugs.push_back(slug);
                }
            }

            pos = eol + 1;
            ++line_num;
        }

        // Scan for #heading-ref links
        line_num = 0;
        pos = 0;
        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }

            const std::string line = content.substr(pos, eol - pos);

            // Look for [text](#heading-slug) patterns
            std::size_t search_pos = 0;
            while (search_pos < line.size())
            {
                auto pattern = line.find("](#", search_pos);
                if (pattern == std::string::npos)
                {
                    break;
                }

                auto paren_close = line.find(')', pattern + 3);
                if (paren_close == std::string::npos)
                {
                    break;
                }

                auto heading_ref = line.substr(pattern + 3, paren_close - pattern - 3);

                if (!heading_ref.empty())
                {
                    bool heading_found = false;
                    for (const auto& slug : heading_slugs)
                    {
                        if (slug == heading_ref)
                        {
                            heading_found = true;
                            break;
                        }
                    }

                    if (!heading_found)
                    {
                        BrokenLink broken;
                        broken.source_document = document_id;
                        broken.source_line = line_num;
                        broken.link_text = "#" + heading_ref;
                        broken.link_target = heading_ref;
                        broken.link_type = BrokenLinkType::kHeadingRef;
                        broken.reason = BrokenLinkReason::kHeadingMissing;
                        results.push_back(std::move(broken));
                    }
                }

                search_pos = paren_close + 1;
            }

            pos = eol + 1;
            ++line_num;
        }
    }

    /// Scan for block references that don't resolve within the same document.
    void scan_block_refs(const std::string& document_id,
                         const std::string& content,
                         std::vector<BrokenLink>& results) const
    {
        // Collect all ^block-ids in this document
        std::vector<std::string> block_ids;
        {
            int line_num = 0;
            std::size_t pos = 0;
            while (pos < content.size())
            {
                auto eol = content.find('\n', pos);
                if (eol == std::string::npos)
                {
                    eol = content.size();
                }
                const std::string line = content.substr(pos, eol - pos);
                auto block_id = extract_trailing_block_id(line);
                if (!block_id.empty())
                {
                    block_ids.push_back(block_id);
                }
                pos = eol + 1;
                ++line_num;
            }
        }

        // Scan for [[...^block-id]] references
        int line_num = 0;
        std::size_t pos = 0;
        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }
            const std::string line = content.substr(pos, eol - pos);

            // Look for ^block-id inside [[...]]
            std::size_t search_pos = 0;
            while (search_pos < line.size())
            {
                auto open_bracket = line.find("[[", search_pos);
                if (open_bracket == std::string::npos)
                {
                    break;
                }
                auto close_bracket = line.find("]]", open_bracket + 2);
                if (close_bracket == std::string::npos)
                {
                    break;
                }

                auto content_text = line.substr(open_bracket + 2, close_bracket - open_bracket - 2);
                auto caret_pos = content_text.find('^');
                if (caret_pos != std::string::npos)
                {
                    auto ref_id = content_text.substr(caret_pos + 1);
                    bool block_found = false;
                    for (const auto& existing_id : block_ids)
                    {
                        if (existing_id == ref_id)
                        {
                            block_found = true;
                            break;
                        }
                    }

                    if (!block_found && !ref_id.empty())
                    {
                        BrokenLink broken;
                        broken.source_document = document_id;
                        broken.source_line = line_num;
                        broken.link_text = "^" + ref_id;
                        broken.link_target = ref_id;
                        broken.link_type = BrokenLinkType::kBlockRef;
                        broken.reason = BrokenLinkReason::kBlockIdMissing;
                        results.push_back(std::move(broken));
                    }
                }

                search_pos = close_bracket + 2;
            }

            pos = eol + 1;
            ++line_num;
        }
    }

    /// Check if a target name resolves in the vault.
    [[nodiscard]] static auto resolves_in_vault(const std::string& target,
                                                const std::vector<std::string>& vault_paths) -> bool
    {
        const auto target_lower = to_lower(target);
        for (const auto& vault_path : vault_paths)
        {
            const auto path_lower = to_lower(vault_path);

            // Exact match
            if (path_lower == target_lower)
            {
                return true;
            }

            // Filename match (shortest-path Obsidian style)
            auto filename = to_lower(extract_filename(vault_path));
            if (filename == target_lower)
            {
                return true;
            }

            // Filename without extension match
            auto dot_pos = filename.rfind('.');
            if (dot_pos != std::string::npos)
            {
                auto name_no_ext = filename.substr(0, dot_pos);
                if (name_no_ext == target_lower)
                {
                    return true;
                }
            }

            // Path ends with target
            if (path_lower.size() >= target_lower.size())
            {
                auto suffix = path_lower.substr(path_lower.size() - target_lower.size());
                if (suffix == target_lower)
                {
                    return true;
                }
            }
        }
        return false;
    }

    /// Extract filename from path.
    [[nodiscard]] static auto extract_filename(const std::string& path) -> std::string
    {
        auto slash_pos = path.rfind('/');
        if (slash_pos != std::string::npos)
        {
            return path.substr(slash_pos + 1);
        }
        return path;
    }

    /// Extract heading slug from a heading line (e.g. "## Title" → "title").
    [[nodiscard]] static auto extract_heading_slug(const std::string& heading_line) -> std::string
    {
        std::size_t idx = 0;
        while (idx < heading_line.size() && heading_line[idx] == '#')
        {
            ++idx;
        }
        if (idx >= heading_line.size() || heading_line[idx] != ' ')
        {
            return {};
        }
        ++idx;

        auto heading_text = heading_line.substr(idx);
        // Remove trailing # decorations and whitespace
        while (!heading_text.empty() && (heading_text.back() == '#' || heading_text.back() == ' ' ||
                                         heading_text.back() == '\t'))
        {
            heading_text.pop_back();
        }

        // Slugify
        std::string slug;
        bool prev_was_dash = true;
        for (const char ch : heading_text)
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

    /// Extract ^block-id from end of a line.
    [[nodiscard]] static auto extract_trailing_block_id(const std::string& line) -> std::string
    {
        auto trimmed = line;
        while (!trimmed.empty() && (trimmed.back() == ' ' || trimmed.back() == '\t' ||
                                    trimmed.back() == '\r' || trimmed.back() == '\n'))
        {
            trimmed.pop_back();
        }
        if (trimmed.empty())
        {
            return {};
        }

        auto caret_pos = trimmed.rfind('^');
        if (caret_pos == std::string::npos || caret_pos == 0)
        {
            return {};
        }
        if (trimmed[caret_pos - 1] != ' ')
        {
            return {};
        }

        auto candidate = trimmed.substr(caret_pos + 1);
        for (const char ch : candidate)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) == 0 && ch != '-')
            {
                return {};
            }
        }
        return candidate;
    }

    /// Levenshtein distance.
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
                matrix[row][col] = std::min({matrix[row - 1][col] + 1,
                                             matrix[row][col - 1] + 1,
                                             matrix[row - 1][col - 1] + cost});
            }
        }
        return matrix[len_a][len_b];
    }

    /// To lowercase.
    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return result;
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
