#include "Types.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ═══════════════════════════════════════════════════════
// MdNode helpers
// ═══════════════════════════════════════════════════════

auto MdNode::is_block() const -> bool
{
    switch (type)
    {
        case MdNodeType::Document:
        case MdNodeType::Paragraph:
        case MdNodeType::Heading:
        case MdNodeType::BlockQuote:
        case MdNodeType::UnorderedList:
        case MdNodeType::OrderedList:
        case MdNodeType::ListItem:
        case MdNodeType::CodeBlock:
        case MdNodeType::FencedCodeBlock:
        case MdNodeType::HorizontalRule:
        case MdNodeType::Table:
        case MdNodeType::TableHead:
        case MdNodeType::TableBody:
        case MdNodeType::TableRow:
        case MdNodeType::TableCell:
        case MdNodeType::HtmlBlock:
        case MdNodeType::MermaidBlock:
            return true;
        default:
            return false;
    }
}

auto MdNode::is_inline() const -> bool
{
    return !is_block();
}

auto MdNode::plain_text() const -> std::string
{
    if (type == MdNodeType::Text || type == MdNodeType::Code)
    {
        return text_content;
    }

    std::string result;
    for (const auto& child : children)
    {
        result += child.plain_text();
    }
    return result;
}

auto MdNode::find_all(MdNodeType target_type) const -> std::vector<const MdNode*>
{
    std::vector<const MdNode*> results;
    if (type == target_type)
    {
        results.push_back(this);
    }
    for (const auto& child : children)
    {
        auto child_results = child.find_all(target_type);
        results.insert(results.end(), child_results.begin(), child_results.end());
    }
    return results;
}

// ═══════════════════════════════════════════════════════
// MarkdownDocument helpers
// ═══════════════════════════════════════════════════════

auto MarkdownDocument::heading_count() const -> size_t
{
    return root.find_all(MdNodeType::Heading).size();
}

auto MarkdownDocument::word_count() const -> size_t
{
    auto text = root.plain_text();
    if (text.empty())
    {
        return 0;
    }

    std::istringstream stream(text);
    size_t count = 0;
    std::string word;
    while (stream >> word)
    {
        ++count;
    }
    return count;
}

auto MarkdownDocument::has_mermaid() const -> bool
{
    return !mermaid_blocks.empty();
}

auto MarkdownDocument::has_tables() const -> bool
{
    return !root.find_all(MdNodeType::Table).empty();
}

auto MarkdownDocument::has_task_lists() const -> bool
{
    return !root.find_all(MdNodeType::TaskListMarker).empty();
}

auto MarkdownDocument::has_footnotes() const -> bool
{
    return has_footnotes_;
}

} // namespace markamp::core
