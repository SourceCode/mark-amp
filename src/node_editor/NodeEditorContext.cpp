// V11 Phase 08: Eventing, Context Keys & State Synchronization

#include "node_editor/NodeEditorContext.h"

namespace markamp::node_editor
{

// ============================================================================
// Active Graph
// ============================================================================

void NodeEditorContext::set_active_graph(NodeGraph* graph)
{
    active_graph_ = graph;
}

auto NodeEditorContext::active_graph() const -> NodeGraph*
{
    return active_graph_;
}

auto NodeEditorContext::has_active_graph() const -> bool
{
    return active_graph_ != nullptr;
}

// ============================================================================
// Selection
// ============================================================================

void NodeEditorContext::select_node(NodeId node_id)
{
    selected_nodes_.insert(node_id);
}

void NodeEditorContext::deselect_node(NodeId node_id)
{
    selected_nodes_.erase(node_id);
}

void NodeEditorContext::clear_selection()
{
    selected_nodes_.clear();
}

void NodeEditorContext::set_selection(const std::vector<NodeId>& node_ids)
{
    selected_nodes_.clear();
    selected_nodes_.insert(node_ids.begin(), node_ids.end());
}

auto NodeEditorContext::selected_nodes() const -> const std::unordered_set<NodeId>&
{
    return selected_nodes_;
}

auto NodeEditorContext::has_selection() const -> bool
{
    return !selected_nodes_.empty();
}

auto NodeEditorContext::selection_count() const -> std::size_t
{
    return selected_nodes_.size();
}

auto NodeEditorContext::is_selected(NodeId node_id) const -> bool
{
    return selected_nodes_.contains(node_id);
}

// ============================================================================
// Editor mode
// ============================================================================

void NodeEditorContext::set_editor_active(bool active)
{
    editor_active_ = active;
}

auto NodeEditorContext::is_editor_active() const -> bool
{
    return editor_active_;
}

// ============================================================================
// Clipboard
// ============================================================================

void NodeEditorContext::set_has_clipboard(bool has_data)
{
    has_clipboard_ = has_data;
}

auto NodeEditorContext::has_clipboard() const -> bool
{
    return has_clipboard_;
}

// ============================================================================
// Context Keys
// ============================================================================

auto NodeEditorContext::context_keys() const -> std::vector<ContextKey>
{
    return {
        {kKeyEditorActive, editor_active_},
        {kKeyHasSelection, has_selection()},
        {kKeyMultiSelection, selection_count() > 1},
        {kKeyHasClipboard, has_clipboard_},
        {kKeyHasActiveGraph, has_active_graph()},
    };
}

} // namespace markamp::node_editor
