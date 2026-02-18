/// @file LinkRefactorer.h
/// @brief V9 Phase 17 Tasks 8–9 — Automated link refactoring on rename/move/format.
///
/// Header-only. Generates precise text edits when documents are renamed,
/// headings change, or link formats need conversion (wiki ↔ markdown).

#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// LinkEdit — a single text replacement within a document
// ============================================================================

/// A precise text edit to update a link reference.
struct LinkEdit
{
    std::string document_id; ///< Document to apply the edit in
    int line{0};             ///< 0-based line number
    std::string old_text;    ///< Original link text to replace
    std::string new_text;    ///< Replacement link text
};

// ============================================================================
// LinkFormat — supported link syntaxes
// ============================================================================

/// Link format for conversion.
enum class LinkFormat : uint8_t
{
    kWikiLink,       ///< [[target|display]]
    kMarkdownInline, ///< [display](target)
    kMarkdownRef     ///< [display][ref] with [ref]: target
};

// ============================================================================
// LinkRefactorer
// ============================================================================

/// Generates link update edits when documents are renamed, headings change,
/// or link formats need conversion.
///
/// Usage:
/// ```cpp
/// LinkRefactorer refactorer;
/// auto edits = refactorer.rename_document("old.md", "new.md", documents);
/// // Apply edits to update all referencing documents
/// ```
class LinkRefactorer
{
public:
    LinkRefactorer() = default;

