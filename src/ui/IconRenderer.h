#pragma once

#include "ui/SvgDocument.h"

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>

namespace markamp::ui
{

/// Core vector rendering engine for converting SVG documents to bitmaps.
class IconRenderer
{
public:
    IconRenderer() = delete;

    /// Renders an SVG document to a bitmap of the given logical size, using the given base color
    /// and DPI scale factor.
    static auto RenderIcon(const SvgDocument& doc,
                           const wxSize& logicalSize,
                           const wxColour& color,
                           double scaleFactor = 1.0) -> wxBitmap;
};

} // namespace markamp::ui
