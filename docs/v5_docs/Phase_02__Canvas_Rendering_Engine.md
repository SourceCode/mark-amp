# Phase 02 -- Canvas Rendering Engine

## Objective

Implement the tile-based rendering pipeline that draws canvas objects to the screen using wxGraphicsContext for anti-aliased 2D output. This includes the CanvasPanel (wxPanel subclass), viewport culling via the Quadtree, a tile cache for off-screen portions, and the object rendering dispatch system that routes each CanvasObjectType to its specialized renderer.

## Prerequisites

- Phase 01 (CanvasTypes, CanvasObject, Quadtree, ViewportTransform)
- Existing ThemeEngine (`src/core/ThemeEngine.h`)
- Existing EventBus (`src/core/EventBus.h`)

## Feature References (PRD)

- PRD #1: Infinite Canvas (rendering the canvas)
- PRD #99: Board Performance Optimization (tile caching, viewport culling)
- PRD #41: Grid & Snap (grid background rendering)

## Data Structures to Implement

### File: `src/canvas/CanvasRenderer.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"
#include "ViewportTransform.h"

#include <memory>
#include <unordered_map>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

/// Abstract renderer interface for a specific canvas object type.
class IObjectRenderer
{
public:
    virtual ~IObjectRenderer() = default;
    virtual auto render(wxGraphicsContext& gc, const CanvasObject& obj,
                        const ViewportTransform& vp) -> void = 0;
    [[nodiscard]] virtual auto handles_type() const -> CanvasObjectType = 0;
};

/// Central rendering coordinator. Holds registered IObjectRenderers and
/// orchestrates the draw pipeline: background -> grid -> objects (by z-order)
/// -> selection overlays -> UI chrome.
class CanvasRenderer
{
public:
    CanvasRenderer();

    /// Register a renderer for a specific object type.
    auto register_renderer(std::unique_ptr<IObjectRenderer> renderer) -> void;

    /// Main render entry point. Draws all visible objects to the given graphics context.
    /// @param gc           wxGraphicsContext for the current frame
    /// @param viewport     Current viewport transform
    /// @param objects      All objects sorted by z-index (ascending)
    /// @param visible_ids  Object IDs in the current viewport (from Quadtree query)
    auto render_frame(wxGraphicsContext& gc,
                      const ViewportTransform& viewport,
                      const std::vector<const CanvasObject*>& objects,
                      const std::vector<ObjectId>& visible_ids) -> void;

    /// Render the canvas background (solid color or pattern).
    auto render_background(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void;

    /// Render the grid overlay.
    auto render_grid(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void;

    // --- Grid settings ---
    auto set_grid_visible(bool visible) -> void;
    auto set_grid_spacing(double spacing) -> void;
    auto set_grid_color(const CanvasColor& color) -> void;
    auto set_background_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto is_grid_visible() const -> bool;
    [[nodiscard]] auto grid_spacing() const -> double;

private:
    std::unordered_map<CanvasObjectType, std::unique_ptr<IObjectRenderer>> renderers_;
    bool grid_visible_{true};
    double grid_spacing_{20.0};
    CanvasColor grid_color_{200, 200, 200, 60};
    CanvasColor background_color_{245, 245, 245, 255};
};

} // namespace markamp::canvas
```

### File: `src/canvas/TileCache.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <memory>
#include <unordered_map>

class wxBitmap;

namespace markamp::canvas
{

/// Tile coordinate (column, row at a given zoom level).
struct TileKey
{
    int col{0};
    int row{0};
    int zoom_level{0}; // Discretized zoom (e.g., floor(log2(zoom)))

    auto operator==(const TileKey& other) const -> bool = default;
};

struct TileKeyHash
{
    auto operator()(const TileKey& key) const -> size_t;
};

/// Off-screen tile cache for pre-rendered canvas regions.
/// Tiles are fixed-size bitmaps (e.g., 256x256) rendered at the current zoom level.
/// Cache is invalidated when objects change or zoom level changes.
class TileCache
{
public:
    static constexpr int kTileSize = 256;

    TileCache();

    /// Get a cached tile. Returns nullptr if not cached.
    [[nodiscard]] auto get_tile(const TileKey& key) const -> wxBitmap*;

    /// Store a rendered tile.
    auto put_tile(const TileKey& key, std::unique_ptr<wxBitmap> bitmap) -> void;

