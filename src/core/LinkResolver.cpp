/// @file LinkResolver.cpp
/// @brief V13 Phase 32 Task 6 — Link resolution implementation.

#include "core/LinkResolver.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>

namespace markamp::core
{

namespace fs = std::filesystem;

LinkResolver::LinkResolver(std::string workspace_root)
    : workspace_root_(std::move(workspace_root))
{
}

auto LinkResolver::resolve_wiki_link(const std::string& link) const -> ResolvedLink
{
    ResolvedLink result;

    if (link.empty())
    {
        result.error = "Empty link";
        return result;
    }

    // Split on # for anchor
    std::string file_part = link;
    auto hash_pos = link.find('#');
    if (hash_pos != std::string::npos)
    {
        file_part = link.substr(0, hash_pos);
        result.anchor = link.substr(hash_pos + 1);
    }

    // Find the file in workspace
    std::string found_path = find_file(file_part);
    if (found_path.empty())
    {
        result.error = "Cannot find file for link: " + link;
        return result;
    }

    result.document_path = found_path;
    result.success = true;
    return result;
}

auto LinkResolver::resolve_markdown_link(const std::string& url,
                                         const std::string& source_file) const -> ResolvedLink
{
    ResolvedLink result;

    if (url.empty())
    {
        result.error = "Empty URL";
        return result;
    }

    // Skip external links
    if (url.substr(0, 4) == "http" || url.substr(0, 6) == "mailto")
    {
        result.error = "External link: " + url;
        return result;
    }

    // Check for anchor in URL
    std::string path_part = url;
    auto hash_pos = url.find('#');
    if (hash_pos != std::string::npos)
    {
        path_part = url.substr(0, hash_pos);
        result.anchor = url.substr(hash_pos + 1);
    }

    // Resolve relative to source file
    try
    {
        fs::path source_dir = fs::path(source_file).parent_path();
        fs::path resolved = source_dir / path_part;
        resolved = fs::weakly_canonical(resolved);

        if (fs::exists(resolved))
        {
            result.document_path = resolved.string();
            result.success = true;
        }
        else
        {
            result.error = "File not found: " + resolved.string();
        }
    }
    catch (const fs::filesystem_error& err)
    {
        result.error = std::string("Path resolution error: ") + err.what();
    }

    return result;
}

auto LinkResolver::resolve_block_ref(const std::string& block_id) const -> ResolvedLink
{
    ResolvedLink result;

    if (block_id.empty())
    {
        result.error = "Empty block reference";
        return result;
    }

    // Search workspace files for the block ID marker
    const std::string block_marker = "^" + block_id;

    for (const auto& file_path : workspace_files_)
    {
        // Simple scan: in a real implementation, this would use an index
        std::ifstream file(file_path);
        if (!file.is_open())
        {
            continue;
        }

        std::string file_line;
        int line_num = 0;
        while (std::getline(file, file_line))
        {
            ++line_num;
            if (file_line.find(block_marker) != std::string::npos)
            {
                result.document_path = file_path;
                result.line = line_num;
                result.success = true;
                return result;
            }
        }
    }

    result.error = "Block reference not found: " + block_id;
    return result;
}

auto LinkResolver::parse_link_token(const std::string& token) -> std::pair<std::string, LinkType>
{
    if (token.size() >= 4 && token.substr(0, 2) == "[[" && token.substr(token.size() - 2) == "]]")
    {
        return {token.substr(2, token.size() - 4), LinkType::kWikiLink};
    }

    if (token.size() >= 4 && token.substr(0, 2) == "((" && token.substr(token.size() - 2) == "))")
    {
        return {token.substr(2, token.size() - 4), LinkType::kBlockRef};
    }

    // Check for [text](url) — extract the url part
    auto paren_start = token.find("](");
    if (paren_start != std::string::npos && !token.empty() && token[0] == '[')
    {
        auto paren_end = token.find(')', paren_start + 2);
        if (paren_end != std::string::npos)
        {
            return {token.substr(paren_start + 2, paren_end - paren_start - 2),
                    LinkType::kMarkdownLink};
        }
    }

    return {token, LinkType::kUnknown};
}

auto LinkResolver::workspace_root() const -> const std::string&
{
    return workspace_root_;
}

void LinkResolver::set_workspace_files(std::vector<std::string> files)
{
    workspace_files_ = std::move(files);
}

auto LinkResolver::workspace_file_count() const -> size_t
{
    return workspace_files_.size();
}

auto LinkResolver::find_file(const std::string& name) const -> std::string
{
    if (name.empty())
    {
        return {};
    }

    // Normalize: try with and without .md extension
    std::vector<std::string> candidates;
    candidates.push_back(name);
    if (name.size() < 3 || name.substr(name.size() - 3) != ".md")
    {
        candidates.push_back(name + ".md");
    }

    for (const auto& file_path : workspace_files_)
    {
        fs::path file_p(file_path);
        std::string filename = file_p.filename().string();
        std::string stem = file_p.stem().string();

        for (const auto& candidate : candidates)
        {
            // Case-insensitive comparison
            auto lower_fn = filename;
            auto lower_cand = candidate;
            std::transform(lower_fn.begin(),
                           lower_fn.end(),
                           lower_fn.begin(),
                           [](unsigned char chr) { return std::tolower(chr); });
            std::transform(lower_cand.begin(),
                           lower_cand.end(),
                           lower_cand.begin(),
                           [](unsigned char chr) { return std::tolower(chr); });

            if (lower_fn == lower_cand || stem == candidate)
            {
                return file_path;
            }
        }
    }

    // Try as an absolute/relative path
    fs::path abs_path = fs::path(workspace_root_) / name;
    if (fs::exists(abs_path))
    {
        return abs_path.string();
    }

    abs_path = fs::path(workspace_root_) / (name + ".md");
    if (fs::exists(abs_path))
    {
        return abs_path.string();
    }

    return {};
}

} // namespace markamp::core
