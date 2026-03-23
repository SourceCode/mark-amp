/// @file V27NavigationTokens.h
/// @brief V27 Phase 06 — Tabs, breadcrumbs, navigation history, context.
#pragma once
#include <cstdint>
namespace markamp::ui
{
struct V27TabBarTokens {
    static constexpr int kTabHeight = 36;
    static constexpr int kTabMinWidth = 80;
    static constexpr int kTabMaxWidth = 240;
    static constexpr int kTabPaddingH = 12;
    static constexpr int kCloseButtonSize = 16;
    static constexpr int kCloseButtonGap = 4;
    static constexpr int kDirtyIndicatorSize = 8;
    static constexpr int kTabIconSize = 16;
    static constexpr int kTabIconGap = 6;
    static constexpr int kActiveIndicatorHeight = 2;
    static constexpr int kTabRadius = 4;
};
struct V27BreadcrumbTokens {
    static constexpr int kHeight = 22;
    static constexpr int kSegmentPaddingH = 6;
    static constexpr int kSeparatorWidth = 16;
    static constexpr int kIconSize = 14;
    static constexpr int kIconGap = 4;
    static constexpr int kDropdownMaxHeight = 300;
};
struct V27HistoryTokens {
    static constexpr int kButtonSize = 24;
    static constexpr int kButtonIconSize = 14;
    static constexpr int kButtonGap = 2;
    static constexpr int kHistoryListRowHeight = 28;
    static constexpr int kHistoryListMaxHeight = 300;
};
} // namespace markamp::ui
