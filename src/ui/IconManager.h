#pragma once

#include "ui/IconCache.h"
#include "ui/IconRegistry.h"

#include <wx/dc.h>

#include <string>

namespace markamp::ui
{

/// A singleton manager tying the registry, cache, and renderer together.
/// Provides high-level drawing APIs for icons onto contexts.
class IconManager
{
public:
    IconManager(const IconManager&) = delete;
    IconManager& operator=(const IconManager&) = delete;

    /// Gets the global singleton instance.
    static auto get() -> IconManager&;

    /// Direct access to the registry for adding icons.
    [[nodiscard]] auto registry() -> IconRegistry&;

    /// Direct access to the cache to clear or inspect size.
    [[nodiscard]] auto cache() -> IconCache&;

    /// Re-renders an icon or fetches it from cache.
    [[nodiscard]] auto get_icon_bitmap(const std::string& name,
                                       const wxSize& size,
                                       const wxColour& color,
                                       double scale = 1.0) -> wxBitmap;

    /// Draws an icon directly to a device context at the given coordinates.
    auto draw_icon(wxDC& dc,
                   const std::string& name,
                   int x,
                   int y,
                   const wxSize& size,
                   const wxColour& color,
                   double scale = 1.0) -> void;

private:
    IconManager() = default;
    ~IconManager() = default;

    IconRegistry registry_;
    IconCache cache_;
};

} // namespace markamp::ui
