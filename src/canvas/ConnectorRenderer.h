#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders ConnectorObject instances: polyline with arrowheads and optional label.
class ConnectorRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;

private:
    auto
    render_arrowhead(wxGraphicsContext& gc, const Point2D& tip, const Point2D& from, double size)
        -> void;
};

} // namespace markamp::canvas
