#include "ui/layout/WorkbenchLayoutModel.h"
#include "ui/layout/WorkbenchZone.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui::layout;

TEST_CASE("WorkbenchLayoutModel enforces zone constraints properly", "[ui][layout][phase06]")
{
    WorkbenchLayoutModel model;

    // Verify initial layout resolves to defaults
    const auto& editor_state = model.get_state(WorkbenchZoneId::kEditorArea);
    const auto& primary_state = model.get_state(WorkbenchZoneId::kPrimarySidebar);

    REQUIRE(editor_state.visible == true);
    REQUIRE(primary_state.visible == true);
    REQUIRE(primary_state.current_width >= 170); // Min bound

    // Verify resize clamping
    int target_huge_width = 99999;
    model.resize_zone(WorkbenchZoneId::kPrimarySidebar, target_huge_width, 100);
    const auto& clamped_state = model.get_state(WorkbenchZoneId::kPrimarySidebar);
    REQUIRE(clamped_state.current_width ==
            model.get_constraints(WorkbenchZoneId::kPrimarySidebar).max_width);
    REQUIRE(clamped_state.current_width < target_huge_width);

    // Verify collapsing a zone
    model.set_zone_visible(WorkbenchZoneId::kPrimarySidebar, false);
    const auto& hidden_state = model.get_state(WorkbenchZoneId::kPrimarySidebar);
    REQUIRE(hidden_state.visible == false);
    REQUIRE(hidden_state.current_width == 0);
    REQUIRE(hidden_state.restored_width > 0);
}

TEST_CASE("WorkbenchLayoutModel override bounds apply purely bounds bypass",
          "[ui][layout][phase06]")
{
    WorkbenchLayoutModel model;
    model.update_window_size(1024, 768);

    model.set_zone_size_override(WorkbenchZoneId::kPrimarySidebar, 50, 100);
    const auto& overriden_state = model.get_state(WorkbenchZoneId::kPrimarySidebar);

    // Override should allow values outside constraints temporarily (for animation)
    REQUIRE(overriden_state.current_width == 50);

    // But standard resize must re-clamp
    model.resize_zone(WorkbenchZoneId::kPrimarySidebar, 50, 100);
    const auto& re_clamped = model.get_state(WorkbenchZoneId::kPrimarySidebar);
    REQUIRE(re_clamped.current_width ==
            model.get_constraints(WorkbenchZoneId::kPrimarySidebar).min_width);
}
