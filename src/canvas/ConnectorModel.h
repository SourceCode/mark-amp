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
};

} // namespace markamp::canvas
