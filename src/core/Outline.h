#pragma once

#include "BlockRef.h"
#include "Types.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

/// A node in the hierarchical document outline tree.
struct OutlineNode
{
    /// Block ID of the heading (if block IDs are assigned).
    std::optional<BlockId> block_id;

    /// Plain text content of the heading (stripped of formatting).
    std::string text;

    /// Heading level (1-6).
    int level{1};

    /// Line number in the source document (0-based).
    int source_line{0};

    /// Character offset in the source document.
    std::size_t source_offset{0};

    /// Nesting depth in the outline tree (0 = top-level).
    int depth{0};

    /// Number of sub-headings directly under this heading.
    [[nodiscard]] auto sub_heading_count() const -> std::size_t
    {
        return children.size();
    }

    /// Total number of sub-headings recursively.
    [[nodiscard]] auto total_sub_heading_count() const -> std::size_t;

    /// Child headings in document order.
    std::vector<OutlineNode> children;
};

/// A flat (non-hierarchical) outline entry for list-style display.
struct OutlineEntry
{
    /// Block ID of the heading (if assigned).
    std::optional<BlockId> block_id;

    /// Plain text content of the heading.
    std::string text;

    /// Heading level (1-6).
    int level{1};

    /// Nesting depth for indentation (derived from hierarchy).
    int depth{0};

    /// Indentation string for display (e.g., "  " per depth level).
    std::string indent;

    /// Line number in the source document (0-based).
    int source_line{0};

    /// Character offset in the source document.
    std::size_t source_offset{0};

    /// Sequential index in the flat list.
    int index{0};
};

/// Represents the complete outline state for a document.
struct DocumentOutline
{
    /// Root document identifier.
    std::string root_id;

    /// Hierarchical outline tree.
    std::vector<OutlineNode> roots;

    /// Total number of headings across all levels.
    int total_heading_count{0};

    /// Heading counts by level (index 0 = h1, index 5 = h6).
    std::array<int, 6> level_counts{};

    /// Whether the outline is empty (no headings found).
    [[nodiscard]] auto is_empty() const -> bool
    {
        return total_heading_count == 0;
    }
};

/// Builds an outline tree from a MarkdownDocument AST.
class OutlineBuilder
{
public:
    /// Build a hierarchical outline from the document AST.
    [[nodiscard]] auto build_from_ast(const MarkdownDocument& doc) -> DocumentOutline;

    /// Build a hierarchical outline from a flat list of heading entries.
    /// Useful when headings are extracted from a block database rather than AST.
    [[nodiscard]] auto build_from_headings(const std::vector<std::pair<std::string, int>>& headings)
        -> DocumentOutline;

private:
    /// Insert a heading into the tree at the correct hierarchy position.
    void
    insert_heading(std::vector<OutlineNode>& roots, const OutlineNode& heading, int current_depth);

    /// Extract plain text from an MdNode heading (strips inline formatting).
    [[nodiscard]] auto extract_heading_text(const MdNode& heading_node) const -> std::string;
};

} // namespace markamp::core
