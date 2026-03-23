/// @file V27TypographySystem.h
/// @brief V27 Phase 02 T02 — Redesigned typography hierarchy.
///
/// Premium typography roles extending the existing TypographyScale and
/// TypographyRole systems with V27-specific additions for shell, panel,
/// metadata, and badge contexts.
#pragma once

#include <string>

namespace markamp::core
{

// ============================================================================
// V27 Typography Roles
// ============================================================================

/// V27 typography role.
enum class V27TypographyRole
{
    // Shell and navigation
    kShellTitle,           ///< 11px semibold — workspace title, region labels
    kShellCaption,         ///< 10px — shell metadata, breadcrumb segments

    // Panel and section
    kPanelSectionLabel,    ///< 11px semibold — panel section headers
    kPanelGroupLabel,      ///< 10px semibold — collapsible group headers

    // Metadata and secondary
    kMetadataCaption,      ///< 10px — row metadata, file info, timestamps
    kMetadataStrong,       ///< 10px bold — emphasized metadata values

    // Badge and chip
    kBadgeCount,           ///< 9px bold — notification counts, badges
    kChipLabel,            ///< 10px — filter chips, tag labels

    // Actions
    kActionLabel,          ///< 11px semibold — menu items, toolbar labels
    kActionShortcut,       ///< 10px mono — keyboard shortcut text

    // Feedback
    kFeedbackTitle,        ///< 12px semibold — feedback/dialog titles
    kFeedbackBody,         ///< 11px — feedback body, toast messages

    // Canvas and creative
    kCanvasLabel,          ///< 11px — canvas object labels
    kCanvasMetadata,       ///< 9px — canvas object metadata

    // Code and mono
    kCodeInline,           ///< 12px mono — inline code in non-editor contexts
    kCodeSmall,            ///< 10px mono — small code annotations
};

/// Total V27 typography roles.
[[nodiscard]] constexpr auto v27_typography_role_count() noexcept -> int { return 16; }

/// V27 typography specification.
struct V27TypeSpec
{
    int size_pt;
    int weight;         ///< 400=normal, 600=semibold, 700=bold
    int line_height_px;
    bool is_mono{false};
};

/// Resolve a V27 typography role to its specification.
[[nodiscard]] constexpr auto v27_resolve_type(V27TypographyRole role) noexcept -> V27TypeSpec
{
    switch (role)
    {
    case V27TypographyRole::kShellTitle:        return {11, 600, 16, false};
    case V27TypographyRole::kShellCaption:      return {10, 400, 14, false};
    case V27TypographyRole::kPanelSectionLabel: return {11, 600, 16, false};
    case V27TypographyRole::kPanelGroupLabel:   return {10, 600, 14, false};
    case V27TypographyRole::kMetadataCaption:   return {10, 400, 14, false};
    case V27TypographyRole::kMetadataStrong:    return {10, 700, 14, false};
    case V27TypographyRole::kBadgeCount:        return {9,  700, 12, false};
    case V27TypographyRole::kChipLabel:         return {10, 400, 14, false};
    case V27TypographyRole::kActionLabel:       return {11, 600, 16, false};
    case V27TypographyRole::kActionShortcut:    return {10, 400, 14, true};
    case V27TypographyRole::kFeedbackTitle:     return {12, 600, 18, false};
    case V27TypographyRole::kFeedbackBody:      return {11, 400, 16, false};
    case V27TypographyRole::kCanvasLabel:       return {11, 400, 16, false};
    case V27TypographyRole::kCanvasMetadata:    return {9,  400, 12, false};
    case V27TypographyRole::kCodeInline:        return {12, 400, 18, true};
    case V27TypographyRole::kCodeSmall:         return {10, 400, 14, true};
    }
    return {11, 400, 16, false};
}

/// Check if a V27 typography role uses monospace font.
[[nodiscard]] constexpr auto v27_type_is_mono(V27TypographyRole role) noexcept -> bool
{
    return v27_resolve_type(role).is_mono;
}

/// Check if a V27 typography role is bold (weight >= 600).
[[nodiscard]] constexpr auto v27_type_is_bold(V27TypographyRole role) noexcept -> bool
{
    return v27_resolve_type(role).weight >= 600;
}

} // namespace markamp::core
