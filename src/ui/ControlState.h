#pragma once

#include <cstdint>

namespace markamp::ui
{

/// V10 Phase 01 Task 2: Unified control state flags.
/// Every interactive control (button, toggle, menu item, tab, list row, tree row,
/// split handle) uses these flags to drive visual and behavioral transitions.
enum class ControlStateFlag : uint8_t
{
    kNormal = 0,
    kHover = 1 << 0,
    kPressed = 1 << 1,
    kFocused = 1 << 2,
    kDisabled = 1 << 3,
    kSelected = 1 << 4,
};

/// Bitwise-OR combination of ControlStateFlag values.
using ControlStateFlags = uint8_t;

inline auto operator|(ControlStateFlag lhs, ControlStateFlag rhs) -> ControlStateFlags
{
    return static_cast<ControlStateFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

inline auto has_flag(ControlStateFlags flags, ControlStateFlag flag) -> bool
{
    return (flags & static_cast<uint8_t>(flag)) != 0;
}

inline void set_flag(ControlStateFlags& flags, ControlStateFlag flag)
{
    flags |= static_cast<uint8_t>(flag);
}

inline void clear_flag(ControlStateFlags& flags, ControlStateFlag flag)
{
    flags &= static_cast<uint8_t>(~static_cast<uint8_t>(flag));
}

/// V10 Phase 01 Task 2: Shared helper for tracking interactive control state.
///
/// Manages state transitions (hover, pressed, focused, disabled, selected) in a
/// canonical way so every control surface produces consistent visual and behavioral
/// feedback. Replaces ad-hoc `hover_index_`, `pressed_index_`, `focus_index_`
/// patterns scattered across ActivityBar, TabBar, Toolbar, and StatusBarPanel.
///
/// Usage:
///   ControlStateTracker tracker;
///   tracker.on_mouse_enter();       // sets kHover
///   tracker.on_mouse_down();        // sets kPressed, clears kHover
///   tracker.on_mouse_up(inside);    // clears kPressed, sets kHover if still inside
///   tracker.on_focus();             // sets kFocused
///   tracker.on_blur();              // clears kFocused
///   tracker.set_disabled(true);     // sets kDisabled, clears interactive flags
///   tracker.set_selected(true);     // sets kSelected
class ControlStateTracker
{
public:
    ControlStateTracker() = default;

    /// Current combined state flags.
    [[nodiscard]] auto flags() const -> ControlStateFlags
    {
        return flags_;
    }

    /// Query individual states.
    [[nodiscard]] auto is_hover() const -> bool
    {
        return has_flag(flags_, ControlStateFlag::kHover);
    }
    [[nodiscard]] auto is_pressed() const -> bool
    {
        return has_flag(flags_, ControlStateFlag::kPressed);
    }
    [[nodiscard]] auto is_focused() const -> bool
    {
        return has_flag(flags_, ControlStateFlag::kFocused);
    }
    [[nodiscard]] auto is_disabled() const -> bool
    {
        return has_flag(flags_, ControlStateFlag::kDisabled);
    }
    [[nodiscard]] auto is_selected() const -> bool
    {
        return has_flag(flags_, ControlStateFlag::kSelected);
    }
    [[nodiscard]] auto is_normal() const -> bool
    {
        return flags_ == 0;
    }

    // --- Pointer transitions ---

    /// Call when pointer enters the control's hit zone.
    void on_mouse_enter();

    /// Call when pointer leaves the control's hit zone.
    void on_mouse_leave();

    /// Call on pointer-down inside the control.
    void on_mouse_down();

    /// Call on pointer-up. `still_inside` indicates whether the release
    /// occurred within the control's hit zone.
    void on_mouse_up(bool still_inside);

    // --- Focus transitions ---

    /// Call when the control receives keyboard focus.
    void on_focus();

    /// Call when the control loses keyboard focus.
    void on_blur();

    // --- Persistent state ---

    /// Set or clear the disabled state. Clears interactive (hover/pressed) flags
    /// when disabled.
    void set_disabled(bool disabled);

    /// Set or clear the selected state.
    void set_selected(bool selected);

    /// Reset all flags to kNormal.
    void reset();

    /// Whether the state changed since the last call to `acknowledge_change()`.
    /// Controls call this to decide whether to repaint.
    [[nodiscard]] auto changed() const -> bool
    {
        return changed_;
    }

    /// Acknowledge the pending change (resets the dirty flag).
    void acknowledge_change()
    {
        changed_ = false;
    }

private:
    ControlStateFlags flags_{0};
    bool changed_{false};

    void mark_changed()
    {
        changed_ = true;
    }
};

/// V10 Phase 01 Task 2: Multi-item control state tracker.
///
/// For controls that manage a list of interactive items (e.g., ActivityBar items,
/// TabBar tabs, StatusBar items), this tracks which item index is hovered,
/// pressed, and focused—preventing inconsistent state across items.
class IndexedControlState
{
public:
    IndexedControlState() = default;

    /// Current hovered item index, or -1 if none.
    [[nodiscard]] auto hover_index() const -> int
    {
        return hover_index_;
    }

    /// Current pressed item index, or -1 if none.
    [[nodiscard]] auto pressed_index() const -> int
    {
        return pressed_index_;
    }

    /// Current focused item index, or -1 if none.
    [[nodiscard]] auto focus_index() const -> int
    {
        return focus_index_;
    }

    /// Get combined flags for a specific item index.
    [[nodiscard]] auto flags_for(int index) const -> ControlStateFlags;

    /// Set hover to the given index (-1 to clear).
    void set_hover(int index);

    /// Set pressed to the given index (-1 to clear).
    void set_pressed(int index);

    /// Set focus to the given index (-1 to clear).
    void set_focus(int index);

    /// Clear all transient state (hover, pressed).
    void clear_transient();

    /// Reset everything including focus.
    void reset();

    /// Whether any state changed since last acknowledgement.
    [[nodiscard]] auto changed() const -> bool
    {
        return changed_;
    }
    void acknowledge_change()
    {
        changed_ = false;
    }

private:
    int hover_index_{-1};
    int pressed_index_{-1};
    int focus_index_{-1};
    bool changed_{false};

    void mark_changed()
    {
        changed_ = true;
    }
};

} // namespace markamp::ui
