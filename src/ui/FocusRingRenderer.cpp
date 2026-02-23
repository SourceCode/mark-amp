#include "FocusRingRenderer.h"

#include <wx/dcclient.h>
#include <wx/dcmemory.h>
#include <wx/graphics.h>

namespace markamp::ui
{

FocusRingRenderer::FocusRingRenderer()
    : animator_([this]() { trigger_redraw(); })
{
}

FocusRingRenderer::~FocusRingRenderer()
{
    if (focus_listener_ != 0)
    {
        FocusManager::get().remove_focus_listener(focus_listener_);
    }
}

auto FocusRingRenderer::get() -> FocusRingRenderer&
{
    static FocusRingRenderer instance;
    return instance;
}

void FocusRingRenderer::initialize()
{
    focus_listener_ = FocusManager::get().on_focus_changed([this](FocusZoneId zone, int item_index)
                                                           { on_focus_changed(zone, item_index); });
}

void FocusRingRenderer::register_item_bounds(FocusZoneId zone,
                                             int item_index,
                                             wxWindow* window,
                                             const wxRect& client_bounds)
{
    if (!window)
        return;

    std::lock_guard lock(mutex_);
    auto key = std::make_tuple(zone, item_index);
    registry_[key] = {window, client_bounds};

    // If this is the currently focused item, update its target rect
    // This allows responsive resizing when the window resizes
    if (FocusManager::get().current_zone() == zone &&
        FocusManager::get().current_item() == item_index)
    {
        wxPoint screen_pt = window->ClientToScreen(client_bounds.GetPosition());
        wxRect screen_rect(screen_pt, client_bounds.GetSize());
        // Only update if dimensions actually changed to avoid jarring re-animations
        if (animator_.get_current_rect() != screen_rect)
        {
            animator_.set_target(screen_rect);
        }
    }
}

void FocusRingRenderer::on_focus_changed(FocusZoneId zone, int item_index)
{
    std::lock_guard lock(mutex_);

    // In case no specific item is focused (e.g., losing focus entirely)
    if (item_index == -1)
    {
        animator_.hide();
        trigger_redraw(); // one last redraw to hide it

        previous_window_ = active_window_;
        active_window_ = nullptr;
        return;
    }

    auto key = std::make_tuple(zone, item_index);
    auto it = registry_.find(key);
    if (it != registry_.end())
    {
        const auto& record = it->second;
        if (record.window)
        {
            // Convert to screen coordinates so animator tracks absolute path
            wxPoint screen_pt = record.window->ClientToScreen(record.client_bounds.GetPosition());
            wxRect screen_rect(screen_pt, record.client_bounds.GetSize());
            animator_.set_target(screen_rect);

            previous_window_ = active_window_;
            active_window_ = record.window;
        }
    }
    else
    {
        // Could be a dynamically generated item not yet rendered/registered
        // We'll hide the ring until it's registered.
        animator_.hide();
        previous_window_ = active_window_;
        active_window_ = nullptr;
    }
}

void FocusRingRenderer::trigger_redraw()
{
    // The animator tells us the properties are changing, we should trigger paint
    if (active_window_)
    {
        active_window_->Refresh();
    }
    if (previous_window_ && previous_window_ != active_window_)
    {
        previous_window_->Refresh();
    }
}

} // namespace markamp::ui
