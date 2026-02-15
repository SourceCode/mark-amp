#pragma once

#include "canvas/CanvasRenderer.h"

namespace markamp::canvas
{

/// Renders StickyNote objects: filled rounded rectangle with shadow and centered text.
class StickyNoteRenderer : public IObjectRenderer
{
public:
    [[nodiscard]] auto object_type() const -> CanvasObjectType override;

    auto render(wxGraphicsContext& gc,
                const CanvasObject& obj,
                const ViewportTransform& viewport,
                bool selected) -> void override;
};

} // namespace markamp::canvas
