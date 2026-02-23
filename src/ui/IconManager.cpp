#include "ui/IconManager.h"

#include "ui/IconRenderer.h"

#include <wx/dcmemory.h>

namespace markamp::ui
{

auto IconManager::get() -> IconManager&
{
    static IconManager instance;
    return instance;
}

auto IconManager::registry() -> IconRegistry&
{
    return registry_;
}

auto IconManager::cache() -> IconCache&
{
    return cache_;
}

auto IconManager::get_icon_bitmap(const std::string& name,
                                  const wxSize& size,
                                  const wxColour& color,
                                  double scale) -> wxBitmap
{
    IconCacheKey key{
        name, size.GetWidth(), size.GetHeight(), IconCache::color_to_rgba(color), scale};

    if (auto cached = cache_.get(key))
    {
        return *cached;
    }

    if (auto doc = registry_.get_icon(name))
    {
        wxBitmap bmp = IconRenderer::RenderIcon(*doc, size, color, scale);
        if (bmp.IsOk())
        {
            cache_.put(key, bmp);
            return bmp;
        }
    }

    // Fallback: render a question mark (or missing glyph) if icon is not found
    wxBitmap fallback_bmp(
        static_cast<int>(size.GetWidth() * scale), static_cast<int>(size.GetHeight() * scale), 32);
    fallback_bmp.UseAlpha();
#if wxCHECK_VERSION(3, 1, 6)
    fallback_bmp.SetScaleFactor(scale);
#endif

    wxMemoryDC memDC;
    memDC.SelectObject(fallback_bmp);
    memDC.SetBackground(*wxTRANSPARENT_BRUSH);
    memDC.Clear();

    // Use a generic system font for the missing icon indicator
    int font_size = static_cast<int>(size.GetHeight() * 0.7);
    wxFont font(font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    memDC.SetFont(font);

    // Apply 50% opacity to the given color to clearly indicate it's a fallback placeholder
    wxColour fallback_color(color.Red(), color.Green(), color.Blue(), 128);
    memDC.SetTextForeground(fallback_color);

    wxString fallback_text = "?";
    wxSize text_size = memDC.GetTextExtent(fallback_text);

    // Center the text
    int tx = (fallback_bmp.GetWidth() - text_size.GetWidth()) / 2;
    int ty = (fallback_bmp.GetHeight() - text_size.GetHeight()) / 2;
    memDC.DrawText(fallback_text, tx, ty);
    memDC.SelectObject(wxNullBitmap);

    cache_.put(key, fallback_bmp);
    return fallback_bmp;
}

auto IconManager::draw_icon(wxDC& dc,
                            const std::string& name,
                            int x,
                            int y,
                            const wxSize& size,
                            const wxColour& color,
                            double scale) -> void
{
    wxBitmap bmp = get_icon_bitmap(name, size, color, scale);
    if (bmp.IsOk())
    {
        dc.DrawBitmap(bmp, x, y, true);
    }
}

} // namespace markamp::ui
