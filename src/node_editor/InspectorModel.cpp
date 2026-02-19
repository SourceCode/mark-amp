#include "InspectorModel.h"

#include <algorithm>

namespace markamp::node_editor
{

// ---------------------------------------------------------------------------
// refresh — rebuild property list from current selection
// ---------------------------------------------------------------------------

void InspectorModel::refresh(const NodeGraph& graph, const SelectionModel& selection)
{
    properties_.clear();
    staged_.clear();
    editing_ = false;

    // Capture selected node IDs.
    selected_ids_.clear();
    selected_ids_ = selection.selected_ids();

    if (selected_ids_.empty())
    {
        return;
    }

    // Build properties from the first selected node's sockets.
    const auto* first_node = graph.find_node(selected_ids_[0]);
    if (first_node == nullptr)
    {
        return;
    }

    // Expose node label as an editable property.
    PropertyEntry label_prop;
    label_prop.name = "Label";
    label_prop.socket_name = "_label";
    label_prop.value = first_node->label;
    label_prop.state = PropertyState::kSingle;

    // Check for mixed label across multi-selection.
    if (selected_ids_.size() > 1)
    {
        for (std::size_t idx = 1; idx < selected_ids_.size(); ++idx)
        {
            const auto* other = graph.find_node(selected_ids_[idx]);
            if (other != nullptr && other->label != first_node->label)
            {
                label_prop.state = PropertyState::kMixed;
                break;
            }
        }
    }
    properties_.push_back(label_prop);

    // Expose position X and Y.
    PropertyEntry pos_x;
    pos_x.name = "Position X";
    pos_x.socket_name = "_pos_x";
    pos_x.value = first_node->position.x;
    pos_x.state = PropertyState::kSingle;

    PropertyEntry pos_y;
    pos_y.name = "Position Y";
    pos_y.socket_name = "_pos_y";
    pos_y.value = first_node->position.y;
    pos_y.state = PropertyState::kSingle;

    // Check mixed position values.
    if (selected_ids_.size() > 1)
    {
        for (std::size_t idx = 1; idx < selected_ids_.size(); ++idx)
        {
            const auto* other = graph.find_node(selected_ids_[idx]);
            if (other != nullptr)
            {
                if (other->position.x != first_node->position.x)
                {
                    pos_x.state = PropertyState::kMixed;
                }
                if (other->position.y != first_node->position.y)
                {
                    pos_y.state = PropertyState::kMixed;
                }
            }
        }
    }
    properties_.push_back(pos_x);
    properties_.push_back(pos_y);

    // Expose input sockets as properties.
    for (const auto& socket_id : first_node->inputs)
    {
        const auto* socket = graph.find_socket(socket_id);
        if (socket == nullptr)
        {
            continue;
        }

        PropertyEntry entry;
        entry.name = socket->label;
        entry.socket_name = socket->label;

        // Derive default value from socket type.
        switch (socket->data_type)
        {
            case SocketDataType::Float:
                entry.value = 0.0F;
                break;
            case SocketDataType::Int:
                entry.value = 0;
                break;
            case SocketDataType::Bool:
                entry.value = false;
                break;
            case SocketDataType::String:
                entry.value = std::string{};
                break;
            case SocketDataType::Vector2:
            case SocketDataType::Vector3:
            case SocketDataType::Color:
                entry.value = Vec2{0.0F, 0.0F};
                break;
            default:
                entry.value = std::string{"<unsupported>"};
                entry.editable = false;
                break;
        }

        entry.state = PropertyState::kSingle;
        properties_.push_back(entry);
    }
}

// ---------------------------------------------------------------------------
// Property access
// ---------------------------------------------------------------------------

auto InspectorModel::property_count() const -> std::size_t
{
    return properties_.size();
}

auto InspectorModel::get_property(std::size_t index) const -> const PropertyEntry&
{
    return properties_.at(index);
}

auto InspectorModel::is_mixed(std::size_t index) const -> bool
{
    return properties_.at(index).state == PropertyState::kMixed;
}

auto InspectorModel::header_text() const -> std::string
{
    if (selected_ids_.empty())
    {
        return "No selection";
    }
    if (selected_ids_.size() == 1)
    {
        return "Node Inspector";
    }
    return std::to_string(selected_ids_.size()) + " nodes selected";
}

// ---------------------------------------------------------------------------
// Editing lifecycle
// ---------------------------------------------------------------------------

void InspectorModel::begin_edit()
{
    editing_ = true;
    staged_.clear();
}

void InspectorModel::set_property(std::size_t index, const PropertyValue& value)
{
    if (!editing_ || index >= properties_.size())
    {
        return;
    }
    staged_[index] = value;
}

void InspectorModel::apply(NodeGraph& graph)
{
    if (!editing_)
    {
        return;
    }

    for (const auto& [index, value] : staged_)
    {
        if (index >= properties_.size())
        {
            continue;
        }

        const auto& prop = properties_[index];

        // Apply to all selected nodes.
        for (const auto& node_id : selected_ids_)
        {
            auto* node = graph.find_node_mut(node_id);
            if (node == nullptr)
            {
                continue;
            }

            if (prop.socket_name == "_label")
            {
                if (const auto* str_val = std::get_if<std::string>(&value))
                {
                    node->label = *str_val;
                }
            }
            else if (prop.socket_name == "_pos_x")
            {
                if (const auto* float_val = std::get_if<float>(&value))
                {
                    node->position.x = *float_val;
                }
            }
            else if (prop.socket_name == "_pos_y")
            {
                if (const auto* float_val = std::get_if<float>(&value))
                {
                    node->position.y = *float_val;
                }
            }
        }

        // Update the property entry to reflect the new value.
        properties_[index].value = value;
        properties_[index].state = PropertyState::kSingle;
    }

    staged_.clear();
    editing_ = false;
}

void InspectorModel::cancel()
{
    staged_.clear();
    editing_ = false;
}

} // namespace markamp::node_editor
