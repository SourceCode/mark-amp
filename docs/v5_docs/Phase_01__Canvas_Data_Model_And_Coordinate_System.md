# Phase 01 -- Canvas Data Model and Coordinate System

## Objective

Define the fundamental 2D canvas data model: the base CanvasObject class hierarchy, Transform2D for world-space positioning, AABB for bounding boxes, a spatial index (quadtree) for efficient hit-testing, and the coordinate-space conversion primitives that all subsequent canvas phases depend on.

## Prerequisites

- Existing EventBus (`src/core/EventBus.h`)
- Existing Config (`src/core/Config.h`)
- Existing ThemeEngine (`src/core/ThemeEngine.h`)

## Feature References (PRD)

- PRD #1: Infinite Canvas (data model foundation)
- PRD #99: Board Performance Optimization (spatial index)
- PRD #96: Object Metadata (metadata map on base object)
- PRD #58: Object Layering / Z-Index (z_index field)

## Data Structures to Implement

### File: `src/canvas/CanvasTypes.h`

```cpp
#pragma once

#include <cmath>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Unique identifier for canvas objects. 64-bit monotonic counter.
using ObjectId = uint64_t;
constexpr ObjectId kInvalidObjectId = 0;

/// 2D point in world or screen coordinates.
struct Point2D
{
    double x{0.0};
    double y{0.0};

    [[nodiscard]] auto distance_to(const Point2D& other) const -> double
    {
        const double dx = x - other.x;
        const double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    auto operator+(const Point2D& rhs) const -> Point2D { return {x + rhs.x, y + rhs.y}; }
    auto operator-(const Point2D& rhs) const -> Point2D { return {x - rhs.x, y - rhs.y}; }
    auto operator*(double s) const -> Point2D { return {x * s, y * s}; }
};

/// 2D size (width, height).
struct Size2D
{
    double width{0.0};
    double height{0.0};
};

/// 2D affine transform: translation, rotation (radians), uniform scale.
struct Transform2D
{
    double tx{0.0};       // Translation X (world space)
    double ty{0.0};       // Translation Y (world space)
    double rotation{0.0}; // Rotation in radians
    double scale_x{1.0};
    double scale_y{1.0};

    /// Apply this transform to a point.
    [[nodiscard]] auto apply(const Point2D& p) const -> Point2D;

    /// Inverse-transform a point (screen -> local).
    [[nodiscard]] auto inverse(const Point2D& p) const -> Point2D;

    /// Compose with another transform (this * other).
    [[nodiscard]] auto compose(const Transform2D& other) const -> Transform2D;

    /// Identity transform.
    [[nodiscard]] static auto identity() -> Transform2D;
};

/// Axis-aligned bounding box.
struct AABB
{
    double min_x{std::numeric_limits<double>::max()};
    double min_y{std::numeric_limits<double>::max()};
    double max_x{std::numeric_limits<double>::lowest()};
    double max_y{std::numeric_limits<double>::lowest()};

    [[nodiscard]] auto width() const -> double { return max_x - min_x; }
    [[nodiscard]] auto height() const -> double { return max_y - min_y; }
    [[nodiscard]] auto center() const -> Point2D;
    [[nodiscard]] auto contains(const Point2D& p) const -> bool;
    [[nodiscard]] auto intersects(const AABB& other) const -> bool;
    [[nodiscard]] auto merged(const AABB& other) const -> AABB;
    [[nodiscard]] auto expanded(double margin) const -> AABB;
    [[nodiscard]] auto is_valid() const -> bool;

    /// Create from center + size.
    [[nodiscard]] static auto from_center_size(const Point2D& center, const Size2D& size) -> AABB;
};

/// RGBA color for canvas objects (0-255 per channel).
struct CanvasColor
{
    uint8_t r{0};
    uint8_t g{0};
    uint8_t b{0};
    uint8_t a{255};

    [[nodiscard]] static auto from_hex(const std::string& hex) -> CanvasColor;
    [[nodiscard]] auto to_hex() const -> std::string;
};

/// Enumeration of all canvas object types.
enum class CanvasObjectType : uint8_t
{
    StickyNote,
    TextBox,
    Shape,
    Connector,
    FreehandPath,
    Image,
    Frame,
    Section,
    Group,
    Table,
    MindMapNode,
    KanbanColumn,
    KanbanCard,
    BookmarkCard,
    VideoEmbed,
    PDFPage,
    Icon,
    Comment,
    DiagramShape
};

} // namespace markamp::canvas
```