    /// Generate edits for all documents that reference a renamed document.
    /// Handles both [[wikilinks]] and [markdown](links).
    [[nodiscard]] auto rename_document(
        const std::string& old_path,
        const std::string& new_path,
        const std::vector<std::pair<std::string, std::string>>& affected_documents) const
        -> std::vector<LinkEdit>
    {
        std::vector<LinkEdit> edits;
        const auto old_name = extract_name(old_path);
        const auto new_name = extract_name(new_path);

        for (const auto& [doc_id, content] : affected_documents)
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

                // Check for [[old_name]] wikilinks
                find_wikilink_renames(doc_id, line, line_num, old_name, new_name, edits);

                // Check for [text](old_path) markdown links
                find_markdown_link_renames(
                    doc_id, line, line_num, old_path, new_path, old_name, new_name, edits);

                pos = eol + 1;
                ++line_num;
            }
        }

        return edits;
    }

    /// Generate edits for all documents that reference a renamed heading.
    [[nodiscard]] auto
    rename_heading(const std::string& document_id,
                   const std::string& old_heading,
                   const std::string& new_heading,
                   const std::vector<std::pair<std::string, std::string>>& affected_documents) const
        -> std::vector<LinkEdit>
    {
        std::vector<LinkEdit> edits;
        const auto old_slug = slugify(old_heading);
        const auto new_slug = slugify(new_heading);

        for (const auto& [doc_id, content] : affected_documents)
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

                // Look for [[doc#old_heading]] or #old_heading references
                find_heading_renames(doc_id,
                                     line,
                                     line_num,
                                     document_id,
                                     old_slug,
                                     new_slug,
                                     old_heading,
                                     new_heading,
                                     edits);

                pos = eol + 1;
                ++line_num;
            }
        }

        return edits;
    }

    /// Convert link format within a document.
    /// e.g., wiki→markdown: [[target|display]] → [display](target.md)
    [[nodiscard]] auto convert_link_format(const std::string& document_id,
                                           const std::string& content,
                                           LinkFormat from_format,
                                           LinkFormat to_format) const -> std::vector<LinkEdit>
    {
        std::vector<LinkEdit> edits;

        if (from_format == LinkFormat::kWikiLink && to_format == LinkFormat::kMarkdownInline)
        {
            convert_wiki_to_markdown(document_id, content, edits);
        }
        else if (from_format == LinkFormat::kMarkdownInline && to_format == LinkFormat::kWikiLink)
        {
            convert_markdown_to_wiki(document_id, content, edits);
        }

        return edits;
    }

    /// Convert an inline link to a reference-style link.
    /// [text](url) → [text][ref] with [ref]: url at end of doc
    [[nodiscard]] auto extract_inline_to_reference(const std::string& document_id,
                                                   const std::string& content,
                                                   int link_line) const -> std::vector<LinkEdit>
    {
        std::vector<LinkEdit> edits;
        int line_num = 0;
        std::size_t pos = 0;

        while (pos < content.size())
        {
            auto eol = content.find('\n', pos);
            if (eol == std::string::npos)
            {
                eol = content.size();
            }

            if (line_num == link_line)
            {
                const std::string line = content.substr(pos, eol - pos);

                // Find [text](url) on this line
                auto bracket_open = line.find('[');
                auto bracket_close = line.find("](", bracket_open);
                auto paren_close =
                    line.find(')', bracket_close != std::string::npos ? bracket_close + 2 : 0);

                if (bracket_open != std::string::npos && bracket_close != std::string::npos &&
                    paren_close != std::string::npos)
                {
                    auto display = line.substr(bracket_open + 1, bracket_close - bracket_open - 1);
                    auto target_url =
                        line.substr(bracket_close + 2, paren_close - bracket_close - 2);

                    // Generate reference label
                    auto ref_label = generate_ref_label(display);
                    auto old_link = line.substr(bracket_open, paren_close - bracket_open + 1);
                    auto new_link = "[" + display + "][" + ref_label + "]";

                    LinkEdit inline_edit;
                    inline_edit.document_id = document_id;
                    inline_edit.line = link_line;
                    inline_edit.old_text = old_link;
                    inline_edit.new_text = new_link;
                    edits.push_back(std::move(inline_edit));

                    // Add reference definition at end
                    LinkEdit ref_edit;
                    ref_edit.document_id = document_id;
                    ref_edit.line = -1; // Append to end
                    ref_edit.old_text = "";
                    ref_edit.new_text = "[" + ref_label + "]: " + target_url;
                    edits.push_back(std::move(ref_edit));
                }
                break;
            }

            pos = eol + 1;
            ++line_num;
        }

        return edits;
    }

    /// Update block reference IDs across documents.
    [[nodiscard]] auto update_block_references(
        const std::string& old_block_id,
        const std::string& new_block_id,
        const std::vector<std::pair<std::string, std::string>>& affected_documents) const
        -> std::vector<LinkEdit>
    {
        std::vector<LinkEdit> edits;

        for (const auto& [doc_id, content] : affected_documents)
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

                // Check for ^old_block_id at end of line (definition)
                auto old_marker = " ^" + old_block_id;
                if (line.size() >= old_marker.size())
                {
                    auto suffix = line.substr(line.size() - old_marker.size());
                    if (suffix == old_marker)
                    {
                        LinkEdit edit;
                        edit.document_id = doc_id;
                        edit.line = line_num;
                        edit.old_text = "^" + old_block_id;
                        edit.new_text = "^" + new_block_id;
                        edits.push_back(std::move(edit));
                    }
                }

                // Check for [[...^old_block_id]] references
                auto ref_pattern = "^" + old_block_id + "]]";
                if (line.find(ref_pattern) != std::string::npos)
                {
                    LinkEdit edit;
                    edit.document_id = doc_id;
                    edit.line = line_num;
                    edit.old_text = "^" + old_block_id;
                    edit.new_text = "^" + new_block_id;
                    edits.push_back(std::move(edit));
                }

                pos = eol + 1;
                ++line_num;
            }
        }

        return edits;
    }

