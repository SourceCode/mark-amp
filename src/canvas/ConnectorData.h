#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

// Forward declare Board for resolve methods.
class Board;

/// Where a connector attaches to an object.
enum class AnchorPosition : uint8_t
{
    kTop,
    kBottom,
    kLeft,
    kRight,
    kCenter,
    kAuto
};

/// Arrowhead styles for connector endpoints.
enum class ArrowheadStyle : uint8_t
{
    kNone,
    kArrow,
    kFilledArrow,
    kCircle,
    kDiamond,
    kOpenArrow
};

/// Line style for connectors.
enum class ConnectorLineStyle : uint8_t
{
    kSolid,
    kDashed,
    kDotted
};

/// One endpoint of a connector — either attached to an object or a free point.
struct ConnectorEndpoint
{
    ObjectId object_id{kInvalidObjectId}; // 0 = free point.
    AnchorPosition anchor{AnchorPosition::kAuto};
    Point2D offset{0.0, 0.0}; // Used as the free point when object_id == 0.

    [[nodiscard]] auto is_attached() const -> bool
    {
        return object_id != kInvalidObjectId;
    }
};

/// A connector (line/arrow) between two points or objects on the canvas.
class ConnectorObject : public CanvasObject
{
public:
    ConnectorObject();

    // ── Endpoints ──────────────────────────────────────────────

    [[nodiscard]] auto start_endpoint() const -> const ConnectorEndpoint&;
    [[nodiscard]] auto end_endpoint() const -> const ConnectorEndpoint&;

    auto set_start_object(ObjectId obj_id, AnchorPosition anchor) -> void;
    auto set_end_object(ObjectId obj_id, AnchorPosition anchor) -> void;
    auto set_free_start(const Point2D& point) -> void;
    auto set_free_end(const Point2D& point) -> void;

    /// Resolve the actual world position of an endpoint.
    /// If attached to an object, looks up the object in the board and computes
    /// the anchor position on its world bounds.
    [[nodiscard]] auto resolve_start(const Board& board) const -> Point2D;
    [[nodiscard]] auto resolve_end(const Board& board) const -> Point2D;

    // ── Waypoints ──────────────────────────────────────────────

    [[nodiscard]] auto waypoints() const -> const std::vector<Point2D>&;
    auto add_waypoint(const Point2D& point) -> void;
    auto clear_waypoints() -> void;

    // ── Line Style ─────────────────────────────────────────────

    [[nodiscard]] auto line_style() const -> ConnectorLineStyle;
    auto set_line_style(ConnectorLineStyle style) -> void;
    [[nodiscard]] auto line_width() const -> double;
    auto set_line_width(double width) -> void;
    [[nodiscard]] auto line_color() const -> CanvasColor;
    auto set_line_color(const CanvasColor& color) -> void;

    // ── Arrowheads ─────────────────────────────────────────────

    [[nodiscard]] auto start_arrow() const -> ArrowheadStyle;
    auto set_start_arrow(ArrowheadStyle style) -> void;
    [[nodiscard]] auto end_arrow() const -> ArrowheadStyle;
    auto set_end_arrow(ArrowheadStyle style) -> void;

    // ── Label ──────────────────────────────────────────────────

    [[nodiscard]] auto label() const -> const std::string&;
    auto set_label(const std::string& label) -> void;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

private:
    ConnectorEndpoint start_;
    ConnectorEndpoint end_;
    std::vector<Point2D> waypoints_;

    ConnectorLineStyle line_style_{ConnectorLineStyle::kSolid};
    double line_width_{2.0};
    CanvasColor line_color_{0, 0, 0, 255};

    ArrowheadStyle start_arrow_{ArrowheadStyle::kNone};
    ArrowheadStyle end_arrow_{ArrowheadStyle::kNone};

    std::string label_;

    /// Resolve an endpoint position from an AABB and anchor.
    [[nodiscard]] static auto resolve_anchor(const AABB& bounds, AnchorPosition anchor) -> Point2D;
};

} // namespace markamp::canvas
