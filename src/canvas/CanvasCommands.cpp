#include "CanvasCommands.h"

namespace markamp::canvas
{

// ── AddObjectCommand ───────────────────────────────────────────

AddObjectCommand::AddObjectCommand(Board& board, std::unique_ptr<CanvasObject> obj)
    : board_(board)
    , object_(std::move(obj))
{
}

auto AddObjectCommand::execute() -> void
{
    if (!executed_)
    {
        added_id_ = board_.add_object(std::move(object_));
        executed_ = true;
    }
    else
    {
        // Re-execute after undo: object_ was restored by undo().
        board_.add_object(std::move(object_));
    }
}

auto AddObjectCommand::undo() -> void
{
    object_ = board_.remove_object(added_id_);
}

auto AddObjectCommand::description() const -> std::string
{
    return "Add Object";
}

// ── RemoveObjectCommand ────────────────────────────────────────

RemoveObjectCommand::RemoveObjectCommand(Board& board, ObjectId obj_id)
    : board_(board)
    , target_id_(obj_id)
{
}

auto RemoveObjectCommand::execute() -> void
{
    removed_object_ = board_.remove_object(target_id_);
}

auto RemoveObjectCommand::undo() -> void
{
    if (removed_object_)
    {
        board_.add_object(std::move(removed_object_));
    }
}

auto RemoveObjectCommand::description() const -> std::string
{
    return "Remove Object";
}

// ── MoveObjectsCommand ────────────────────────────────────────

MoveObjectsCommand::MoveObjectsCommand(Board& board,
                                       std::vector<ObjectId> ids,
                                       double delta_x,
                                       double delta_y)
    : board_(board)
    , target_ids_(std::move(ids))
    , delta_x_(delta_x)
    , delta_y_(delta_y)
{
}

auto MoveObjectsCommand::execute() -> void
{
    for (const auto obj_id : target_ids_)
    {
        auto* obj = board_.get_object_mut(obj_id);
        if (obj != nullptr)
        {
            auto xform = obj->transform();
            xform.tx += delta_x_;
            xform.ty += delta_y_;
            obj->set_transform(xform);
        }
    }
}

auto MoveObjectsCommand::undo() -> void
{
    for (const auto obj_id : target_ids_)
    {
        auto* obj = board_.get_object_mut(obj_id);
        if (obj != nullptr)
        {
            auto xform = obj->transform();
            xform.tx -= delta_x_;
            xform.ty -= delta_y_;
            obj->set_transform(xform);
        }
    }
}

auto MoveObjectsCommand::description() const -> std::string
{
    return "Move Objects";
}

auto MoveObjectsCommand::can_merge_with(const ICanvasCommand& other) const -> bool
{
    const auto* move_cmd = dynamic_cast<const MoveObjectsCommand*>(&other);
    if (move_cmd == nullptr)
    {
        return false;
    }
    return target_ids_ == move_cmd->target_ids_;
}

auto MoveObjectsCommand::merge_with(const ICanvasCommand& other) -> bool
{
    const auto* move_cmd = dynamic_cast<const MoveObjectsCommand*>(&other);
    if (move_cmd == nullptr)
    {
        return false;
    }
    delta_x_ += move_cmd->delta_x_;
    delta_y_ += move_cmd->delta_y_;
    return true;
}

// ── CompoundCommand ────────────────────────────────────────────

CompoundCommand::CompoundCommand(std::string desc)
    : description_(std::move(desc))
{
}

auto CompoundCommand::add(std::unique_ptr<ICanvasCommand> cmd) -> void
{
    commands_.push_back(std::move(cmd));
}

auto CompoundCommand::execute() -> void
{
    for (auto& cmd : commands_)
    {
        cmd->execute();
    }
}

auto CompoundCommand::undo() -> void
{
    // Undo in reverse order.
    for (auto reverse_iter = commands_.rbegin(); reverse_iter != commands_.rend(); ++reverse_iter)
    {
        (*reverse_iter)->undo();
    }
}

auto CompoundCommand::description() const -> std::string
{
    return description_;
}

auto CompoundCommand::command_count() const -> size_t
{
    return commands_.size();
}

// ── ReorderZCommand ────────────────────────────────────────────

ReorderZCommand::ReorderZCommand(Board& board, ObjectId obj_id, Direction dir)
    : board_(board)
    , target_id_(obj_id)
    , direction_(dir)
{
}

auto ReorderZCommand::execute() -> void
{
    const auto* obj = board_.get_object(target_id_);
    if (obj == nullptr)
    {
        return;
    }

    previous_z_ = obj->z_index();

    switch (direction_)
    {
        case Direction::BringToFront:
            board_.bring_to_front(target_id_);
            break;
        case Direction::SendToBack:
            board_.send_to_back(target_id_);
            break;
        case Direction::BringForward:
            board_.bring_forward(target_id_);
            break;
        case Direction::SendBackward:
            board_.send_backward(target_id_);
            break;
    }
}

auto ReorderZCommand::undo() -> void
{
    auto* obj = board_.get_object_mut(target_id_);
    if (obj != nullptr)
    {
        obj->set_z_index(previous_z_);
    }
}

auto ReorderZCommand::description() const -> std::string
{
    switch (direction_)
    {
        case Direction::BringToFront:
            return "Bring to Front";
        case Direction::SendToBack:
            return "Send to Back";
        case Direction::BringForward:
            return "Bring Forward";
        case Direction::SendBackward:
            return "Send Backward";
    }
    return "Reorder Z";
}

// ── RemotePatchCommand ─────────────────────────────────────────

RemotePatchCommand::RemotePatchCommand(Board& board,
                                       ObjectId obj_id,
                                       std::string patch_type,
                                       std::string patch_json,
                                       std::string participant_id)
    : board_(board)
    , target_id_(obj_id)
    , patch_type_(std::move(patch_type))
    , patch_json_(std::move(patch_json))
    , participant_id_(std::move(participant_id))
{
}

auto RemotePatchCommand::execute() -> void
{
    // Save state for undo.
    auto* obj = board_.get_object_mut(target_id_);
    if (obj == nullptr)
    {
        return;
    }
    previous_state_ = obj->to_json();

    // Parse patch_json_ and apply the appropriate transformation.
    // Patch format: {"op":"move","dx":10,"dy":20} or {"op":"text","value":"new text"}
    // or {"op":"style","color_r":255,"color_g":0,"color_b":0} etc.
    auto find_str = [&](const std::string& key) -> std::string
    {
        const std::string needle = "\"" + key + "\":\"";
        auto pos = patch_json_.find(needle);
        if (pos == std::string::npos)
        {
            return "";
        }
        pos += needle.size();
        const auto end_pos = patch_json_.find('"', pos);
        if (end_pos == std::string::npos)
        {
            return "";
        }
        return patch_json_.substr(pos, end_pos - pos);
    };

    auto find_num = [&](const std::string& key) -> double
    {
        const std::string needle = "\"" + key + "\":";
        auto pos = patch_json_.find(needle);
        if (pos == std::string::npos)
        {
            return 0.0;
        }
        pos += needle.size();
        return std::stod(patch_json_.substr(pos));
    };

    const std::string operation = find_str("op");

    if (operation == "move")
    {
        auto xform = obj->transform();
        xform.tx += find_num("dx");
        xform.ty += find_num("dy");
        obj->set_transform(xform);
    }
    else if (operation == "resize")
    {
        auto xform = obj->transform();
        const double scale_x = find_num("scale_x");
        const double scale_y = find_num("scale_y");
        if (scale_x > 0.0)
        {
            xform.scale_x = scale_x;
        }
        if (scale_y > 0.0)
        {
            xform.scale_y = scale_y;
        }
        obj->set_transform(xform);
    }
    else if (operation == "text")
    {
        const std::string new_text = find_str("value");
        obj->set_name(new_text);
    }
    else if (operation == "style")
    {
        const auto color_r = static_cast<uint8_t>(find_num("color_r"));
        const auto color_g = static_cast<uint8_t>(find_num("color_g"));
        const auto color_b = static_cast<uint8_t>(find_num("color_b"));
        obj->set_custom_color({color_r, color_g, color_b, 255});
    }
    else if (operation == "visibility")
    {
        obj->set_visible(find_num("visible") > 0.5);
    }
    else if (operation == "rotate")
    {
        auto xform = obj->transform();
        xform.rotation = find_num("angle");
        obj->set_transform(xform);
    }
    obj->mark_dirty();
}

auto RemotePatchCommand::undo() -> void
{
    auto* obj = board_.get_object_mut(target_id_);
    if (obj != nullptr && !previous_state_.empty())
    {
        obj->from_json(previous_state_);
    }
}

auto RemotePatchCommand::description() const -> std::string
{
    return "Remote Patch (" + patch_type_ + ")";
}

auto RemotePatchCommand::patch_type() const -> const std::string&
{
    return patch_type_;
}

auto RemotePatchCommand::patch_json() const -> const std::string&
{
    return patch_json_;
}

auto RemotePatchCommand::source_participant() const -> const std::string&
{
    return participant_id_;
}

// ── GroupObjectsCommand (#30) ──────────────────────────────────

GroupObjectsCommand::GroupObjectsCommand(Board& board, std::vector<ObjectId> child_ids)
    : board_(board)
    , child_ids_(std::move(child_ids))
{
}

namespace
{

/// Minimal concrete subclass for group containers.
class GroupObject final : public CanvasObject
{
public:
    GroupObject()
        : CanvasObject(CanvasObjectType::Group)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return {0.0, 0.0, 0.0, 0.0};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        auto copy = std::make_unique<GroupObject>();
        copy->set_name(name());
        return copy;
    }
};

} // anonymous namespace

