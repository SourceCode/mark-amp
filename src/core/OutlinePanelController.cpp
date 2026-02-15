/// @file OutlinePanelController.cpp
/// @brief V4 Phase 12 – Document Outline / Table-of-Contents implementation.

#include "core/OutlinePanelController.h"

#include "core/Events.h"
#include "core/VaultService.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// OutlineNode helpers
// ============================================================================

auto OutlineNode::child_count() const -> size_t
{
    return children.size();
}

auto OutlineNode::total_count() const -> size_t
{
    size_t count = 1;
    for (const auto& child : children)
    {
        count += child.total_count();
    }
    return count;
}

// ============================================================================
// Constructor
// ============================================================================

OutlinePanelController::OutlinePanelController(EventBus& event_bus, VaultService& vault_service)
    : event_bus_(event_bus)
    , vault_service_(vault_service)
{
}

// ============================================================================
// Build Outline
// ============================================================================

auto OutlinePanelController::build_outline(const std::string& markdown) const
    -> std::vector<OutlineNode>
{
    std::vector<OutlineNode> root;
    std::istringstream stream(markdown);
    std::string line;
    int line_num = 0;
    bool in_code_block = false;

    while (std::getline(stream, line))
    {
        // Track code blocks to skip headings inside them
        if (line.starts_with("```") || line.starts_with("~~~"))
        {
            in_code_block = !in_code_block;
            ++line_num;
            continue;
        }

        if (in_code_block)
        {
            ++line_num;
            continue;
        }

        // Check for ATX heading (# Heading)
        int level = 0;
        size_t pos = 0;
        while (pos < line.size() && line[pos] == '#')
        {
            ++level;
            ++pos;
        }

        if (level > 0 && level <= 6 && pos < line.size() && line[pos] == ' ')
        {
            std::string heading_text = line.substr(pos + 1);

            // Trim trailing #s and whitespace
            while (!heading_text.empty() &&
                   (heading_text.back() == '#' ||
                    std::isspace(static_cast<unsigned char>(heading_text.back()))))
            {
                heading_text.pop_back();
            }

            if (!heading_text.empty())
            {
                OutlineNode node;
                node.text = heading_text;
                node.level = level;
                node.line_number = line_num;
                insert_node(root, node);
            }
        }

        ++line_num;
    }

    return root;
}

// ============================================================================
// Flatten
// ============================================================================

auto OutlinePanelController::flatten(const std::vector<OutlineNode>& nodes) const
    -> std::vector<OutlineNode>
{
    std::vector<OutlineNode> result;

    for (const auto& node : nodes)
    {
        OutlineNode flat_node;
        flat_node.text = node.text;
        flat_node.level = node.level;
        flat_node.line_number = node.line_number;
        flat_node.is_expanded = node.is_expanded;
        result.push_back(flat_node);

        auto child_flat = flatten(node.children);
        result.insert(result.end(), child_flat.begin(), child_flat.end());
    }

    return result;
}

// ============================================================================
// Find Active
// ============================================================================

auto OutlinePanelController::find_active(const std::vector<OutlineNode>& nodes,
                                         int cursor_line) const -> std::optional<OutlineNode>
{
    auto flat = flatten(nodes);

    // Find the heading just before or at cursor_line
    std::optional<OutlineNode> best;

    for (const auto& node : flat)
    {
        if (node.line_number <= cursor_line)
        {
            best = node;
        }
        else
        {
            break;
        }
    }

    return best;
}

// ============================================================================
// Update For Document
// ============================================================================

auto OutlinePanelController::update_for_document(const std::string& document_id) -> void
{
    current_doc_id_ = document_id;

    // Try to open and read the document
    auto open_result = vault_service_.open_document(document_id);
    if (!open_result)
    {
        current_outline_.clear();
        return;
    }

    const auto& doc = open_result.value();
    current_outline_ = build_outline(doc->markdown());

    // Publish update event
    auto event = events::OutlineUpdatedEvent{};
    event.heading_count = static_cast<int>(flatten(current_outline_).size());
    event_bus_.publish(event);
}

// ============================================================================
// Current Outline
// ============================================================================

auto OutlinePanelController::current_outline() const -> const std::vector<OutlineNode>&
{
    return current_outline_;
}

// ============================================================================
// Scroll To Heading
// ============================================================================

auto OutlinePanelController::scroll_to_heading(int line_number) -> void
{
    auto event = events::ScrollToLineRequestEvent{};
    event.line_number = line_number;
    event_bus_.publish(event);
}

// ============================================================================
// Insert Node (tree builder)
// ============================================================================

auto OutlinePanelController::insert_node(std::vector<OutlineNode>& root, const OutlineNode& node)
    -> void
{
    if (root.empty())
    {
        root.push_back(node);
        return;
    }

    auto& last = root.back();

    // If new node is deeper, insert as child of last
    if (node.level > last.level)
    {
        insert_node(last.children, node);
    }
    else
    {
        // Same or higher level: add as sibling
        root.push_back(node);
    }
}

} // namespace markamp::core
