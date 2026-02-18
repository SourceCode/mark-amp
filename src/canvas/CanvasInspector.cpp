// ============================================================================
// File: src/canvas/CanvasInspector.cpp
// Phase 11: Canvas Workbench Shell — inspector implementation
// ============================================================================
#include "CanvasInspector.h"

#include "canvas/Board.h"

#include <algorithm>
#include <utility>

namespace markamp::canvas
{

CanvasInspector::CanvasInspector(Board& board,
                                 UndoRedoStack& undo_stack,
                                 std::shared_ptr<core::EventBus> event_bus)
    : board_(board)
    , undo_stack_(undo_stack)
    , event_bus_(std::move(event_bus))
{
}

// ── Inspection ─────────────────────────────────────────────────────

auto CanvasInspector::inspect(ObjectId obj_id) -> void
{
    inspected_id_ = obj_id;
    multi_ids_.clear();
    rebuild_properties();
}

auto CanvasInspector::inspect_multi(const std::vector<ObjectId>& obj_ids) -> void
{
    inspected_id_ = std::nullopt;
    multi_ids_ = obj_ids;
    rebuild_properties();
}

auto CanvasInspector::clear() -> void
{
    inspected_id_ = std::nullopt;
    multi_ids_.clear();
    current_properties_.clear();
}

auto CanvasInspector::inspected_object_id() const -> std::optional<ObjectId>
{
    return inspected_id_;
}

auto CanvasInspector::is_multi_inspect() const -> bool
{
    return !multi_ids_.empty();
}

auto CanvasInspector::properties() const -> const std::vector<PropertyDescriptor>&
{
    return current_properties_;
}

// ── Property Editing ───────────────────────────────────────────────

auto CanvasInspector::apply_property_change(ObjectId obj_id,
                                            const std::string& property_key,
                                            const PropertyValue& new_value) -> PropertyChangeResult
{
    auto* obj = board_.get_object_mut(obj_id);
    if (obj == nullptr)
    {
        return PropertyChangeResult::kObjectNotFound;
    }

    if (obj->is_locked())
    {
        return PropertyChangeResult::kObjectLocked;
    }

    // Read old value
    auto old_val = read_value_from_object(*obj, property_key);
    if (!old_val.has_value())
    {
        return PropertyChangeResult::kPropertyNotFound;
    }

    // Apply the change
    auto result = apply_value_to_object(*obj, property_key, new_value);
    if (result != PropertyChangeResult::kSuccess)
    {
        return result;
    }

    // Record for undo and notify
    PropertyChangeRecord record;
    record.object_id = obj_id;
    record.property_key = property_key;
    record.old_value = *old_val;
    record.new_value = new_value;

    if (on_changed_)
    {
        on_changed_(record);
    }

    board_.mark_dirty();
    rebuild_properties();
    return PropertyChangeResult::kSuccess;
}

auto CanvasInspector::apply_to_all_selected(const std::string& property_key,
                                            const PropertyValue& new_value) -> PropertyChangeResult
{
    if (multi_ids_.empty() && !inspected_id_.has_value())
    {
        return PropertyChangeResult::kObjectNotFound;
    }

    const auto& ids = multi_ids_.empty() ? std::vector<ObjectId>{*inspected_id_} : multi_ids_;

    PropertyChangeResult last_result = PropertyChangeResult::kSuccess;
    for (const auto& oid : ids)
    {
        auto result = apply_property_change(oid, property_key, new_value);
        if (result != PropertyChangeResult::kSuccess)
        {
            last_result = result;
        }
    }

    return last_result;
}

// ── Common Property Helpers ────────────────────────────────────────

auto CanvasInspector::common_properties(const CanvasObject& obj) -> std::vector<PropertyDescriptor>
{
    std::vector<PropertyDescriptor> props;
    const auto& transform = obj.transform();
    const auto& bounds = obj.world_bounds();

    props.push_back({"Name", "name", "General", obj.name(), false, true});
    props.push_back({"X", "x", "Transform", transform.tx, false, true});
    props.push_back({"Y", "y", "Transform", transform.ty, false, true});
    props.push_back({"Width", "width", "Transform", bounds.width(), false, true});
    props.push_back({"Height", "height", "Transform", bounds.height(), false, true});
    props.push_back({"Rotation", "rotation", "Transform", transform.rotation, false, true});
    props.push_back({"Locked", "locked", "State", obj.is_locked(), false, true});
    props.push_back({"Visible", "visible", "State", obj.is_visible(), false, true});
    props.push_back({"Layer", "layer", "State", obj.layer(), false, true});

    return props;
}

auto CanvasInspector::type_properties(const CanvasObject& obj) -> std::vector<PropertyDescriptor>
{
    std::vector<PropertyDescriptor> props;

    // Type-specific properties would be extracted based on object_type().
    // For now, provide the common fill/stroke that most objects support.
    props.push_back({"Fill Color", "fill_color", "Style", obj.custom_color(), false, true});
    props.push_back({"Stroke Color", "stroke_color", "Style", obj.custom_color(), false, true});

    return props;
}

// ── Event Subscription ─────────────────────────────────────────────

auto CanvasInspector::on_selection_changed(const std::vector<ObjectId>& selected_ids) -> void
{
    if (selected_ids.empty())
    {
        clear();
    }
    else if (selected_ids.size() == 1)
    {
        inspect(selected_ids[0]);
    }
    else
    {
        inspect_multi(selected_ids);
    }
}

auto CanvasInspector::set_on_property_changed(PropertyChangedCallback callback) -> void
{
    on_changed_ = std::move(callback);
}

// ── Private ────────────────────────────────────────────────────────

auto CanvasInspector::rebuild_properties() -> void
{
    current_properties_.clear();

    if (inspected_id_.has_value())
    {
        const auto* obj = board_.get_object(*inspected_id_);
        if (obj != nullptr)
        {
            auto common = common_properties(*obj);
            auto typed = type_properties(*obj);
            current_properties_.insert(current_properties_.end(), common.begin(), common.end());
            current_properties_.insert(current_properties_.end(), typed.begin(), typed.end());
        }
    }
    else if (!multi_ids_.empty())
    {
        // Multi-inspect: show only common properties from the first object
        if (const auto* obj = board_.get_object(multi_ids_[0]); obj != nullptr)
        {
            current_properties_ = common_properties(*obj);
        }
    }
}

auto CanvasInspector::apply_value_to_object(CanvasObject& obj,
                                            const std::string& key,
                                            const PropertyValue& val) -> PropertyChangeResult
{
    if (key == "name" && std::holds_alternative<std::string>(val))
    {
        obj.set_name(std::get<std::string>(val));
    }
    else if (key == "x" && std::holds_alternative<double>(val))
    {
        auto xform = obj.transform();
        xform.tx = std::get<double>(val);
        obj.set_transform(xform);
    }
    else if (key == "y" && std::holds_alternative<double>(val))
    {
        auto xform = obj.transform();
        xform.ty = std::get<double>(val);
        obj.set_transform(xform);
    }
    else if (key == "rotation" && std::holds_alternative<double>(val))
    {
        auto xform = obj.transform();
        xform.rotation = std::get<double>(val);
        obj.set_transform(xform);
    }
    else if (key == "locked" && std::holds_alternative<bool>(val))
    {
        obj.set_locked(std::get<bool>(val));
    }
    else if (key == "visible" && std::holds_alternative<bool>(val))
    {
        obj.set_visible(std::get<bool>(val));
    }
    else if (key == "fill_color" && std::holds_alternative<CanvasColor>(val))
    {
        obj.set_custom_color(std::get<CanvasColor>(val));
    }
    else if (key == "stroke_color" && std::holds_alternative<CanvasColor>(val))
    {
        obj.set_custom_color(std::get<CanvasColor>(val));
    }
    else
    {
        return PropertyChangeResult::kPropertyNotFound;
    }

    return PropertyChangeResult::kSuccess;
}

auto CanvasInspector::read_value_from_object(const CanvasObject& obj, const std::string& key)
    -> std::optional<PropertyValue>
{
    if (key == "name")
    {
        return obj.name();
    }
    if (key == "x")
    {
        return obj.transform().tx;
    }
    if (key == "y")
    {
        return obj.transform().ty;
    }
    if (key == "rotation")
    {
        return obj.transform().rotation;
    }
    if (key == "locked")
    {
        return obj.is_locked();
    }
    if (key == "visible")
    {
        return obj.is_visible();
    }
    if (key == "fill_color")
    {
        return obj.custom_color();
    }
    if (key == "stroke_color")
    {
        return obj.custom_color();
    }
    return std::nullopt;
}

} // namespace markamp::canvas
