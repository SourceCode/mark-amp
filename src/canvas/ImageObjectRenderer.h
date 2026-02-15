#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders ImageObject: loads and draws a bitmap, or renders a placeholder rect.
class ImageObjectRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