    /// Invalidate tiles that intersect a world-space region.
    auto invalidate_region(const AABB& world_region, int zoom_level) -> void;

    /// Invalidate all tiles.
    auto invalidate_all() -> void;

    /// Compute which tile keys are needed for the given viewport.
    [[nodiscard]] auto tiles_for_viewport(const AABB& visible_region,
                                           double zoom) const -> std::vector<TileKey>;

    /// Evict tiles exceeding the cache budget.
    auto evict_if_needed() -> void;

    /// Set maximum number of cached tiles.
    auto set_max_tiles(size_t max) -> void;

    [[nodiscard]] auto cached_tile_count() const -> size_t;

private:
    std::unordered_map<TileKey, std::unique_ptr<wxBitmap>, TileKeyHash> tiles_;
    size_t max_tiles_{512};
};

} // namespace markamp::canvas
```

### File: `src/ui/CanvasPanel.h`

```cpp
#pragma once

#include "../canvas/CanvasRenderer.h"
#include "../canvas/CanvasTypes.h"
#include "../canvas/Quadtree.h"
#include "../canvas/ViewportTransform.h"

#include <wx/panel.h>

#include <memory>
#include <vector>

namespace markamp::core
{
class EventBus;
class Config;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::canvas
{
class TileCache;
} // namespace markamp::canvas

namespace markamp::ui
{

/// The main wxPanel that hosts the infinite canvas.
/// Handles paint events, timer-driven animation, and delegates input.
class CanvasPanel : public wxPanel
{
public:
    CanvasPanel(wxWindow* parent,
                core::EventBus& event_bus,
                core::Config& config,
                core::ThemeEngine& theme_engine);

    // --- Object management ---
    auto add_object(std::unique_ptr<canvas::CanvasObject> obj) -> canvas::ObjectId;
    auto remove_object(canvas::ObjectId id) -> bool;
    [[nodiscard]] auto get_object(canvas::ObjectId id) -> canvas::CanvasObject*;
    [[nodiscard]] auto get_object(canvas::ObjectId id) const -> const canvas::CanvasObject*;
    [[nodiscard]] auto object_count() const -> size_t;

    // --- Viewport ---
    [[nodiscard]] auto viewport() const -> const canvas::ViewportTransform&;
    auto set_zoom(double zoom) -> void;
    auto zoom_to_fit() -> void;
    auto center_on(const canvas::Point2D& world_point) -> void;

    // --- Spatial queries ---
    [[nodiscard]] auto objects_in_region(const canvas::AABB& region) const
        -> std::vector<canvas::ObjectId>;
    [[nodiscard]] auto object_at_point(const canvas::Point2D& screen_point) const
        -> std::vector<canvas::ObjectId>;

    // --- Renderer ---
    auto renderer() -> canvas::CanvasRenderer&;

    // --- Invalidation ---
    auto invalidate_object(canvas::ObjectId id) -> void;
    auto invalidate_all() -> void;

private:
    // wxWidgets event handlers
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnEraseBackground(wxEraseEvent& event);

    core::EventBus& event_bus_;
    core::Config& config_;
    core::ThemeEngine& theme_engine_;

    canvas::ViewportTransform viewport_;
    canvas::CanvasRenderer renderer_;
    std::unique_ptr<canvas::TileCache> tile_cache_;
    canvas::Quadtree spatial_index_;

    /// Objects stored by ID for O(1) lookup.
    std::unordered_map<canvas::ObjectId, std::unique_ptr<canvas::CanvasObject>> objects_;

    /// Z-sorted list rebuilt on z-index changes.
    std::vector<canvas::ObjectId> z_sorted_ids_;
    bool z_order_dirty_{true};

