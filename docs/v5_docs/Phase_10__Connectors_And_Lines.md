# Phase 10 -- Connectors and Lines

## Objective

Implement connector/line objects that visually link canvas objects with configurable line styles, arrow heads, and auto-routing that avoids obstacles. Connectors attach to connection points on shapes and update their paths when connected objects move.

## Prerequisites

- Phase 01 (CanvasObject, Point2D, AABB)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 03 (CanvasInputManager, tool system)
- Phase 09 (ShapeObject::connection_points)

## Feature References (PRD)

- PRD #5: Connectors & Lines

## Data Structures to Implement

### File: `src/canvas/ConnectorObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Arrow head style.
enum class ArrowHead : uint8_t
{
    None,
    Triangle,         // Filled triangle
    TriangleOutline,  // Outline triangle
    Diamond,
    Circle,
    Square,
    OpenArrow,        // V-shaped
    Bar               // Perpendicular line
};

/// Connector routing mode.
enum class ConnectorRouting : uint8_t
{
    Straight,     // Direct line
    Orthogonal,   // Right-angle bends only
    Curved,       // Bezier curve
    ElbowSingle,  // Single elbow bend
    ElbowDouble   // Double elbow bend
};

/// Connection endpoint -- either attached to an object or a free point.
struct ConnectionEndpoint
{
    std::optional<ObjectId> attached_object_id;  // Object this end is connected to
    int connection_point_index{0};                // Which connection point on the object
    Point2D position;                             // World position (updated when object moves)
    bool is_attached() const { return attached_object_id.has_value(); }
};

/// A connector/line that links two points or two objects.
class ConnectorObject : public CanvasObject
{
public:
    ConnectorObject();

    // --- Endpoints ---
    [[nodiscard]] auto source() const -> const ConnectionEndpoint&;
    auto set_source(const ConnectionEndpoint& ep) -> void;
    [[nodiscard]] auto target() const -> const ConnectionEndpoint&;
    auto set_target(const ConnectionEndpoint& ep) -> void;

    /// Update endpoint positions from the connected objects' current positions.
    auto update_endpoints(const std::function<Point2D(ObjectId, int)>& resolve_fn) -> void;

    // --- Routing ---
    [[nodiscard]] auto routing() const -> ConnectorRouting;
    auto set_routing(ConnectorRouting routing) -> void;

    /// The computed path waypoints (intermediate points for orthogonal/curved routing).
    [[nodiscard]] auto waypoints() const -> const std::vector<Point2D>&;
    auto set_waypoints(const std::vector<Point2D>& points) -> void;
    auto add_waypoint(const Point2D& point) -> void;
    auto clear_waypoints() -> void;

    // --- Stroke ---
    [[nodiscard]] auto stroke_color() const -> const CanvasColor&;
    auto set_stroke_color(const CanvasColor& color) -> void;
    [[nodiscard]] auto stroke_width() const -> double;
    auto set_stroke_width(double w) -> void;
    [[nodiscard]] auto dash_pattern() const -> const std::vector<double>&;
    auto set_dash_pattern(const std::vector<double>& pattern) -> void;

    // --- Arrow heads ---
    [[nodiscard]] auto source_arrow() const -> ArrowHead;
    auto set_source_arrow(ArrowHead arrow) -> void;
    [[nodiscard]] auto target_arrow() const -> ArrowHead;
    auto set_target_arrow(ArrowHead arrow) -> void;
    [[nodiscard]] auto arrow_size() const -> double;
    auto set_arrow_size(double size) -> void;

    // --- Label ---
    [[nodiscard]] auto label() const -> const std::string&;
    auto set_label(const std::string& label) -> void;

    // --- CanvasObject overrides ---
    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    ConnectionEndpoint source_;
    ConnectionEndpoint target_;
    ConnectorRouting routing_{ConnectorRouting::Straight};
    std::vector<Point2D> waypoints_;
    CanvasColor stroke_color_{100, 100, 100, 255};
    double stroke_width_{2.0};
    std::vector<double> dash_pattern_;
    ArrowHead source_arrow_{ArrowHead::None};
    ArrowHead target_arrow_{ArrowHead::Triangle};
    double arrow_size_{12.0};
    std::string label_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/ConnectorRouter.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <vector>

namespace markamp::canvas
{

/// Computes the path for a connector between two endpoints, optionally avoiding obstacles.
class ConnectorRouter
{
public:
    /// Route a straight line (just source -> target).
    [[nodiscard]] auto route_straight(const Point2D& source, const Point2D& target) const
        -> std::vector<Point2D>;

    /// Route an orthogonal path (horizontal/vertical segments only).
    [[nodiscard]] auto route_orthogonal(const Point2D& source, const Point2D& target,
                                         const std::vector<AABB>& obstacles) const
        -> std::vector<Point2D>;

    /// Route a curved path (cubic Bezier control points).
    [[nodiscard]] auto route_curved(const Point2D& source, const Point2D& target) const
        -> std::vector<Point2D>;

