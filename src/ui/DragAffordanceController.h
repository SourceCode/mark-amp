#pragma once

/**
 * @file DragAffordanceController.h
 * @brief Phase 32 Task 1: Drag affordance with intent detection and cursor feedback.
 *
 * Wraps DragDropModel with surface-specific affordance logic: drag intent
 * detection (distance + hold time), cursor feedback states, and error
 * message generation for invalid drops.
 */

#include "DragDropModel.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Cursor feedback state during drag operations.
enum class DragCursorState : uint8_t
{
    kDefault,  ///< Normal pointer
    kGrab,     ///< Open hand (hovering draggable)
    kGrabbing, ///< Closed hand (actively dragging)
    kNoDrop,   ///< Forbidden indicator (invalid zone)
    kCopy,     ///< Copy indicator (Ctrl/Alt held)
    kMove,     ///< Move indicator (default drop)
};

/// Drag intent signal combining distance and hold time.
struct DragIntent
{
    int distance_px{0};
    int hold_time_ms{0};
    bool modifier_held{false}; ///< Ctrl/Alt for copy mode

    /// Check if intent meets drag initiation criteria.
    [[nodiscard]] auto meets_threshold(int min_distance, int min_hold_ms) const -> bool;
};

/// Error detail for an invalid drop attempt.
struct DropError
{
    std::string zone_id;
    std::string reason;
    std::string suggestion;
};

/**
 * @brief Higher-level drag affordance controller.
 *
 * Adds intent detection, cursor feedback, copy/move semantics, and
 * error messaging on top of the DragDropModel state machine.
 */
class DragAffordanceController
{
public:
    DragAffordanceController() = default;

    // ── Configuration ──────────────────────────────────────────────

    /// Set the minimum distance threshold for drag initiation (pixels).
    void set_distance_threshold(int pixels);

    /// Get current distance threshold.
    [[nodiscard]] auto distance_threshold() const -> int;

    /// Set the minimum hold time threshold for drag initiation (ms).
    void set_hold_threshold(int ms);

    /// Get current hold threshold.
    [[nodiscard]] auto hold_threshold() const -> int;

    // ── Intent detection ───────────────────────────────────────────

    /// Evaluate drag intent. Returns true if drag should initiate.
    [[nodiscard]] auto evaluate_intent(const DragIntent& intent) const -> bool;

    // ── Drag lifecycle ─────────────────────────────────────────────

    /// Begin a drag attempt on a source element.
    void begin(const std::string& source_id);

    /// Update with accumulated distance. May transition to dragging.
    void update(int distance_px);

    /// Complete the drag at a target zone.
    auto complete(const std::string& zone_id) -> bool;

    /// Cancel the current drag (escape key).
    void cancel();

    /// Reset to idle.
    void reset();

    // ── State queries ──────────────────────────────────────────────

    /// Get the current drag state.
    [[nodiscard]] auto state() const -> DragState;

    /// Get the cursor state based on current drag state and target zone.
    [[nodiscard]] auto cursor_state(const std::string& hover_zone_id = "") const -> DragCursorState;

    /// Check if currently in an active drag.
    [[nodiscard]] auto is_dragging() const -> bool;

    /// Get the source element ID.
    [[nodiscard]] auto source_id() const -> const std::string&;

    // ── Drop zones ─────────────────────────────────────────────────

    /// Register available drop zones.
    void set_drop_zones(std::vector<DropZone> zones);

    /// Get registered drop zones.
    [[nodiscard]] auto drop_zones() const -> const std::vector<DropZone>&;

    /// Get zone validity for a specific zone.
    [[nodiscard]] auto zone_validity(const std::string& zone_id) const -> DropValidity;

    // ── Copy/Move mode ─────────────────────────────────────────────

    /// Set copy mode (modifier key held).
    void set_copy_mode(bool enabled);

    /// Check if in copy mode.
    [[nodiscard]] auto is_copy_mode() const -> bool;

    // ── Error messaging ────────────────────────────────────────────

    /// Get the last drop error (empty if none).
    [[nodiscard]] auto last_error() const -> DropError;

    /// Generate an error for an invalid drop.
    [[nodiscard]] static auto make_error(const std::string& zone_id, const std::string& reason)
        -> DropError;

private:
    DragDropModel model_;
    int distance_threshold_{5};
    int hold_threshold_ms_{0};
    bool copy_mode_{false};
    DropError last_error_;
};

} // namespace markamp::ui
