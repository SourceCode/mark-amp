#include "CanvasInputModel.h"

namespace markamp::canvas
{

auto CanvasInputModel::state() const -> ToolState
{
    return state_;
}

void CanvasInputModel::hover()
{
    if (state_ == ToolState::kIdle || state_ == ToolState::kHover)
    {
        state_ = ToolState::kHover;
    }
}

void CanvasInputModel::press()
{
    if (state_ == ToolState::kIdle || state_ == ToolState::kHover)
    {
        state_ = ToolState::kPressed;
    }
}

void CanvasInputModel::drag()
{
    if (state_ == ToolState::kPressed || state_ == ToolState::kDragging)
    {
        state_ = ToolState::kDragging;
    }
}

void CanvasInputModel::commit()
{
    if (state_ == ToolState::kPressed || state_ == ToolState::kDragging)
    {
        state_ = ToolState::kCommitted;
    }
}

void CanvasInputModel::cancel()
{
    if (state_ == ToolState::kPressed || state_ == ToolState::kDragging)
    {
        state_ = ToolState::kCancelled;
    }
}

void CanvasInputModel::reset()
{
    state_ = ToolState::kIdle;
}

auto CanvasInputModel::can_transition(ToolState target) const -> bool
{
    switch (state_)
    {
        case ToolState::kIdle:
            return target == ToolState::kHover || target == ToolState::kPressed;
        case ToolState::kHover:
            return target == ToolState::kIdle || target == ToolState::kHover ||
                   target == ToolState::kPressed;
        case ToolState::kPressed:
            return target == ToolState::kDragging || target == ToolState::kCommitted ||
                   target == ToolState::kCancelled;
        case ToolState::kDragging:
            return target == ToolState::kDragging || target == ToolState::kCommitted ||
                   target == ToolState::kCancelled;
        case ToolState::kCommitted:
        case ToolState::kCancelled:
            return target == ToolState::kIdle;
    }
    return false;
}

void CanvasInputModel::set_modifiers(ModifierKeys modifiers)
{
    modifiers_ = modifiers;
}
auto CanvasInputModel::modifiers() const -> const ModifierKeys&
{
    return modifiers_;
}
auto CanvasInputModel::is_constrained() const -> bool
{
    return modifiers_.shift;
}
auto CanvasInputModel::is_additive() const -> bool
{
    return modifiers_.ctrl || modifiers_.cmd;
}

void CanvasInputModel::set_tool(ToolType tool)
{
    tool_ = tool;
}
auto CanvasInputModel::tool() const -> ToolType
{
    return tool_;
}

void CanvasInputModel::safe_switch(ToolType tool)
{
    if (is_gesture_active())
    {
        cancel();
    }
    tool_ = tool;
    state_ = ToolState::kIdle;
}

auto CanvasInputModel::is_gesture_active() const -> bool
{
    return state_ == ToolState::kPressed || state_ == ToolState::kDragging;
}

} // namespace markamp::canvas
