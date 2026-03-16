#pragma once

/**
 * @file ConnectorAnchorModel.h
 * @brief Phase 44 Task 1: Robust anchor attachment for connectors.
 *
 * Anchor types (top/bottom/left/right/center/auto), stable attachment
 * through object movement/resize, and position recalculation.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Anchor position on an object boundary.
enum class AnchorPosition : uint8_t
{
    kTop,
    kBottom,
    kLeft,
    kRight,
    kCenter,
    kAuto, ///< Nearest edge
};

/// An anchor point attached to an object.
struct ConnectorAnchor
{
    std::string object_id;
    AnchorPosition position{AnchorPosition::kAuto};
    double offset_x{0.0}; ///< Relative offset from anchor center
    double offset_y{0.0};

    /// Get position as string.
    [[nodiscard]] auto position_name() const -> std::string;

    // ── Round 4 Batch 7 (#64-66) ────────────────────────────────

    /// (#64) Whether the anchor uses auto-positioning.
    [[nodiscard]] auto is_auto() const noexcept -> bool
    {
        return position == AnchorPosition::kAuto;
    }

    /// (#65) Whether a manual offset is applied.
    [[nodiscard]] auto has_offset() const noexcept -> bool
    {
        return offset_x != 0.0 || offset_y != 0.0;
    }

    /// (#66) Whether an object is attached.
    [[nodiscard]] auto has_object() const noexcept -> bool
    {
        return !object_id.empty();
    }
};

/// Object bounds for anchor recalculation.
struct ObjectBounds
{
    std::string object_id;
    double pos_x{0.0};
    double pos_y{0.0};
    double width{100.0};
    double height{100.0};

    // ── Round 4 Batch 7 (#67-68) ────────────────────────────────

    /// (#67) Area of the bounding box.
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width * height;
    }

    /// (#68) Whether an object ID is set.
    [[nodiscard]] auto has_id() const noexcept -> bool
    {
        return !object_id.empty();
    }
};

/**
 * @brief Manages connector anchor attachment and position recalculation.
 */
class ConnectorAnchorModel
{
public:
    ConnectorAnchorModel() = default;

    // ── Anchor resolution ──────────────────────────────────────────

    /// Resolve absolute position for an anchor given object bounds.
    struct AnchorPoint
    {
        double pos_x{0.0};
        double pos_y{0.0};
    };

    [[nodiscard]] static auto resolve_anchor(const ConnectorAnchor& anchor,
                                             const ObjectBounds& bounds) -> AnchorPoint;

    // ── Connection management ──────────────────────────────────────

    struct Connection
    {
        std::string connection_id;
        ConnectorAnchor source;
        ConnectorAnchor target;
    };

    /// Add a connection.
    void add_connection(const Connection& conn);

    /// Remove a connection.
    void remove_connection(const std::string& connection_id);

    /// Find a connection.
    [[nodiscard]] auto find_connection(const std::string& connection_id) const -> const Connection*;

    /// Get connection count.
    [[nodiscard]] auto connection_count() const -> int;

    /// Get connections attached to an object.
    [[nodiscard]] auto connections_for_object(const std::string& object_id) const
        -> std::vector<const Connection*>;

    // ── Stability ──────────────────────────────────────────────────

    /// Check if an anchor is still valid (object exists in bounds map).
    [[nodiscard]] auto is_anchor_valid(const ConnectorAnchor& anchor,
                                       const std::vector<ObjectBounds>& objects) const -> bool;

private:
    std::vector<Connection> connections_;
};

} // namespace markamp::canvas
