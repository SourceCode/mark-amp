#include "CanvasToolStateMachine.h"

namespace markamp::canvas
{

CanvasToolStateMachine::CanvasToolStateMachine() = default;

auto CanvasToolStateMachine::current_state() const -> ToolState
{
    return state_;
}

auto CanvasToolStateMachine::state_name(ToolState state) -> std::string
{
    switch (state)
    {
        case ToolState::kIdle:
            return "idle";
        case ToolState::kHover:
            return "hover";
        case ToolState::kPressed:
            return "pressed";
        case ToolState::kDrag:
            return "drag";
        case ToolState::kCommit:
            return "commit";
        case ToolState::kCancel:
            return "cancel";
        case ToolState::kTextEditing:
            return "text_editing";
    }
    return "unknown";
}

auto CanvasToolStateMachine::can_transition(ToolState target) const -> bool
{
    // Valid transitions:
    // idle -> hover, pressed
    // hover -> idle, pressed
    // pressed -> drag, commit, cancel
    // drag -> commit, cancel
    // commit -> idle
    // cancel -> idle
    switch (state_)
    {
        case ToolState::kIdle:
            return target == ToolState::kHover || target == ToolState::kPressed ||
                   target == ToolState::kTextEditing;
        case ToolState::kHover:
            return target == ToolState::kIdle || target == ToolState::kPressed ||
                   target == ToolState::kTextEditing;
        case ToolState::kPressed:
            return target == ToolState::kDrag || target == ToolState::kCommit ||
                   target == ToolState::kCancel;
        case ToolState::kDrag:
            return target == ToolState::kCommit || target == ToolState::kCancel;
        case ToolState::kCommit:
            return target == ToolState::kIdle;
        case ToolState::kCancel:
            return target == ToolState::kIdle;
        case ToolState::kTextEditing:
            return target == ToolState::kIdle || target == ToolState::kCommit ||
                   target == ToolState::kCancel;
    }
    return false;
}

auto CanvasToolStateMachine::transition_to(ToolState target) -> bool
{
    if (!can_transition(target))
    {
        return false;
    }
    previous_state_ = state_;
    state_history_.push_back(state_name(state_) + " -> " + state_name(target));
    state_ = target;
    ++transition_count_;
    return true;
}

void CanvasToolStateMachine::reset()
{
    state_ = ToolState::kIdle;
    previous_state_ = ToolState::kIdle;
    modifiers_ = 0;
}

auto CanvasToolStateMachine::transition_count() const -> int
{
    return transition_count_;
}

void CanvasToolStateMachine::set_modifiers(uint8_t modifiers)
{
    modifiers_ = modifiers;
}

auto CanvasToolStateMachine::has_modifier(ModifierKey key) const -> bool
{
    return (modifiers_ & static_cast<uint8_t>(key)) != 0;
}

auto CanvasToolStateMachine::modifier_description() const -> std::string
{
    std::string desc;
    if (has_modifier(ModifierKey::kShift))
    {
        desc += "Shift";
    }
    if (has_modifier(ModifierKey::kCtrl))
    {
        if (!desc.empty())
            desc += "+";
        desc += "Ctrl";
    }
    if (has_modifier(ModifierKey::kAlt))
    {
        if (!desc.empty())
            desc += "+";
        desc += "Alt";
    }
    if (has_modifier(ModifierKey::kCmd))
    {
        if (!desc.empty())
            desc += "+";
        desc += "Cmd";
    }
    if (desc.empty())
    {
        desc = "none";
    }
    return desc;
}

auto CanvasToolStateMachine::is_safe_to_switch() const -> bool
{
    return state_ == ToolState::kIdle || state_ == ToolState::kHover;
}

auto CanvasToolStateMachine::force_cancel_for_switch() -> bool
{
    if (is_safe_to_switch())
    {
        return false; // Already safe
    }
    // Force cancel from pressed or drag
    state_ = ToolState::kCancel;
    ++transition_count_;
    // Then transition to idle
    state_ = ToolState::kIdle;
    ++transition_count_;
    return true;
}

// ── V17 Phase 01 W06: Input State Machine ──────────────────────────

auto CanvasToolStateMachine::escape_cancel() -> bool
{
    if (is_safe_to_switch())
    {
        return false; // Nothing to cancel
    }
    previous_state_ = state_;
    state_history_.push_back(state_name(state_) + " -> cancel (escape)");
    state_ = ToolState::kCancel;
    ++transition_count_;
    // Immediately return to idle
    state_history_.push_back("cancel -> idle");
    state_ = ToolState::kIdle;
    ++transition_count_;
    return true;
}

auto CanvasToolStateMachine::resume_previous_state() -> bool
{
    if (previous_state_ == state_)
    {
        return false;
    }
    if (!can_transition(previous_state_))
    {
        return false;
    }
    state_history_.push_back(state_name(state_) + " -> " + state_name(previous_state_) + " (resume)");
    state_ = previous_state_;
    ++transition_count_;
    return true;
}

auto CanvasToolStateMachine::is_mid_gesture() const -> bool
{
    return state_ != ToolState::kIdle && state_ != ToolState::kHover;
}

auto CanvasToolStateMachine::state_history() const -> const std::vector<std::string>&
{
    return state_history_;
}

} // namespace markamp::canvas
