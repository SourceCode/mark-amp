#pragma once

#include "EventBus.h"
#include "IThemeEngine.h"
#include "Theme.h"
#include "ThemeRegistry.h"

#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/pen.h>

#include <functional>
#include <unordered_map>

class wxWindow;

namespace markamp::core
{

/// Color tokens for themed drawing (10 base + 7 derived).
enum class ThemeColorToken
{
    BgApp,
    BgPanel,
    BgHeader,
    BgInput,
    TextMain,
    TextMuted,
    AccentPrimary,
    AccentSecondary,
    BorderLight,
    BorderDark,
    // Derived
    SelectionBg,
    HoverBg,
    ErrorColor,
    SuccessColor,
    ScrollbarTrack,
    ScrollbarThumb,
    ScrollbarHover,

    // Phase 4: Syntax tokens
    SyntaxKeyword,
    SyntaxString,
    SyntaxComment,
    SyntaxNumber,
    SyntaxType,
    SyntaxFunction,
    SyntaxOperator,
    SyntaxPreprocessor,

    // Phase 4: Render/preview tokens
    RenderHeading,
    RenderLink,
    RenderCodeBg,
    RenderCodeFg,
    RenderBlockquoteBorder,
    RenderBlockquoteBg,
    RenderTableBorder,
    RenderTableHeaderBg,
};

/// Font tokens for themed text rendering.
enum class ThemeFontToken
{
    SansRegular,
    SansSemiBold,
    SansBold,
    MonoRegular,
    MonoBold,
    UISmall,
    UILabel,
    UIHeading,
};

/// Runtime theme engine — applies colors to wxWidgets components and
/// enables instant theme hot-swapping via EventBus notifications.
class ThemeEngine : public IThemeEngine
{
public:
    explicit ThemeEngine(EventBus& event_bus, ThemeRegistry& registry);

    // IThemeEngine interface
    [[nodiscard]] auto current_theme() const -> const Theme& override;
    void apply_theme(const std::string& theme_id) override;
    [[nodiscard]] auto available_themes() const -> std::vector<ThemeInfo> override;
    void import_theme(const std::filesystem::path& path) override;
    void export_theme(const std::string& theme_id, const std::filesystem::path& path) override;

    /// Access the underlying registry.
    [[nodiscard]] auto registry() -> ThemeRegistry&;
    [[nodiscard]] auto registry() const -> const ThemeRegistry&;

    /// Apply current theme to a window and all its children.
    void apply_to_window(wxWindow* window);

    /// Get cached themed drawing resources by token.
    [[nodiscard]] auto brush(ThemeColorToken token) const -> const wxBrush&;
    [[nodiscard]] auto pen(ThemeColorToken token, int width = 1) const -> wxPen;
    [[nodiscard]] auto color(ThemeColorToken token) const -> const wxColour&;
    [[nodiscard]] auto font(ThemeFontToken token) const -> const wxFont&;

    /// Subscribe to theme change notifications. Returns RAII subscription.
    [[nodiscard]] auto
    subscribe_theme_change(std::function<void(const std::string& theme_id)> handler)
        -> Subscription;

    // Phase 4: Layered theme application
    /// Apply only chrome (UI) tokens from the current theme.
    void apply_chrome_theme();
    /// Apply only syntax (editor highlighting) tokens from the current theme.
    void apply_syntax_theme();
    /// Apply only render (preview) tokens from the current theme.
    void apply_render_theme();

private:
    EventBus& event_bus_;
    ThemeRegistry& registry_;
    Theme current_theme_;

    // Cached wxWidgets rendering objects — rebuilt on theme change.
    struct CachedResources
    {
        std::unordered_map<ThemeColorToken, wxBrush> brushes;
        std::unordered_map<ThemeColorToken, wxColour> colours;
        std::unordered_map<ThemeFontToken, wxFont> fonts;
    };
    CachedResources cache_;

    void rebuild_cache();
    void build_fonts();
    void propagate_theme(wxWindow* window);
};

} // namespace markamp::core
