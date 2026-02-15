#include "core/DocumentModel.h"

#include "core/WikiLinkParser.h"

#include <algorithm>

namespace markamp::core
{

DocumentModel::DocumentModel(const std::string& id, const std::string& notebook_id)
    : id_(id)
    , notebook_id_(notebook_id)
{
}

// ============================================================================
// Identity
// ============================================================================

auto DocumentModel::id() const -> const std::string&
{
    return id_;
}

auto DocumentModel::notebook_id() const -> const std::string&
{
    return notebook_id_;
}

auto DocumentModel::title() const -> const std::string&
{
    return title_;
}

auto DocumentModel::file_path() const -> const std::filesystem::path&
{
    return file_path_;
}

auto DocumentModel::set_title(const std::string& title) -> void
{
    title_ = title;
    dirty_ = true;
}

auto DocumentModel::set_file_path(const std::filesystem::path& path) -> void
{
    file_path_ = path;
}

// ============================================================================
// Content
// ============================================================================

auto DocumentModel::markdown() const -> const std::string&
{
    return markdown_;
}

auto DocumentModel::set_markdown(const std::string& content) -> void
{
    markdown_ = content;
    dirty_ = true;
}

// ============================================================================
// Reparse
// ============================================================================

auto DocumentModel::reparse() -> void
{
    WikiLinkParser parser;

    // Parse frontmatter
    auto fm = parser.parse_frontmatter(markdown_);
    if (fm.has_value())
    {
        frontmatter_ = std::move(fm.value());
    }
    else
    {
        frontmatter_ = Frontmatter{};
    }

    // Update title from frontmatter if available
    if (!frontmatter_.title.empty())
    {
        title_ = frontmatter_.title;
    }

    // Parse outgoing links
    outgoing_links_ = parser.parse_links(markdown_);

    // Parse tags from content
    tags_ = parser.parse_tags(markdown_);

    // Also add frontmatter tags (if not already present)
    for (const auto& fm_tag : frontmatter_.tags)
    {
        bool already_present = false;
        for (const auto& existing_tag : tags_)
        {
            if (existing_tag.full_tag == fm_tag)
            {
                already_present = true;
                break;
            }
        }
        if (!already_present)
        {
            ParsedTag tag;
            tag.full_tag = fm_tag;
            tag.from_frontmatter = true;

            // Split on '/'
            std::string part;
            for (char ch : fm_tag)
            {
                if (ch == '/')
                {
                    if (!part.empty())
                    {
                        tag.parts.push_back(part);
                        part.clear();
                    }
                }
                else
                {
                    part += ch;
                }
            }
            if (!part.empty())
            {
                tag.parts.push_back(part);
            }

            tags_.push_back(std::move(tag));
        }
    }

    dirty_ = false;
}

// ============================================================================
// Frontmatter
// ============================================================================

auto DocumentModel::frontmatter() const -> const Frontmatter&
{
    return frontmatter_;
}

auto DocumentModel::set_frontmatter(const Frontmatter& fm) -> void
{
    frontmatter_ = fm;
    dirty_ = true;
}

// ============================================================================
// Outgoing Links
// ============================================================================

auto DocumentModel::outgoing_links() const -> const std::vector<WikiLink>&
{
    return outgoing_links_;
}

auto DocumentModel::outgoing_embeds() const -> std::vector<WikiLink>
{
    std::vector<WikiLink> embeds;
    for (const auto& link : outgoing_links_)
    {
        if (link.is_embed)
        {
            embeds.push_back(link);
        }
    }
    return embeds;
}

// ============================================================================
// Tags
// ============================================================================

auto DocumentModel::tags() const -> const std::vector<ParsedTag>&
{
    return tags_;
}

auto DocumentModel::all_tag_strings() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(tags_.size());
    for (const auto& tag : tags_)
    {
        result.push_back(tag.full_tag);
    }
    return result;
}

// ============================================================================
// Block Tree
// ============================================================================

auto DocumentModel::root_block() const -> const std::shared_ptr<Block>&
{
    return root_block_;
}

auto DocumentModel::set_root_block(std::shared_ptr<Block> root) -> void
{
    root_block_ = std::move(root);
}

// ============================================================================
// Aliases
// ============================================================================

auto DocumentModel::aliases() const -> const std::vector<std::string>&
{
    return frontmatter_.aliases;
}

// ============================================================================
// Dirty tracking
// ============================================================================

auto DocumentModel::is_dirty() const -> bool
{
    return dirty_;
}

auto DocumentModel::mark_dirty() -> void
{
    dirty_ = true;
}

auto DocumentModel::mark_clean() -> void
{
    dirty_ = false;
}

} // namespace markamp::core
