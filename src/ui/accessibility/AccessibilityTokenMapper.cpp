#include "AccessibilityTokenMapper.h"

namespace markamp::ui::accessibility
{

using Token = core::ThemeColorToken;

auto AccessibilityTokenMapper::get_token_pairs() -> std::vector<std::pair<Token, Token>>
{
    // Phase 05 Task 23: Map foreground tokens to their most likely background tokens
    // for runtime WCAG contrast validation.
    return {// Main App UI
            {Token::TextMain, Token::BgApp},
            {Token::TextMuted, Token::BgApp},
            {Token::AccentPrimary, Token::BgApp},

            // Panel UI
            {Token::TextMain, Token::BgPanel},
            {Token::TextMuted, Token::BgPanel},
            {Token::AccentPrimary, Token::BgPanel},

            // Header UI
            {Token::TextMain, Token::BgHeader},
            {Token::TextMuted, Token::BgHeader},

            // Input UI
            {Token::TextMain, Token::BgInput},
            {Token::TextMuted, Token::BgInput},

            // Phase 4 Syntax / Editor (mostly against EditorActiveLine or BgApp/EditorBg)
            // Since we don't have separate semantic tokens in this mapping, map against BgApp as
            // default editor background
            {Token::SyntaxKeyword, Token::BgApp},
            {Token::SyntaxString, Token::BgApp},
            {Token::SyntaxComment, Token::BgApp},
            {Token::SyntaxNumber, Token::BgApp},
            {Token::SyntaxType, Token::BgApp},
            {Token::SyntaxFunction, Token::BgApp},
            {Token::SyntaxOperator, Token::BgApp},
            {Token::SyntaxPreprocessor, Token::BgApp},

            // Render preview
            {Token::RenderHeading, Token::BgApp},
            {Token::RenderLink, Token::BgApp},
            {Token::RenderCodeFg, Token::RenderCodeBg},

            // V9 Phase 3: Semantic UI
            {Token::SidebarFg, Token::SidebarBg},
            {Token::ActivityBarFg, Token::ActivityBarBg},
            {Token::ActivityBarBadgeFg, Token::ActivityBarBadgeBg},
            {Token::BreadcrumbFg, Token::BgApp},
            {Token::BreadcrumbFocusFg, Token::BgApp},
            {Token::TabActiveFg, Token::TabActiveBg},
            {Token::TabInactiveFg, Token::TabInactiveBg},

            // V10 Control states
            {Token::ControlFgNormal, Token::ControlBgNormal},
            {Token::ControlFgNormal, Token::ControlBgHover},
            {Token::ControlFgNormal, Token::ControlBgPressed},
            {Token::ControlFgNormal, Token::ControlBgFocus},
            {Token::ControlFgNormal, Token::ControlBgSelected},
            {Token::ControlFgDisabled, Token::ControlBgDisabled}};
}

} // namespace markamp::ui::accessibility
