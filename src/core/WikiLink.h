#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

/// Represents a parsed [[wikilink]] or ![[embed]] from Markdown content.
struct WikiLink
{
    std::string target;       // The target note name or path (e.g., "My Note")
    std::string display_text; // Optional display text after | (e.g., "alias")
    std::string heading;      // Optional heading anchor after # (e.g., "Section")
    std::string block_ref;    // Optional block reference after ^ (e.g., "block-id")
    bool is_embed{false};     // True if ![[embed]] syntax
    int source_line{0};       // Line number in source document
    int source_col{0};        // Column offset in source line

    /// Full canonical target string (target#heading^block_ref)
    [[nodiscard]] auto canonical_target() const -> std::string;

    /// True if this link targets a heading section
    [[nodiscard]] auto has_heading() const -> bool;

    /// True if this link targets a specific block
    [[nodiscard]] auto has_block_ref() const -> bool;
};

/// Represents a #tag extracted from document content.
struct ParsedTag
{
    std::string full_tag;           // Full tag text (e.g., "project/alpha")
    std::vector<std::string> parts; // Hierarchy parts (e.g., ["project", "alpha"])
    int source_line{0};
    int source_col{0};
    bool from_frontmatter{false}; // True if tag came from YAML frontmatter

    /// The top-level tag (first part of hierarchy)
    [[nodiscard]] auto root_tag() const -> std::string;

    /// Nesting depth (number of / separators + 1)
    [[nodiscard]] auto depth() const -> int;
};

/// Parsed YAML frontmatter from a document.
struct Frontmatter
{
    std::unordered_map<std::string, std::string> string_fields;
    std::vector<std::string> tags;    // tags: [tag1, tag2]
    std::vector<std::string> aliases; // aliases: [name1, name2]
    std::string title;                // title field or empty
    std::string date;                 // date field or empty
    std::string css_class;            // cssclass field or empty
    bool publish{false};              // publish: true/false

    [[nodiscard]] auto has_field(const std::string& key) const -> bool;
    [[nodiscard]] auto get_field(const std::string& key) const -> std::optional<std::string>;
    auto set_field(const std::string& key, const std::string& value) -> void;

    /// Serialize back to YAML string (for frontmatter round-trip)
    [[nodiscard]] auto to_yaml() const -> std::string;
};

} // namespace markamp::core
