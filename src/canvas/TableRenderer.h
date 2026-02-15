#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders a TableObject as a grid with header row, cell text, and alternating row colors.
class TableRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& graphics_ctx,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