auto GroupObjectsCommand::execute() -> void
{
    // Save previous parent IDs.
    previous_parents_.clear();
    for (const auto child_id : child_ids_)
    {
        const auto* obj = board_.get_object(child_id);
        previous_parents_.push_back(obj != nullptr ? obj->parent_id() : kInvalidObjectId);
    }

    // Create a Group canvas object to act as the parent.
    auto group_obj = std::make_unique<GroupObject>();
    group_obj->set_name("Group");
    group_id_ = board_.add_object(std::move(group_obj));

    // Re-parent children under the group.
    for (const auto child_id : child_ids_)
    {
        auto* child = board_.get_object_mut(child_id);
        if (child != nullptr)
        {
            child->set_parent_id(group_id_);
        }
    }
}

auto GroupObjectsCommand::undo() -> void
{
    // Restore original parent IDs.
    for (size_t idx = 0; idx < child_ids_.size(); ++idx)
    {
        auto* child = board_.get_object_mut(child_ids_[idx]);
        if (child != nullptr)
        {
            child->set_parent_id(idx < previous_parents_.size() ? previous_parents_[idx]
                                                                : kInvalidObjectId);
        }
    }
    // Remove the group object.
    if (group_id_ != kInvalidObjectId)
    {
        board_.remove_object(group_id_);
        group_id_ = kInvalidObjectId;
    }
}

