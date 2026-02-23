#include "ThemeEngine.h"

#include "../ui/ColorPaletteGenerator.h"
#include "BuiltInThemes.h"
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

void ThemeEngine::apply_theme(const Theme& theme)
{
    current_theme_ = theme;
    rebuild_cache();

    // Publish theme changed event
    events::ThemeChangedEvent event;
    event.theme_id = theme.id;
    event_bus_.publish(event);

    MARKAMP_LOG_INFO("Theme applied directly: {} ({})", current_theme_.name, current_theme_.id);
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
    auto iter = cache_.colours.find(token);
    if (iter != cache_.colours.end())
    {
        return iter->second;
    }
    return kFallbackColour;
}

auto ThemeEngine::color_fast(ThemeColorToken token) const -> const wxColour&
{
    auto idx = static_cast<std::size_t>(token);
    if (idx < kColorTokenCount)
    {
        return flat_colours_[idx];
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

    // Helper to cache a color token (writes to both map and flat array)
    auto cache_color =
        [this](ThemeColorToken token, const std::string& scoped_name, const Color& clr)
    {
        // 1. Set the color in the new scoped token map
        token_map_.set(scoped_name, clr.to_wx_colour());

        // 2. Resolve the color and cache it in the legacy V1 structures for backward compatibility
        auto resolved = token_map_.resolve(scoped_name);
        wxColour wx_clr = resolved ? *resolved : clr.to_wx_colour();

        cache_.colours[token] = wx_clr;
        cache_.brushes[token] = wxBrush(wx_clr);
        auto idx = static_cast<std::size_t>(token);
        if (idx < kColorTokenCount)
        {
            flat_colours_[idx] = wx_clr;
        }
    };

    // Sync layers from flat colors
    current_theme_.sync_layers_from_colors();

    // 10 base tokens
    cache_color(ThemeColorToken::BgApp, "workbench.background", current_theme_.colors.bg_app);
    cache_color(ThemeColorToken::BgPanel, "panel.background", current_theme_.colors.bg_panel);
    cache_color(
        ThemeColorToken::BgHeader, "titleBar.activeBackground", current_theme_.colors.bg_header);
    cache_color(ThemeColorToken::BgInput, "input.background", current_theme_.colors.bg_input);
    cache_color(ThemeColorToken::TextMain, "foreground", current_theme_.colors.text_main);
    cache_color(
        ThemeColorToken::TextMuted, "descriptionForeground", current_theme_.colors.text_muted);
    cache_color(
        ThemeColorToken::AccentPrimary, "focusBorder", current_theme_.colors.accent_primary);
    cache_color(ThemeColorToken::AccentSecondary,
                "textLink.foreground",
                current_theme_.colors.accent_secondary);
    cache_color(ThemeColorToken::BorderLight, "panel.border", current_theme_.colors.border_light);
    cache_color(ThemeColorToken::BorderDark, "widget.shadow", current_theme_.colors.border_dark);

    // 7 derived tokens
    cache_color(
        ThemeColorToken::SelectionBg, "editor.selectionBackground", current_theme_.selection_bg());
    cache_color(ThemeColorToken::HoverBg, "list.hoverBackground", current_theme_.hover_bg());
    cache_color(ThemeColorToken::ErrorColor, "errorForeground", current_theme_.error_color());
    cache_color(ThemeColorToken::SuccessColor, "successForeground", current_theme_.success_color());
    cache_color(ThemeColorToken::ScrollbarTrack,
                "scrollbarSlider.background",
                current_theme_.scrollbar_track());
    cache_color(ThemeColorToken::ScrollbarThumb,
                "scrollbarSlider.hoverBackground",
                current_theme_.scrollbar_thumb());
    cache_color(ThemeColorToken::ScrollbarHover,
                "scrollbarSlider.activeBackground",
                current_theme_.scrollbar_hover());

    // Phase 4: Syntax tokens
    cache_color(
        ThemeColorToken::SyntaxKeyword, "editor.token.keyword", current_theme_.syntax.keyword);
    cache_color(
        ThemeColorToken::SyntaxString, "editor.token.string", current_theme_.syntax.string_literal);
    cache_color(
        ThemeColorToken::SyntaxComment, "editor.token.comment", current_theme_.syntax.comment);
    cache_color(ThemeColorToken::SyntaxNumber, "editor.token.number", current_theme_.syntax.number);
    cache_color(ThemeColorToken::SyntaxType, "editor.token.type", current_theme_.syntax.type_name);
    cache_color(ThemeColorToken::SyntaxFunction,
                "editor.token.function",
                current_theme_.syntax.function_name);
    cache_color(ThemeColorToken::SyntaxOperator,
                "editor.token.operator",
                current_theme_.syntax.operator_tok);
    cache_color(ThemeColorToken::SyntaxPreprocessor,
                "editor.token.preprocessor",
                current_theme_.syntax.preprocessor);

    // Phase 4: Render tokens
    cache_color(ThemeColorToken::RenderHeading,
                "markdown.headingForeground",
                current_theme_.render.heading);
    cache_color(ThemeColorToken::RenderLink, "markdown.linkForeground", current_theme_.render.link);
    cache_color(
        ThemeColorToken::RenderCodeBg, "markdown.codeBackground", current_theme_.render.code_bg);
    cache_color(
        ThemeColorToken::RenderCodeFg, "markdown.codeForeground", current_theme_.render.code_fg);
    cache_color(ThemeColorToken::RenderBlockquoteBorder,
                "markdown.blockquoteBorder",
                current_theme_.render.blockquote_border);
    cache_color(ThemeColorToken::RenderBlockquoteBg,
                "markdown.blockquoteBackground",
                current_theme_.render.blockquote_bg);
    cache_color(ThemeColorToken::RenderTableBorder,
                "markdown.tableBorder",
                current_theme_.render.table_border);
    cache_color(ThemeColorToken::RenderTableHeaderBg,
                "markdown.tableHeaderBackground",
                current_theme_.render.table_header_bg);

    // V8 Phase 9: Semantic editor tokens
    cache_color(ThemeColorToken::EditorActiveLine,
                "editor.lineHighlightBackground",
                current_theme_.colors.editor_active_line);
    cache_color(ThemeColorToken::EditorGutterError,
                "editorGutter.deletedBackground",
                current_theme_.colors.editor_gutter_error);
    cache_color(ThemeColorToken::EditorGutterWarn,
                "editorGutter.modifiedBackground",
                current_theme_.colors.editor_gutter_warn);
    cache_color(ThemeColorToken::EditorGutterInfo,
                "editorGutter.addedBackground",
                current_theme_.colors.editor_gutter_info);
    cache_color(ThemeColorToken::EditorMatchHighlight,
                "editor.findMatchHighlightBackground",
                current_theme_.colors.editor_match_highlight);
    cache_color(ThemeColorToken::EditorFindHit,
                "editor.findMatchBackground",
                current_theme_.colors.editor_find_hit);
    cache_color(ThemeColorToken::EditorQuickFix,
                "editorLightBulb.foreground",
                current_theme_.colors.editor_quick_fix);

    // V9 Phase 3: Extended semantic tokens
    cache_color(ThemeColorToken::SidebarBg, "sideBar.background", current_theme_.colors.sidebar_bg);
    cache_color(ThemeColorToken::SidebarFg, "sideBar.foreground", current_theme_.colors.sidebar_fg);
    cache_color(ThemeColorToken::ActivityBarBg,
                "activityBar.background",
                current_theme_.colors.activity_bar_bg);
    cache_color(ThemeColorToken::ActivityBarFg,
                "activityBar.foreground",
                current_theme_.colors.activity_bar_fg);
    cache_color(ThemeColorToken::ActivityBarBadgeBg,
                "activityBarBadge.background",
                current_theme_.colors.activity_bar_badge_bg);
    cache_color(ThemeColorToken::ActivityBarBadgeFg,
                "activityBarBadge.foreground",
                current_theme_.colors.activity_bar_badge_fg);
    cache_color(ThemeColorToken::BreadcrumbFg,
                "breadcrumb.foreground",
                current_theme_.colors.breadcrumb_fg);
    cache_color(ThemeColorToken::BreadcrumbFocusFg,
                "breadcrumb.focusForeground",
                current_theme_.colors.breadcrumb_focus_fg);
    cache_color(
        ThemeColorToken::TabActiveBg, "tab.activeBackground", current_theme_.colors.tab_active_bg);
    cache_color(ThemeColorToken::TabInactiveBg,
                "tab.inactiveBackground",
                current_theme_.colors.tab_inactive_bg);
    cache_color(
        ThemeColorToken::TabActiveFg, "tab.activeForeground", current_theme_.colors.tab_active_fg);
    cache_color(ThemeColorToken::TabInactiveFg,
                "tab.inactiveForeground",
                current_theme_.colors.tab_inactive_fg);
    cache_color(ThemeColorToken::DiffInsertedBg,
                "diffEditor.insertedTextBackground",
                current_theme_.colors.diff_inserted_bg);
    cache_color(ThemeColorToken::DiffRemovedBg,
                "diffEditor.removedTextBackground",
                current_theme_.colors.diff_removed_bg);
    cache_color(ThemeColorToken::MinimapBg, "minimap.background", current_theme_.colors.minimap_bg);
    cache_color(ThemeColorToken::PeekViewBorderColor,
                "peekView.border",
                current_theme_.colors.peek_view_border);
    cache_color(ThemeColorToken::NotebookCellBg,
                "notebook.cellEditorBackground",
                current_theme_.colors.notebook_cell_bg);

    // V10 Phase 02: Control state tokens
    ui::ColorPaletteGenerator palette_gen;
    auto extended = palette_gen.generate_extended_palette(current_theme_);
    for (const auto& [tok, clr] : extended)
    {
        // Simple mapping for extended control tokens
        std::string name = "control." + std::to_string(static_cast<int>(tok));
        cache_color(tok, name, Color(clr.Red(), clr.Green(), clr.Blue(), clr.Alpha()));
    }

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

    // V8 Phase 9: Semantic editor tokens (cached from flat colors struct)
    cache_color(ThemeColorToken::EditorActiveLine, current_theme_.colors.editor_active_line);
    cache_color(ThemeColorToken::EditorGutterError, current_theme_.colors.editor_gutter_error);
    cache_color(ThemeColorToken::EditorGutterWarn, current_theme_.colors.editor_gutter_warn);
    cache_color(ThemeColorToken::EditorGutterInfo, current_theme_.colors.editor_gutter_info);
    cache_color(ThemeColorToken::EditorMatchHighlight,
                current_theme_.colors.editor_match_highlight);
    cache_color(ThemeColorToken::EditorFindHit, current_theme_.colors.editor_find_hit);
    cache_color(ThemeColorToken::EditorQuickFix, current_theme_.colors.editor_quick_fix);
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

// --- V9 Phase 3: New APIs ---

void ThemeEngine::apply_fx_tokens()
{
    MARKAMP_LOG_INFO("Applied FX tokens (master_enabled={})",
                     current_theme_.fx_settings.master_enabled);
}

auto ThemeEngine::missing_tokens() const -> std::vector<ThemeColorToken>
{
    std::vector<ThemeColorToken> missing;
    for (std::size_t idx = 0; idx < kColorTokenCount; ++idx)
    {
        auto token = static_cast<ThemeColorToken>(idx);
        if (!cache_.colours.contains(token))
        {
            missing.push_back(token);
        }
    }
    return missing;
}

// --- V2 Phase 03: Scoped Token Architecture APIs ---

auto ThemeEngine::resolve_token(const std::string& token_name) const -> std::optional<wxColour>
{
    auto result = token_map_.resolve(token_name);
    if (!result)
    {
        return std::nullopt;
    }
    return *result;
}

auto ThemeEngine::is_token_explicit(const std::string& token_name) const -> bool
{
    return token_map_.is_explicit(token_name);
}

void ThemeEngine::register_scoped_token(const std::string& token_name,
                                        const std::string& fallback_token)
{
    token_map_.resolver().register_fallback(token_name, fallback_token);
}

// ----------------------------------------------------

auto ThemeEngine::scope_mapper() const -> const ThemeScopeMapper&
{
    return scope_mapper_;
}

void ThemeEngine::set_reduced_motion(bool enabled)
{
    reduced_motion_ = enabled;
    if (enabled)
    {
        current_theme_.fx_settings.reduced_motion = true;
    }
    MARKAMP_LOG_INFO("Reduced motion: {}", enabled ? "enabled" : "disabled");
}

auto ThemeEngine::is_reduced_motion() const -> bool
{
    return reduced_motion_;
}

void ThemeEngine::push_undo()
{
    if (undo_stack_.size() >= kMaxUndoStack)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
    undo_stack_.push_back(current_theme_);
}

auto ThemeEngine::can_undo() const -> bool
{
    return !undo_stack_.empty();
}

void ThemeEngine::undo_theme_change()
{
    if (undo_stack_.empty())
    {
        return;
    }
    // Push current to redo stack before undoing
    redo_stack_.push_back(current_theme_);
    current_theme_ = undo_stack_.back();
    undo_stack_.pop_back();
    rebuild_cache();

    events::ThemeChangedEvent event;
    event.theme_id = current_theme_.id;
    event_bus_.publish(event);
    MARKAMP_LOG_INFO("Theme undo: reverted to {}", current_theme_.id);
}

void ThemeEngine::redo_theme_change()
{
    if (redo_stack_.empty())
    {
        return;
    }
    undo_stack_.push_back(current_theme_);
    current_theme_ = redo_stack_.back();
    redo_stack_.pop_back();
    rebuild_cache();

    events::ThemeChangedEvent event;
    event.theme_id = current_theme_.id;
    event_bus_.publish(event);
    MARKAMP_LOG_INFO("Theme redo: restored to {}", current_theme_.id);
}

auto ThemeEngine::can_redo() const -> bool
{
    return !redo_stack_.empty();
}

void ThemeEngine::populate_scope_mapper(const Theme& theme)
{
    // Clear existing rules and populate from theme syntax colors.
    scope_mapper_ = ThemeScopeMapper{};

    // Map standard TextMate scopes to theme syntax colors.
    auto make_rule = [](const std::string& sel, const std::string& color_hex) {
        return ThemeScopeMapper::ScopeRule{sel, color_hex, FontStyleFlag::kNone, false};
    };

    scope_mapper_.add_rule(make_rule("keyword", theme.syntax.keyword.to_hex()));
    scope_mapper_.add_rule(make_rule("keyword.control", theme.syntax.keyword.to_hex()));
    scope_mapper_.add_rule(make_rule("string", theme.syntax.string_literal.to_hex()));
    scope_mapper_.add_rule(make_rule("string.quoted", theme.syntax.string_literal.to_hex()));
    scope_mapper_.add_rule(make_rule("comment", theme.syntax.comment.to_hex()));
    scope_mapper_.add_rule(make_rule("comment.line", theme.syntax.comment.to_hex()));
    scope_mapper_.add_rule(make_rule("constant.numeric", theme.syntax.number.to_hex()));
    scope_mapper_.add_rule(make_rule("entity.name.type", theme.syntax.type_name.to_hex()));
    scope_mapper_.add_rule(make_rule("entity.name.function", theme.syntax.function_name.to_hex()));
    scope_mapper_.add_rule(make_rule("keyword.operator", theme.syntax.operator_tok.to_hex()));
    scope_mapper_.add_rule(make_rule("meta.preprocessor", theme.syntax.preprocessor.to_hex()));

    // Phase 12 (Task 23): Load custom token rules from tokenColors block
    for (const auto& token_rule : theme.token_colors)
    {
        ThemeScopeMapper::ScopeRule mapper_rule;
        mapper_rule.selector = token_rule.scope;

        // If foreground is missing, we use empty string (ScopeMapper handles it, or depends on
        // fallback)
        if (token_rule.foreground)
        {
            mapper_rule.foreground = token_rule.foreground->to_hex();
        }

        if (token_rule.font_style)
        {
            mapper_rule.font_style = ThemeScopeMapper::parse_font_style(*token_rule.font_style);
        }
        else
        {
            mapper_rule.font_style = FontStyleFlag::kNone;
        }

        mapper_rule.is_semantic = false;
        scope_mapper_.add_rule(mapper_rule);
    }

    MARKAMP_LOG_INFO("Scope mapper populated with {} rules", scope_mapper_.rule_count());
}

void ThemeEngine::discover_extension_themes()
{
    const auto themes = available_themes();
    for (const auto& info : themes)
    {
        // Extension-contributed themes are non-builtin with a file path
        if (!info.is_builtin && info.file_path.has_value())
        {
            events::ExtensionThemeDiscoveredEvent event;
            event.theme_id = info.id;
            event.extension_id = info.file_path.value();
            event_bus_.publish(event);
        }
    }
}

void ThemeEngine::preview_theme(const std::string& theme_id)
{
    // Push current state so we can revert
    push_undo();
    previewing_ = true;

    // Apply the preview theme
    apply_theme(theme_id);

    events::ThemePreviewRequestEvent event;
    event.theme_id = theme_id;
    event_bus_.publish(event);
    MARKAMP_LOG_INFO("Theme preview: previewing {}", theme_id);
}

void ThemeEngine::preview_theme(const Theme& theme)
{
    // Push current state so we can revert
    push_undo();
    previewing_ = true;

    // Apply the preview theme directly
    apply_theme(theme);

    events::ThemePreviewRequestEvent event;
    event.theme_id = theme.id;
    event_bus_.publish(event);
    MARKAMP_LOG_INFO("Theme preview: previewing custom theme object {}", theme.id);
}

void ThemeEngine::cancel_preview()
{
    if (!previewing_)
    {
        return;
    }
    previewing_ = false;
    undo_theme_change();
    MARKAMP_LOG_INFO("Theme preview: cancelled, reverted");
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
