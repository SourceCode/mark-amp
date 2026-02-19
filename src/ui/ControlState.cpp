#include "ControlState.h"

namespace markamp::ui
{

// ── ControlStateTracker ─────────────────────────────────────────────

void ControlStateTracker::on_mouse_enter()
{
    if (is_disabled())
    {
        return;
    }
    set_flag(flags_, ControlStateFlag::kHover);
    mark_changed();
}

void ControlStateTracker::on_mouse_leave()
{
    clear_flag(flags_, ControlStateFlag::kHover);
    clear_flag(flags_, ControlStateFlag::kPressed);
    mark_changed();
}

void ControlStateTracker::on_mouse_down()
{
    if (is_disabled())
    {
        return;
    }
    set_flag(flags_, ControlStateFlag::kPressed);
    clear_flag(flags_, ControlStateFlag::kHover);
    mark_changed();
}

void ControlStateTracker::on_mouse_up(bool still_inside)
{
    clear_flag(flags_, ControlStateFlag::kPressed);
    if (still_inside && !is_disabled())
    {
        set_flag(flags_, ControlStateFlag::kHover);
    }
    mark_changed();
}

void ControlStateTracker::on_focus()
{
    set_flag(flags_, ControlStateFlag::kFocused);
    mark_changed();
}

void ControlStateTracker::on_blur()
{
    clear_flag(flags_, ControlStateFlag::kFocused);
    mark_changed();
}

void ControlStateTracker::set_disabled(bool disabled)
{
    if (disabled)
    {
        set_flag(flags_, ControlStateFlag::kDisabled);
        // Clear interactive flags when disabled
        clear_flag(flags_, ControlStateFlag::kHover);
        clear_flag(flags_, ControlStateFlag::kPressed);
    }
    else
    {
        clear_flag(flags_, ControlStateFlag::kDisabled);
    }
    mark_changed();
}

void ControlStateTracker::set_selected(bool selected)
{
    if (selected)
    {
        set_flag(flags_, ControlStateFlag::kSelected);
    }
    else
    {
        clear_flag(flags_, ControlStateFlag::kSelected);
    }
    mark_changed();
}

void ControlStateTracker::reset()
{
    flags_ = 0;
    mark_changed();
}

// ── IndexedControlState ─────────────────────────────────────────────

auto IndexedControlState::flags_for(int index) const -> ControlStateFlags
{
    ControlStateFlags result{0};
    if (index == hover_index_)
    {
        set_flag(result, ControlStateFlag::kHover);
    }
    if (index == pressed_index_)
    {
        set_flag(result, ControlStateFlag::kPressed);
    }
    if (index == focus_index_)
    {
        set_flag(result, ControlStateFlag::kFocused);
    }
    return result;
}

void IndexedControlState::set_hover(int index)
{
    if (hover_index_ != index)
    {
        hover_index_ = index;
        mark_changed();
    }
}

void IndexedControlState::set_pressed(int index)
{
    if (pressed_index_ != index)
    {
        pressed_index_ = index;
        mark_changed();
    }
}

void IndexedControlState::set_focus(int index)
{
    if (focus_index_ != index)
    {
        focus_index_ = index;
        mark_changed();
    }
}

void IndexedControlState::clear_transient()
{
    hover_index_ = -1;
    pressed_index_ = -1;
    mark_changed();
}

void IndexedControlState::reset()
{
    hover_index_ = -1;
    pressed_index_ = -1;
    focus_index_ = -1;
    mark_changed();
}

} // namespace markamp::ui
