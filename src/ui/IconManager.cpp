/// @file IconManager.cpp
/// @brief V16 Phase 04 — Upgraded IconManager with IconPipeline integration.
///
/// The singleton now holds an IconPipeline instance alongside the legacy
/// registry/cache. New callers use pipeline methods; legacy callers
/// continue to work through the get_icon_bitmap/draw_icon interface.

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

auto IconManager::pipeline() -> IconPipeline&
{
    return pipeline_;
}

void IconManager::initialize_pipeline(const IconPipelineConfig& config)
{
    [[maybe_unused]] auto initialized = pipeline_.initialize(config);
}

void IconManager::on_theme_changed()
{
    // Invalidate all cached bitmaps when the theme changes
    cache_.clear();
    pipeline_.invalidate_cache();
}

auto IconManager::get_icon_bitmap(const std::string& name,
                                  const wxSize& size,
                                  const wxColour& color,
                                  double scale) -> wxBitmap
{
    const IconCacheKey key{
        name, size.GetWidth(), size.GetHeight(), IconCache::color_to_rgba(color), scale};

    if (auto cached = cache_.get(key))
    {
        return *cached;
    }

    // Try pipeline first (manifest-driven)
    if (pipeline_.is_initialized())
    {
        auto bmp = pipeline_.get_icon_by_id(name, size, color, scale);
        if (bmp.IsOk())
        {
            cache_.put(key, bmp);
            return bmp;
        }
    }

    // Fallback: try legacy registry
    if (auto doc = registry_.get_icon(name))
    {
        const wxBitmap bmp = IconRenderer::RenderIcon(*doc, size, color, scale);
        if (bmp.IsOk())
        {
            cache_.put(key, bmp);
            return bmp;
        }
    }

    // Last resort: render a question mark fallback placeholder
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

    const int font_size = static_cast<int>(size.GetHeight() * 0.7);
    const wxFont font(font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    memDC.SetFont(font);

    const wxColour fallback_color(color.Red(), color.Green(), color.Blue(), 128);
    memDC.SetTextForeground(fallback_color);

    const wxString fallback_text = "?";
    const wxSize text_size = memDC.GetTextExtent(fallback_text);

    const int text_x = (fallback_bmp.GetWidth() - text_size.GetWidth()) / 2;
    const int text_y = (fallback_bmp.GetHeight() - text_size.GetHeight()) / 2;
    memDC.DrawText(fallback_text, text_x, text_y);
    memDC.SelectObject(wxNullBitmap);

    cache_.put(key, fallback_bmp);
    return fallback_bmp;
}

auto IconManager::draw_icon(wxDC& drawing_context,
                            const std::string& name,
                            int x_pos,
                            int y_pos,
                            const wxSize& size,
                            const wxColour& color,
                            double scale) -> void
{
    const wxBitmap bmp = get_icon_bitmap(name, size, color, scale);
    if (bmp.IsOk())
    {
        drawing_context.DrawBitmap(bmp, x_pos, y_pos, true);
    }
}

} // namespace markamp::ui
