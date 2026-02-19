#pragma once

#include "NodeEditorTypes.h"
#include "NodeGraph.h"
#include "SelectionModel.h"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// PropertyValue — type-safe variant for inspector property values
// ---------------------------------------------------------------------------

using PropertyValue = std::variant<float, int, bool, std::string, Vec2>;

/// Describes mixed-state when multiple nodes are selected with differing values.
enum class PropertyState
{
    kSingle,  ///< Single value (one node selected or all values match)
    kMixed,   ///< Different values across selection
    kReadOnly ///< Not editable (computed / locked)
};

/// A single property exposed in the inspector.
struct PropertyEntry
{
    std::string name;        ///< Display name (e.g. "Threshold")
    std::string socket_name; ///< Source socket/param name
    PropertyValue value;     ///< Current or representative value
    PropertyState state{PropertyState::kSingle};
    bool editable{true};
};

// ---------------------------------------------------------------------------
// InspectorModel — view-model for the node properties inspector panel
// ---------------------------------------------------------------------------

class InspectorModel
{
public:
    InspectorModel() = default;

    // --- Refresh from current selection -----------------------------------

    /// Rebuild property list from graph + selection.
    void refresh(const NodeGraph& graph, const SelectionModel& selection);

    // --- Property access --------------------------------------------------

    [[nodiscard]] auto property_count() const -> std::size_t;
    [[nodiscard]] auto get_property(std::size_t index) const -> const PropertyEntry&;
    [[nodiscard]] auto is_mixed(std::size_t index) const -> bool;
    [[nodiscard]] auto is_empty() const -> bool
    {
        return properties_.empty();
    }

    /// Header text describing what is being inspected.
    [[nodiscard]] auto header_text() const -> std::string;

    // --- Editing lifecycle ------------------------------------------------

    /// Begin an edit session (buffers the current values).
    void begin_edit();

    /// Stage a property change (does not commit until apply()).
    void set_property(std::size_t index, const PropertyValue& value);

    /// Apply all staged changes to the graph.
    void apply(NodeGraph& graph);

    /// Cancel edit and revert staged changes.
    void cancel();

    [[nodiscard]] auto is_editing() const -> bool
    {
        return editing_;
    }
    [[nodiscard]] auto has_staged_changes() const -> bool
    {
        return !staged_.empty();
    }

    // --- Selection info ---------------------------------------------------

    [[nodiscard]] auto selected_count() const -> std::size_t
    {
        return selected_ids_.size();
    }
    [[nodiscard]] auto selected_ids() const -> const std::vector<NodeId>&
    {
        return selected_ids_;
    }

private:
    std::vector<PropertyEntry> properties_;
    std::vector<NodeId> selected_ids_;
    bool editing_{false};

    /// Staged edits: index → new value.
    std::unordered_map<std::size_t, PropertyValue> staged_;
};

} // namespace markamp::node_editor
