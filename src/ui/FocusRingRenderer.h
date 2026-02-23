#pragma once

#include "FocusManager.h"
#include "animation/FocusRingAnimator.h"
#include "core/ThemeEngine.h"

#include <wx/dc.h>
#include <wx/graphics.h>
#include <wx/window.h>

#include <map>
#include <mutex>
#include <tuple>

namespace markamp::ui
{

/// Global utility that coordinates focus rings across all UI surfaces.
/// Handles the morphing animation of the focus ring as it moves between
/// different windows and control zones.
class FocusRingRenderer
{
public:
    static auto get() -> FocusRingRenderer&;

    /// Start listening to global FocusManager events.
    /// Should be called during App startup.
    void initialize();

    /// Register a logical focusable item so the renderer knows its bounding box.
    /// \param zone The zone this item belongs to.
    /// \param item_index The integer index within the zone.
    /// \param window The window that claims this item (receives pain/refresh calls).
    /// \param client_bounds The bounding box relative to `window`.
    void register_item_bounds(FocusZoneId zone,
                              int item_index,
                              wxWindow* window,
                              const wxRect& client_bounds);

    /// Draw the global animated focus ring into the given device context.
    /// This should be called at the end of the `window`'s EVT_PAINT handler.
    template <typename DC>
    void draw(DC& dc, wxWindow* window, core::ThemeEngine& theme_engine)
    {
        if (!FocusManager::get().is_keyboard_mode_active())
            return;

        if (!window || !animator_.is_active())
            return;

        float alpha = animator_.get_alpha();
        if (alpha <= 0.01f)
            return;

        // Convert the animated screen rectangle back to this specific window's client coordinates
        wxRect screen_rect = animator_.get_current_rect();
        if (screen_rect.IsEmpty())
            return;

        wxPoint client_pt = window->ScreenToClient(screen_rect.GetPosition());
        wxRect client_rect(client_pt, screen_rect.GetSize());

        // If the focus rect doesn't intersect this window at all, no need to draw.
        // (We inflate slightly just to account for stroke width)
        wxRect bounds = window->GetClientRect();
        bounds.Inflate(4, 4);
        if (!bounds.Intersects(client_rect))
        {
            return;
        }

        std::unique_ptr<wxGraphicsContext> gc(wxGraphicsContext::Create(dc));
        if (!gc)
            return;

        // Expand by 2px to wrap around the component
        client_rect.Inflate(2, 2);

        auto color = theme_engine.resolve_token("focusRing").value_or(wxColour(0, 120, 215));
        // Apply alpha fading
        wxColour fade_color(
            color.Red(), color.Green(), color.Blue(), static_cast<wxByte>(255 * alpha));

        gc->SetPen(wxPen(fade_color, 2, wxPENSTYLE_SOLID));
        gc->SetBrush(*wxTRANSPARENT_BRUSH);

        // Rounded rectangle using 4px radius
        gc->DrawRoundedRectangle(client_rect.GetX(),
                                 client_rect.GetY(),
                                 client_rect.GetWidth(),
                                 client_rect.GetHeight(),
                                 4.0);
    }

private:
    FocusRingRenderer();
    ~FocusRingRenderer();

    void on_focus_changed(FocusZoneId zone, int item_index);
    void trigger_redraw();

    struct ItemRecord
    {
        wxWindow* window{nullptr};
        wxRect client_bounds;
    };

    std::map<std::tuple<FocusZoneId, int>, ItemRecord> registry_;
    mutable std::mutex mutex_;

    std::size_t focus_listener_{0};

    // We pass our own redraw callback into the animator
    animation::FocusRingAnimator animator_;

    wxWindow* active_window_{nullptr};
    wxWindow* previous_window_{nullptr};
};

} // namespace markamp::ui
