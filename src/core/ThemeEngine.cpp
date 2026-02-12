#include "ThemeEngine.h"

#include "BuiltinThemes.h"
#include "Events.h"
#include "Logger.h"

#include <wx/window.h>

namespace markamp::core
{

// Hash for ThemeColorToken / ThemeFontToken (needed for unordered_map)
struct TokenHash
{
    template <typename T>
    auto operator()(T t) const -> std::size_t
    {
        return static_cast<std::size_t>(t);
    }
};

ThemeEngine::ThemeEngine(EventBus& event_bus, ThemeRegistry& registry)
    : event_bus_(event_bus)
    , registry_(registry)
    , current_theme_(get_default_theme())
{
    rebuild_cache();
    MARKAMP_LOG_INFO("ThemeEngine initialized with theme: {}", current_theme_.id);
}

auto ThemeEngine::current_theme() const -> const Theme&
{
    return current_theme_;
}

void ThemeEngine::apply_theme(const std::string& theme_id)
{
    // Improvement 17: skip if same theme is already active
    if (theme_id == current_theme_.id)
    {
        return;
    }

    auto theme = registry_.get_theme(theme_id);
    if (!theme)
    {
        MARKAMP_LOG_WARN("Theme not found: {}", theme_id);
        return;
    }

    current_theme_ = *theme;
    rebuild_cache();

    // Publish theme changed event
    events::ThemeChangedEvent event;
    event.theme_id = theme_id;
    event_bus_.publish(event);

    MARKAMP_LOG_INFO("Theme applied: {} ({})", current_theme_.name, current_theme_.id);
}

auto ThemeEngine::available_themes() const -> std::vector<ThemeInfo>
{
    return registry_.list_themes();
}

void ThemeEngine::import_theme(const std::filesystem::path& path)
{
    auto result = registry_.import_theme(path);
    if (!result)
    {
        MARKAMP_LOG_WARN("Failed to import theme: {}", result.error());
    }
}

void ThemeEngine::export_theme(const std::string& theme_id, const std::filesystem::path& path)
{
    auto result = registry_.export_theme(theme_id, path);
    if (!result)
    {
        MARKAMP_LOG_WARN("Failed to export theme: {}", result.error());
    }
}

auto ThemeEngine::registry() -> ThemeRegistry&
{
    return registry_;
}

auto ThemeEngine::registry() const -> const ThemeRegistry&
{
    return registry_;
}

auto ThemeEngine::subscribe_theme_change(std::function<void(const std::string& theme_id)> handler)
    -> Subscription
{
    return event_bus_.subscribe<events::ThemeChangedEvent>(
        [h = std::move(handler)](const events::ThemeChangedEvent& e) { h(e.theme_id); });
}

void ThemeEngine::apply_to_window(wxWindow* window)
{
    if (window == nullptr)
    {
        return;
    }
    propagate_theme(window);
    window->Refresh();
    window->Update();
}

// --- Cached resource access ---

static const wxBrush kFallbackBrush(*wxBLACK);
static const wxColour kFallbackColour(*wxBLACK);
static const wxFont kFallbackFont(wxFontInfo(12));

auto ThemeEngine::brush(ThemeColorToken token) const -> const wxBrush&
{
    auto it = cache_.brushes.find(token);
    if (it != cache_.brushes.end())
    {
        return it->second;
    }
    return kFallbackBrush;
}

auto ThemeEngine::pen(ThemeColorToken token, int width) const -> wxPen
{
    auto it = cache_.colours.find(token);
    if (it != cache_.colours.end())
    {
        return wxPen(it->second, width);
    }
    return wxPen(*wxBLACK, width);
}

auto ThemeEngine::color(ThemeColorToken token) const -> const wxColour&
{
    auto it = cache_.colours.find(token);
    if (it != cache_.colours.end())
    {
        return it->second;
    }
    return kFallbackColour;
}

auto ThemeEngine::font(ThemeFontToken token) const -> const wxFont&
{
    auto it = cache_.fonts.find(token);
    if (it != cache_.fonts.end())
    {
        return it->second;
    }
    return kFallbackFont;
}

// --- Cache rebuilding ---

void ThemeEngine::rebuild_cache()
{
    cache_.brushes.clear();
    cache_.colours.clear();

    // Helper to cache a color token
    auto cache_color = [this](ThemeColorToken token, const Color& clr)
    {
        wxColour wx_clr = clr.to_wx_colour();
        cache_.colours[token] = wx_clr;
        cache_.brushes[token] = wxBrush(wx_clr);
    };

    // Sync layers from flat colors
    current_theme_.sync_layers_from_colors();

    // 10 base tokens
    cache_color(ThemeColorToken::BgApp, current_theme_.colors.bg_app);
    cache_color(ThemeColorToken::BgPanel, current_theme_.colors.bg_panel);
    cache_color(ThemeColorToken::BgHeader, current_theme_.colors.bg_header);
    cache_color(ThemeColorToken::BgInput, current_theme_.colors.bg_input);
    cache_color(ThemeColorToken::TextMain, current_theme_.colors.text_main);
    cache_color(ThemeColorToken::TextMuted, current_theme_.colors.text_muted);
    cache_color(ThemeColorToken::AccentPrimary, current_theme_.colors.accent_primary);
    cache_color(ThemeColorToken::AccentSecondary, current_theme_.colors.accent_secondary);
    cache_color(ThemeColorToken::BorderLight, current_theme_.colors.border_light);
    cache_color(ThemeColorToken::BorderDark, current_theme_.colors.border_dark);

    // 7 derived tokens
    cache_color(ThemeColorToken::SelectionBg, current_theme_.selection_bg());
    cache_color(ThemeColorToken::HoverBg, current_theme_.hover_bg());
    cache_color(ThemeColorToken::ErrorColor, current_theme_.error_color());
    cache_color(ThemeColorToken::SuccessColor, current_theme_.success_color());
    cache_color(ThemeColorToken::ScrollbarTrack, current_theme_.scrollbar_track());
    cache_color(ThemeColorToken::ScrollbarThumb, current_theme_.scrollbar_thumb());
    cache_color(ThemeColorToken::ScrollbarHover, current_theme_.scrollbar_hover());

    // Phase 4: Syntax tokens
    cache_color(ThemeColorToken::SyntaxKeyword, current_theme_.syntax.keyword);
    cache_color(ThemeColorToken::SyntaxString, current_theme_.syntax.string_literal);
    cache_color(ThemeColorToken::SyntaxComment, current_theme_.syntax.comment);
    cache_color(ThemeColorToken::SyntaxNumber, current_theme_.syntax.number);
    cache_color(ThemeColorToken::SyntaxType, current_theme_.syntax.type_name);
    cache_color(ThemeColorToken::SyntaxFunction, current_theme_.syntax.function_name);
    cache_color(ThemeColorToken::SyntaxOperator, current_theme_.syntax.operator_tok);
    cache_color(ThemeColorToken::SyntaxPreprocessor, current_theme_.syntax.preprocessor);

    // Phase 4: Render tokens
    cache_color(ThemeColorToken::RenderHeading, current_theme_.render.heading);
    cache_color(ThemeColorToken::RenderLink, current_theme_.render.link);
    cache_color(ThemeColorToken::RenderCodeBg, current_theme_.render.code_bg);
    cache_color(ThemeColorToken::RenderCodeFg, current_theme_.render.code_fg);
    cache_color(ThemeColorToken::RenderBlockquoteBorder, current_theme_.render.blockquote_border);
    cache_color(ThemeColorToken::RenderBlockquoteBg, current_theme_.render.blockquote_bg);
    cache_color(ThemeColorToken::RenderTableBorder, current_theme_.render.table_border);
    cache_color(ThemeColorToken::RenderTableHeaderBg, current_theme_.render.table_header_bg);

    // Rebuild fonts
    build_fonts();
}

void ThemeEngine::build_fonts()
{
    cache_.fonts.clear();

    // System font fallbacks (platform-aware)
#if defined(__APPLE__)
    const wxString kMonoFace = "Menlo";
    const wxString kSansFace = "SF Pro Display";
#elif defined(_WIN32)
    const wxString kMonoFace = "Consolas";
    const wxString kSansFace = "Segoe UI";
#else
    const wxString kMonoFace = "DejaVu Sans Mono";
    const wxString kSansFace = "Noto Sans";
#endif

    // Sans fonts
    cache_.fonts[ThemeFontToken::SansRegular] = wxFont(wxFontInfo(12).FaceName(kSansFace));
    cache_.fonts[ThemeFontToken::SansSemiBold] = wxFont(wxFontInfo(12).FaceName(kSansFace).Bold());
    cache_.fonts[ThemeFontToken::SansBold] = wxFont(wxFontInfo(12).FaceName(kSansFace).Bold());

    // Mono fonts
    cache_.fonts[ThemeFontToken::MonoRegular] = wxFont(wxFontInfo(13).FaceName(kMonoFace));
    cache_.fonts[ThemeFontToken::MonoBold] = wxFont(wxFontInfo(13).FaceName(kMonoFace).Bold());

    // UI-specific size tokens
    cache_.fonts[ThemeFontToken::UISmall] = wxFont(wxFontInfo(10).FaceName(kSansFace));
    cache_.fonts[ThemeFontToken::UILabel] = wxFont(wxFontInfo(12).FaceName(kSansFace));
    cache_.fonts[ThemeFontToken::UIHeading] = wxFont(wxFontInfo(14).FaceName(kSansFace).Bold());
}

// --- Phase 4: Layered theme application ---

void ThemeEngine::apply_chrome_theme()
{
    current_theme_.sync_layers_from_colors();

    auto cache_color = [this](ThemeColorToken token, const Color& clr)
    {
        wxColour wx_clr = clr.to_wx_colour();
        cache_.colours[token] = wx_clr;
        cache_.brushes[token] = wxBrush(wx_clr);
    };

    cache_color(ThemeColorToken::BgApp, current_theme_.chrome.bg_app);
    cache_color(ThemeColorToken::BgPanel, current_theme_.chrome.bg_panel);
    cache_color(ThemeColorToken::BgHeader, current_theme_.chrome.bg_header);
    cache_color(ThemeColorToken::BgInput, current_theme_.chrome.bg_input);
    cache_color(ThemeColorToken::BorderLight, current_theme_.chrome.border_light);
    cache_color(ThemeColorToken::BorderDark, current_theme_.chrome.border_dark);
    cache_color(ThemeColorToken::AccentPrimary, current_theme_.chrome.accent_primary);
    cache_color(ThemeColorToken::AccentSecondary, current_theme_.chrome.accent_secondary);
}

void ThemeEngine::apply_syntax_theme()
{
    auto cache_color = [this](ThemeColorToken token, const Color& clr)
    {
        wxColour wx_clr = clr.to_wx_colour();
        cache_.colours[token] = wx_clr;
        cache_.brushes[token] = wxBrush(wx_clr);
    };

    cache_color(ThemeColorToken::SyntaxKeyword, current_theme_.syntax.keyword);
    cache_color(ThemeColorToken::SyntaxString, current_theme_.syntax.string_literal);
    cache_color(ThemeColorToken::SyntaxComment, current_theme_.syntax.comment);
    cache_color(ThemeColorToken::SyntaxNumber, current_theme_.syntax.number);
    cache_color(ThemeColorToken::SyntaxType, current_theme_.syntax.type_name);
    cache_color(ThemeColorToken::SyntaxFunction, current_theme_.syntax.function_name);
    cache_color(ThemeColorToken::SyntaxOperator, current_theme_.syntax.operator_tok);
    cache_color(ThemeColorToken::SyntaxPreprocessor, current_theme_.syntax.preprocessor);
}

void ThemeEngine::apply_render_theme()
{
    auto cache_color = [this](ThemeColorToken token, const Color& clr)
    {
        wxColour wx_clr = clr.to_wx_colour();
        cache_.colours[token] = wx_clr;
        cache_.brushes[token] = wxBrush(wx_clr);
    };

    cache_color(ThemeColorToken::RenderHeading, current_theme_.render.heading);
    cache_color(ThemeColorToken::RenderLink, current_theme_.render.link);
    cache_color(ThemeColorToken::RenderCodeBg, current_theme_.render.code_bg);
    cache_color(ThemeColorToken::RenderCodeFg, current_theme_.render.code_fg);
    cache_color(ThemeColorToken::RenderBlockquoteBorder, current_theme_.render.blockquote_border);
    cache_color(ThemeColorToken::RenderBlockquoteBg, current_theme_.render.blockquote_bg);
    cache_color(ThemeColorToken::RenderTableBorder, current_theme_.render.table_border);
    cache_color(ThemeColorToken::RenderTableHeaderBg, current_theme_.render.table_header_bg);
}

// --- Recursive propagation ---

void ThemeEngine::propagate_theme(wxWindow* window)
{
    if (window == nullptr)
    {
        return;
    }

    // Set background and foreground for all windows
    window->SetBackgroundColour(color(ThemeColorToken::BgApp));
    window->SetForegroundColour(color(ThemeColorToken::TextMain));
    window->Refresh();

    // Recursively process children
    for (auto* child : window->GetChildren())
    {
        propagate_theme(child);
    }
}

} // namespace markamp::core
