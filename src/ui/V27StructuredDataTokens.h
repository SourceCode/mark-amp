/// @file V27StructuredDataTokens.h
/// @brief V27 Phase 14 — Trees, lists, tables, AV, structured information.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27TreeRowTokens {
    static constexpr int kRowHeight = 22;
    static constexpr int kIndentWidth = 16;
    static constexpr int kIconSize = 16;
    static constexpr int kIconGap = 4;
    static constexpr int kBadgeMinWidth = 18;
    static constexpr int kBadgeHeight = 16;
    static constexpr int kBadgeRadius = 8;
    static constexpr int kExpanderSize = 14;
    static constexpr int kGuideWidth = 1;
    static constexpr int kGuideOpacityPct = 20;
};
struct V27TableTokens {
    static constexpr int kHeaderHeight = 28;
    static constexpr int kHeaderPaddingH = 8;
    static constexpr int kRowHeight = 28;
    static constexpr int kCellPaddingH = 8;
    static constexpr int kCellPaddingV = 4;
    static constexpr int kGridLineWidth = 1;
    static constexpr int kSortIconSize = 12;
    static constexpr int kFilterChipHeight = 22;
    static constexpr int kResizeHandleWidth = 8;
    static constexpr int kColumnMinWidth = 60;
};
struct V27StructuredEdgeTokens {
    static constexpr int kEmptyIconSize = 32;
    static constexpr int kEmptyTitleGap = 12;
    static constexpr int kEmptyBodyGap = 8;
    static constexpr int kLoadingShimmerHeight = 16;
    static constexpr int kLoadingShimmerGap = 8;
    static constexpr int kErrorIconSize = 16;
    static constexpr int kErrorBannerHeight = 32;
};
} // namespace markamp::ui
