/// @file V27PanelTokens.h
/// @brief V27 Phase 11 — Panels, explorers, inspectors, side surfaces.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27PanelShellTokens {
    static constexpr int kHeaderHeight = 32;
    static constexpr int kHeaderPaddingH = 12;
    static constexpr int kHeaderIconSize = 16;
    static constexpr int kHeaderIconGap = 6;
    static constexpr int kHeaderActionSize = 20;
    static constexpr int kHeaderActionGap = 4;
    static constexpr int kDividerThickness = 1;
    static constexpr int kSectionGap = 16;
    static constexpr int kGroupGap = 8;
};
struct V27DenseRowTokens {
    static constexpr int kRowHeight = 22;
    static constexpr int kRowHeightCompact = 18;
    static constexpr int kIndentWidth = 16;
    static constexpr int kIconColumnWidth = 16;
    static constexpr int kIconGap = 4;
    static constexpr int kBadgeSize = 14;
    static constexpr int kBadgePadding = 4;
    static constexpr int kMetadataGap = 8;
    static constexpr int kRowPaddingH = 8;
    static constexpr int kNestGuideWidth = 1;
};
} // namespace markamp::ui
