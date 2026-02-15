#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasRenderer.h"
#include "canvas/CanvasTypes.h"
#include "canvas/Quadtree.h"
#include "canvas/TileCache.h"
#include "canvas/ViewportTransform.h"
#include "core/EventBus.h"

#include <wx/panel.h>

#include <memory>
#include <optional>
#include <vector>

namespace markamp::canvas
{

/// wxPanel subclass that provides the canvas drawing surface.
/// Manages the scene graph (object storage + quadtree), viewport,
/// renderer, and serves as the integration point between model and UI.
class CanvasPanel : public wxPanel
{
public:
    CanvasPanel(wxWindow* parent, wxWindowID panel_id, std::shared_ptr<core::EventBus> event_bus);
    ~CanvasPanel() override;

    // ── Object Management ──────────────────────────────────────────

    /// Add an object to the canvas. Takes ownership.
    auto add_object(std::unique_ptr<CanvasObject> obj) -> ObjectId;

    /// Remove an object by ID.
    auto remove_object(ObjectId obj_id) -> bool;

    /// Get a read-only pointer to an object. Returns nullptr if not found.
    [[nodiscard]] auto get_object(ObjectId obj_id) const -> const CanvasObject*;

    /// Get a mutable pointer to an object. Returns nullptr if not found.
    auto get_object_mut(ObjectId obj_id) -> CanvasObject*;

    /// Number of objects on the canvas.
    [[nodiscard]] auto object_count() const -> size_t;

    /// Retrieve all objects sorted by z-index for rendering.
    [[nodiscard]] auto objects_by_z_order() const -> std::vector<CanvasObject*>;

    // ── Spatial Queries ────────────────────────────────────────────

    /// Find objects that intersect a world-space rectangle.
    [[nodiscard]] auto query_region(const AABB& region) const -> std::vector<ObjectId>;

    /// Find objects at a world-space point (hit test).
    [[nodiscard]] auto query_point(const Point2D& point) const -> std::vector<ObjectId>;

    // ── Viewport & Rendering ───────────────────────────────────────

    [[nodiscard]] auto viewport() const -> const ViewportTransform&;
    auto viewport_mut() -> ViewportTransform&;

    [[nodiscard]] auto renderer() -> CanvasRenderer&;

    /// Request a full repaint.
    auto request_repaint() -> void;

    /// Notify the quadtree that an object's bounds have changed.
    auto update_object_bounds(ObjectId obj_id) -> void;

    /// Rebuild the entire quadtree from current objects.
    auto rebuild_spatial_index() -> void;

private:
    // wxWidgets event handlers.
    auto on_paint(wxPaintEvent& event) -> void;
    auto on_size(wxSizeEvent& event) -> void;

    // Core systems.
    std::shared_ptr<core::EventBus> event_bus_;
    ViewportTransform viewport_;
    CanvasRenderer renderer_;
    Quadtree spatial_index_;
    TileCache tile_cache_;

    // Scene graph: object storage.
    std::vector<std::unique_ptr<CanvasObject>> objects_;

    // Object ID → index lookup.
    std::unordered_map<ObjectId, size_t> id_to_index_;
};

} // namespace markamp::canvas
