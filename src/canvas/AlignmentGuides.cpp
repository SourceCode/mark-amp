#include "AlignmentGuides.h"

#include <wx/graphics.h>
#include <wx/pen.h>

namespace markamp::canvas
{

auto AlignmentGuides::render_guides(wxGraphicsContext& gc,
                                    const std::vector<GuideLine>& guides,
                                    const ViewportTransform& viewport) -> void
{
    if (guides.empty())
    {
        return;
    }

    gc.SetPen(wxPen(wxColour(255, 0, 200, 180), 1, wxPENSTYLE_SHORT_DASH));

    for (const auto& guide : guides)
    {
        const auto screen_start = viewport.world_to_screen(guide.start);
        const auto screen_end = viewport.world_to_screen(guide.end);
        gc.StrokeLine(screen_start.x, screen_start.y, screen_end.x, screen_end.y);
    }
}

} // namespace markamp::canvas