    auto rebuild_z_order() -> void;
    auto publish_viewport_event() -> void;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. `CanvasPanel::OnPaint()` -- Create wxGraphicsContext from wxPaintDC. Query Quadtree for visible objects. Sort by z-index. Call `renderer_.render_frame()`. Blit tile cache for off-screen tiles.

2. `CanvasRenderer::render_frame()` -- Call render_background, render_grid, then iterate visible objects by z-order. For each, look up the IObjectRenderer by type and call `render()`. Fall back to a debug wireframe if no renderer registered.

3. `CanvasRenderer::render_grid()` -- Compute visible grid lines based on viewport. At high zoom, show fine grid; at low zoom, show coarse grid (adaptive grid density). Draw with semi-transparent lines via wxGraphicsContext.

4. `TileCache::tiles_for_viewport()` -- Compute tile column/row range that covers the visible AABB. Return TileKey vector.

5. `CanvasPanel::add_object()` -- Insert into objects_ map, insert into spatial_index_, mark z_order_dirty_, publish CanvasObjectAddedEvent, refresh.

6. `CanvasPanel::rebuild_z_order()` -- Collect all object IDs, sort by z_index ascending, store in z_sorted_ids_.

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `canvas.grid_visible` | bool | `true` | Show grid background |
| `canvas.grid_spacing` | double | `20.0` | Grid cell size in world units |
| `canvas.background_color` | string | `"#f5f5f5"` | Canvas background hex color |
| `canvas.tile_cache_size` | int | `512` | Max tiles in cache |
| `canvas.anti_aliasing` | bool | `true` | Enable anti-aliased rendering |

## Test Cases

File: `tests/unit/test_canvas_renderer.cpp`

1. **Renderer registration** -- Register an IObjectRenderer, verify handles_type returns correct type.
2. **Grid line computation** -- Verify grid lines computed for a known viewport cover the visible area.
3. **Tile key computation** -- Verify tiles_for_viewport returns correct keys for a 800x600 screen at zoom=1.0.
4. **Tile cache put/get** -- Store a tile, retrieve it, verify not null.
5. **Tile cache invalidation** -- Store tiles, invalidate a region, verify affected tiles removed.
6. **Tile cache eviction** -- Fill cache to capacity, verify oldest tiles evicted.
7. **Z-order sorting** -- Add objects with z-indices [5, 1, 3], verify sorted order is [1, 3, 5].
8. **Viewport culling** -- Add objects, verify only those in viewport appear in visible_ids.
9. **Object add/remove** -- Add object, verify object_count increments. Remove, verify decrements.
10. **Adaptive grid density** -- At zoom < 0.25, verify grid spacing increases (coarser grid).

## Acceptance Criteria

- [ ] CanvasPanel is a wxPanel subclass with OnPaint using wxGraphicsContext
- [ ] CanvasRenderer dispatches to registered IObjectRenderers by CanvasObjectType
- [ ] Grid rendering with adaptive density based on zoom level
- [ ] TileCache stores and retrieves pre-rendered 256x256 tiles
- [ ] Viewport culling via Quadtree: only visible objects are rendered
- [ ] Objects are rendered in z-index order (ascending)
- [ ] CanvasObjectAddedEvent/RemovedEvent published on add/remove
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CanvasRenderer.h` | Renderer coordinator + IObjectRenderer interface |
| CREATE | `src/canvas/CanvasRenderer.cpp` | Render pipeline implementation |
| CREATE | `src/canvas/TileCache.h` | Off-screen tile cache |
| CREATE | `src/canvas/TileCache.cpp` | TileCache implementation |
| CREATE | `src/ui/CanvasPanel.h` | wxPanel subclass for canvas |
| CREATE | `src/ui/CanvasPanel.cpp` | Paint, object management, spatial queries |
| MODIFY | `src/core/Events.h` | Verify canvas events from Phase 01 are present |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_canvas_renderer.cpp` | 10 Catch2 tests |

## Architecture Notes

- CanvasPanel owns all CanvasObject instances (via unique_ptr map). It is the single source of truth for the object graph.
- The Quadtree is rebuilt or updated incrementally as objects are added/removed/moved.
- wxGraphicsContext is created per-frame from wxPaintDC. It provides anti-aliased drawing via Core Graphics (macOS), Direct2D (Windows), or Cairo (Linux).
- The tile cache is optional -- it improves scrolling performance for boards with many static objects. During active editing, tiles are invalidated and re-rendered.
- IObjectRenderer is the extension point for all future object types. Each phase that adds a new object type also registers a renderer.
- The render pipeline order: background -> grid -> objects (z-order) -> selection handles -> drag previews -> UI overlays. Selection and drag rendering are added in Phases 3-4.

## Estimated Complexity

**XL** -- wxPanel integration with wxGraphicsContext, tile caching system, full render pipeline with z-ordering and viewport culling, adaptive grid, and the IObjectRenderer plugin architecture.
