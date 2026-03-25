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
struct V27GalleryCardTokens {
    static constexpr int kCardWidth = 200;
    static constexpr int kCardHeight = 240;
    static constexpr int kPreviewRatioPct = 60;
    static constexpr int kCardRadius = 6;
    static constexpr int kCardGap = 12;
    static constexpr int kMetadataGap = 8;
    static constexpr int kMetadataPaddingH = 8;
    static constexpr int kBadgeOverlapOffset = 4;
};
struct V27KanbanTokens {
    static constexpr int kLaneMinWidth = 240;
    static constexpr int kLaneMaxWidth = 360;
    static constexpr int kLaneHeaderHeight = 36;
    static constexpr int kCardHeight = 80;
    static constexpr int kCardRadius = 6;
    static constexpr int kCardGap = 8;
    static constexpr int kCardPaddingH = 12;
    static constexpr int kCardPaddingV = 8;
    static constexpr int kLaneGap = 12;
    static constexpr int kAddCardHeight = 28;
};
} // namespace markamp::ui
