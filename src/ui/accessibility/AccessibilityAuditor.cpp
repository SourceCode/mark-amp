#include "AccessibilityAuditor.h"

#include "AccessibilityTokenMapper.h"
#include "core/ContrastRatioValidator.h"

namespace markamp::ui::accessibility
{

auto AccessibilityAuditor::run_contrast_audit(const core::Theme& theme) -> ContrastAuditReport
{
    ContrastAuditReport report;
    const auto pairs = AccessibilityTokenMapper::get_token_pairs();
    report.total_pairs = static_cast<int>(pairs.size());

    for (const auto& [fg_token, bg_token] : pairs)
    {
        wxColour fg_color = resolve_token_to_color(theme, fg_token);
        wxColour bg_color = resolve_token_to_color(theme, bg_token);

        auto result = core::ContrastRatioValidator::validate(fg_color, bg_color);

        if (result.is_aaa_compliant())
        {
            report.compliant_aaa++;
            report.compliant_aa++; // AAA implies AA
        }
        else if (result.is_aa_compliant())
        {
            report.compliant_aa++;
        }
        else
        {
            report.failures++;
            report.failure_details.push_back(
                {token_to_string(fg_token), token_to_string(bg_token), result.ratio});
        }
    }

    return report;
}

// Map the enum token back to a color retrieved from the `Theme` flat-struct.
auto AccessibilityAuditor::resolve_token_to_color(const core::Theme& theme,
                                                  core::ThemeColorToken token) -> wxColour
{
    switch (token)
    {
        case core::ThemeColorToken::BgApp:
            return theme.colors.bg_app.to_wx_colour();
        case core::ThemeColorToken::BgPanel:
            return theme.colors.bg_panel.to_wx_colour();
        case core::ThemeColorToken::BgHeader:
            return theme.colors.bg_header.to_wx_colour();
        case core::ThemeColorToken::BgInput:
            return theme.colors.bg_input.to_wx_colour();
        case core::ThemeColorToken::TextMain:
            return theme.colors.text_main.to_wx_colour();
        case core::ThemeColorToken::TextMuted:
            return theme.colors.text_muted.to_wx_colour();
        case core::ThemeColorToken::AccentPrimary:
            return theme.colors.accent_primary.to_wx_colour();
        case core::ThemeColorToken::AccentSecondary:
            return theme.colors.accent_secondary.to_wx_colour();
        case core::ThemeColorToken::BorderLight:
            return theme.colors.border_light.to_wx_colour();
        case core::ThemeColorToken::BorderDark:
            return theme.colors.border_dark.to_wx_colour();
        case core::ThemeColorToken::SelectionBg:
            return theme.selection_bg().to_wx_colour();
        case core::ThemeColorToken::HoverBg:
            return theme.hover_bg().to_wx_colour();
        case core::ThemeColorToken::ErrorColor:
            return theme.error_color().to_wx_colour();
        case core::ThemeColorToken::SuccessColor:
            return theme.success_color().to_wx_colour();
        case core::ThemeColorToken::ScrollbarTrack:
            return theme.scrollbar_track().to_wx_colour();
        case core::ThemeColorToken::ScrollbarThumb:
            return theme.scrollbar_thumb().to_wx_colour();
        case core::ThemeColorToken::ScrollbarHover:
            return theme.scrollbar_hover().to_wx_colour();

        case core::ThemeColorToken::SyntaxKeyword:
            return theme.syntax.keyword.to_wx_colour();
        case core::ThemeColorToken::SyntaxString:
            return theme.syntax.string_literal.to_wx_colour();
        case core::ThemeColorToken::SyntaxComment:
            return theme.syntax.comment.to_wx_colour();
        case core::ThemeColorToken::SyntaxNumber:
            return theme.syntax.number.to_wx_colour();
        case core::ThemeColorToken::SyntaxType:
            return theme.syntax.type_name.to_wx_colour();
        case core::ThemeColorToken::SyntaxFunction:
            return theme.syntax.function_name.to_wx_colour();
        case core::ThemeColorToken::SyntaxOperator:
            return theme.syntax.operator_tok.to_wx_colour();
        case core::ThemeColorToken::SyntaxPreprocessor:
            return theme.syntax.preprocessor.to_wx_colour();

        case core::ThemeColorToken::RenderHeading:
            return theme.render.heading.to_wx_colour();
        case core::ThemeColorToken::RenderLink:
            return theme.render.link.to_wx_colour();
        case core::ThemeColorToken::RenderCodeBg:
            return theme.render.code_bg.to_wx_colour();
        case core::ThemeColorToken::RenderCodeFg:
            return theme.render.code_fg.to_wx_colour();
        case core::ThemeColorToken::RenderBlockquoteBorder:
            return theme.render.blockquote_border.to_wx_colour();
        case core::ThemeColorToken::RenderBlockquoteBg:
            return theme.render.blockquote_bg.to_wx_colour();
        case core::ThemeColorToken::RenderTableBorder:
            return theme.render.table_border.to_wx_colour();
        case core::ThemeColorToken::RenderTableHeaderBg:
            return theme.render.table_header_bg.to_wx_colour();

        case core::ThemeColorToken::EditorActiveLine:
            return theme.colors.editor_active_line.to_wx_colour();
        case core::ThemeColorToken::EditorGutterError:
            return theme.colors.editor_gutter_error.to_wx_colour();
        case core::ThemeColorToken::EditorGutterWarn:
            return theme.colors.editor_gutter_warn.to_wx_colour();
        case core::ThemeColorToken::EditorGutterInfo:
            return theme.colors.editor_gutter_info.to_wx_colour();
        case core::ThemeColorToken::EditorMatchHighlight:
            return theme.colors.editor_match_highlight.to_wx_colour();
        case core::ThemeColorToken::EditorFindHit:
            return theme.colors.editor_find_hit.to_wx_colour();
        case core::ThemeColorToken::EditorQuickFix:
            return theme.colors.editor_quick_fix.to_wx_colour();

        case core::ThemeColorToken::SidebarBg:
            return theme.colors.sidebar_bg.to_wx_colour();
        case core::ThemeColorToken::SidebarFg:
            return theme.colors.sidebar_fg.to_wx_colour();
        case core::ThemeColorToken::ActivityBarBg:
            return theme.colors.activity_bar_bg.to_wx_colour();
        case core::ThemeColorToken::ActivityBarFg:
            return theme.colors.activity_bar_fg.to_wx_colour();
        case core::ThemeColorToken::ActivityBarBadgeBg:
            return theme.colors.activity_bar_badge_bg.to_wx_colour();
        case core::ThemeColorToken::ActivityBarBadgeFg:
            return theme.colors.activity_bar_badge_fg.to_wx_colour();
        case core::ThemeColorToken::BreadcrumbFg:
            return theme.colors.breadcrumb_fg.to_wx_colour();
        case core::ThemeColorToken::BreadcrumbFocusFg:
            return theme.colors.breadcrumb_focus_fg.to_wx_colour();
        case core::ThemeColorToken::TabActiveBg:
            return theme.colors.tab_active_bg.to_wx_colour();
        case core::ThemeColorToken::TabInactiveBg:
            return theme.colors.tab_inactive_bg.to_wx_colour();
        case core::ThemeColorToken::TabActiveFg:
            return theme.colors.tab_active_fg.to_wx_colour();
        case core::ThemeColorToken::TabInactiveFg:
            return theme.colors.tab_inactive_fg.to_wx_colour();
        case core::ThemeColorToken::DiffInsertedBg:
            return theme.colors.diff_inserted_bg.to_wx_colour();
        case core::ThemeColorToken::DiffRemovedBg:
            return theme.colors.diff_removed_bg.to_wx_colour();
        case core::ThemeColorToken::MinimapBg:
            return theme.colors.minimap_bg.to_wx_colour();
        case core::ThemeColorToken::PeekViewBorderColor:
            return theme.colors.peek_view_border.to_wx_colour();
        case core::ThemeColorToken::NotebookCellBg:
            return theme.colors.notebook_cell_bg.to_wx_colour();

        case core::ThemeColorToken::ControlBgNormal:
            return theme.control_bg_normal.to_wx_colour();
        case core::ThemeColorToken::ControlBgHover:
            return theme.control_bg_hover.to_wx_colour();
        case core::ThemeColorToken::ControlBgPressed:
            return theme.control_bg_pressed.to_wx_colour();
        case core::ThemeColorToken::ControlBgFocus:
            return theme.control_bg_focus.to_wx_colour();
        case core::ThemeColorToken::ControlBgDisabled:
            return theme.control_bg_disabled.to_wx_colour();
        case core::ThemeColorToken::ControlBgSelected:
            return theme.control_bg_selected.to_wx_colour();
        case core::ThemeColorToken::ControlFgNormal:
            return theme.control_fg_normal.to_wx_colour();
        case core::ThemeColorToken::ControlFgDisabled:
            return theme.control_fg_disabled.to_wx_colour();
        case core::ThemeColorToken::ControlBorderNormal:
            return theme.control_border_normal.to_wx_colour();
        case core::ThemeColorToken::ControlBorderFocus:
            return theme.control_border_focus.to_wx_colour();
        case core::ThemeColorToken::FocusRingColor:
            return theme.focus_ring_color.to_wx_colour();

        default:
            return theme.colors.bg_app.to_wx_colour();
    }
}

auto AccessibilityAuditor::token_to_string(core::ThemeColorToken token) -> std::string
{
    // A simplified toString primarily for debugging/logging. We cast to int.
    return "TokenID_" + std::to_string(static_cast<int>(token));
}

} // namespace markamp::ui::accessibility
