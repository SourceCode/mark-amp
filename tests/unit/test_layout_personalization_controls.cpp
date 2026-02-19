// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/LayoutPresetModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_presets() -> LayoutPresetModel
{
    LayoutPresetModel model;
    model.set_presets({
        {"writing-default", "Writing", true, false, true, true},
        {"coding-default", "Coding", true, true, true, true},
        {"minimal", "Minimal", false, false, false, false},
    });
    model.set_controls({
        {"sidebar", "Sidebar", true},
        {"toolbar", "Toolbar", true},
        {"minimap", "Minimap", false},
    });
    return model;
}

TEST_CASE("Preset lookup", "[layout][preset]")
{
    auto model = make_test_presets();
    REQUIRE(model.preset_by_id("writing-default") != nullptr);
    REQUIRE(model.preset_by_id("nonexistent") == nullptr);
}

TEST_CASE("Add and remove preset", "[layout][preset]")
{
    auto model = make_test_presets();
    model.add_preset({"custom-1", "Custom", true, true, true, true});
    REQUIRE(model.presets().size() == 4);
    model.remove_preset("custom-1");
    REQUIRE(model.presets().size() == 3);
}

TEST_CASE("Remove active preset clears active", "[layout][preset]")
{
    auto model = make_test_presets();
    model.set_active_preset("minimal");
    model.remove_preset("minimal");
    REQUIRE(model.active_preset_id().empty());
}

TEST_CASE("Control visibility toggle", "[layout][visibility]")
{
    auto model = make_test_presets();
    REQUIRE(model.is_control_visible("sidebar"));
    model.toggle_control("sidebar");
    REQUIRE_FALSE(model.is_control_visible("sidebar"));
}

TEST_CASE("Profile default preset mapping", "[layout][profile]")
{
    REQUIRE(LayoutPresetModel::default_preset_for(ProfileType::kWriting) == "writing-default");
    REQUIRE(LayoutPresetModel::default_preset_for(ProfileType::kCoding) == "coding-default");
    REQUIRE(LayoutPresetModel::default_preset_for(ProfileType::kCanvas) == "canvas-default");
}

TEST_CASE("Reset to defaults", "[layout][reset]")
{
    auto model = make_test_presets();
    model.toggle_control("sidebar");
    model.set_active_preset("minimal");
    model.reset_to_defaults();
    REQUIRE(model.is_control_visible("sidebar"));
    REQUIRE(model.active_preset_id().empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
