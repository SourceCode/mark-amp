#include "CanvasInputManager.h"

#include "canvas/CanvasObject.h"
#include "core/Events.h"
#include "ui/CanvasPanel.h"

#include <algorithm>

namespace markamp::canvas
{

CanvasInputManager::CanvasInputManager(CanvasPanel& panel,
                                       std::shared_ptr<core::EventBus> event_bus)
    : panel_(panel)
    , event_bus_(std::move(event_bus))
{
}

auto CanvasInputManager::register_tool(std::unique_ptr<ICanvasTool> tool) -> void
{
    const auto mode_key = static_cast<uint8_t>(tool->tool_mode());
    tools_[mode_key] = std::move(tool);
}

auto CanvasInputManager::set_active_tool(ToolMode mode) -> bool
{
    const auto mode_key = static_cast<uint8_t>(mode);
    const auto tool_iter = tools_.find(mode_key);
    if (tool_iter == tools_.end())
    {
        return false;
    }

    if (active_tool_ != nullptr)
    {
        active_tool_->deactivate(panel_);
    }

    active_tool_ = tool_iter->second.get();
    active_tool_mode_ = mode;
    active_tool_->activate(panel_);

    if (event_bus_)
    {
        core::events::CanvasToolChangedEvent evt;
        evt.tool_mode = static_cast<uint8_t>(mode);
        event_bus_->publish(evt);
    }

    return true;
}

auto CanvasInputManager::active_tool() const -> ICanvasTool*
{
    return active_tool_;
}

auto CanvasInputManager::active_tool_mode() const -> ToolMode
{
    return active_tool_mode_;
}

auto CanvasInputManager::handle_mouse_down(const CanvasInputEvent& evt) -> bool
{
    if (active_tool_ == nullptr)
    {
        return false;
    }
    return active_tool_->on_mouse_down(panel_, evt);
}

auto CanvasInputManager::handle_mouse_up(const CanvasInputEvent& evt) -> bool
{
    if (active_tool_ == nullptr)
    {
        return false;
    }
    return active_tool_->on_mouse_up(panel_, evt);
}

auto CanvasInputManager::handle_mouse_move(const CanvasInputEvent& evt) -> bool
{
    if (active_tool_ == nullptr)
    {
        return false;
    }
    return active_tool_->on_mouse_move(panel_, evt);
}

auto CanvasInputManager::handle_mouse_scroll(const CanvasInputEvent& evt) -> bool
{
    if (active_tool_ == nullptr)
    {
        return false;
    }
    return active_tool_->on_mouse_scroll(panel_, evt);
}

auto CanvasInputManager::handle_key_down(int key_code, ModifierKeys mods) -> bool
{
    if (active_tool_ == nullptr)
    {
        return false;
    }
    return active_tool_->on_key_down(panel_, key_code, mods);
}

auto CanvasInputManager::handle_key_up(int key_code, ModifierKeys mods) -> bool
{
    if (active_tool_ == nullptr)
    {
        return false;
    }
    return active_tool_->on_key_up(panel_, key_code, mods);
}

auto CanvasInputManager::hit_test(const Point2D& world_point) const -> ObjectId
{
    const auto hits = panel_.query_point(world_point);
    if (hits.empty())
    {
        return kInvalidObjectId;
    }

    // Return topmost (highest z-index) visible, unlocked object.
    ObjectId best_id = kInvalidObjectId;
    int best_z = std::numeric_limits<int>::min();

    for (const auto object_id : hits)
    {
        const auto* obj = panel_.get_object(object_id);
        if (obj == nullptr || !obj->is_visible() || obj->is_locked())
        {
            continue;
        }
        if (obj->z_index() > best_z)
        {
            best_z = obj->z_index();
            best_id = object_id;
        }
    }

    return best_id;
}

auto CanvasInputManager::hit_test_all(const Point2D& world_point) const -> std::vector<ObjectId>
{
    auto hits = panel_.query_point(world_point);

    // Sort by z-index descending (topmost first).
    std::sort(hits.begin(),
              hits.end(),
              [this](ObjectId lhs_id, ObjectId rhs_id)
              {
                  const auto* lhs_obj = panel_.get_object(lhs_id);
                  const auto* rhs_obj = panel_.get_object(rhs_id);
                  const int lhs_z = (lhs_obj != nullptr) ? lhs_obj->z_index() : 0;
                  const int rhs_z = (rhs_obj != nullptr) ? rhs_obj->z_index() : 0;
                  return lhs_z > rhs_z;
              });

    return hits;
}

auto CanvasInputManager::set_space_held(bool held) -> void
{
    if (held && !space_held_)
    {
        previous_tool_mode_ = active_tool_mode_;
        set_active_tool(ToolMode::Pan);
    }
    else if (!held && space_held_)
    {
        set_active_tool(previous_tool_mode_);
    }
    space_held_ = held;
}

auto CanvasInputManager::is_space_held() const -> bool
{
    return space_held_;
}

// ── W05: Pointer Device & Capture ──────────────────────────────────

auto CanvasInputManager::set_device_type(PointerDeviceType device) -> void
{
    if (device_type_ == device)
    {
        return;
    }
    device_type_ = device;

    if (event_bus_)
    {
        core::events::PointerDeviceChangedEvent evt;
        switch (device)
        {
        case PointerDeviceType::kMouse: evt.device_type = "mouse"; break;
        case PointerDeviceType::kTrackpad: evt.device_type = "trackpad"; break;
        case PointerDeviceType::kStylus: evt.device_type = "stylus"; break;
        case PointerDeviceType::kTouch: evt.device_type = "touch"; break;
        }
        event_bus_->publish(evt);
    }
}

auto CanvasInputManager::set_captured(bool captured) -> void
{
    if (captured_ == captured)
    {
        return;
    }
    captured_ = captured;

    if (event_bus_)
    {
        core::events::PointerCaptureChangedEvent evt;
        evt.captured = captured;
        evt.tool_mode = static_cast<uint8_t>(active_tool_mode_);
        event_bus_->publish(evt);
    }
}

} // namespace markamp::canvas
