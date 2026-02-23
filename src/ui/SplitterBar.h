#pragma once

#include "DesignSystemContext.h"
#include "ThemeAwareWindow.h"
#include "animation/TransitionManager.h"

namespace markamp::ui
{

class LayoutManager;

/// Draggable splitter bar between sidebar and content area.
/// 4px hit area, 1px visual line, cursor changes on hover.
class SplitterBar : public ThemeAwareWindow
{
public:
    SplitterBar(wxWindow* parent, DesignSystemContext& ds, LayoutManager* layout_manager);

    void UpdateLayoutMetrics();

    static constexpr float kHoverFadeStep = 0.08F; // 8C: per-tick animation increment

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    LayoutManager* layout_manager_;
    bool is_dragging_{false};
    int drag_start_x_{0};
    int drag_start_width_{0};

    // 8C: Hover animation state
    float hover_alpha_{0.0F};
    bool is_hovered_{false};
    animation::TransitionManager transition_manager_{this};

    void OnPaint(wxPaintEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
};

} // namespace markamp::ui
