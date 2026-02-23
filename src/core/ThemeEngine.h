#pragma once

#include "EventBus.h"
#include "IThemeEngine.h"
#include "ScopedTokenMap.h"
#include "Theme.h"
#include "ThemeRegistry.h"
#include "ThemeScopeMapper.h"

#include <wx/brush.h>
#include <wx/colour.h>
#include <wx/font.h>
#include <wx/pen.h>

#include <array>
#include <functional>
#include <unordered_map>
#include <vector>

class wxWindow;

namespace markamp::core
{

#include "ThemeTokens.h"

/// Runtime theme engine — applies colors to wxWidgets components and
/// enables instant theme hot-swapping via EventBus notifications.
class ThemeEngine : public IThemeEngine
{
public:
    explicit ThemeEngine(EventBus& event_bus, ThemeRegistry& registry);

    // IThemeEngine interface
    [[nodiscard]] auto current_theme() const -> const Theme& override;
    void apply_theme(const std::string& theme_id) override;
    void apply_theme(const Theme& theme) override;
    [[nodiscard]] auto available_themes() const -> std::vector<ThemeInfo> override;
    void import_theme(const std::filesystem::path& path) override;
    void export_theme(const std::string& theme_id, const std::filesystem::path& path) override;

    /// Access the underlying registry.
    [[nodiscard]] auto registry() -> ThemeRegistry&;
    [[nodiscard]] auto registry() const -> const ThemeRegistry&;

    // V2 Phase 03: Scoped Token Architecture APIs
    /// Resolve a specific scoped token name (e.g. "editor.background") to its current color.
    [[nodiscard]] auto resolve_token(const std::string& token_name) const
        -> std::optional<wxColour>;

    /// Check if a scoped token is explicitly defined in the current theme.
    [[nodiscard]] auto is_token_explicit(const std::string& token_name) const -> bool;

    /// Allow an extension to register a new themeable token and its default fallback string.
    void register_scoped_token(const std::string& token_name, const std::string& fallback_token);

    /// Apply current theme to a window and all its children.
    void apply_to_window(wxWindow* window);

    /// Get cached themed drawing resources by token.
    [[nodiscard]] auto brush(ThemeColorToken token) const -> const wxBrush&;
    [[nodiscard]] auto pen(ThemeColorToken token, int width = 1) const -> wxPen;
    [[nodiscard]] auto color(ThemeColorToken token) const -> const wxColour&;
    [[nodiscard]] auto font(ThemeFontToken token) const -> const wxFont&;

    /// O(1) array-indexed color access for hot paths (no hash lookup).
    [[nodiscard]] auto color_fast(ThemeColorToken token) const -> const wxColour&;

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

    // V9 Phase 3: New APIs
    /// Apply FX tokens from the current theme's fx_settings.
    void apply_fx_tokens();

    /// Returns token names that have no color definition (fallback to black).
    [[nodiscard]] auto missing_tokens() const -> std::vector<ThemeColorToken>;

    /// Access the live scope mapper (populated from current theme's token rules).
    [[nodiscard]] auto scope_mapper() const -> const ThemeScopeMapper&;

    /// Reduced-motion support.
    void set_reduced_motion(bool enabled);
    [[nodiscard]] auto is_reduced_motion() const -> bool;

    /// Theme undo.
    void push_undo();
    [[nodiscard]] auto can_undo() const -> bool;
    void undo_theme_change();

    /// V9: Theme redo.
    void redo_theme_change();
    [[nodiscard]] auto can_redo() const -> bool;

    /// V9: Populate scope mapper from theme syntax colors.
    void populate_scope_mapper(const Theme& theme);

    /// V9: Discover extension-contributed themes and publish events.
    void discover_extension_themes();

    /// V9: Preview a theme without committing (pushes undo first).
    void preview_theme(const std::string& theme_id);
    void preview_theme(const Theme& theme);

    /// V9: Cancel preview and revert to previous theme.
    void cancel_preview();

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

    /// Flat array for O(1) indexed color access — rebuilt alongside cache_.
    std::array<wxColour, kColorTokenCount> flat_colours_{};

    /// V9 Phase 3: ThemeScopeMapper populated from current theme.
    ThemeScopeMapper scope_mapper_;

    /// V2 Phase 03: The core token mapping dictionary for semantic colors.
    ScopedTokenMap token_map_;

    /// V9 Phase 3: Reduced-motion flag.
    bool reduced_motion_{false};

    /// V9 Phase 3: Theme undo stack.
    std::vector<Theme> undo_stack_;
    static constexpr std::size_t kMaxUndoStack = 20;

    /// V9 Phase 3: Theme redo stack.
    std::vector<Theme> redo_stack_;

    /// V9: Preview state.
    bool previewing_{false};

    void rebuild_cache();
    void build_fonts();
    void propagate_theme(wxWindow* window);
};

} // namespace markamp::core
