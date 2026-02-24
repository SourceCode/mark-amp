#pragma once

#include "../DesignSystemContext.h"
#include "../ThemeAwareWindow.h"
#include "../animation/TransitionManager.h"

#include <functional>

namespace markamp::ui::layout
{

enum class ResizeOrientation
{
    kVertical,  ///< Separates left/right zones (tall handle, draggable left/right)
    kHorizontal ///< Separates top/bottom zones (wide handle, draggable up/down)
};

class ResizeHandle : public ThemeAwareWindow
{
public:
    ResizeHandle(wxWindow* parent,
                 DesignSystemContext& ds,
                 ResizeOrientation orientation,
                 std::function<void(int delta)> on_drag,
                 std::function<void()> on_drag_end = nullptr);

    void UpdateLayoutMetrics();

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    ResizeOrientation orientation_;
    std::function<void(int delta)> on_drag_;
    std::function<void()> on_drag_end_;

    bool is_dragging_{false};
    int drag_start_pos_{0};

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

} // namespace markamp::ui::layout
