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
    REQUIRE(icon_component_family_count() >= 8);
}
TEST_CASE("V27 P03: V27 component families compile", "[v27][p03]") {
    [[maybe_unused]] auto n = IconComponentFamily::kActivityBar;
    [[maybe_unused]] auto s = IconComponentFamily::kSettings;
    [[maybe_unused]] auto e = IconComponentFamily::kEmptyState;
    [[maybe_unused]] auto b = IconComponentFamily::kBreadcrumb;
    [[maybe_unused]] auto p = IconComponentFamily::kCommandPalette;
    REQUIRE(true);
}
TEST_CASE("V27 P03: IconProvider V27 MUI IDs", "[v27][p03]") {
    REQUIRE(std::string(IconProvider::kMuiFileIcon) == "mui-file-text");
    REQUIRE(std::string(IconProvider::kMuiEditIcon) == "mui-pencil");
    REQUIRE(std::string(IconProvider::kMuiViewIcon) == "mui-eye");
    REQUIRE(std::string(IconProvider::kMuiNavigationIcon) == "mui-compass");
    REQUIRE(std::string(IconProvider::kMuiTerminalIcon) == "mui-terminal");
    REQUIRE(std::string(IconProvider::kMuiExtensionIcon) == "mui-puzzle-piece");
    REQUIRE(std::string(IconProvider::kMuiEditorIcon) == "mui-edit-3");
    REQUIRE(std::string(IconProvider::kMuiSearchIcon) == "mui-search");
    REQUIRE(std::string(IconProvider::kMuiDebugIcon) == "mui-bug");
    REQUIRE(std::string(IconProvider::kMuiSettingsIcon) == "mui-settings");
    REQUIRE(std::string(IconProvider::kMuiDefaultIcon) == "mui-circle-dot");
}
TEST_CASE("V27 P03: IconProvider emoji constants are renderable", "[v27][p03]") {
    // Ensure emoji constants are non-empty UTF-8 characters, not MUI IDs
    REQUIRE_FALSE(std::string(IconProvider::kFileIcon).empty());
    REQUIRE(std::string(IconProvider::kFileIcon).find("mui-") == std::string::npos);
    REQUIRE_FALSE(std::string(IconProvider::kEditIcon).empty());
    REQUIRE_FALSE(std::string(IconProvider::kDefaultIcon).empty());
}
TEST_CASE("V27 P03: IconProvider V27 additional icons", "[v27][p03]") {
    REQUIRE(std::string(IconProvider::kMuiCanvasIcon) == "mui-layout");
    REQUIRE(std::string(IconProvider::kMuiNotebookIcon) == "mui-book-open");
    REQUIRE(std::string(IconProvider::kMuiThemeIcon) == "mui-palette");
    REQUIRE(std::string(IconProvider::kMuiExportIcon) == "mui-share");
    REQUIRE(std::string(IconProvider::kMuiAIIcon) == "mui-sparkles");
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
