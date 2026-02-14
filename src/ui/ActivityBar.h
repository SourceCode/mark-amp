#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Thin vertical icon rail on the left side of the window, inspired by VS Code's activity bar.
/// Shows icons for File Explorer, Search, Settings, and Themes.
/// Clicking an icon fires an ActivityBarSelectionEvent.
class ActivityBar : public wxPanel
{
public:
    ActivityBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    /// Set the currently active item (visually highlights it)
    void SetActiveItem(core::events::ActivityBarItem item);

    /// Get the currently active item
    [[nodiscard]] auto GetActiveItem() const -> core::events::ActivityBarItem;

    static constexpr int kBarWidth = 48;
    static constexpr int kIconSize = 24;
    static constexpr int kIconPadding = 12;

    /// R18 Fix 25: Set badge count on an activity bar item
    void SetBadge(core::events::ActivityBarItem item, int count);

private:
    struct BarItem
    {
        core::events::ActivityBarItem item_id;
        std::string label;     // Tooltip text
        std::string icon_char; // Unicode/emoji fallback icon
        wxRect bounds;         // Hit-test rectangle
        int badge_count{0};    // R18 Fix 25: Badge count indicator
    };

    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;

    std::vector<BarItem> items_;
    core::events::ActivityBarItem active_item_{core::events::ActivityBarItem::FileExplorer};
    int hover_index_{-1};
    int pressed_index_{-1}; // R20 Fix 18: index of item being pressed

    core::Subscription theme_sub_;

    void CreateItems();
    void ApplyTheme();

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);     // R20 Fix 18: release press
    void OnDoubleClick(wxMouseEvent& event); // R20 Fix 16: collapse sidebar
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    auto HitTest(const wxPoint& pos) const -> int;
};

} // namespace markamp::ui