    /// Route an elbow path (single 90-degree bend).
    [[nodiscard]] auto route_elbow(const Point2D& source, const Point2D& target) const
        -> std::vector<Point2D>;

    /// Set the margin around obstacles for routing.
    auto set_obstacle_margin(double margin) -> void;

private:
    double obstacle_margin_{20.0};

    /// A* or simplified routing around obstacles for orthogonal paths.
    [[nodiscard]] auto find_orthogonal_path(const Point2D& source, const Point2D& target,
                                             const std::vector<AABB>& obstacles) const
        -> std::vector<Point2D>;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `ConnectorObject::update_endpoints()` -- For each endpoint that is_attached, call resolve_fn(object_id, point_index) to get the current world position. Update endpoint.position.

2. `ConnectorRouter::route_orthogonal()` -- Compute a path using horizontal and vertical segments. Start with source, add intermediate points to reach target with only 90-degree turns. Avoid overlapping obstacle AABBs by routing around them.

3. `ConnectorRouter::route_curved()` -- Compute cubic Bezier control points. The control points are offset from source/target along the direction perpendicular to the connection.

4. `ConnectorRenderer::render()` -- For straight: draw line from source to target. For orthogonal: draw polyline through waypoints. For curved: draw Bezier curve via wxGraphicsPath::AddCurveToPoint. Draw arrow heads at endpoints.

5. `ConnectorRenderer::draw_arrow_head()` -- Compute arrow geometry based on ArrowHead type and the line direction at the endpoint. Triangle: 3-point filled polygon. Diamond: 4-point polygon. Circle: filled circle.

6. Connector creation tool: Click on a connection point (highlighted when hovering a shape), drag to another connection point. Creates a ConnectorObject linking the two shapes.

7. Connection point highlighting: When the Connector tool is active and mouse hovers over a shape, highlight the nearest connection point with a blue circle.

## Test Cases

File: `tests/unit/test_connector.cpp`

1. **Default construction** -- Verify straight routing, target arrow = Triangle, no source arrow.
2. **Set endpoints** -- Set source and target positions, verify getters.
3. **Attached endpoints** -- Attach source to object ID 5, verify is_attached().
4. **Route straight** -- Source (0,0), target (100,100). Verify 2-point path.
5. **Route orthogonal** -- Source (0,0), target (100,50). Verify path has 3+ points, all segments axis-aligned.
6. **Route orthogonal with obstacle** -- Obstacle between source and target. Verify path routes around it.
7. **Route curved** -- Verify 4 control points returned.
8. **Arrow head types** -- Set each ArrowHead type, verify getter.
9. **Label** -- Set label, verify getter.
10. **JSON round-trip** -- Full connector with attached endpoints, serialize/deserialize.
11. **Local bounds** -- Verify bounds enclose all waypoints and endpoints.
12. **Update endpoints** -- Attach to objects, call update_endpoints with resolve function, verify positions updated.

## Acceptance Criteria

- [ ] ConnectorObject with source/target endpoints (attached or free)
- [ ] 5 routing modes: straight, orthogonal, curved, elbow single, elbow double
- [ ] Orthogonal routing avoids obstacles
- [ ] 8 arrow head styles with configurable size
- [ ] Endpoint positions update when connected objects move
- [ ] Connector creation tool with connection point snapping
- [ ] Line label displayed at midpoint
- [ ] JSON serialization preserves all fields including attached object IDs
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/ConnectorObject.h` | Connector data model |
| CREATE | `src/canvas/ConnectorObject.cpp` | Connector implementation |
| CREATE | `src/canvas/ConnectorRouter.h` | Path routing algorithms |
| CREATE | `src/canvas/ConnectorRouter.cpp` | Routing implementation |
| CREATE | `src/canvas/ConnectorRenderer.h` | Connector rendering |
| CREATE | `src/canvas/ConnectorRenderer.cpp` | Path + arrow drawing |
| CREATE | `src/canvas/ConnectorTool.h` | Connector creation tool |
| CREATE | `src/canvas/ConnectorTool.cpp` | Tool implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register ConnectorObject factory |
| MODIFY | `src/ui/CanvasPanel.cpp` | Update connector endpoints on object move |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_connector.cpp` | 12 Catch2 tests |

## Architecture Notes

- ConnectorObject stores endpoint references (ObjectId + point index) but does not own or reference the actual objects. A resolve function is passed during update_endpoints to look up current positions.
- When an object moves (CanvasObjectMovedEvent), CanvasPanel iterates all connectors attached to the moved object and calls update_endpoints. This keeps connectors in sync.
- The ConnectorRouter is a stateless utility. Obstacle AABBs are passed in from the Quadtree.
- Arrow heads are drawn in screen space (constant pixel size regardless of zoom) for readability.
- The connector hit-test must check proximity to the line/curve path, not just the AABB. This requires point-to-segment distance calculation.

## Estimated Complexity

**XL** -- Multiple routing algorithms, obstacle avoidance, arrow head geometry, dynamic endpoint updates, creation tool with connection point detection, 12 tests.