auto GroupObjectsCommand::description() const -> std::string
{
    return "Group Objects";
}

auto GroupObjectsCommand::group_id() const -> ObjectId
{
    return group_id_;
}

// ── Batch 10 (#58-60) ──────────────────────────────────────────

ResizeObjectCommand::ResizeObjectCommand(Board& board,
                                         ObjectId obj_id,
                                         double new_width,
                                         double new_height)
    : board_(board)
    , target_id_(obj_id)
    , new_width_(new_width)
    , new_height_(new_height)
{
}

auto ResizeObjectCommand::execute() -> void
{
    auto* obj = board_.get_object_mut(target_id_);
    if (obj == nullptr)
    {
        return;
    }
    old_width_ = obj->world_bounds().width();
    old_height_ = obj->world_bounds().height();
    // Apply through transform scaling.
    if (old_width_ > 0.0 && old_height_ > 0.0)
    {
        auto xform = obj->transform();
        xform.scale_x *= (new_width_ / old_width_);
        xform.scale_y *= (new_height_ / old_height_);
        obj->set_transform(xform);
    }
    obj->mark_dirty();
}

auto ResizeObjectCommand::undo() -> void
{
    auto* obj = board_.get_object_mut(target_id_);
    if (obj != nullptr)
    {
        if (old_width_ > 0.0 && old_height_ > 0.0)
        {
            auto xform = obj->transform();
            xform.scale_x *= (old_width_ / new_width_);
            xform.scale_y *= (old_height_ / new_height_);
            obj->set_transform(xform);
        }
        obj->mark_dirty();
    }
}

auto ResizeObjectCommand::description() const -> std::string
{
    return "Resize Object";
}

SetStyleCommand::SetStyleCommand(Board& board,
                                 ObjectId obj_id,
                                 CanvasColor new_stroke,
                                 CanvasColor new_fill)
    : board_(board)
    , target_id_(obj_id)
    , new_stroke_(new_stroke)
    , new_fill_(new_fill)
{
}

auto SetStyleCommand::execute() -> void
{
    auto* obj = board_.get_object_mut(target_id_);
    if (obj == nullptr)
    {
        return;
    }
    old_stroke_ = obj->custom_color();
    obj->set_custom_color(new_stroke_);
    obj->mark_dirty();
}

auto SetStyleCommand::undo() -> void
{
    auto* obj = board_.get_object_mut(target_id_);
    if (obj != nullptr)
    {
        obj->set_custom_color(old_stroke_);
        obj->mark_dirty();
    }
}

auto SetStyleCommand::description() const -> std::string
{
    return "Set Style";
}

DuplicateObjectsCommand::DuplicateObjectsCommand(Board& board,
                                                 std::vector<ObjectId> source_ids,
                                                 Point2D offset)
    : board_(board)
    , source_ids_(std::move(source_ids))
    , offset_(offset)
{
}

auto DuplicateObjectsCommand::execute() -> void
{
    created_ids_.clear();
    for (const auto src_id : source_ids_)
    {
        const auto* src_obj = board_.get_object(src_id);
        if (src_obj == nullptr)
        {
            continue;
        }
        auto cloned = src_obj->clone();
        auto xform = cloned->transform();
        xform.tx += offset_.x;
        xform.ty += offset_.y;
        cloned->set_transform(xform);
        const auto new_id = board_.add_object(std::move(cloned));
        created_ids_.push_back(new_id);
    }
}

auto DuplicateObjectsCommand::undo() -> void
{
    for (const auto created_id : created_ids_)
    {
        board_.remove_object(created_id);
    }
    created_ids_.clear();
}

auto DuplicateObjectsCommand::description() const -> std::string
{
    return "Duplicate Objects";
}

auto DuplicateObjectsCommand::duplicated_ids() const -> const std::vector<ObjectId>&
{
    return created_ids_;
}

} // namespace markamp::canvas
