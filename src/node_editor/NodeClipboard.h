#pragma once

// V11 Phase 28: Clipboard Duplicate Group And Paste Semantics
// Copy/cut/paste/duplicate subgraph operations with ID remapping.

#include "NodeEditorTypes.h"
#include "NodeGraph.h"

#include <string>
#include <vector>

namespace markamp::node_editor
{

class NodeCommandStack; // Forward declaration

/// Clipboard data: serialized subgraph for paste operations.
struct ClipboardData
{
    std::string serialized_json;
    std::size_t node_count{0};
    std::size_t link_count{0};
    bool valid{false};
};

/// Copy/paste/duplicate operations for node subgraphs.
class NodeClipboard
{
public:
    NodeClipboard() = default;

    /// Copy selected nodes and their internal links to clipboard.
    auto copy(const NodeGraph& graph, const std::vector<NodeId>& selected_ids) -> ClipboardData;

    /// Cut: copy + remove from graph.
    auto cut(NodeGraph& graph, const std::vector<NodeId>& selected_ids) -> ClipboardData;

    /// Paste from clipboard data into graph at the given position.
    /// Returns the IDs of the newly created nodes.
    auto paste(NodeGraph& graph, const ClipboardData& data, Vec2 position) -> std::vector<NodeId>;

    /// Duplicate selected nodes with an offset. Returns new node IDs.
    auto duplicate(NodeGraph& graph, const std::vector<NodeId>& selected_ids, Vec2 offset)
        -> std::vector<NodeId>;

    /// Check if clipboard data is valid for paste.
    [[nodiscard]] static auto is_valid(const ClipboardData& data) -> bool;

    /// Access the last copied data.
    [[nodiscard]] auto last_copied() const -> const ClipboardData&;

private:
    ClipboardData last_copied_;
};

} // namespace markamp::node_editor
