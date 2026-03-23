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

    // V22 Phase 01: Container surface tokens
    ContainerBgSurface,
    ContainerBgElevated,
    ContainerBgSunken,
    ContainerBorderSubtle,
    ContainerShadow,

    // V22 Phase 01: State feedback tokens
    WarningColor,
    InfoColor,
    WarningBg,
    ErrorBg,
    SuccessBg,
    InfoBg,

    // V22 Phase 02–05: Navigation & shell chrome tokens
    BreadcrumbSeparatorFg,
    TabModifiedDot,
    TabPreviewBg,
    TabPinnedBg,
    SplitterHoverBg,
    SplitterDragBg,
    TitleBarInactiveBg,

    // V22 Phase 03–04: Panel & command surface tokens
    PanelHeaderBg,
    PanelHeaderFg,
    PanelHeaderBorder,
    ToolbarSeparator,
    CommandPaletteBg,
    CommandPaletteInputBg,
    CommandPaletteResultHoverBg,
    CommandPaletteShortcutFg,
    ContextMenuBg,
    ContextMenuBorder,
    ContextMenuSeparator,
    TooltipBg,
    TooltipFg,
    TooltipBorder,

    // V26 Phase 01: Surface tier tokens
    SurfaceShellBg,
    SurfaceWorkBg,
    SurfaceElevatedBg,
    SurfaceOverlayBg,
    SurfaceSunkenBg,
    SurfaceCanvasBg,

    // V26 Phase 01: Divider grammar tokens
    DividerSectionColor,
    DividerGroupColor,
    DividerInlineColor,
    DividerSurfaceBorderColor,
    DividerSubtleRuleColor,

    // V26 Phase 01: Interaction state overlay tokens
    StateHoverOverlay,
    StatePressedOverlay,
    StateFocusRing,
    StateSelectedBg,
    StateSelectedFg,
    StateDisabledFg,
    StateDisabledBg,
    StateDragGhost,
    StateDropTargetBorder,

    // V26 Phase 01: Feedback severity tokens
    FeedbackInfoFg,
    FeedbackInfoBorder,
    FeedbackSuccessFg,
    FeedbackSuccessBorder,
    FeedbackWarningFg,
    FeedbackWarningBorder,
    FeedbackErrorFg,
    FeedbackErrorBorder,

    // V26 Phase 01: Canvas surface tokens
    CanvasGridColor,
    CanvasSelectionHandleBg,
    CanvasSelectionHandleBorder,
    CanvasGuideColor,
    CanvasInlineEditBg,
    CanvasInlineEditBorder,
};

/// Total number of ThemeColorToken values.
static constexpr std::size_t kColorTokenCount =
    static_cast<std::size_t>(ThemeColorToken::CanvasInlineEditBorder) + 1;

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

    // V22 Phase 01: Extended typography scale tokens
    UICaption,   /// 10px — captions, timestamps, tertiary metadata
    UIBodyLarge, /// 13px — primary editor text, prominent body
    UISubtitle,  /// 14px — section headers, panel titles
    UIDisplay,   /// 20px — hero text, welcome headings

    // V26 Phase 01: Premium typography role tokens
    UIRowDescription,   /// 11px — settings help text, secondary metadata
    UIBadgeLabel,       /// 10px bold — notification counts, tags
    UIButtonLabel,      /// 12px semibold — action button text
    UIEmptyStateTitle,  /// 16px bold — empty panel headings
};

} // namespace markamp::core

