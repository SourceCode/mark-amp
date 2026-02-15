#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders TextBox objects: optional fill/border rectangle + styled text.
class TextBoxRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
