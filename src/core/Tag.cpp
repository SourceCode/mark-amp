#include "Tag.h"

#include <algorithm>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════
// ParsedTag
// ════════════════════════════════════════════════════════════

auto ParsedTag::components() const -> std::vector<std::string>
{
    return TagParser::split_path(full_path);
}

// ════════════════════════════════════════════════════════════
// TagParser
// ════════════════════════════════════════════════════════════

auto TagParser::parse_tags(std::string_view content) const -> std::vector<ParsedTag>
{
    std::vector<ParsedTag> results;
    // Match #tag-name# but NOT # Heading (space after #).
    // Pattern: non-start-of-line # followed by valid chars followed by #.
    static const std::regex tag_regex(R"((?:^|[^\\])#([a-zA-Z0-9_/\-]+)#)", std::regex::optimize);

    std::string content_str(content);
    auto begin = std::sregex_iterator(content_str.begin(), content_str.end(), tag_regex);
    auto end = std::sregex_iterator();

    for (auto iter = begin; iter != end; ++iter)
    {
        const auto& match = *iter;
        ParsedTag tag;
        tag.full_path = match[1].str();

        // Calculate offset of the actual # delimiter
        auto match_pos = static_cast<std::size_t>(match.position());
        // If the match starts with a non-# character (boundary), adjust offset
        if (match[0].str()[0] != '#')
        {
            match_pos++;
        }
        tag.source_offset = match_pos;
        tag.source_length = tag.full_path.size() + 2; // +2 for the # delimiters

        results.push_back(std::move(tag));
    }

    return results;
}

auto TagParser::validate_tag(std::string_view tag_path) const -> bool
{
    if (tag_path.empty())
    {
        return false;
    }

    // Max length check
    if (tag_path.size() > 256)
    {
        return false;
    }

    // No leading or trailing slashes
    if (tag_path.front() == '/' || tag_path.back() == '/')
    {
        return false;
    }

    // No empty components (consecutive slashes)
    if (tag_path.find("//") != std::string_view::npos)
    {
        return false;
    }

    // Valid characters only: alphanumeric, underscore, hyphen, slash
    static const std::regex valid_chars(R"(^[a-zA-Z0-9_/\-]+$)");
    std::string path_str(tag_path);
    return std::regex_match(path_str, valid_chars);
}

auto TagParser::split_path(std::string_view tag_path) -> std::vector<std::string>
{
    std::vector<std::string> parts;
    std::string current;
    for (char ch : tag_path)
    {
        if (ch == '/')
        {
            if (!current.empty())
            {
                parts.push_back(std::move(current));
                current.clear();
            }
        }
        else
        {
            current += ch;
        }
    }
    if (!current.empty())
    {
        parts.push_back(std::move(current));
    }
    return parts;
}

auto TagParser::join_path(const std::vector<std::string>& components) -> std::string
{
    std::string result;
    for (std::size_t idx = 0; idx < components.size(); ++idx)
    {
        if (idx > 0)
        {
            result += '/';
        }
        result += components[idx];
    }
    return result;
}

// ════════════════════════════════════════════════════════════
// TagIndex
// ════════════════════════════════════════════════════════════

void TagIndex::add_tag(const BlockId& block_id, const std::string& tag_path)
{
    tag_to_blocks_[tag_path].insert(block_id);
    block_to_tags_[block_id].insert(tag_path);
}

void TagIndex::remove_tag(const BlockId& block_id, const std::string& tag_path)
{
    auto tag_iter = tag_to_blocks_.find(tag_path);
    if (tag_iter != tag_to_blocks_.end())
    {
        tag_iter->second.erase(block_id);
        if (tag_iter->second.empty())
        {
            tag_to_blocks_.erase(tag_iter);
        }
    }

    auto block_iter = block_to_tags_.find(block_id);
    if (block_iter != block_to_tags_.end())
    {
        block_iter->second.erase(tag_path);
        if (block_iter->second.empty())
        {
            block_to_tags_.erase(block_iter);
        }
    }
}

void TagIndex::remove_block(const BlockId& block_id)
{
    auto block_iter = block_to_tags_.find(block_id);
    if (block_iter == block_to_tags_.end())
    {
        return;
    }

    // Remove this block from all tag sets
    for (const auto& tag_path : block_iter->second)
    {
        auto tag_iter = tag_to_blocks_.find(tag_path);
        if (tag_iter != tag_to_blocks_.end())
        {
            tag_iter->second.erase(block_id);
            if (tag_iter->second.empty())
            {
                tag_to_blocks_.erase(tag_iter);
            }
        }
    }
    block_to_tags_.erase(block_iter);
}

auto TagIndex::get_blocks_with_tag(const std::string& tag_path) const -> std::vector<BlockId>
{
    auto iter = tag_to_blocks_.find(tag_path);
    if (iter == tag_to_blocks_.end())
    {
        return {};
    }
    return {iter->second.begin(), iter->second.end()};
}