private:
    /// Find wikilink references to rename.
    void find_wikilink_renames(const std::string& doc_id,
                               const std::string& line,
                               int line_num,
                               const std::string& old_name,
                               const std::string& new_name,
                               std::vector<LinkEdit>& edits) const
    {
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

            auto link_content = line.substr(open_bracket + 2, close_bracket - open_bracket - 2);

            // Extract target (before | and #)
            auto pipe_pos = link_content.find('|');
            auto hash_pos = link_content.find('#');
            auto target = link_content;
            if (pipe_pos != std::string::npos)
            {
                target = link_content.substr(0, pipe_pos);
            }
            else if (hash_pos != std::string::npos)
            {
                target = link_content.substr(0, hash_pos);
            }

            if (to_lower(target) == to_lower(old_name))
            {
                auto old_full = "[[" + link_content + "]]";
                auto new_content = link_content;
                // Replace the target portion
                new_content.replace(0, target.size(), new_name);
                auto new_full = "[[" + new_content + "]]";

                LinkEdit edit;
                edit.document_id = doc_id;
                edit.line = line_num;
                edit.old_text = old_full;
                edit.new_text = new_full;
                edits.push_back(std::move(edit));
            }

            search_pos = close_bracket + 2;
        }
    }

    /// Find markdown link references to rename.
    void find_markdown_link_renames(const std::string& doc_id,
                                    const std::string& line,
                                    int line_num,
                                    const std::string& old_path,
                                    const std::string& new_path,
                                    const std::string& old_name,
                                    const std::string& new_name,
                                    std::vector<LinkEdit>& edits) const
    {
        std::size_t search_pos = 0;
        while (search_pos < line.size())
        {
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

            if (to_lower(url) == to_lower(old_path) || to_lower(url) == to_lower(old_name) ||
                to_lower(url) == to_lower(old_name + ".md"))
            {
                auto old_link = "](" + url + ")";
                auto new_url = (url == old_path) ? new_path : new_name;
                // Preserve .md extension if original had it
                if (url.size() >= 3 && url.substr(url.size() - 3) == ".md" && new_url.size() >= 3 &&
                    new_url.substr(new_url.size() - 3) != ".md")
                {
                    new_url += ".md";
                }
                auto new_link = "](" + new_url + ")";

                LinkEdit edit;
                edit.document_id = doc_id;
                edit.line = line_num;
                edit.old_text = old_link;
                edit.new_text = new_link;
                edits.push_back(std::move(edit));
            }

            search_pos = paren_close + 1;
        }
    }

    /// Find heading references to rename.
    void find_heading_renames(const std::string& doc_id,
                              const std::string& line,
                              int line_num,
                              const std::string& target_document,
                              const std::string& old_slug,
                              const std::string& new_slug,
                              const std::string& old_heading,
                              const std::string& new_heading,
                              std::vector<LinkEdit>& edits) const
    {
        // [[doc#old_heading]] → [[doc#new_heading]]
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

            auto link_content = line.substr(open_bracket + 2, close_bracket - open_bracket - 2);
            auto hash_pos = link_content.find('#');
            if (hash_pos != std::string::npos)
            {
                auto heading_part = link_content.substr(hash_pos + 1);
                // Strip display text
                auto pipe_pos = heading_part.find('|');
                if (pipe_pos != std::string::npos)
                {
                    heading_part = heading_part.substr(0, pipe_pos);
                }

                if (to_lower(heading_part) == to_lower(old_heading) ||
                    to_lower(heading_part) == to_lower(old_slug))
                {
                    auto old_full = "[[" + link_content + "]]";
                    auto new_content = link_content;
                    new_content.replace(hash_pos + 1, heading_part.size(), new_heading);
                    auto new_full = "[[" + new_content + "]]";

                    LinkEdit edit;
                    edit.document_id = doc_id;
                    edit.line = line_num;
                    edit.old_text = old_full;
                    edit.new_text = new_full;
                    edits.push_back(std::move(edit));
                }
            }

            search_pos = close_bracket + 2;
        }

        // [text](#old_slug) → [text](#new_slug) within same document
        if (doc_id == target_document)
        {
            std::size_t anchor_pos = 0;
            while (anchor_pos < line.size())
            {
                auto pattern = line.find("](#" + old_slug + ")", anchor_pos);
                if (pattern == std::string::npos)
                {
                    break;
                }

                auto old_ref = "](#" + old_slug + ")";
                auto new_ref = "](#" + new_slug + ")";

                LinkEdit edit;
                edit.document_id = doc_id;
                edit.line = line_num;
                edit.old_text = old_ref;
                edit.new_text = new_ref;
                edits.push_back(std::move(edit));

                anchor_pos = pattern + old_ref.size();
            }
        }
    }

    /// Convert [[wikilinks]] to [text](url) format.
    void convert_wiki_to_markdown(const std::string& document_id,
                                  const std::string& content,
                                  std::vector<LinkEdit>& edits) const
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
            std::size_t search_pos = 0;

            while (search_pos < line.size())
            {
                auto open_bracket = line.find("[[", search_pos);
                if (open_bracket == std::string::npos)
                {
                    break;
                }
                // Skip embeds ![[...]]
                if (open_bracket > 0 && line[open_bracket - 1] == '!')
                {
                    search_pos = open_bracket + 2;
                    continue;
                }

                auto close_bracket = line.find("]]", open_bracket + 2);
                if (close_bracket == std::string::npos)
                {
                    break;
                }

                auto link_content = line.substr(open_bracket + 2, close_bracket - open_bracket - 2);

                // Parse target and display
                auto pipe_pos = link_content.find('|');
                std::string target_text;
                std::string display;
                if (pipe_pos != std::string::npos)
                {
                    target_text = link_content.substr(0, pipe_pos);
                    display = link_content.substr(pipe_pos + 1);
                }
                else
                {
                    target_text = link_content;
                    display = link_content;
                }

                auto old_link = "[[" + link_content + "]]";
                auto new_link = "[" + display + "](" + target_text + ".md)";

                LinkEdit edit;
                edit.document_id = document_id;
                edit.line = line_num;
                edit.old_text = old_link;
                edit.new_text = new_link;
                edits.push_back(std::move(edit));

                search_pos = close_bracket + 2;
            }

            pos = eol + 1;
            ++line_num;
        }
    }

    /// Convert [text](url) links to [[wikilinks]].
    void convert_markdown_to_wiki(const std::string& document_id,
                                  const std::string& content,
                                  std::vector<LinkEdit>& edits) const
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
            std::size_t search_pos = 0;

            while (search_pos < line.size())
            {
                auto bracket_open = line.find('[', search_pos);
                if (bracket_open == std::string::npos)
                {
                    break;
                }
                // Skip image links ![
                if (bracket_open > 0 && line[bracket_open - 1] == '!')
                {
                    search_pos = bracket_open + 1;
                    continue;
                }

                auto bracket_close = line.find("](", bracket_open);
                if (bracket_close == std::string::npos)
                {
                    break;
                }
                auto paren_close = line.find(')', bracket_close + 2);
                if (paren_close == std::string::npos)
                {
                    break;
                }

                auto display = line.substr(bracket_open + 1, bracket_close - bracket_open - 1);
                auto url = line.substr(bracket_close + 2, paren_close - bracket_close - 2);

                // Only convert relative local links (not http/https)
                if (url.find("://") == std::string::npos &&
                    url.find("mailto:") == std::string::npos)
                {
                    // Remove .md extension from target
                    auto target_text = url;
                    if (target_text.size() >= 3 &&
                        target_text.substr(target_text.size() - 3) == ".md")
                    {
                        target_text = target_text.substr(0, target_text.size() - 3);
                    }

                    auto old_link = line.substr(bracket_open, paren_close - bracket_open + 1);
                    std::string new_link;
                    if (display != target_text)
                    {
                        new_link = "[[" + target_text + "|" + display + "]]";
                    }
                    else
                    {
                        new_link = "[[" + target_text + "]]";
                    }

                    LinkEdit edit;
                    edit.document_id = document_id;
                    edit.line = line_num;
                    edit.old_text = old_link;
                    edit.new_text = new_link;
                    edits.push_back(std::move(edit));
                }

                search_pos = paren_close + 1;
            }

            pos = eol + 1;
            ++line_num;
        }
    }

    /// Extract filename without extension from path.
    [[nodiscard]] static auto extract_name(const std::string& path) -> std::string
    {
        auto slash_pos = path.rfind('/');
        auto name = (slash_pos != std::string::npos) ? path.substr(slash_pos + 1) : path;
        auto dot_pos = name.rfind('.');
        if (dot_pos != std::string::npos)
        {
            name = name.substr(0, dot_pos);
        }
        return name;
    }

    /// Generate a reference label from display text.
    [[nodiscard]] static auto generate_ref_label(const std::string& display) -> std::string
    {
        std::string label;
        for (const char ch : display)
        {
            if (std::isalnum(static_cast<unsigned char>(ch)) != 0)
            {
                label += static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
            else if (ch == ' ' || ch == '-')
            {
                if (!label.empty() && label.back() != '-')
                {
                    label += '-';
                }
            }
        }
        if (!label.empty() && label.back() == '-')
        {
            label.pop_back();
        }
        return label;
    }

    /// Slugify heading text.
    [[nodiscard]] static auto slugify(const std::string& text) -> std::string
    {
        std::string slug;
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
};

} // namespace markamp::core
