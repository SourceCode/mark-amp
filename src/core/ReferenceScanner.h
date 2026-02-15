#pragma once

#include "GraphTypes.h"

#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Scans Markdown content for block references, document links, embeds, and tags.
/// Extracts ExtractedReference objects for graph construction.
class ReferenceScanner
{
public:
    /// Scan a Markdown document for all references.
    [[nodiscard]] auto scan(const std::string& document_id, std::string_view markdown_content) const
        -> std::vector<ExtractedReference>;

    /// Extract block references: ((block-id)) syntax.
    [[nodiscard]] auto extract_block_refs(const std::string& source_id,
                                          std::string_view content) const
        -> std::vector<ExtractedReference>;

    /// Extract standard Markdown links: [text](target.md) where target is local.
    [[nodiscard]] auto extract_doc_links(const std::string& source_id,
                                         std::string_view content) const
        -> std::vector<ExtractedReference>;

    /// Extract embed references: !((block-id)) transclusion syntax.
    [[nodiscard]] auto extract_embeds(const std::string& source_id, std::string_view content) const
        -> std::vector<ExtractedReference>;

    /// Extract tag references: #tag or #multi-word-tag syntax.
    [[nodiscard]] auto extract_tags(const std::string& source_id, std::string_view content) const
        -> std::vector<ExtractedReference>;

private:
    /// Count newlines before a position to determine line number.
    [[nodiscard]] static auto line_number_at(std::string_view content, size_t position) -> int;

    /// Check if a position is inside a fenced code block.
    [[nodiscard]] static auto is_in_code_block(std::string_view content, size_t position) -> bool;
};

} // namespace markamp::core
