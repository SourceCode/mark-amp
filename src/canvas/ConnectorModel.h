#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Connector routing mode.
enum class RoutingMode : uint8_t
{
    kStraight,
    kOrthogonal,
    kCurved,
};

/// Anchor attachment point on a shape.
struct AnchorPoint
{
    std::string object_id;
    double offset_x{0.5}; ///< 0–1 normalized position on object bounds
    double offset_y{0.5};

    // ── Round 4 Batch 6 (#56-57) ────────────────────────────────

    /// (#56) Whether an object is attached.
    [[nodiscard]] auto has_object() const noexcept -> bool
    {
        return !object_id.empty();
    }

    /// (#57) Whether the anchor is at the center (default).
    [[nodiscard]] auto is_centered() const noexcept -> bool
    {
        return offset_x == 0.5 && offset_y == 0.5;
    }
};

/// A bend point for manual route tuning.
struct BendPoint
{
    double x{0.0};
    double y{0.0};
};

/// Connector label.
struct ConnectorLabel
{
    std::string text;
    double position{0.5}; ///< 0–1 along the connector's length

    // ── Round 4 Batch 6 (#58-59) ────────────────────────────────

    /// (#58) Whether label text is set.
    [[nodiscard]] auto has_text() const noexcept -> bool
    {
        return !text.empty();
    }

    /// (#59) Whether the label is at the midpoint.
    [[nodiscard]] auto is_midpoint() const noexcept -> bool
    {
        return position == 0.5;
    }
};

/// Testable model for Connector Routing & Line Editing (Phase 44).
///
/// Encapsulates:
/// - Anchor attachment (source/target objects)
/// - Routing modes (straight/orthogonal/curved)
/// - Manual bend point editing
/// - Inline label placement
class ConnectorModel
{
public:
    // ── Anchors ─────────────────────────────────────────────────────

    void set_source(AnchorPoint anchor);
    void set_target(AnchorPoint anchor);
    [[nodiscard]] auto source() const -> const AnchorPoint&;
    [[nodiscard]] auto target() const -> const AnchorPoint&;
    [[nodiscard]] auto is_attached() const -> bool;

    // ── Routing ─────────────────────────────────────────────────────

    void set_routing(RoutingMode mode);
    [[nodiscard]] auto routing() const -> RoutingMode;

    // ── Bends ───────────────────────────────────────────────────────

    void add_bend(BendPoint point);
    void remove_bend(int index);
    void clear_bends();
    [[nodiscard]] auto bends() const -> const std::vector<BendPoint>&;
    [[nodiscard]] auto bend_count() const -> int;

    // ── Labels ──────────────────────────────────────────────────────

    void set_label(ConnectorLabel label);
    [[nodiscard]] auto label() const -> const ConnectorLabel&;
    [[nodiscard]] auto has_label() const -> bool;

private:
    AnchorPoint source_;
    AnchorPoint target_;
    RoutingMode routing_{RoutingMode::kStraight};
    std::vector<BendPoint> bends_;
    ConnectorLabel label_;

    // ── Round 4 Batch 6-7 (#60-63) ──────────────────────────────

    /// (#60) Whether routing is straight.
    [[nodiscard]] auto is_straight() const noexcept -> bool
    {
        return routing_ == RoutingMode::kStraight;
    }

    /// (#61) Whether routing is orthogonal.
    [[nodiscard]] auto is_orthogonal() const noexcept -> bool
    {
        return routing_ == RoutingMode::kOrthogonal;
    }

    /// (#62) Whether routing is curved.
    [[nodiscard]] auto is_curved() const noexcept -> bool
    {
        return routing_ == RoutingMode::kCurved;
    }

    /// (#63) Whether manual bend points exist.
    [[nodiscard]] auto has_bends() const noexcept -> bool
    {
        return !bends_.empty();
    }
};

} // namespace markamp::canvas
