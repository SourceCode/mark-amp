// ============================================================================
// File: src/canvas/CanvasClipboardService.cpp
// Phase 12: Canvas Advanced Objects — clipboard cut/copy/paste/duplicate
// ============================================================================
#include "canvas/CanvasClipboardService.h"

#include "canvas/CanvasCommands.h"

#include <fmt/format.h>
#include <sstream>

namespace markamp::canvas
{

CanvasClipboardService::CanvasClipboardService(Board& board,
                                               SelectionManager& selection,
                                               UndoRedoStack& undo_stack)
    : board_(board)
    , selection_(selection)
    , undo_stack_(undo_stack)
{
}

// ── Core Operations ───────────────────────────────────────────────

auto CanvasClipboardService::copy() -> bool
{
    const auto& selected = selection_.selected_ids();
    if (selected.empty())
    {
        return false;
    }

    clipboard_.serialized_json = serialize_objects(selected);
    clipboard_.object_count = selected.size();
    clipboard_.original_center = compute_center(selected);
    clipboard_.has_connectors = false;

    clipboard_.types.clear();
    for (const auto obj_id : selected)
    {
        const auto* obj = board_.get_object(obj_id);
        if (obj != nullptr)
        {
            clipboard_.types.push_back(obj->type());
            if (obj->type() == CanvasObjectType::Connector)
            {
                clipboard_.has_connectors = true;
            }
        }
    }

    paste_count_ = 0;
    return true;
}

auto CanvasClipboardService::cut() -> bool
{
    if (!copy())
    {
        return false;
    }

    // Delete the selected objects with undo support
    const auto selected = selection_.selected_ids(); // copy before clearing
    for (const auto obj_id : selected)
    {
        auto removed = board_.remove_object(obj_id);
        if (removed)
        {
            auto cmd = std::make_unique<AddObjectCommand>(board_, std::move(removed));
            undo_stack_.execute(std::move(cmd));
        }
    }
    selection_.clear_selection();
    return true;
}

auto CanvasClipboardService::paste() -> PasteResult
{
    if (!has_data())
    {
        return {false, {}, "Clipboard is empty"};
    }

    ++paste_count_;

    const double offset_x = paste_offset_.x * static_cast<double>(paste_count_);
    const double offset_y = paste_offset_.y * static_cast<double>(paste_count_);

    auto objects = deserialize_objects(clipboard_.serialized_json);
    if (objects.empty())
    {
        return {false, {}, "Failed to deserialize clipboard data"};
    }

    PasteResult result;
    result.success = true;

    selection_.clear_selection();

    for (auto& obj : objects)
    {
        auto xform = obj->transform();
        xform.tx += offset_x;
        xform.ty += offset_y;
        obj->set_transform(xform);

        const auto new_id = board_.add_object(std::move(obj));
        result.pasted_ids.push_back(new_id);
        selection_.add_to_selection(new_id);
    }

    return result;
}

auto CanvasClipboardService::paste_at(const Point2D& position) -> PasteResult
{
    if (!has_data())
    {
        return {false, {}, "Clipboard is empty"};
    }

    auto objects = deserialize_objects(clipboard_.serialized_json);
    if (objects.empty())
    {
        return {false, {}, "Failed to deserialize clipboard data"};
    }

    // Compute offset from original center to new position
    const double delta_x = position.x - clipboard_.original_center.x;
    const double delta_y = position.y - clipboard_.original_center.y;

    PasteResult result;
    result.success = true;

    selection_.clear_selection();

    for (auto& obj : objects)
    {
        auto xform = obj->transform();
        xform.tx += delta_x;
        xform.ty += delta_y;
        obj->set_transform(xform);

        const auto new_id = board_.add_object(std::move(obj));
        result.pasted_ids.push_back(new_id);
        selection_.add_to_selection(new_id);
    }

    paste_count_ = 0;
    return result;
}

auto CanvasClipboardService::duplicate_selection() -> PasteResult
{
    if (!copy())
    {
        return {false, {}, "Nothing selected to duplicate"};
    }
    return paste();
}

// ── Clipboard State ───────────────────────────────────────────────

auto CanvasClipboardService::has_data() const -> bool
{
    return !clipboard_.serialized_json.empty() && clipboard_.object_count > 0;
}

auto CanvasClipboardService::clipboard_count() const -> size_t
{
    return clipboard_.object_count;
}

auto CanvasClipboardService::clear() -> void
{
    clipboard_ = ClipboardData{};
    paste_count_ = 0;
}

auto CanvasClipboardService::clipboard_info() const -> const ClipboardData&
{
    return clipboard_;
}

// (#71) Report clipboard object types for UI.
auto CanvasClipboardService::clipboard_types() const -> const std::vector<CanvasObjectType>&
{
    return clipboard_.types;
}

// ── Configuration ─────────────────────────────────────────────────

auto CanvasClipboardService::set_paste_offset(const Point2D& offset) -> void
{
    paste_offset_ = offset;
}

auto CanvasClipboardService::paste_offset() const -> const Point2D&
{
    return paste_offset_;
}

// ── Private Helpers ───────────────────────────────────────────────

auto CanvasClipboardService::serialize_objects(const std::vector<ObjectId>& ids) -> std::string
{
    std::ostringstream oss;
    oss << "[";
    bool first = true;
    for (const auto obj_id : ids)
    {
        const auto* obj = board_.get_object(obj_id);
        if (obj == nullptr)
        {
            continue;
        }
        if (!first)
        {
            oss << ",";
        }
        oss << obj->to_json();
        first = false;
    }
    oss << "]";
    return oss.str();
}

auto CanvasClipboardService::deserialize_objects(const std::string& json)
    -> std::vector<std::unique_ptr<CanvasObject>>
{
    std::vector<std::unique_ptr<CanvasObject>> result;

    // Parse the JSON array and reconstruct each object via factory.
    if (json.empty() || json.front() != '[')
    {
        return result;
    }

    // Find each object block { } within the array.
    size_t pos = 1;
    while (pos < json.size())
    {
        const auto obj_start = json.find('{', pos);
        if (obj_start == std::string::npos)
        {
            break;
        }

        int brace_depth = 1;
        auto obj_end = obj_start + 1;
        while (obj_end < json.size() && brace_depth > 0)
        {
            if (json[obj_end] == '{') ++brace_depth;
            if (json[obj_end] == '}') --brace_depth;
            ++obj_end;
        }

        const auto obj_json = json.substr(obj_start, obj_end - obj_start);
        auto obj = factory_.from_json(obj_json);
        if (obj)
        {
            result.push_back(std::move(obj));
        }

        pos = obj_end;
    }

    return result;
}

auto CanvasClipboardService::compute_center(const std::vector<ObjectId>& ids) const -> Point2D
{
    if (ids.empty())
    {
        return {0.0, 0.0};
    }

    AABB combined;
    for (const auto obj_id : ids)
    {
        const auto* obj = board_.get_object(obj_id);
        if (obj == nullptr)
        {
            continue;
        }
        combined = combined.merged(obj->world_bounds());
    }

    return combined.center();
}

} // namespace markamp::canvas
