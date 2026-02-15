#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders FreehandPath objects as smooth polylines with round caps and joins.
class FreehandPathRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
