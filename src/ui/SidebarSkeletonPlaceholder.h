#pragma once

#include "animation/SkeletonLoader.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/sizer.h>

namespace markamp::ui
{

/// A helper class that provides a skeleton loading state for either
/// a list-based view (like Explorer tree) or a card-based view (like Extensions).
/// It automatically regenerates its blocks when resized.
class SidebarSkeletonPlaceholder : public wxPanel
{
public:
    enum class Style
    {
        kList,
        kCards
    };

    SidebarSkeletonPlaceholder(wxWindow* parent, core::ThemeEngine& theme_engine, Style style);

    /// Show the skeleton and start the pulsing animation.
    void ShowAndAnimate();

    /// Hide the skeleton and stop the animation.
    void HideAndStop();

    /// Refresh layout blocks based on current size
    void GenerateBlocks();

private:
    animation::SkeletonLoader* loader_{nullptr};
    Style style_{Style::kList};

    void OnSize(wxSizeEvent& event);
};

} // namespace markamp::ui
