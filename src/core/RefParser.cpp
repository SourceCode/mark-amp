#include "core/RefParser.h"

#include <algorithm>
#include <regex>

namespace markamp::core
{

auto RefParser::parse_refs(std::string_view content) const -> std::vector<ParsedRef>
{
    std::vector<ParsedRef> results;

    if (content.empty())
    {
        return results;
    }

    // Pattern: ((YYYYMMDDHHmmss-xxxxxxx "optional text")) or ((YYYYMMDDHHmmss-xxxxxxx))
    static const std::regex ref_pattern(
        R"regex(\(\((\d{14}-[a-z0-9]{7})(?:\s+"([^"]*)")?\)\))regex");

    auto content_str = std::string(content);
    auto begin = std::sregex_iterator(content_str.begin(), content_str.end(), ref_pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        const auto& match = *it;

        ParsedRef ref;
        ref.type = RefType::BlockRef;
        ref.def_block_id = BlockId{match[1].str()};
        ref.source_offset = static_cast<std::size_t>(match.position());
        ref.source_length = static_cast<std::size_t>(match.length());

        if (match[2].matched)
        {
            ref.anchor_text = match[2].str();
        }

        results.push_back(std::move(ref));
    }

    return results;
}

auto RefParser::parse_embeds(std::string_view content) const -> std::vector<ParsedRef>
{
    std::vector<ParsedRef> results;

    if (content.empty())
    {
        return results;
    }

    // Pattern: {{YYYYMMDDHHmmss-xxxxxxx}}
    static const std::regex embed_pattern(R"(\{\{(\d{14}-[a-z0-9]{7})\}\})");

    auto content_str = std::string(content);
    auto begin = std::sregex_iterator(content_str.begin(), content_str.end(), embed_pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        const auto& match = *it;

        ParsedRef ref;
        ref.type = RefType::BlockEmbed;
        ref.def_block_id = BlockId{match[1].str()};
        ref.source_offset = static_cast<std::size_t>(match.position());
        ref.source_length = static_cast<std::size_t>(match.length());

        results.push_back(std::move(ref));
    }

    return results;
}

auto RefParser::parse_file_annotation_refs(std::string_view content) const -> std::vector<ParsedRef>
{
    std::vector<ParsedRef> results;

    if (content.empty())
    {
        return results;
    }

    // Pattern: <<path/annotation-id "text">> or <<path/annotation-id>>
    static const std::regex annot_pattern(
        R"regex(<<([^>]+?)/([a-z0-9-]+)(?:\s+"([^"]*)")?\s*>>)regex");

    auto content_str = std::string(content);
    auto begin = std::sregex_iterator(content_str.begin(), content_str.end(), annot_pattern);
    auto end = std::sregex_iterator();

    for (auto it = begin; it != end; ++it)
    {
        const auto& match = *it;

        ParsedRef ref;
        ref.type = RefType::FileAnnotationRef;
        ref.def_block_id = BlockId{match[2].str()};
        ref.asset_path = match[1].str();
        ref.source_offset = static_cast<std::size_t>(match.position());
        ref.source_length = static_cast<std::size_t>(match.length());

        if (match[3].matched)
        {
            ref.anchor_text = match[3].str();
        }

        results.push_back(std::move(ref));
    }

    return results;
}

auto RefParser::parse_all(std::string_view content) const -> std::vector<ParsedRef>
{
    auto refs = parse_refs(content);
    auto embeds = parse_embeds(content);
    auto annotations = parse_file_annotation_refs(content);

    // Merge all
    refs.insert(
        refs.end(), std::make_move_iterator(embeds.begin()), std::make_move_iterator(embeds.end()));
    refs.insert(refs.end(),
                std::make_move_iterator(annotations.begin()),
                std::make_move_iterator(annotations.end()));

    // Sort by offset
    std::sort(refs.begin(),
              refs.end(),
              [](const ParsedRef& a, const ParsedRef& b)
              { return a.source_offset < b.source_offset; });

    return refs;
}

auto RefParser::detect_virtual_refs(
    std::string_view content, const std::vector<std::pair<BlockId, std::string>>& known_names) const
    -> std::vector<ParsedRef>
{
    std::vector<ParsedRef> results;

    if (content.empty() || known_names.empty())
    {
        return results;
    }

    auto content_str = std::string(content);
    auto content_lower = content_str;
    std::transform(content_lower.begin(),
                   content_lower.end(),
                   content_lower.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

    for (const auto& [block_id, name] : known_names)
    {
        if (name.empty())
        {
            continue;
        }

        auto name_lower = name;
        std::transform(name_lower.begin(),
                       name_lower.end(),
                       name_lower.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

        std::size_t pos = 0;
        while ((pos = content_lower.find(name_lower, pos)) != std::string::npos)
        {
            ParsedRef ref;
            ref.type = RefType::VirtualRef;
            ref.def_block_id = block_id;
            ref.source_offset = pos;
            ref.source_length = name.size();
            ref.anchor_text = content_str.substr(pos, name.size());

            results.push_back(std::move(ref));
            pos += name.size();
        }
    }

    // Sort by offset
    std::sort(results.begin(),
              results.end(),
              [](const ParsedRef& a, const ParsedRef& b)
              { return a.source_offset < b.source_offset; });

    return results;
}

auto RefParser::validate_block_id(std::string_view candidate) const -> bool
{
    return BlockId{std::string(candidate)}.is_valid();
}

} // namespace markamp::core
