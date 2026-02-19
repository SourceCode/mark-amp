#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Drag operation state.
enum class DragState : uint8_t
{
    kIdle,
    kPending, ///< Mouse down, below threshold
    kDragging,
    kCancelled,
    kCompleted,
};

/// Drop zone validity.
enum class DropValidity : uint8_t
{
    kValid,
    kInvalid,
    kUnknown,
};

/// Drop zone descriptor.
struct DropZone
{
    std::string zone_id;
    std::string label;
    DropValidity validity{DropValidity::kUnknown};
};

/// Testable model for Drag/Drop Controls (Phase 32).
///
/// Encapsulates:
/// - Drag state machine (idle → pending → dragging → completed/cancelled)
/// - Threshold-based drag initiation
/// - Drop zone registration and validity
/// - Escape to cancel
/// - Error messages for invalid drops
class DragDropModel
{
public:
    // ── Threshold ───────────────────────────────────────────────────

    void set_drag_threshold(int pixels);
    [[nodiscard]] auto drag_threshold() const -> int;

    // ── State machine ───────────────────────────────────────────────

    [[nodiscard]] auto state() const -> DragState;

    /// Begin a drag attempt (enters pending state).
    void begin_drag(const std::string& source_id);

    /// Accumulate drag distance. Transitions to dragging when threshold met.
    void update_distance(int pixels);

    /// Complete the drag at a target zone.
    void complete(const std::string& zone_id);

    /// Cancel the current drag (escape key).
    void cancel();

    /// Reset to idle.
    void reset();

    [[nodiscard]] auto source_id() const -> const std::string&;

    // ── Drop zones ──────────────────────────────────────────────────

    void set_drop_zones(std::vector<DropZone> zones);
    [[nodiscard]] auto drop_zones() const -> const std::vector<DropZone>&;
    [[nodiscard]] auto zone_validity(const std::string& zone_id) const -> DropValidity;

    // ── Error messaging ─────────────────────────────────────────────

    [[nodiscard]] auto last_error() const -> const std::string&;

private:
    int drag_threshold_{5};
    DragState state_{DragState::kIdle};
    std::string source_id_;
    int accumulated_distance_{0};
    std::vector<DropZone> zones_;
    std::string last_error_;
};

} // namespace markamp::ui
