#pragma once

#include "core/BlockRef.h"

#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace markamp::core
{

/// Parses block reference syntax from markdown content.
/// All methods are const — no mutable state.
class RefParser
{
public:
    /// Parse block references: ((id "text")) or ((id))
    [[nodiscard]] auto parse_refs(std::string_view content) const -> std::vector<ParsedRef>;

    /// Parse embed/transclusion syntax: {{id}}
    [[nodiscard]] auto parse_embeds(std::string_view content) const -> std::vector<ParsedRef>;

    /// Parse file annotation references: <<path/annotation-id "text">>
    [[nodiscard]] auto parse_file_annotation_refs(std::string_view content) const
        -> std::vector<ParsedRef>;

    /// Convenience: parse all reference types combined, sorted by offset.
    [[nodiscard]] auto parse_all(std::string_view content) const -> std::vector<ParsedRef>;

    /// Detect virtual references (name/alias mentions) in content.
    [[nodiscard]] auto
    detect_virtual_refs(std::string_view content,
                        const std::vector<std::pair<BlockId, std::string>>& known_names) const
        -> std::vector<ParsedRef>;

    /// Validate that a string matches the block ID format.
    [[nodiscard]] auto validate_block_id(std::string_view candidate) const -> bool;
};

} // namespace markamp::core