### File: `src/canvas/CanvasObject.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Base class for all objects that live on the infinite canvas.
/// Every canvas object has an ID, transform, bounds, z-index, metadata,
/// lock state, and a parent reference for grouping.
class CanvasObject
{
public:
    explicit CanvasObject(CanvasObjectType type);
    virtual ~CanvasObject() = default;

    // Non-copyable, movable
    CanvasObject(const CanvasObject&) = delete;
    auto operator=(const CanvasObject&) -> CanvasObject& = delete;
    CanvasObject(CanvasObject&&) = default;
    auto operator=(CanvasObject&&) -> CanvasObject& = default;

    // --- Identity ---
    [[nodiscard]] auto id() const -> ObjectId;
    [[nodiscard]] auto type() const -> CanvasObjectType;
    [[nodiscard]] auto name() const -> const std::string&;
    auto set_name(const std::string& name) -> void;

    // --- Transform ---
    [[nodiscard]] auto transform() const -> const Transform2D&;
    auto set_transform(const Transform2D& t) -> void;
    auto set_position(double x, double y) -> void;
    auto set_rotation(double radians) -> void;
    auto set_scale(double sx, double sy) -> void;
    [[nodiscard]] auto position() const -> Point2D;

    // --- Bounds ---
    /// Local-space bounding box (before transform). Subclasses override.
    [[nodiscard]] virtual auto local_bounds() const -> AABB = 0;
    /// World-space AABB (local bounds transformed).
    [[nodiscard]] auto world_bounds() const -> AABB;

    // --- Z-Order ---
    [[nodiscard]] auto z_index() const -> int;
    auto set_z_index(int z) -> void;

    // --- Lock ---
    [[nodiscard]] auto is_locked() const -> bool;
    auto set_locked(bool locked) -> void;

    // --- Visibility ---
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    // --- Opacity ---
    [[nodiscard]] auto opacity() const -> double;
    auto set_opacity(double o) -> void;

    // --- Metadata (arbitrary key-value) ---
    [[nodiscard]] auto metadata() const -> const std::unordered_map<std::string, std::string>&;
    auto set_metadata(const std::string& key, const std::string& value) -> void;
    auto remove_metadata(const std::string& key) -> void;
    [[nodiscard]] auto get_metadata(const std::string& key) const -> std::optional<std::string>;

    // --- Tags ---
    [[nodiscard]] auto tags() const -> const std::vector<std::string>&;
    auto add_tag(const std::string& tag) -> void;
    auto remove_tag(const std::string& tag) -> void;

    // --- Parent / Group ---
    [[nodiscard]] auto parent_id() const -> ObjectId;
    auto set_parent_id(ObjectId parent) -> void;

    // --- Serialization ---
    /// Serialize to JSON object. Subclasses override to add type-specific fields.
    [[nodiscard]] virtual auto to_json() const -> std::string;
    /// Deserialize from JSON. Subclasses override.
    virtual auto from_json(const std::string& json) -> void;

    // --- Clone ---
    [[nodiscard]] virtual auto clone() const -> std::unique_ptr<CanvasObject> = 0;

    // --- Dirty tracking ---
    [[nodiscard]] auto is_dirty() const -> bool;
    auto mark_dirty() -> void;
    auto mark_clean() -> void;

protected:
    ObjectId id_;
    CanvasObjectType type_;
    std::string name_;
    Transform2D transform_;
    int z_index_{0};
    bool locked_{false};
    bool visible_{true};
    double opacity_{1.0};
    std::unordered_map<std::string, std::string> metadata_;
    std::vector<std::string> tags_;
    ObjectId parent_id_{kInvalidObjectId};
    bool dirty_{true};

