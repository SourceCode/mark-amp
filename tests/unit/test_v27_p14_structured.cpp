/// @file test_v27_p14_structured.cpp
/// @brief V27 Phase 14: Tree row, table, gallery, kanban, edge tokens, coordinator.
#include <catch2/catch_test_macros.hpp>
#include "ui/V27StructuredDataTokens.h"
#include "core/V27StructuredDataCoordinator.h"
using namespace markamp::ui;
using namespace markamp::core;
TEST_CASE("V27 P14: Tree row tokens", "[v27][p14]") {
    REQUIRE(V27TreeRowTokens::kRowHeight > 0);
    REQUIRE(V27TreeRowTokens::kGuideWidth > 0);
    REQUIRE(V27TreeRowTokens::kBadgeRadius > 0);
}
TEST_CASE("V27 P14: Table tokens", "[v27][p14]") {
    REQUIRE(V27TableTokens::kColumnMinWidth > 0);
    REQUIRE(V27TableTokens::kHeaderHeight > 0);
}
TEST_CASE("V27 P14: Structured edge tokens", "[v27][p14]") {
    REQUIRE(V27StructuredEdgeTokens::kEmptyIconSize >= 24);
    REQUIRE(V27StructuredEdgeTokens::kLoadingShimmerHeight > 0);
}
TEST_CASE("V27 P14: Gallery card tokens", "[v27][p14]") {
    REQUIRE(V27GalleryCardTokens::kCardWidth > 0);
    REQUIRE(V27GalleryCardTokens::kCardHeight > V27GalleryCardTokens::kCardWidth);
    REQUIRE(V27GalleryCardTokens::kPreviewRatioPct > 0);
    REQUIRE(V27GalleryCardTokens::kCardRadius > 0);
}
TEST_CASE("V27 P14: Kanban tokens", "[v27][p14]") {
    REQUIRE(V27KanbanTokens::kLaneMaxWidth > V27KanbanTokens::kLaneMinWidth);
    REQUIRE(V27KanbanTokens::kCardHeight > 0);
    REQUIRE(V27KanbanTokens::kCardRadius > 0);
    REQUIRE(V27KanbanTokens::kAddCardHeight > 0);
}
TEST_CASE("V27 P14: Structured data coordinator", "[v27][p14]") {
    V27StructuredDataCoordinator coord;
    coord.register_surface({"Explorer Tree", StructuredSurfaceType::kTree, true, true, true});
    coord.register_surface({"AV Table", StructuredSurfaceType::kTable, true, false, true});
    coord.register_surface({"AV Gallery", StructuredSurfaceType::kGallery, false, false, true});
    REQUIRE(coord.surface_count() == 3);
    REQUIRE(coord.rows_redesigned_count() == 2);
    REQUIRE(coord.edge_states_redesigned_count() == 1);
    REQUIRE(coord.count_by_type(StructuredSurfaceType::kTable) == 1);
    REQUIRE(coord.count_by_type(StructuredSurfaceType::kGallery) == 1);
}
