#include "MinimapHoverToolbar.h"

#include "core/Events.h"

namespace markamp::ui
{

MinimapHoverToolbar::MinimapHoverToolbar(wxWindow* parent,
                                         core::ThemeEngine& theme_engine,
                                         core::EventBus& event_bus)
    : FloatingToolbar(parent, theme_engine, event_bus, "minimap_toolbar")
{
    SetDraggable(false);
    SetSnapToEdges(false);
    SetAutoHideMs(kDefaultAutoHideMs);
    BuildButtons();
}

void MinimapHoverToolbar::ShowNearMinimap(const wxPoint& minimap_pos)
{
    // Position to the left of the minimap
    const auto toolbar_size = FloatingToolbar::CalculateSize();
    const wxPoint offset_pos(minimap_pos.x - toolbar_size.x - 8, minimap_pos.y);
    ShowAt(offset_pos);
}

void MinimapHoverToolbar::BuildButtons()
{
    std::vector<FloatingToolbarButton> buttons;

    // Zoom to Fit
    FloatingToolbarButton zoom_fit_btn;
    zoom_fit_btn.id = "zoom_fit";
    zoom_fit_btn.label = "\u2922"; // ⤢ (expand)
    zoom_fit_btn.tooltip = "Zoom to Fit";
    zoom_fit_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "minimap.zoom_fit";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(zoom_fit_btn));

    // Navigate to Line
    FloatingToolbarButton nav_btn;
    nav_btn.id = "navigate";
    nav_btn.label = "\u2192"; // →
    nav_btn.tooltip = "Navigate to Line";
    nav_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "minimap.navigate";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(nav_btn));

    // Separator
    FloatingToolbarButton sep;
    sep.is_separator = true;
    buttons.push_back(std::move(sep));

    // Toggle Minimap
    FloatingToolbarButton toggle_btn;
    toggle_btn.id = "toggle";
    toggle_btn.label = "\u25A8"; // ▨
    toggle_btn.tooltip = "Toggle Minimap";
    toggle_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "minimap.toggle";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(toggle_btn));

    // Switch Side
    FloatingToolbarButton side_btn;
    side_btn.id = "switch_side";
    side_btn.label = "\u21C4"; // ⇄
    side_btn.tooltip = "Switch Minimap Side";
    side_btn.callback = [this]()
    {
        core::events::CommandExecutedEvent evt;
        evt.command_id = "minimap.switch_side";
        evt.source = "toolbar";
        event_bus_.publish(evt);
    };
    buttons.push_back(std::move(side_btn));

    SetButtons(std::move(buttons));
}

} // namespace markamp::ui
