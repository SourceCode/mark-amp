#pragma once

#include "WikiLink.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Stateless parser for Obsidian-style wikilinks, tags, and YAML frontmatter.
/// No EventBus dependency — pure parsing utility.
class WikiLinkParser
{
public:
    /// Parse all [[wikilinks]] and ![[embeds]] from markdown content.
    /// Skips links inside fenced code blocks and inline code spans.
    [[nodiscard]] auto parse_links(std::string_view content) const -> std::vector<WikiLink>;

    /// Parse all #tags (including nested #parent/child) from markdown content.
    /// Skips tags inside code blocks, code spans, and URLs.
    [[nodiscard]] auto parse_tags(std::string_view content) const -> std::vector<ParsedTag>;

    /// Parse YAML frontmatter from the beginning of a markdown string.
    /// Returns nullopt if no valid frontmatter found.
    [[nodiscard]] auto parse_frontmatter(std::string_view content) const
        -> std::optional<Frontmatter>;

    /// Resolve a wikilink target to a file path within the vault.
    /// Handles shortest-path matching (Obsidian style).
    [[nodiscard]] auto resolve_link(const WikiLink& link,
                                    const std::vector<std::string>& vault_paths) const
        -> std::optional<std::string>;

private:
    /// Check if a position is inside a fenced code block.
    [[nodiscard]] static auto
    is_in_code_block(std::string_view content,
                     std::size_t pos,
                     const std::vector<std::pair<std::size_t, std::size_t>>& code_ranges) -> bool;

    /// Find all fenced code block and inline code ranges.
    [[nodiscard]] static auto find_code_ranges(std::string_view content)
        -> std::vector<std::pair<std::size_t, std::size_t>>;
};

} // namespace markamp::core
