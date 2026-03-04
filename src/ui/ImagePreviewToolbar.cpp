#include "ImagePreviewToolbar.h"

#include "core/Events.h"

namespace markamp::ui
{

ImagePreviewToolbar::ImagePreviewToolbar(wxWindow* parent,
                                         core::ThemeEngine& theme_engine,
                                         core::EventBus& event_bus)
    : FloatingToolbar(parent, theme_engine, event_bus, "image_toolbar")
{
    SetDraggable(true);
    SetSnapToEdges(true);
    SetAutoHideMs(kDefaultAutoHideMs);
    BuildButtons();
}

void ImagePreviewToolbar::ShowForImage(const wxPoint& position)
{
    ShowAt(position);
}

void ImagePreviewToolbar::set_on_zoom(ZoomCallback callback)
{
    on_zoom_ = std::move(callback);
}

void ImagePreviewToolbar::set_on_rotate(RotateCallback callback)
{
    on_rotate_ = std::move(callback);
}

auto ImagePreviewToolbar::current_zoom() const -> float
{
    return current_zoom_;
}

auto ImagePreviewToolbar::current_rotation() const -> int
{
    return current_rotation_;
}

void ImagePreviewToolbar::BuildButtons()
{
    std::vector<FloatingToolbarButton> buttons;

    // Zoom In
    FloatingToolbarButton zoom_in_btn;
    zoom_in_btn.id = "zoom_in";
    zoom_in_btn.label = "+";
    zoom_in_btn.tooltip = "Zoom In (25%)";
    zoom_in_btn.callback = [this]()
    {
        current_zoom_ += 0.25F;
        if (on_zoom_)
        {
            on_zoom_(current_zoom_);
        }
    };
    buttons.push_back(std::move(zoom_in_btn));

    // Zoom Out
    FloatingToolbarButton zoom_out_btn;
    zoom_out_btn.id = "zoom_out";
    zoom_out_btn.label = "\u2212"; // −
    zoom_out_btn.tooltip = "Zoom Out (25%)";
    zoom_out_btn.callback = [this]()
    {
        current_zoom_ = std::max(0.25F, current_zoom_ - 0.25F);
        if (on_zoom_)
        {
            on_zoom_(current_zoom_);
        }
    };
    buttons.push_back(std::move(zoom_out_btn));

    // Fit
    FloatingToolbarButton fit_btn;
    fit_btn.id = "fit";
    fit_btn.label = "\u2922"; // ⤢
    fit_btn.tooltip = "Zoom to Fit";
    fit_btn.callback = [this]()
    {
        current_zoom_ = 1.0F;
        if (on_zoom_)
        {
            on_zoom_(current_zoom_);
        }
    };
    buttons.push_back(std::move(fit_btn));

    // Separator
    FloatingToolbarButton sep1;
    sep1.is_separator = true;
    buttons.push_back(std::move(sep1));

    // Rotate Left
    FloatingToolbarButton rotate_left_btn;
    rotate_left_btn.id = "rotate_left";
    rotate_left_btn.label = "\u21BA"; // ↺
    rotate_left_btn.tooltip = "Rotate Left (90\u00B0)";
    rotate_left_btn.callback = [this]()
    {
        current_rotation_ = (current_rotation_ - 90 + 360) % 360;
        if (on_rotate_)
        {
            on_rotate_(current_rotation_);
        }
    };
    buttons.push_back(std::move(rotate_left_btn));

    // Rotate Right
    FloatingToolbarButton rotate_right_btn;
    rotate_right_btn.id = "rotate_right";
    rotate_right_btn.label = "\u21BB"; // ↻
    rotate_right_btn.tooltip = "Rotate Right (90\u00B0)";
    rotate_right_btn.callback = [this]()
    {
        current_rotation_ = (current_rotation_ + 90) % 360;
        if (on_rotate_)
        {
            on_rotate_(current_rotation_);
        }
    };
    buttons.push_back(std::move(rotate_right_btn));

    // Separator
    FloatingToolbarButton sep2;
    sep2.is_separator = true;
    buttons.push_back(std::move(sep2));

    // Copy Image
    FloatingToolbarButton copy_btn;
    copy_btn.id = "copy_image";
    copy_btn.label = "\u2398"; // ⎘
    copy_btn.tooltip = "Copy Image";
    copy_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "image.copy";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(copy_btn));

    // Open Original
    FloatingToolbarButton open_btn;
    open_btn.id = "open_original";
    open_btn.label = "\u2197"; // ↗
    open_btn.tooltip = "Open in System Viewer";
    open_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "image.open_external";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(open_btn));

    SetButtons(std::move(buttons));
}

} // namespace markamp::ui
