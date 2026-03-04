#pragma once

#include "ThemeAwareWindow.h"

namespace markamp::ui
{

/// Phase 45: Drop zone indicator direction.
enum class DropDirection
{
    kNone,
    kLeft,
    kRight,
    kTop,
    kBottom,
    kCenter,  ///< Full area highlight (e.g., merge into existing group).
    kBetween, ///< Insert between items.
};

/// Phase 45: Visual overlay for drop zone highlighting.
class DropIndicatorOverlay : public ThemeAwareWindow
{
public:
    DropIndicatorOverlay(wxWindow* parent, core::ThemeEngine& theme_engine);

    void show_indicator(const wxRect& target_rect, DropDirection direction);
    void hide_indicator();

    [[nodiscard]] auto direction() const -> DropDirection
    {
        return direction_;
    }
    [[nodiscard]] auto is_showing() const -> bool
    {
        return is_showing_;
    }

private:
    void on_paint(wxPaintEvent& event);

    DropDirection direction_{DropDirection::kNone};
    wxRect target_rect_;
    bool is_showing_{false};
    static constexpr int kIndicatorThickness = 3;
};

} // namespace markamp::ui
