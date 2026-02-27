#pragma once

#include "ui/ThemeAwareWindow.h"

#include <wx/event.h>
#include <wx/panel.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{
class IconManager;

/// A horizontally arranged set of icon buttons for the Explorer panel
class ExplorerToolbar : public ThemeAwareWindow
{
public:
    ExplorerToolbar(wxWindow* parent, core::ThemeEngine& theme_engine, IconManager& icon_manager);

    enum class Action
    {
        kNewFile,
        kNewFolder,
        kCollapseAll,
        kRefresh,
        kFilter
    };

    using ActionCallback = std::function<void(Action)>;
    void SetActionCallback(ActionCallback callback);

protected:
    void OnPaint(wxPaintEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnLeftDown(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    IconManager& icon_manager_;
    ActionCallback callback_;

    struct ToolbarButton
    {
        Action action;
        std::string icon_name;
        wxRect bounds;
        std::string tooltip;
    };

    std::vector<ToolbarButton> buttons_;
    int hovered_index_{-1};
    int pressed_index_{-1};

    void UpdateLayout();
    int GetButtonHit(const wxPoint& pos) const;

    wxDECLARE_EVENT_TABLE();
};

} // namespace markamp::ui
