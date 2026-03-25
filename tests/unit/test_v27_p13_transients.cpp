/// @file test_v27_p13_transients.cpp
/// @brief V27 Phase 13: Dialog, tooltip, notification, popover tokens, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27TransientSurfaceTokens.h"
#include "core/V27TransientDesignCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
TEST_CASE("V27 P13: Modal tokens", "[v27][p13]") {
    REQUIRE(V27ModalTokens::kMinWidth < V27ModalTokens::kMaxWidth);
    REQUIRE(V27ModalTokens::kRadius > 0);
    REQUIRE(V27ModalTokens::kBackdropAlpha <= 255);
}
TEST_CASE("V27 P13: Tooltip tokens", "[v27][p13]") {
    REQUIRE(V27TooltipTokens::kMaxWidth > 0);
    REQUIRE(V27TooltipTokens::kDelayMs > 0);
}
TEST_CASE("V27 P13: Notification tokens", "[v27][p13]") {
    REQUIRE(V27NotificationTokens::kToastWidth > 0);
    REQUIRE(V27NotificationTokens::kToastMaxVisible > 0);
    REQUIRE(V27NotificationTokens::kToastDurationMs > 0);
}
TEST_CASE("V27 P13: Popover tokens", "[v27][p13]") {
    REQUIRE(V27PopoverTokens::kMinWidth < V27PopoverTokens::kMaxWidth);
    REQUIRE(V27PopoverTokens::kRadius > 0);
    REQUIRE(V27PopoverTokens::kRowHeight > 0);
    REQUIRE(V27PopoverTokens::kShadowBlur > 0);
}
TEST_CASE("V27 P13: Transient coordinator registration", "[v27][p13]") {
    V27TransientDesignCoordinator coord;
    coord.register_surface({"Settings Dialog", TransientSurfaceClass::kDialog, 360, 600, 16, 16, true});
    coord.register_surface({"Toolbar Tooltip", TransientSurfaceClass::kTooltip, 0, 300, 0, 0, true});
    coord.register_surface({"Action Popover", TransientSurfaceClass::kPopover, 200, 400, 16, 8, true});
    REQUIRE(coord.surface_count() == 3);
    REQUIRE(coord.surfaces_with_canonical_icons() == 3);
    REQUIRE(coord.count_by_class(TransientSurfaceClass::kDialog) == 1);
    REQUIRE(coord.count_by_class(TransientSurfaceClass::kTooltip) == 1);
}