    /// Monotonic ID generator.
    [[nodiscard]] static auto next_id() -> ObjectId;
};

} // namespace markamp::canvas
```

### File: `src/canvas/Quadtree.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <array>
#include <functional>
#include <memory>
#include <vector>

namespace markamp::canvas
{

/// Quadtree spatial index for efficient region queries and hit-testing.
/// Stores ObjectId + AABB pairs. Supports insert, remove, query-region, and
/// query-point (hit-test).
class Quadtree
{
public:
    struct Entry
    {
        ObjectId id{kInvalidObjectId};
        AABB bounds;
    };

    /// Construct a quadtree covering the given world-space bounds.
    explicit Quadtree(const AABB& bounds, int max_depth = 8, int max_entries_per_node = 16);

    /// Insert an object into the index.
    auto insert(ObjectId id, const AABB& bounds) -> void;

    /// Remove an object by ID. Returns true if found and removed.
    auto remove(ObjectId id) -> bool;

    /// Update the bounds of an existing object. Equivalent to remove + insert.
    auto update(ObjectId id, const AABB& new_bounds) -> void;

    /// Query all objects whose AABB intersects the given region.
    [[nodiscard]] auto query_region(const AABB& region) const -> std::vector<ObjectId>;

    /// Query the object at a specific point (hit-test). Returns the topmost
    /// candidate by insertion order; caller resolves z-order.
    [[nodiscard]] auto query_point(const Point2D& point) const -> std::vector<ObjectId>;

    /// Clear all entries and reset.
    auto clear() -> void;

    /// Rebuild the tree from scratch (call after bulk modifications).
    auto rebuild(const std::vector<Entry>& entries) -> void;

    /// Total number of entries stored.
    [[nodiscard]] auto size() const -> size_t;

private:
    struct Node
    {
        AABB bounds;
        std::vector<Entry> entries;
        std::array<std::unique_ptr<Node>, 4> children;
        bool is_leaf{true};
    };

    std::unique_ptr<Node> root_;
    int max_depth_;
    int max_entries_per_node_;
    size_t total_entries_{0};

    auto subdivide(Node& node, int depth) -> void;
    auto insert_into(Node& node, const Entry& entry, int depth) -> void;
    auto query_region_impl(const Node& node, const AABB& region,
                           std::vector<ObjectId>& results) const -> void;
    auto remove_from(Node& node, ObjectId id) -> bool;
    [[nodiscard]] auto child_index(const Node& node, const AABB& bounds) const -> int;
};

} // namespace markamp::canvas
```

### File: `src/canvas/ViewportTransform.h`

```cpp
#pragma once

#include "CanvasTypes.h"

namespace markamp::canvas
{

/// Manages the mapping between world coordinates and screen coordinates.
/// Tracks zoom level, pan offset, and screen dimensions.
class ViewportTransform
{
public:
    ViewportTransform() = default;

    /// Set the screen (panel) size in pixels.
    auto set_screen_size(double width, double height) -> void;

    /// Current zoom level (1.0 = 100%).
    [[nodiscard]] auto zoom() const -> double;
    auto set_zoom(double z) -> void;

    /// Zoom centered on a screen point.
    auto zoom_at(double screen_x, double screen_y, double new_zoom) -> void;

    /// Pan offset in world coordinates.
    [[nodiscard]] auto pan() const -> Point2D;
    auto set_pan(const Point2D& p) -> void;
    auto pan_by(double dx, double dy) -> void;

    /// Convert world coords to screen coords.
    [[nodiscard]] auto world_to_screen(const Point2D& world) const -> Point2D;
    [[nodiscard]] auto world_to_screen(const AABB& world) const -> AABB;

