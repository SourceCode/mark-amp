#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Canonical tool state.
enum class ToolState : uint8_t
{
    kIdle,
    kHover,
    kPressed,
    kDragging,
    kCommitted,
    kCancelled,
};

/// Active modifier keys.
struct ModifierKeys
{
    bool shift{false}; ///< Constrain proportions/angles
    bool ctrl{false};  ///< Duplicate / additive select (Win/Linux)
    bool cmd{false};   ///< Duplicate / additive select (macOS)
    bool alt{false};   ///< Alternate mode (e.g., center-draw)
};

/// Canvas tool type.
enum class ToolType : uint8_t
{
    kSelect,
    kDraw,
    kShape,
    kConnector,
    kText,
    kEraser,
    kHand,
};

/// Testable model for Canvas Input & Tool State Machine (Phase 41).
///
/// Encapsulates:
/// - Deterministic tool state machine (idle → hover → pressed → drag → commit/cancel)
/// - Modifier key contract
/// - Tool switching safety (cancel active gesture on switch)
/// - Valid transition enforcement
class CanvasInputModel
{
public:
    // ── State machine ───────────────────────────────────────────────

    [[nodiscard]] auto state() const -> ToolState;

    void hover();
    void press();
    void drag();
    void commit();
    void cancel();
    void reset();

    /// Returns true if the transition from current state to `target` is valid.
    [[nodiscard]] auto can_transition(ToolState target) const -> bool;

    // ── Modifiers ───────────────────────────────────────────────────

    void set_modifiers(ModifierKeys modifiers);
    [[nodiscard]] auto modifiers() const -> const ModifierKeys&;
    [[nodiscard]] auto is_constrained() const -> bool;
    [[nodiscard]] auto is_additive() const -> bool;

    // ── Tool management ─────────────────────────────────────────────

    void set_tool(ToolType tool);
    [[nodiscard]] auto tool() const -> ToolType;

    /// Switch tool safely: cancels if mid-gesture.
    void safe_switch(ToolType tool);

    /// True if a gesture is in progress (pressed or dragging).
    [[nodiscard]] auto is_gesture_active() const -> bool;

private:
    ToolState state_{ToolState::kIdle};
    ModifierKeys modifiers_;
    ToolType tool_{ToolType::kSelect};
};

} // namespace markamp::canvas
