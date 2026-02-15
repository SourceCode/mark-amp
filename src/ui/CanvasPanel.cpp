#include "CanvasPanel.h"

#include "core/Events.h"

#include <wx/dcbuffer.h>
#include <wx/graphics.h>

#include <algorithm>

namespace markamp::canvas
{

CanvasPanel::CanvasPanel(wxWindow* parent,
                         wxWindowID panel_id,
                         std::shared_ptr<core::EventBus> event_bus)
    : wxPanel(parent,
              panel_id,
              wxDefaultPosition,
              wxDefaultSize,
              wxFULL_REPAINT_ON_RESIZE | wxBORDER_NONE)
    , event_bus_(std::move(event_bus))
    , spatial_index_(AABB{-50000.0, -50000.0, 50000.0, 50000.0})
{
    SetBackgroundStyle(wxBG_STYLE_PAINT);
    SetDoubleBuffered(true);

    Bind(wxEVT_PAINT, &CanvasPanel::on_paint, this);
    Bind(wxEVT_SIZE, &CanvasPanel::on_size, this);
}

CanvasPanel::~CanvasPanel() = default;

// ── Object Management ──────────────────────────────────────────────

auto CanvasPanel::add_object(std::unique_ptr<CanvasObject> obj) -> ObjectId
{
    const auto obj_id = obj->id();
    const auto world_box = obj->world_bounds();

    id_to_index_[obj_id] = objects_.size();
    objects_.push_back(std::move(obj));

    spatial_index_.insert(obj_id, world_box);

    if (event_bus_)
    {
        core::events::CanvasObjectAddedEvent evt;
        evt.object_id = obj_id;
        evt.object_type = static_cast<uint8_t>(objects_.back()->type());
        event_bus_->publish(evt);
    }

    return obj_id;
}

auto CanvasPanel::remove_object(ObjectId obj_id) -> bool
{
    const auto it = id_to_index_.find(obj_id);
    if (it == id_to_index_.end())
    {
        return false;
    }

    const auto idx = it->second;

    // Swap-and-pop removal for O(1) erase.
    if (idx < objects_.size() - 1)
    {
        const auto moved_id = objects_.back()->id();
        std::swap(objects_[idx], objects_.back());
        id_to_index_[moved_id] = idx;
    }

    objects_.pop_back();
    id_to_index_.erase(it);
    spatial_index_.remove(obj_id);

    if (event_bus_)
    {
        core::events::CanvasObjectRemovedEvent evt;
        evt.object_id = obj_id;
        event_bus_->publish(evt);
    }

    return true;
}

auto CanvasPanel::get_object(ObjectId obj_id) const -> const CanvasObject*
{
    const auto it = id_to_index_.find(obj_id);
    if (it == id_to_index_.end())
    {
        return nullptr;
    }
    return objects_[it->second].get();
}

auto CanvasPanel::get_object_mut(ObjectId obj_id) -> CanvasObject*
{
    const auto it = id_to_index_.find(obj_id);
    if (it == id_to_index_.end())
    {
        return nullptr;
    }
    return objects_[it->second].get();
}

auto CanvasPanel::object_count() const -> size_t
{
    return objects_.size();
}

auto CanvasPanel::objects_by_z_order() const -> std::vector<CanvasObject*>
{
    std::vector<CanvasObject*> sorted;
    sorted.reserve(objects_.size());
    for (const auto& obj : objects_)
    {
        sorted.push_back(obj.get());
    }
    std::sort(sorted.begin(),
              sorted.end(),
              [](const CanvasObject* lhs, const CanvasObject* rhs)
              { return lhs->z_index() < rhs->z_index(); });
    return sorted;
}

// ── Spatial Queries ────────────────────────────────────────────────

auto CanvasPanel::query_region(const AABB& region) const -> std::vector<ObjectId>
{
    return spatial_index_.query_region(region);
}

auto CanvasPanel::query_point(const Point2D& point) const -> std::vector<ObjectId>
{
    return spatial_index_.query_point(point);
}

// ── Viewport & Rendering ───────────────────────────────────────────

auto CanvasPanel::viewport() const -> const ViewportTransform&
{
    return viewport_;
}

auto CanvasPanel::viewport_mut() -> ViewportTransform&
{
    return viewport_;
}

auto CanvasPanel::renderer() -> CanvasRenderer&
{
    return renderer_;
}

auto CanvasPanel::request_repaint() -> void
{
    Refresh();
}

auto CanvasPanel::update_object_bounds(ObjectId obj_id) -> void
{
    const auto* obj = get_object(obj_id);
    if (obj)
    {
        spatial_index_.update(obj_id, obj->world_bounds());
    }
}

auto CanvasPanel::rebuild_spatial_index() -> void
{
    std::vector<Quadtree::Entry> entries;
    entries.reserve(objects_.size());
    for (const auto& obj : objects_)
    {
        entries.push_back({obj->id(), obj->world_bounds()});
    }
    spatial_index_.rebuild(entries);
}

// ── wxWidgets Event Handlers ───────────────────────────────────────

auto CanvasPanel::on_paint(wxPaintEvent& /*event*/) -> void
{
    wxAutoBufferedPaintDC dc(this);
    std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));

    if (!gc)
    {
        return;
    }

    const auto sorted = objects_by_z_order();
    const std::vector<ObjectId> selected_ids; // Phase 4 will populate this.

    renderer_.render_frame(*gc, sorted, viewport_, selected_ids);
}

auto CanvasPanel::on_size(wxSizeEvent& event) -> void
{
    const auto client_size = GetClientSize();
    viewport_.set_screen_size(static_cast<double>(client_size.GetWidth()),
                              static_cast<double>(client_size.GetHeight()));

    tile_cache_.invalidate_all();

    if (event_bus_)
    {
        const auto visible = viewport_.visible_region();
        core::events::CanvasViewportChangedEvent evt;
        evt.zoom = viewport_.zoom();
        evt.pan_x = viewport_.pan().x;
        evt.pan_y = viewport_.pan().y;
        evt.visible_min_x = visible.min_x;
        evt.visible_min_y = visible.min_y;
        evt.visible_max_x = visible.max_x;
        evt.visible_max_y = visible.max_y;
        event_bus_->publish(evt);
    }

    Refresh();
    event.Skip();
}

} // namespace markamp::canvas
