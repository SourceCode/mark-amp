#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders ShapeObject instances. Dispatches to per-shape-type drawing routines.
class ShapeRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;

private:
    auto render_rectangle(
        wxGraphicsContext& gc, double x, double y, double w, double h, double corner_radius)
        -> void;
    auto render_ellipse(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto render_diamond(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto render_triangle(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto render_hexagon(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto render_star(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
};

} // namespace markamp::canvas
