#pragma once

// V11 Phase 23: Node Block Rendering Theme Tokens And States
// Semantic theme token identifiers for all node editor visual elements.
// Pure enum — maps to ThemeColorToken via the rendering adapter layer.

#include <cstdint>
#include <string_view>

namespace markamp::node_editor
{

/// Semantic theme tokens for node editor visual elements.
/// These map to concrete ThemeColorToken values at the rendering adapter layer.
enum class NodeThemeToken : uint8_t
{
    // --- Canvas ---
    CanvasBackground,
    GridMajor,
    GridMinor,

    // --- Node chrome ---
    NodeBackground,
    NodeHeader,
    NodeHeaderText,
    NodeBorder,
    NodeBorderSelected,
    NodeBorderActive,
    NodeBorderError,
    NodeShadow,

    // --- Node states ---
    NodeMutedOverlay,
    NodeCollapseIndicator,

    // --- Socket colors by data type ---
    SocketFloat,
    SocketInt,
    SocketBool,
    SocketString,
    SocketVector,
    SocketColor,
    SocketImage,
    SocketGeometry,
    SocketShader,
    SocketObject,
    SocketEnum,
    SocketCustom,

    // --- Links ---
    LinkDefault,
    LinkActive,
    LinkPreview,
    LinkInvalid,

    // --- Selection ---
    SelectionRect,
    SelectionRectFill,
    LassoStroke,

    // --- Annotations ---
    FrameBackground,
    FrameBorder,
    FrameLabel,
    CommentBackground,
    CommentText,

    // --- Interaction feedback ---
    DragPreview,
    DropTargetHighlight,
    SearchHighlight,

    // --- Minimap ---
    MinimapBackground,
    MinimapViewport,
    MinimapNode,
};

/// Total number of NodeThemeToken values.
inline constexpr std::size_t kNodeThemeTokenCount =
    static_cast<std::size_t>(NodeThemeToken::MinimapNode) + 1;

/// Convert a NodeThemeToken to a display-friendly name.
[[nodiscard]] constexpr auto node_theme_token_name(NodeThemeToken token) noexcept
    -> std::string_view
{
    switch (token)
    {
        case NodeThemeToken::CanvasBackground:
            return "CanvasBackground";
        case NodeThemeToken::GridMajor:
            return "GridMajor";
        case NodeThemeToken::GridMinor:
            return "GridMinor";
        case NodeThemeToken::NodeBackground:
            return "NodeBackground";
        case NodeThemeToken::NodeHeader:
            return "NodeHeader";
        case NodeThemeToken::NodeHeaderText:
            return "NodeHeaderText";
        case NodeThemeToken::NodeBorder:
            return "NodeBorder";
        case NodeThemeToken::NodeBorderSelected:
            return "NodeBorderSelected";
        case NodeThemeToken::NodeBorderActive:
            return "NodeBorderActive";
        case NodeThemeToken::NodeBorderError:
            return "NodeBorderError";
        case NodeThemeToken::NodeShadow:
            return "NodeShadow";
        case NodeThemeToken::NodeMutedOverlay:
            return "NodeMutedOverlay";
        case NodeThemeToken::NodeCollapseIndicator:
            return "NodeCollapseIndicator";
        case NodeThemeToken::SocketFloat:
            return "SocketFloat";
        case NodeThemeToken::SocketInt:
            return "SocketInt";
        case NodeThemeToken::SocketBool:
            return "SocketBool";
        case NodeThemeToken::SocketString:
            return "SocketString";
        case NodeThemeToken::SocketVector:
            return "SocketVector";
        case NodeThemeToken::SocketColor:
            return "SocketColor";
        case NodeThemeToken::SocketImage:
            return "SocketImage";
        case NodeThemeToken::SocketGeometry:
            return "SocketGeometry";
        case NodeThemeToken::SocketShader:
            return "SocketShader";
        case NodeThemeToken::SocketObject:
            return "SocketObject";
        case NodeThemeToken::SocketEnum:
            return "SocketEnum";
        case NodeThemeToken::SocketCustom:
            return "SocketCustom";
        case NodeThemeToken::LinkDefault:
            return "LinkDefault";
        case NodeThemeToken::LinkActive:
            return "LinkActive";
        case NodeThemeToken::LinkPreview:
            return "LinkPreview";
        case NodeThemeToken::LinkInvalid:
            return "LinkInvalid";
        case NodeThemeToken::SelectionRect:
            return "SelectionRect";
        case NodeThemeToken::SelectionRectFill:
            return "SelectionRectFill";
        case NodeThemeToken::LassoStroke:
            return "LassoStroke";
        case NodeThemeToken::FrameBackground:
            return "FrameBackground";
        case NodeThemeToken::FrameBorder:
            return "FrameBorder";
        case NodeThemeToken::FrameLabel:
            return "FrameLabel";
        case NodeThemeToken::CommentBackground:
            return "CommentBackground";
        case NodeThemeToken::CommentText:
            return "CommentText";
        case NodeThemeToken::DragPreview:
            return "DragPreview";
        case NodeThemeToken::DropTargetHighlight:
            return "DropTargetHighlight";
        case NodeThemeToken::SearchHighlight:
            return "SearchHighlight";
        case NodeThemeToken::MinimapBackground:
            return "MinimapBackground";
        case NodeThemeToken::MinimapViewport:
            return "MinimapViewport";
        case NodeThemeToken::MinimapNode:
            return "MinimapNode";
    }
    return "Unknown";
}

} // namespace markamp::node_editor
