#pragma once

#include "canvas/CanvasTypes.h"
#include "canvas/SnapEngine.h"
#include "canvas/ViewportTransform.h"

#include <vector>

// Forward declare wxGraphicsContext.
class wxGraphicsContext;

namespace markamp::canvas
{

/// Renders alignment guide lines (dashed colored lines) during snap operations.
class AlignmentGuides
{
public:
    /// Render a set of guide lines onto the graphics context.
    static auto render_guides(wxGraphicsContext& gc,
                              const std::vector<GuideLine>& guides,
                              const ViewportTransform& viewport) -> void;
};

} // namespace markamp::canvas
