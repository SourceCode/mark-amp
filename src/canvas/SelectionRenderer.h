#pragma once

#include "canvas/CanvasTypes.h"
#include "canvas/SelectionManager.h"
#include "canvas/ViewportTransform.h"

#include <vector>

// Forward declare wxGraphicsContext.
class wxGraphicsContext;

namespace markamp::canvas
{

/// Renders selection visual overlays: outlines, handles, marquee box, rotation indicator.
class SelectionRenderer
{
public:
    /// Render blue outlines around selected objects.
    static auto render_selection_outlines(wxGraphicsContext& ctx,
                                          const std::vector<AABB>& selected_bounds,
                                          const ViewportTransform& viewport) -> void;

    /// Render 8 resize handles + 1 rotation handle around the selection bounding box.
    static auto render_selection_handles(wxGraphicsContext& ctx,
                                         const AABB& combined_bounds,
                                         const ViewportTransform& viewport) -> void;

    /// Render a translucent blue marquee rectangle during drag-select.
    static auto render_marquee(wxGraphicsContext& ctx,
                               const AABB& marquee_bounds,
                               const ViewportTransform& viewport) -> void;

    /// Render a rotation angle indicator arc.
    static auto render_rotation_indicator(wxGraphicsContext& ctx,
                                          const AABB& combined_bounds,
                                          double current_angle_radians,
                                          const ViewportTransform& viewport) -> void;

    /// Size of resize handles in screen pixels.
    static constexpr double kHandleSize = 8.0;
};

} // namespace markamp::canvas
