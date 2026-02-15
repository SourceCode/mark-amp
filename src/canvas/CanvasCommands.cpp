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

} // namespace markamp::canvas
