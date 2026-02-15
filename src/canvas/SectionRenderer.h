#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders SectionObject: semi-transparent tinted background with label header.
/// Collapsed state renders only the header bar.
class SectionRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
