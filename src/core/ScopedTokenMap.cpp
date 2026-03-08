#include "ScopedTokenMap.h"

namespace markamp::core
{

ScopedTokenMap::ScopedTokenMap() = default;

void ScopedTokenMap::set(const std::string& token, const wxColour& color)
{
    token_values_[token] = color;
}

auto ScopedTokenMap::get_explicit(const std::string& token) const -> std::optional<wxColour>
{
    if (const auto it = token_values_.find(token); it != token_values_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

auto ScopedTokenMap::resolve(const std::string& token) const -> std::optional<wxColour>
{
    // Try the explicit token first
    if (auto color = get_explicit(token))
    {
        return color;
    }

    // Traverse the inheritance chain
    const std::vector<std::string> chain = inheritance_resolver_.get_resolution_chain(token);

    for (const auto& fallback_token : chain)
    {
        if (auto fallback_color = get_explicit(fallback_token))
        {
            return fallback_color;
        }
    }

    return std::nullopt;
}

auto ScopedTokenMap::resolve(ThemeColorToken enum_token) const -> std::optional<wxColour>
{
    const auto& mapping = get_enum_mapping();
    if (const auto it = mapping.find(enum_token); it != mapping.end())
    {
        return resolve(it->second);
    }
    return std::nullopt;
}

auto ScopedTokenMap::is_explicit(const std::string& token) const -> bool
{
    return token_values_.contains(token);
}

auto ScopedTokenMap::resolver() const -> const TokenInheritanceResolver&
{
    return inheritance_resolver_;
}

auto ScopedTokenMap::resolver() -> TokenInheritanceResolver&
{
    return inheritance_resolver_;
}

void ScopedTokenMap::clear()
{
    token_values_.clear();
}

auto ScopedTokenMap::get_explicit_tokens() const -> std::vector<std::string>
{
    std::vector<std::string> keys;
    keys.reserve(token_values_.size());
    for (const auto& [key, _] : token_values_)
    {
        keys.push_back(key);
    }
    return keys;
}

auto ScopedTokenMap::get_enum_mapping() -> const std::unordered_map<ThemeColorToken, std::string>&
{
    static const std::unordered_map<ThemeColorToken, std::string> mapping = {
        // Base tokens
        {ThemeColorToken::BgApp, "window.background"},
        {ThemeColorToken::BgPanel, "panel.background"},
        {ThemeColorToken::BgHeader, "titleBar.activeBackground"},
        {ThemeColorToken::BgInput, "input.background"},
        {ThemeColorToken::TextMain, "editor.foreground"},
        {ThemeColorToken::TextMuted, "window.foreground"}, // Dimmed in usage
        {ThemeColorToken::AccentPrimary, "focusBorder"},
        {ThemeColorToken::AccentSecondary, "button.background"},
        {ThemeColorToken::BorderLight, "window.border"},
        {ThemeColorToken::BorderDark, "contrastBorder"},

        // Derived UI tokens
        {ThemeColorToken::SelectionBg, "selection.background"},
        {ThemeColorToken::HoverBg, "list.hoverBackground"},
        {ThemeColorToken::ErrorColor, "errorForeground"},
        {ThemeColorToken::SuccessColor, "successForeground"},
        {ThemeColorToken::ScrollbarTrack, "scrollbarSlider.background"},
        {ThemeColorToken::ScrollbarThumb, "scrollbarSlider.hoverBackground"},
        {ThemeColorToken::ScrollbarHover, "scrollbarSlider.activeBackground"},

        // Phase 4: Syntax tokens
        {ThemeColorToken::SyntaxKeyword, "keyword"},
        {ThemeColorToken::SyntaxString, "string"},
        {ThemeColorToken::SyntaxComment, "comment"},
        {ThemeColorToken::SyntaxNumber, "number"},
        {ThemeColorToken::SyntaxType, "type"},
        {ThemeColorToken::SyntaxFunction, "function"},
        {ThemeColorToken::SyntaxOperator, "operator"},
        {ThemeColorToken::SyntaxPreprocessor, "preprocessor"},

        // V16 Phase 16: Fine-grained syntax tokens
        {ThemeColorToken::SyntaxVariable, "variable"},
        {ThemeColorToken::SyntaxConstant, "constant"},
        {ThemeColorToken::SyntaxTag, "entity.name.tag"},
        {ThemeColorToken::SyntaxAttribute, "entity.other.attribute-name"},
        {ThemeColorToken::SyntaxProperty, "variable.other.property"},
        {ThemeColorToken::SyntaxNamespace, "entity.name.namespace"},
        {ThemeColorToken::SyntaxEnum, "entity.name.type.enum"},
        {ThemeColorToken::SyntaxInterface, "entity.name.type.interface"},
        {ThemeColorToken::SyntaxStruct, "entity.name.type.struct"},
        {ThemeColorToken::SyntaxParameter, "variable.parameter"},
        {ThemeColorToken::SyntaxDecorator, "meta.decorator"},
        {ThemeColorToken::SyntaxRegex, "string.regexp"},
        {ThemeColorToken::SyntaxEscape, "constant.character.escape"},
        {ThemeColorToken::SyntaxDocComment, "comment.block.documentation"},
        {ThemeColorToken::SyntaxMacro, "entity.name.function.macro"},

        // Render tokens
        {ThemeColorToken::RenderHeading, "markdown.heading"},
        {ThemeColorToken::RenderLink, "markdown.link"},
        {ThemeColorToken::RenderCodeBg, "markdown.codeBackground"},
        {ThemeColorToken::RenderCodeFg, "markdown.codeForeground"},
        {ThemeColorToken::RenderBlockquoteBorder, "markdown.blockquoteBorder"},
        {ThemeColorToken::RenderBlockquoteBg, "markdown.blockquoteBackground"},
        {ThemeColorToken::RenderTableBorder, "markdown.tableBorder"},
        {ThemeColorToken::RenderTableHeaderBg, "markdown.tableHeaderBackground"},

        // Semantic editor tokens
        {ThemeColorToken::EditorActiveLine, "editor.lineHighlightBackground"},
        {ThemeColorToken::EditorGutterError, "editorError.foreground"},
        {ThemeColorToken::EditorGutterWarn, "editorWarning.foreground"},
        {ThemeColorToken::EditorGutterInfo, "editorInfo.foreground"},
        {ThemeColorToken::EditorMatchHighlight, "editor.wordHighlightBackground"},
        {ThemeColorToken::EditorFindHit, "editor.findMatchBackground"},
        {ThemeColorToken::EditorQuickFix, "editorLightBulb.foreground"},

        // Extended UI tokens
        {ThemeColorToken::SidebarBg, "sideBar.background"},
        {ThemeColorToken::SidebarFg, "sideBar.foreground"},
        {ThemeColorToken::ActivityBarBg, "activityBar.background"},
        {ThemeColorToken::ActivityBarFg, "activityBar.foreground"},
        {ThemeColorToken::ActivityBarBadgeBg, "activityBarBadge.background"},
        {ThemeColorToken::ActivityBarBadgeFg, "activityBarBadge.foreground"},
        {ThemeColorToken::BreadcrumbFg, "breadcrumb.foreground"},
        {ThemeColorToken::BreadcrumbFocusFg, "breadcrumb.focusForeground"},
        {ThemeColorToken::TabActiveBg, "tab.activeBackground"},
        {ThemeColorToken::TabInactiveBg, "tab.inactiveBackground"},
        {ThemeColorToken::TabActiveFg, "tab.activeForeground"},
        {ThemeColorToken::TabInactiveFg, "tab.inactiveForeground"},
        {ThemeColorToken::DiffInsertedBg, "diffEditor.insertedTextBackground"},
        {ThemeColorToken::DiffRemovedBg, "diffEditor.removedTextBackground"},
        {ThemeColorToken::MinimapBg, "minimap.background"},
        {ThemeColorToken::PeekViewBorderColor, "peekView.border"},
        {ThemeColorToken::NotebookCellBg, "notebook.cellEditorBackground"},

        // Control state tokens
        {ThemeColorToken::ControlBgNormal, "button.secondaryBackground"},
        {ThemeColorToken::ControlBgHover, "button.secondaryHoverBackground"},
        {ThemeColorToken::ControlBgPressed, "button.background"}, // Uses primary for pressed?
        {ThemeColorToken::ControlBgFocus, "button.secondaryBackground"},
        {ThemeColorToken::ControlBgDisabled, "button.disabledBackground"},
        {ThemeColorToken::ControlBgSelected, "button.background"},
        {ThemeColorToken::ControlFgNormal, "button.secondaryForeground"},
        {ThemeColorToken::ControlFgDisabled, "button.disabledForeground"},
        {ThemeColorToken::ControlBorderNormal, "button.border"},
        {ThemeColorToken::ControlBorderFocus, "focusBorder"},
        {ThemeColorToken::FocusRingColor, "focusBorder"},
    };
    return mapping;
}

} // namespace markamp::core
