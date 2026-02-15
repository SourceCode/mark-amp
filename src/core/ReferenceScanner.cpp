#include "ReferenceScanner.h"

#include <regex>
#include <string>

namespace markamp::core
{

auto ReferenceScanner::scan(const std::string& document_id, std::string_view markdown_content) const
    -> std::vector<ExtractedReference>
{
    std::vector<ExtractedReference> result;

    auto block_refs = extract_block_refs(document_id, markdown_content);
    auto doc_links = extract_doc_links(document_id, markdown_content);
    auto embeds = extract_embeds(document_id, markdown_content);
    auto tags = extract_tags(document_id, markdown_content);

    result.insert(result.end(),
                  std::make_move_iterator(block_refs.begin()),
                  std::make_move_iterator(block_refs.end()));
    result.insert(result.end(),
                  std::make_move_iterator(doc_links.begin()),
                  std::make_move_iterator(doc_links.end()));
    result.insert(result.end(),
                  std::make_move_iterator(embeds.begin()),
                  std::make_move_iterator(embeds.end()));
    result.insert(
        result.end(), std::make_move_iterator(tags.begin()), std::make_move_iterator(tags.end()));

    return result;
}

auto ReferenceScanner::extract_block_refs(const std::string& source_id,
                                          std::string_view content) const
    -> std::vector<ExtractedReference>
{
    std::vector<ExtractedReference> refs;
    const std::string content_str(content);

    // Match ((block-id)) syntax — block IDs are alphanumeric + hyphens
    static const std::regex block_ref_pattern(R"(\(\(([a-zA-Z0-9\-]+)\)\))");

    auto iter = std::sregex_iterator(content_str.begin(), content_str.end(), block_ref_pattern);
    const std::sregex_iterator end_iter;

    for (; iter != end_iter; ++iter)
    {
        const auto& match = *iter;
        const auto match_pos = static_cast<size_t>(match.position());

        // Skip references inside code blocks
        if (is_in_code_block(content, match_pos))
        {
            continue;
        }

        // Skip embed references (preceded by '!')
        if (match_pos > 0 && content[match_pos - 1] == '!')
        {
            continue;
        }

        ExtractedReference ref;
        ref.source_block_id = source_id;
        ref.target_id = match[1].str();
        ref.ref_type = GraphRefType::BlockRef;
        ref.line_number = line_number_at(content, match_pos);
        refs.push_back(std::move(ref));
    }

    return refs;
}

auto ReferenceScanner::extract_doc_links(const std::string& source_id,
                                         std::string_view content) const
    -> std::vector<ExtractedReference>
{
    std::vector<ExtractedReference> refs;
    const std::string content_str(content);

    // Match [text](target) — standard Markdown link syntax
    static const std::regex link_pattern(R"(\[([^\]]*)\]\(([^)]+)\))");

    auto iter = std::sregex_iterator(content_str.begin(), content_str.end(), link_pattern);
    const std::sregex_iterator end_iter;

    for (; iter != end_iter; ++iter)
    {
        const auto& match = *iter;
        const auto match_pos = static_cast<size_t>(match.position());

        if (is_in_code_block(content, match_pos))
        {
            continue;
        }

        const auto& target = match[2].str();

        // Skip external URLs (http:// or https://)
        if (target.starts_with("http://") || target.starts_with("https://"))
        {
            continue;
        }

        // Skip mailto links
        if (target.starts_with("mailto:"))
        {
            continue;
        }

        // Accept local links: .md, .sy, or no-extension paths
        ExtractedReference ref;
        ref.source_block_id = source_id;
        ref.target_id = target;
        ref.ref_type = GraphRefType::DocLink;
        ref.alias = match[1].str();
        ref.line_number = line_number_at(content, match_pos);
        refs.push_back(std::move(ref));
    }

    return refs;
}

auto ReferenceScanner::extract_embeds(const std::string& source_id, std::string_view content) const
    -> std::vector<ExtractedReference>
{
    std::vector<ExtractedReference> refs;
    const std::string content_str(content);

    // Match !((block-id)) — embed/transclusion syntax
    static const std::regex embed_pattern(R"(!\(\(([a-zA-Z0-9\-]+)\)\))");

    auto iter = std::sregex_iterator(content_str.begin(), content_str.end(), embed_pattern);
    const std::sregex_iterator end_iter;

    for (; iter != end_iter; ++iter)
    {
        const auto& match = *iter;
        const auto match_pos = static_cast<size_t>(match.position());

        if (is_in_code_block(content, match_pos))
        {
            continue;
        }

        ExtractedReference ref;
        ref.source_block_id = source_id;
        ref.target_id = match[1].str();
        ref.ref_type = GraphRefType::Embed;
        ref.line_number = line_number_at(content, match_pos);
        refs.push_back(std::move(ref));
    }

    return refs;
}

auto ReferenceScanner::extract_tags(const std::string& source_id, std::string_view content) const
    -> std::vector<ExtractedReference>
{
    std::vector<ExtractedReference> refs;
    const std::string content_str(content);

    // Match #tag-name (alphanumeric + hyphens, at least 1 char after #)
    // Negative lookbehind: not preceded by alphanumeric (to avoid #123 in code)
    static const std::regex tag_pattern(R"((?:^|[^a-zA-Z0-9&])#([a-zA-Z][a-zA-Z0-9\-]*))");

    auto iter = std::sregex_iterator(content_str.begin(), content_str.end(), tag_pattern);
    const std::sregex_iterator end_iter;

    for (; iter != end_iter; ++iter)
    {
        const auto& match = *iter;
        const auto match_pos = static_cast<size_t>(match.position());

        if (is_in_code_block(content, match_pos))
        {
            continue;
        }

        // Skip Markdown headings (# followed by space)
        const auto tag_text = match[1].str();

        ExtractedReference ref;
        ref.source_block_id = source_id;
        ref.target_id = tag_text;
        ref.ref_type = GraphRefType::Tag;
        ref.line_number = line_number_at(content, match_pos);
        refs.push_back(std::move(ref));
    }

    return refs;
}

auto ReferenceScanner::line_number_at(std::string_view content, size_t position) -> int
{
    int line = 1;
    for (size_t idx = 0; idx < position && idx < content.size(); ++idx)
    {
        if (content[idx] == '\n')
        {
            ++line;
        }
    }
    return line;
}

auto ReferenceScanner::is_in_code_block(std::string_view content, size_t position) -> bool
{
    // Count fenced code block delimiters (```) before this position
    int fence_count = 0;
    size_t idx = 0;

    while (idx < position && idx < content.size())
    {
        // Check for ``` at line start
        if (content[idx] == '`' && idx + 2 < content.size() && content[idx + 1] == '`' &&
            content[idx + 2] == '`')
        {
            // Verify it's at the start of a line (or start of content)
            if (idx == 0 || content[idx - 1] == '\n')
            {
                ++fence_count;
                idx += 3;
                continue;
            }
        }
        ++idx;
    }

    // Odd fence count means we're inside a code block
    return (fence_count % 2) != 0;
}

} // namespace markamp::core
