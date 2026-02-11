#include "ui/CustomChrome.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("CustomChrome: constants are valid", "[ui][chrome]")
{
    REQUIRE(CustomChrome::kChromeHeight == 40);
    REQUIRE(CustomChrome::kButtonWidth == 36);
    REQUIRE(CustomChrome::kButtonIconSize == 12);
    REQUIRE(CustomChrome::kLogoDotRadius == 7);
    REQUIRE(CustomChrome::kResizeZone == 4);
}

TEST_CASE("CustomChrome: HitZone enum values are distinct", "[ui][chrome]")
{
    REQUIRE(CustomChrome::HitZone::None != CustomChrome::HitZone::DragArea);
    REQUIRE(CustomChrome::HitZone::DragArea != CustomChrome::HitZone::MinimizeButton);
    REQUIRE(CustomChrome::HitZone::MinimizeButton != CustomChrome::HitZone::MaximizeButton);
    REQUIRE(CustomChrome::HitZone::MaximizeButton != CustomChrome::HitZone::CloseButton);
    REQUIRE(CustomChrome::HitZone::CloseButton != CustomChrome::HitZone::SidebarToggle);
}

TEST_CASE("CustomChrome: ResizeEdge values from platform are distinct", "[ui][chrome]")
{
    using markamp::platform::ResizeEdge;

    REQUIRE(ResizeEdge::None != ResizeEdge::Top);
    REQUIRE(ResizeEdge::Top != ResizeEdge::Bottom);
    REQUIRE(ResizeEdge::Left != ResizeEdge::Right);
    REQUIRE(ResizeEdge::TopLeft != ResizeEdge::BottomRight);
    REQUIRE(ResizeEdge::TopRight != ResizeEdge::BottomLeft);
}
