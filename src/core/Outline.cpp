#include "Outline.h"

#include <stack>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════
// OutlineNode
// ════════════════════════════════════════════════════════════

auto OutlineNode::total_sub_heading_count() const -> std::size_t
{
    std::size_t count = children.size();
    for (const auto& child : children)
    {
        count += child.total_sub_heading_count();
    }
    return count;
}

// ════════════════════════════════════════════════════════════
// OutlineBuilder
// ════════════════════════════════════════════════════════════

auto OutlineBuilder::extract_heading_text(const MdNode& heading_node) const -> std::string
{
    // Recursively collect plain text from all child nodes of the heading.
    std::string result;
    for (const auto& child : heading_node.children)
    {
        if (child.type == MdNodeType::Text)
        {
            result += child.text_content;
        }
        else
        {
            // Recurse into inline formatting nodes (emphasis, strong, code, etc.)
            for (const auto& grandchild : child.children)
            {
                if (grandchild.type == MdNodeType::Text)
                {
                    result += grandchild.text_content;
                }
            }
        }
    }
    return result;
}

void OutlineBuilder::insert_heading(std::vector<OutlineNode>& roots,
                                    const OutlineNode& heading,
                                    int current_depth)
{
    if (roots.empty() || heading.level <= roots.back().level)
    {
        // Same or higher level: insert as sibling at current level
        auto node_copy = heading;
        node_copy.depth = current_depth;
        roots.push_back(std::move(node_copy));
    }
    else
    {
        // Lower level heading: insert as child of the last node
        insert_heading(roots.back().children, heading, current_depth + 1);
    }
}

auto OutlineBuilder::build_from_ast(const MarkdownDocument& doc) -> DocumentOutline
{
    DocumentOutline outline;
    int line_counter = 0;

    // Walk the AST root's children looking for heading nodes.
    for (const auto& node : doc.root.children)
    {
        if (node.type == MdNodeType::Heading && node.heading_level >= 1 && node.heading_level <= 6)
        {
            OutlineNode heading;
            heading.text = extract_heading_text(node);
            heading.level = node.heading_level;
            heading.source_line = line_counter;

            insert_heading(outline.roots, heading, 0);

            outline.total_heading_count++;
            outline.level_counts[static_cast<std::size_t>(node.heading_level - 1)]++;
        }
        line_counter++;
    }

    return outline;
}

auto OutlineBuilder::build_from_headings(const std::vector<std::pair<std::string, int>>& headings)
    -> DocumentOutline
{
    DocumentOutline outline;
    int line_index = 0;

    for (const auto& [text, level] : headings)
    {
        if (level < 1 || level > 6)
        {
            line_index++;
            continue;
        }

        OutlineNode heading;
        heading.text = text;
        heading.level = level;
        heading.source_line = line_index;

        insert_heading(outline.roots, heading, 0);

        outline.total_heading_count++;
        outline.level_counts[static_cast<std::size_t>(level - 1)]++;
        line_index++;
    }

    return outline;
}

} // namespace markamp::core
