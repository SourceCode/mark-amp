#pragma once

/**
 * @file CanvasToolStateMachine.h
 * @brief Phase 41 Task 1,3,4: Deterministic tool state lifecycle.
 *
 * Standardizes idle/hover/pressed/drag/commit/cancel states,
 * defines modifier key contracts, and enforces tool-change safety.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Tool lifecycle state.
enum class ToolState : uint8_t
{
    kIdle,
    kHover,
    kPressed,
    kDrag,
    kCommit,
    kCancel,
    kTextEditing, ///< Double-click initiated in-place text editing
};

/// Modifier key flags for tool behavior.
enum class ModifierKey : uint8_t
{
    kNone = 0,
    kShift = 1 << 0, ///< Constrain proportions/angles
    kCtrl = 1 << 1,  ///< Duplicate object
    kAlt = 1 << 2,   ///< Additive select
    kCmd = 1 << 3,   ///< Platform-specific
};

/// A state transition record.
struct ToolTransition
{
    ToolState from{ToolState::kIdle};
    ToolState to{ToolState::kIdle};
    bool valid{false};

    // ── Round 4 Batch 10 (#99-100) ───────────────────────────────

    /// (#99) Whether this transition is valid.
    [[nodiscard]] auto is_valid() const noexcept -> bool
    {
        return valid;
    }

    /// (#100) Whether this is a no-op transition (same state).
    [[nodiscard]] auto is_noop() const noexcept -> bool
    {
        return from == to;
    }
};

/**
 * @brief Deterministic tool state machine with modifier contract.
 */
class CanvasToolStateMachine
{
public:
    CanvasToolStateMachine();

    // ── State ──────────────────────────────────────────────────────

    /// Get the current state.
    [[nodiscard]] auto current_state() const -> ToolState;

    /// Get state as string.
    [[nodiscard]] static auto state_name(ToolState state) -> std::string;

    // ── Transitions ────────────────────────────────────────────────

    /// Attempt a transition. Returns true if valid.
    auto transition_to(ToolState target) -> bool;

    /// Check if a transition is valid without performing it.
    [[nodiscard]] auto can_transition(ToolState target) const -> bool;

    /// Force reset to idle (for error recovery).
    void reset();

    /// Get the transition history.
    [[nodiscard]] auto transition_count() const -> int;

    // ── Modifier contract ──────────────────────────────────────────

    /// Set active modifiers.
    void set_modifiers(uint8_t modifiers);

    /// Check if a modifier is active.
    [[nodiscard]] auto has_modifier(ModifierKey key) const -> bool;

    /// Get modifier description (e.g. "Shift+Ctrl").
    [[nodiscard]] auto modifier_description() const -> std::string;

    // ── Tool-change safety ─────────────────────────────────────────

    /// Check if it's safe to switch tools (not mid-gesture).
    [[nodiscard]] auto is_safe_to_switch() const -> bool;

    /// Force-cancel active gesture for tool switch. Returns true if cancelled.
    auto force_cancel_for_switch() -> bool;

    // ── V17 Phase 01 W06: Input State Machine ──────────────────────

    /// Escape-cancel: force-cancel active gesture from Escape key.
    auto escape_cancel() -> bool;

    /// Resume the state that was active before the most recent transition.
    auto resume_previous_state() -> bool;

    /// Whether the state machine is mid-gesture (not Idle or Hover).
    [[nodiscard]] auto is_mid_gesture() const -> bool;

    /// Get the recent transition history (from → to pairs as strings).
    [[nodiscard]] auto state_history() const -> const std::vector<std::string>&;

private:
    ToolState state_{ToolState::kIdle};
    ToolState previous_state_{ToolState::kIdle};
    uint8_t modifiers_{0};
    int transition_count_{0};
    std::vector<std::string> state_history_;
};

} // namespace markamp::canvas