auto TagIndex::get_blocks_with_tag_recursive(const std::string& tag_path) const
    -> std::vector<BlockId>
{
    std::unordered_set<BlockId, BlockIdHash> result_set;

    // Collect direct matches
    auto direct = tag_to_blocks_.find(tag_path);
    if (direct != tag_to_blocks_.end())
    {
        result_set.insert(direct->second.begin(), direct->second.end());
    }

    // Collect descendant matches (tags starting with tag_path + "/")
    std::string prefix = tag_path + "/";
    for (const auto& [path, blocks] : tag_to_blocks_)
    {
        if (path.size() > prefix.size() && path.substr(0, prefix.size()) == prefix)
        {
            result_set.insert(blocks.begin(), blocks.end());
        }
    }

    return {result_set.begin(), result_set.end()};
}

auto TagIndex::get_tags_for_block(const BlockId& block_id) const -> std::vector<std::string>
{
    auto iter = block_to_tags_.find(block_id);
    if (iter == block_to_tags_.end())
    {
        return {};
    }
    std::vector<std::string> result(iter->second.begin(), iter->second.end());
    std::sort(result.begin(), result.end());
    return result;
}

auto TagIndex::tag_count(const std::string& tag_path) const -> int
{
    auto iter = tag_to_blocks_.find(tag_path);
    if (iter == tag_to_blocks_.end())
    {
        return 0;
    }
    return static_cast<int>(iter->second.size());
}

auto TagIndex::build_tree() const -> TagTree
{
    TagTree tree;
    auto paths = all_tag_paths();
    tree.roots = build_subtree("", paths);
    tree.unique_tag_count = static_cast<int>(paths.size());

    for (const auto& [path, blocks] : tag_to_blocks_)
    {
        tree.total_usage_count += static_cast<int>(blocks.size());
    }
    return tree;
}

auto TagIndex::search_tags(const std::string& query) const -> std::vector<TagInfo>
{
    std::vector<TagInfo> results;
    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    for (const auto& [path, blocks] : tag_to_blocks_)
    {
        std::string lower_path = path;
        std::transform(lower_path.begin(),
                       lower_path.end(),
                       lower_path.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

        if (lower_path.find(lower_query) == 0 || lower_path.find(lower_query) != std::string::npos)
        {
            TagInfo info;
            auto parts = TagParser::split_path(path);
            info.label = parts.empty() ? path : parts.back();
            info.full_path = path;
            info.direct_count = static_cast<int>(blocks.size());
            info.total_count = info.direct_count;
            results.push_back(std::move(info));
        }
    }
    return results;
}

auto TagIndex::all_tag_paths() const -> std::vector<std::string>
{
    std::vector<std::string> paths;
    paths.reserve(tag_to_blocks_.size());
    for (const auto& [path, blocks] : tag_to_blocks_)
    {
        paths.push_back(path);
    }
    std::sort(paths.begin(), paths.end());
    return paths;
}

void TagIndex::clear()
{
    tag_to_blocks_.clear();
    block_to_tags_.clear();
}

auto TagIndex::build_subtree(const std::string& prefix,
                             const std::vector<std::string>& all_paths) const
    -> std::vector<TagInfo>
{
    // Group paths by their first component after the prefix.
    std::unordered_map<std::string, std::vector<std::string>> groups;

    for (const auto& path : all_paths)
    {
        std::string remainder;
        if (prefix.empty())
        {
            remainder = path;
        }
        else if (path.size() > prefix.size() + 1 && path.substr(0, prefix.size()) == prefix &&
                 path[prefix.size()] == '/')
        {
            remainder = path.substr(prefix.size() + 1);
        }
        else if (path == prefix)
        {
            // Exact match, skip (will be counted in parent)
            continue;
        }
        else
        {
            continue;
        }

        // Get first component of remainder
        auto slash_pos = remainder.find('/');
        std::string first_component =
            (slash_pos != std::string::npos) ? remainder.substr(0, slash_pos) : remainder;

        groups[first_component].push_back(path);
    }

    std::vector<TagInfo> nodes;
    for (auto& [component, child_paths] : groups)
    {
        TagInfo info;
        info.label = component;
        info.full_path = prefix.empty() ? component : prefix + "/" + component;

        // Direct count
        auto direct_iter = tag_to_blocks_.find(info.full_path);
        if (direct_iter != tag_to_blocks_.end())
        {
            info.direct_count = static_cast<int>(direct_iter->second.size());
        }

        // Recurse for children
        info.children = build_subtree(info.full_path, all_paths);

        // Total count = direct + children total
        info.total_count = info.direct_count;
        for (const auto& child : info.children)
        {
            info.total_count += child.total_count;
        }

        nodes.push_back(std::move(info));
    }

    // Sort alphabetically by label
    std::sort(nodes.begin(),
              nodes.end(),
              [](const TagInfo& tag_a, const TagInfo& tag_b) { return tag_a.label < tag_b.label; });

    return nodes;
}

} // namespace markamp::core