    /// Convert screen coords to world coords.
    [[nodiscard]] auto screen_to_world(const Point2D& screen) const -> Point2D;
    [[nodiscard]] auto screen_to_world(const AABB& screen) const -> AABB;

    /// Visible world-space region (the viewport).
    [[nodiscard]] auto visible_region() const -> AABB;

    /// Screen dimensions.
    [[nodiscard]] auto screen_width() const -> double;
    [[nodiscard]] auto screen_height() const -> double;

    /// Zoom constraints.
    static constexpr double kMinZoom = 0.05;  // 5%
    static constexpr double kMaxZoom = 20.0;  // 2000%

    /// Fit a world-space AABB into the current screen, adjusting zoom and pan.
    auto fit_to_bounds(const AABB& world_bounds, double padding = 50.0) -> void;

private:
    double zoom_{1.0};
    Point2D pan_{0.0, 0.0};
    double screen_width_{800.0};
    double screen_height_{600.0};
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `Transform2D::apply()` -- Apply rotation, scale, then translation to a point. Use standard 2D rotation matrix: `x' = cos(r)*x - sin(r)*y`, `y' = sin(r)*x + cos(r)*y`, then scale and translate.

2. `Transform2D::inverse()` -- Reverse the transform: un-translate, un-scale, un-rotate.

3. `AABB::contains()` -- `p.x >= min_x && p.x <= max_x && p.y >= min_y && p.y <= max_y`.

4. `AABB::intersects()` -- Standard AABB overlap test: no separation on either axis.

5. `CanvasObject::world_bounds()` -- Transform the four corners of `local_bounds()` using `transform_`, then compute the enclosing AABB of the transformed corners.

6. `Quadtree::insert()` -- Recursive insertion. If a leaf node has fewer than `max_entries_per_node_` entries, insert directly. Otherwise subdivide into 4 quadrants and redistribute.

7. `Quadtree::query_region()` -- Recursive: if node bounds don't intersect query region, skip. Otherwise check entries and recurse into children.

8. `ViewportTransform::world_to_screen()` -- `screen = (world - pan) * zoom`.

9. `ViewportTransform::screen_to_world()` -- `world = screen / zoom + pan`.

10. `ViewportTransform::zoom_at()` -- Adjust pan so the point under the cursor stays fixed: `new_pan = screen_point / new_zoom - screen_point / old_zoom + old_pan`.

## Events to Add

```cpp
// In Events.h -- Canvas section

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasViewportChangedEvent)
double zoom{1.0};
double pan_x{0.0};
double pan_y{0.0};
double visible_min_x{0.0};
double visible_min_y{0.0};
double visible_max_x{0.0};
double visible_max_y{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectAddedEvent)
uint64_t object_id{0};
uint8_t object_type{0}; // CanvasObjectType as uint8_t
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectRemovedEvent)
uint64_t object_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectModifiedEvent)
uint64_t object_id{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_canvas_types.cpp`

1. **Point2D distance** -- Verify distance_to computes Euclidean distance correctly.
2. **Point2D arithmetic** -- Verify +, -, * operators.
3. **Transform2D identity** -- Verify identity().apply(p) == p.
4. **Transform2D translation** -- Apply translate-only transform, verify point moves.
5. **Transform2D rotation** -- Rotate 90 degrees, verify (1,0) becomes (0,1).
6. **Transform2D inverse** -- Apply then inverse, verify round-trip to original point.
7. **Transform2D compose** -- Compose two transforms, verify equivalent to sequential application.
8. **AABB contains** -- Point inside returns true, outside returns false.
9. **AABB intersects** -- Overlapping AABBs return true, separated return false.
10. **AABB merged** -- Merged AABB encloses both inputs.
11. **CanvasColor hex round-trip** -- from_hex("#FF8800") then to_hex() returns "#ff8800".

File: `tests/unit/test_quadtree.cpp`

12. **Insert and query point** -- Insert 10 objects, query a point inside one, verify found.
13. **Query region** -- Insert scattered objects, query a sub-region, verify only intersecting objects returned.
14. **Remove** -- Insert, remove, verify query no longer finds it.
15. **Update bounds** -- Insert, update bounds, verify new bounds are used.
16. **Large dataset** -- Insert 10,000 objects, verify query_region performance is sub-linear vs naive scan.

File: `tests/unit/test_viewport_transform.cpp`

17. **World to screen identity** -- At zoom=1.0, pan=(0,0), world == screen.
18. **World to screen with zoom** -- At zoom=2.0, world (50,50) -> screen (100,100).
19. **World to screen with pan** -- Pan=(100,0), world (150,0) -> screen (50,0).
20. **Screen to world round-trip** -- Verify world_to_screen then screen_to_world returns original.
21. **Zoom at cursor** -- Zoom in at a screen point, verify that point maps to same world coord before and after.
22. **Visible region** -- Verify visible_region returns correct world AABB for current zoom/pan.
23. **Fit to bounds** -- Fit a known AABB, verify zoom and pan center the content.
24. **Zoom clamping** -- Set zoom below kMinZoom, verify clamped. Same for above kMaxZoom.

## Acceptance Criteria

- [ ] CanvasObject base class with id, transform, z_index, metadata, tags, lock, visibility, opacity
- [ ] Transform2D supports apply, inverse, compose with rotation + scale + translation
- [ ] AABB supports contains, intersects, merged, expanded, from_center_size
- [ ] Quadtree supports insert, remove, update, query_region, query_point with max depth 8
- [ ] ViewportTransform converts between world and screen coordinates with zoom + pan
- [ ] ViewportTransform::zoom_at keeps the cursor point fixed
- [ ] CanvasObject::world_bounds computes transformed AABB correctly
- [ ] ObjectId is a monotonic 64-bit counter, kInvalidObjectId == 0
- [ ] All 24 test cases pass
- [ ] C++23, Allman brace style, 4-space indent, trailing return types, markamp::canvas namespace

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CanvasTypes.h` | Point2D, Size2D, Transform2D, AABB, CanvasColor, CanvasObjectType |
| CREATE | `src/canvas/CanvasTypes.cpp` | Transform2D, AABB, CanvasColor implementations |
| CREATE | `src/canvas/CanvasObject.h` | Base CanvasObject class |
| CREATE | `src/canvas/CanvasObject.cpp` | CanvasObject implementation + ID generator |
| CREATE | `src/canvas/Quadtree.h` | Quadtree spatial index |
| CREATE | `src/canvas/Quadtree.cpp` | Quadtree implementation |
| CREATE | `src/canvas/ViewportTransform.h` | World/screen coordinate mapping |
| CREATE | `src/canvas/ViewportTransform.cpp` | ViewportTransform implementation |
| MODIFY | `src/core/Events.h` | Add Canvas event declarations |
| MODIFY | `CMakeLists.txt` | Add src/canvas/ sources, create canvas library target |
| CREATE | `tests/unit/test_canvas_types.cpp` | 11 tests for Point2D, Transform2D, AABB, CanvasColor |
| CREATE | `tests/unit/test_quadtree.cpp` | 5 tests for Quadtree spatial index |
| CREATE | `tests/unit/test_viewport_transform.cpp` | 8 tests for ViewportTransform |

## Architecture Notes

- All canvas code lives under `src/canvas/` in the `markamp::canvas` namespace. This is a new source directory.
- CanvasObject is a pure data model; it does not know about wxWidgets, EventBus, or rendering.
- The Quadtree is a standalone spatial index used by both the rendering pipeline (viewport culling) and the input system (hit-testing).
- ViewportTransform is stateless math; it can be used anywhere without dependencies.
- The monotonic ObjectId generator should use `std::atomic<uint64_t>` for thread safety.
- JSON serialization uses nlohmann/json (already available in the project via the extension manifest system).

## Estimated Complexity

**L** -- Multiple interrelated data structures (Transform2D math, AABB geometry, Quadtree recursion, ViewportTransform), a polymorphic base class, and 24 unit tests.
