/// @file V27SettingsTokens.h
/// @brief V27 Phase 12 — Settings, preferences, theme management, controls.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27SettingsShellTokens {
    static constexpr int kCategoryRailWidth = 220;
    static constexpr int kSearchHeaderHeight = 40;
    static constexpr int kSearchInputHeight = 32;
    static constexpr int kCategoryIconSize = 16;
    static constexpr int kCategoryItemHeight = 28;
    static constexpr int kCategoryIconGap = 8;
};
struct V27ThemeGalleryTokens {
    static constexpr int kCardWidth = 220;
    static constexpr int kCardHeight = 160;
    static constexpr int kPreviewHeight = 100;
    static constexpr int kCardRadius = 8;
    static constexpr int kCardGap = 16;
    static constexpr int kMetadataHeight = 48;
    static constexpr int kBadgeSize = 20;
};
struct V27ControlFamilyTokens {
    static constexpr int kButtonHeight = 28;
    static constexpr int kButtonHeightSm = 24;
    static constexpr int kButtonPaddingH = 12;
    static constexpr int kButtonRadius = 4;
    static constexpr int kToggleWidth = 36;
    static constexpr int kToggleHeight = 20;
    static constexpr int kToggleRadius = 10;
    static constexpr int kInputHeight = 28;
    static constexpr int kInputRadius = 4;
    static constexpr int kInputPaddingH = 8;
    static constexpr int kChipHeight = 22;
    static constexpr int kChipRadius = 11;
    static constexpr int kChipPaddingH = 8;
    static constexpr int kSelectHeight = 28;
    static constexpr int kSelectRadius = 4;
};
} // namespace markamp::ui
