/// @file ThemeCompatibility.cpp
/// @brief V9 Phase 3 — Theme compatibility scoring implementation.

#include "core/ThemeCompatibility.h"

#include "core/Logger.h"

#include <fmt/format.h>

namespace markamp::core
{

namespace
{

const std::vector<std::string>& token_names()
{
    static const std::vector<std::string> kNames = {
        "BgApp",
        "BgPanel",
        "BgHeader",
        "BgInput",
        "TextMain",
        "TextMuted",
        "AccentPrimary",
        "AccentSecondary",
        "BorderLight",
        "BorderDark",
        "SelectionBg",
        "HoverBg",
        "ErrorColor",
        "SuccessColor",
        "ScrollbarTrack",
        "ScrollbarThumb",
        "ScrollbarHover",
        "SyntaxKeyword",
        "SyntaxString",
        "SyntaxComment",
        "SyntaxNumber",
        "SyntaxType",
        "SyntaxFunction",
        "SyntaxOperator",
        "SyntaxPreprocessor",
        "RenderHeading",
        "RenderLink",
        "RenderCodeBg",
        "RenderCodeFg",
        "RenderBlockquoteBorder",
        "RenderBlockquoteBg",
        "RenderTableBorder",
        "RenderTableHeaderBg",
        "EditorActiveLine",
        "EditorGutterError",
        "EditorGutterWarn",
        "EditorGutterInfo",
        "EditorMatchHighlight",
        "EditorFindHit",
        "EditorQuickFix",
        "SidebarBg",
        "SidebarFg",
        "ActivityBarBg",
        "ActivityBarFg",
        "ActivityBarBadgeBg",
        "ActivityBarBadgeFg",
        "BreadcrumbFg",
        "BreadcrumbFocusFg",
        "TabActiveBg",
        "TabInactiveBg",
        "TabActiveFg",
        "TabInactiveFg",
        "DiffInsertedBg",
        "DiffRemovedBg",
        "MinimapBg",
        "PeekViewBorderColor",
        "NotebookCellBg",
    };
    return kNames;
}

auto is_default_color(const Color& clr) -> bool
{
    return clr.r == 0 && clr.g == 0 && clr.b == 0;
}

} // anonymous namespace

auto ThemeCompatibilityScorer::score_theme(const Theme& theme) -> CompatibilityResult
{
    CompatibilityResult result;
    const auto& names = token_names();
    const auto total = names.size();

    // Base 10 tokens are always mapped (they're required fields)
    constexpr std::size_t kBaseTokens = 10;
    for (std::size_t idx = 0; idx < kBaseTokens && idx < total; ++idx)
    {
        result.mapped_tokens.push_back(names[idx]);
    }

    // Check extended tokens for default values
    auto check_color = [&](std::size_t idx, const Color& clr)
    {
        if (idx < total)
        {
            if (!is_default_color(clr))
            {
                result.mapped_tokens.push_back(names[idx]);
            }
            else
            {
                result.unmapped_tokens.push_back(names[idx]);
            }
        }
    };

    // Syntax tokens (indices 17-24)
    check_color(17, theme.syntax.keyword);
    check_color(18, theme.syntax.string_literal);
    check_color(19, theme.syntax.comment);
    check_color(20, theme.syntax.number);
    check_color(21, theme.syntax.type_name);
    check_color(22, theme.syntax.function_name);
    check_color(23, theme.syntax.operator_tok);
    check_color(24, theme.syntax.preprocessor);

    // Render tokens (indices 25-32)
    check_color(25, theme.render.heading);
    check_color(26, theme.render.link);
    check_color(27, theme.render.code_bg);
    check_color(28, theme.render.code_fg);
    check_color(29, theme.render.blockquote_border);
    check_color(30, theme.render.blockquote_bg);
    check_color(31, theme.render.table_border);
    check_color(32, theme.render.table_header_bg);

    // V9 extended semantic tokens (indices 40-56)
    check_color(40, theme.colors.sidebar_bg);
    check_color(41, theme.colors.sidebar_fg);
    check_color(42, theme.colors.activity_bar_bg);
    check_color(43, theme.colors.activity_bar_fg);
    check_color(44, theme.colors.activity_bar_badge_bg);
    check_color(45, theme.colors.activity_bar_badge_fg);
    check_color(46, theme.colors.breadcrumb_fg);
    check_color(47, theme.colors.breadcrumb_focus_fg);
    check_color(48, theme.colors.tab_active_bg);
    check_color(49, theme.colors.tab_inactive_bg);
    check_color(50, theme.colors.tab_active_fg);
    check_color(51, theme.colors.tab_inactive_fg);
    check_color(52, theme.colors.diff_inserted_bg);
    check_color(53, theme.colors.diff_removed_bg);
    check_color(54, theme.colors.minimap_bg);
    check_color(55, theme.colors.peek_view_border);
    check_color(56, theme.colors.notebook_cell_bg);

    // Mark remaining tokens as mapped (derived, semantic editor, etc.)
    for (std::size_t idx = kBaseTokens; idx < total; ++idx)
    {
        bool already_counted = false;
        for (const auto& mapped : result.mapped_tokens)
        {
            if (mapped == names[idx])
            {
                already_counted = true;
                break;
            }
        }
        for (const auto& unmapped : result.unmapped_tokens)
        {
            if (unmapped == names[idx])
            {
                already_counted = true;
                break;
            }
        }
        if (!already_counted)
        {
            result.mapped_tokens.push_back(names[idx]);
        }
    }

    if (total > 0)
    {
        result.score =
            static_cast<double>(result.mapped_tokens.size()) / static_cast<double>(total);
    }

    result.summary = fmt::format("{}/{} tokens mapped ({:.0f}% coverage)",
                                 result.mapped_tokens.size(),
                                 total,
                                 result.score * 100.0);

    result.repair_suggestions = suggest_repairs(theme);

    MARKAMP_LOG_INFO("Theme compatibility: {}", result.summary);
    return result;
}

auto ThemeCompatibilityScorer::suggest_repairs(const Theme& theme)
    -> std::vector<std::pair<std::string, std::string>>
{
    std::vector<std::pair<std::string, std::string>> suggestions;

    if (is_default_color(theme.colors.sidebar_bg))
    {
        suggestions.emplace_back("SidebarBg", "#1e1e32");
    }
    if (is_default_color(theme.colors.activity_bar_bg))
    {
        suggestions.emplace_back("ActivityBarBg", "#161628");
    }
    if (is_default_color(theme.colors.minimap_bg))
    {
        suggestions.emplace_back("MinimapBg", "#141426");
    }

    return suggestions;
}

} // namespace markamp::core
