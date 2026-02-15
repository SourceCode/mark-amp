#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders FrameObject: bordered rectangle with optional title bar above.
class FrameRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
