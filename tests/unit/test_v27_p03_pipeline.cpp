/// @file test_v27_p03_pipeline.cpp
/// @brief V27 Phase 03: Canonical pipeline, emoji replacement, migration tracker.
#include <catch2/catch_test_macros.hpp>
#include "core/MuiIconPipeline.h"
#include "core/IconMetricsPolicy.h"
#include "ui/IconProvider.h"
#include "core/V27IconMigrationTracker.h"

using namespace markamp::core;
using namespace markamp::ui;

TEST_CASE("V27 P03: Pipeline is canonical", "[v27][p03]") {
    REQUIRE(MuiIconPipeline::is_canonical());
}
TEST_CASE("V27 P03: Icon state count includes V27 additions", "[v27][p03]") {
    REQUIRE(icon_state_count() == 9);
    [[maybe_unused]] auto f = IconState::kFocused;
    [[maybe_unused]] auto d = IconState::kDragging;
    [[maybe_unused]] auto w = IconState::kWarning;
    REQUIRE(true);
}
TEST_CASE("V27 P03: Icon component family count", "[v27][p03]") {
    REQUIRE(icon_component_family_count() == 16);
}
TEST_CASE("V27 P03: V27 component families compile", "[v27][p03]") {
    [[maybe_unused]] auto n = IconComponentFamily::kNotebook;
    [[maybe_unused]] auto c = IconComponentFamily::kCanvas;
    [[maybe_unused]] auto s = IconComponentFamily::kSettings;
    [[maybe_unused]] auto e = IconComponentFamily::kEmptyState;
    [[maybe_unused]] auto b = IconComponentFamily::kBreadcrumb;
    [[maybe_unused]] auto p = IconComponentFamily::kCommandPalette;
    REQUIRE(true);
}
TEST_CASE("V27 P03: IconProvider uses MUI IDs not emoji", "[v27][p03]") {
    REQUIRE(std::string(IconProvider::kFileIcon) == "mui-file-text");
    REQUIRE(std::string(IconProvider::kEditIcon) == "mui-pencil");
    REQUIRE(std::string(IconProvider::kViewIcon) == "mui-eye");
    REQUIRE(std::string(IconProvider::kNavigationIcon) == "mui-compass");
    REQUIRE(std::string(IconProvider::kTerminalIcon) == "mui-terminal");
    REQUIRE(std::string(IconProvider::kExtensionIcon) == "mui-puzzle-piece");
    REQUIRE(std::string(IconProvider::kEditorIcon) == "mui-edit-3");
    REQUIRE(std::string(IconProvider::kSearchIcon) == "mui-search");
    REQUIRE(std::string(IconProvider::kDebugIcon) == "mui-bug");
    REQUIRE(std::string(IconProvider::kSettingsIcon) == "mui-settings");
    REQUIRE(std::string(IconProvider::kDefaultIcon) == "mui-circle-dot");
}
TEST_CASE("V27 P03: IconProvider V27 additional icons", "[v27][p03]") {
    REQUIRE(std::string(IconProvider::kCanvasIcon) == "mui-layout");
    REQUIRE(std::string(IconProvider::kNotebookIcon) == "mui-book-open");
    REQUIRE(std::string(IconProvider::kThemeIcon) == "mui-palette");
    REQUIRE(std::string(IconProvider::kExportIcon) == "mui-share");
    REQUIRE(std::string(IconProvider::kAIIcon) == "mui-sparkles");
}
TEST_CASE("V27 P03: IconProvider category icon count", "[v27][p03]") {
    REQUIRE(IconProvider::v27_category_icon_count() == 16);
}
TEST_CASE("V27 P03: Migration tracker baseline", "[v27][p03]") {
    V27IconMigrationTracker tracker;
    tracker.populate_v27_baseline();
    REQUIRE(tracker.surface_count() == 17);
    REQUIRE(tracker.total_icons() > 0);
    REQUIRE_FALSE(tracker.is_migration_complete());
    REQUIRE(tracker.total_remaining_emoji() > 0);
}
TEST_CASE("V27 P03: Migration tracker update", "[v27][p03]") {
    V27IconMigrationTracker tracker;
    tracker.register_surface({"TestSurface", 5, 0, 3, 2});
    auto status = tracker.surface_status("TestSurface");
    REQUIRE(status != nullptr);
    REQUIRE(status->percent_complete() == 0);
    tracker.update_surface("TestSurface", 3);
    status = tracker.surface_status("TestSurface");
    REQUIRE(status->migrated == 3);
    REQUIRE(status->remaining_emoji == 0);
}
