#include "ui/IconManager.h"

#include "ui/IconRenderer.h"

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

    return wxBitmap();
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
