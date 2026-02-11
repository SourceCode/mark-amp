#pragma once

#include "ThemeAwareWindow.h"
#include "core/ThemeEngine.h"

namespace markamp::ui
{

class LayoutManager;

/// Draggable splitter bar between sidebar and content area.
/// 4px hit area, 1px visual line, cursor changes on hover.
class SplitterBar : public ThemeAwareWindow
{
public:
    SplitterBar(wxWindow* parent, core::ThemeEngine& theme_engine, LayoutManager* layout_manager);

    static constexpr int kHitWidth = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    LayoutManager* layout_manager_;
    bool is_dragging_{false};
    int drag_start_x_{0};
    int drag_start_width_{0};

    void OnPaint(wxPaintEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
};

} // namespace markamp::ui
