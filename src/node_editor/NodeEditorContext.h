#pragma once

// V11 Phase 08: Eventing, Context Keys & State Synchronization
// Tracks active node editor state and publishes context keys for conditional UI enablement.

#include "node_editor/NodeEditorTypes.h"
#include "node_editor/NodeGraph.h"

#include <string>
#include <unordered_set>
#include <vector>

namespace markamp::node_editor
{

// ============================================================================
// Context Key
// ============================================================================

/// A named boolean context key used for conditional UI enablement.
struct ContextKey
{
    std::string key;
    bool value{false};
};

// ============================================================================
// Node Editor Context
// ============================================================================

/// State holder for the active node editor session.
///
/// Tracks the active graph, selected nodes, editor mode, and publishes
/// context keys that can be queried by the UI layer for conditional
/// enablement of menus, toolbars, and keyboard shortcuts.
class NodeEditorContext
{
public:
    NodeEditorContext() = default;

    // -- Active graph --
    void set_active_graph(NodeGraph* graph);
    [[nodiscard]] auto active_graph() const -> NodeGraph*;
    [[nodiscard]] auto has_active_graph() const -> bool;

    // -- Selection --
    void select_node(NodeId node_id);
    void deselect_node(NodeId node_id);
    void clear_selection();
    void set_selection(const std::vector<NodeId>& node_ids);
    [[nodiscard]] auto selected_nodes() const -> const std::unordered_set<NodeId>&;
    [[nodiscard]] auto has_selection() const -> bool;
    [[nodiscard]] auto selection_count() const -> std::size_t;
    [[nodiscard]] auto is_selected(NodeId node_id) const -> bool;

    // -- Editor mode --
    void set_editor_active(bool active);
    [[nodiscard]] auto is_editor_active() const -> bool;

    // -- Clipboard --
    void set_has_clipboard(bool has_data);
    [[nodiscard]] auto has_clipboard() const -> bool;

    // -- Context keys --
    /// Collect all current context keys reflecting editor state.
    [[nodiscard]] auto context_keys() const -> std::vector<ContextKey>;

    // -- Named key constants --
    static constexpr auto kKeyEditorActive = "nodeEditor.active";
    static constexpr auto kKeyHasSelection = "nodeEditor.hasSelection";
    static constexpr auto kKeyMultiSelection = "nodeEditor.multiSelection";
    static constexpr auto kKeyHasClipboard = "nodeEditor.hasClipboard";
    static constexpr auto kKeyHasActiveGraph = "nodeEditor.hasActiveGraph";
    static constexpr auto kKeyCanUndo = "nodeEditor.canUndo";
    static constexpr auto kKeyCanRedo = "nodeEditor.canRedo";

private:
    NodeGraph* active_graph_{nullptr};
    std::unordered_set<NodeId> selected_nodes_;
    bool editor_active_{false};
    bool has_clipboard_{false};
};

} // namespace markamp::node_editor
