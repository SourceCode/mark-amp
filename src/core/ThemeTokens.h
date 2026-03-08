#pragma once

#include <cstddef>

namespace markamp::core
{

/// Color tokens for themed drawing (10 base + 7 derived + extended UI).
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

    // V16 Phase 16: Fine-grained syntax tokens
    SyntaxVariable,
    SyntaxConstant,
    SyntaxTag,
    SyntaxAttribute,
    SyntaxProperty,
    SyntaxNamespace,
    SyntaxEnum,
    SyntaxInterface,
    SyntaxStruct,
    SyntaxParameter,
    SyntaxDecorator,
    SyntaxRegex,
    SyntaxEscape,
    SyntaxDocComment,
    SyntaxMacro,

    // Phase 4: Render/preview tokens
    RenderHeading,
    RenderLink,
    RenderCodeBg,
    RenderCodeFg,
    RenderBlockquoteBorder,
    RenderBlockquoteBg,
    RenderTableBorder,
    RenderTableHeaderBg,

    // V8 Phase 9: Semantic editor tokens
    EditorActiveLine,
    EditorGutterError,
    EditorGutterWarn,
    EditorGutterInfo,
    EditorMatchHighlight,
    EditorFindHit,
    EditorQuickFix,

    // V9 Phase 3: Extended semantic tokens
    SidebarBg,
    SidebarFg,
    ActivityBarBg,
    ActivityBarFg,
    ActivityBarBadgeBg,
    ActivityBarBadgeFg,
    BreadcrumbFg,
    BreadcrumbFocusFg,
    TabActiveBg,
    TabInactiveBg,
    TabActiveFg,
    TabInactiveFg,
    DiffInsertedBg,
    DiffRemovedBg,
    MinimapBg,
    PeekViewBorderColor,
    NotebookCellBg,

    // V10 Phase 02: Control state tokens
    ControlBgNormal,
    ControlBgHover,
    ControlBgPressed,
    ControlBgFocus,
    ControlBgDisabled,
    ControlBgSelected,
    ControlFgNormal,
    ControlFgDisabled,
    ControlBorderNormal,
    ControlBorderFocus,
    FocusRingColor,
};

/// Total number of ThemeColorToken values.
static constexpr std::size_t kColorTokenCount =
    static_cast<std::size_t>(ThemeColorToken::FocusRingColor) + 1;

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

} // namespace markamp::core
