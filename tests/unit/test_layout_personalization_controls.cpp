/**
 * @file test_layout_personalization_controls.cpp
 * @brief Phase 37: Tests for LayoutPresetController and ControlVisibilityPrefs.
 */

#include "ui/ControlVisibilityPrefs.h"
#include "ui/LayoutPresetController.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// LayoutPreset
// ═══════════════════════════════════════════════════════

TEST_CASE("LayoutPreset - profile name", "[personalization][preset]")
{
    LayoutPreset preset;
    preset.profile = ProfileType::kCoding;
    CHECK(preset.profile_name() == "coding");

    preset.profile = ProfileType::kWriting;
    CHECK(preset.profile_name() == "writing");

    preset.profile = ProfileType::kCanvas;
    CHECK(preset.profile_name() == "canvas");

    preset.profile = ProfileType::kCustom;
    CHECK(preset.profile_name() == "custom");
}

// ═══════════════════════════════════════════════════════
// LayoutPresetController
// ═══════════════════════════════════════════════════════

TEST_CASE("LayoutPresetController - empty state", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    CHECK(ctrl.preset_count() == 0);
    CHECK(ctrl.current_profile() == ProfileType::kCoding);
}

TEST_CASE("LayoutPresetController - create and find", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    LayoutPreset preset;
    preset.preset_id = "my_preset";
    preset.name = "My Layout";
    preset.profile = ProfileType::kCoding;

    ctrl.create_preset(preset);
    CHECK(ctrl.preset_count() == 1);

    const auto* found = ctrl.find_preset("my_preset");
    REQUIRE(found != nullptr);
    CHECK(found->name == "My Layout");
}

TEST_CASE("LayoutPresetController - rename", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    ctrl.create_preset({"p1", "Original", ProfileType::kCoding, false, {}, {}});

    ctrl.rename_preset("p1", "Renamed");
    const auto* found = ctrl.find_preset("p1");
    REQUIRE(found != nullptr);
    CHECK(found->name == "Renamed");
}

TEST_CASE("LayoutPresetController - delete", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    ctrl.create_preset({"p1", "Preset 1", ProfileType::kCoding, false, {}, {}});
    ctrl.delete_preset("p1");
    CHECK(ctrl.preset_count() == 0);
}

TEST_CASE("LayoutPresetController - profile filtering", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    ctrl.create_preset({"c1", "Coding 1", ProfileType::kCoding, false, {}, {}});
    ctrl.create_preset({"w1", "Writing 1", ProfileType::kWriting, false, {}, {}});
    ctrl.create_preset({"c2", "Coding 2", ProfileType::kCoding, false, {}, {}});

    auto coding = ctrl.presets_for_profile(ProfileType::kCoding);
    CHECK(coding.size() == 2);

    auto writing = ctrl.presets_for_profile(ProfileType::kWriting);
    CHECK(writing.size() == 1);
}

TEST_CASE("LayoutPresetController - apply preset", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    ctrl.create_preset({"p1", "Preset 1", ProfileType::kCoding, false, {}, {}});

    ctrl.apply_preset("p1");
    CHECK(ctrl.active_preset_id() == "p1");
}

TEST_CASE("LayoutPresetController - standard presets", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    ctrl.register_standard_presets();

    CHECK(ctrl.preset_count() == 3);

    const auto* coding = ctrl.default_for_profile(ProfileType::kCoding);
    REQUIRE(coding != nullptr);
    CHECK(coding->is_default);
    CHECK_FALSE(coding->visible_panels.empty());

    const auto* writing = ctrl.default_for_profile(ProfileType::kWriting);
    REQUIRE(writing != nullptr);

    const auto* canvas = ctrl.default_for_profile(ProfileType::kCanvas);
    REQUIRE(canvas != nullptr);
}

TEST_CASE("LayoutPresetController - reset to default", "[personalization][controller]")
{
    LayoutPresetController ctrl;
    ctrl.register_standard_presets();

    ctrl.set_profile(ProfileType::kCoding);
    ctrl.apply_preset("coding_default");
    ctrl.apply_preset("writing_default"); // Change away

    ctrl.reset_to_default();
    CHECK(ctrl.active_preset_id() == "coding_default");
}

// ═══════════════════════════════════════════════════════
// ControlVisibilityPrefs
// ═══════════════════════════════════════════════════════

TEST_CASE("ControlVisibilityPrefs - empty state", "[personalization][visibility]")
{
    ControlVisibilityPrefs prefs;
    CHECK(prefs.cluster_count() == 0);
    CHECK_FALSE(prefs.has_user_modifications());
}

TEST_CASE("ControlVisibilityPrefs - register clusters", "[personalization][visibility]")
{
    ControlVisibilityPrefs prefs;
    prefs.register_cluster("toolbar_main", "Main Toolbar", true);
    prefs.register_cluster("toolbar_search", "Search Bar", false);
    CHECK(prefs.cluster_count() == 2);
    CHECK(prefs.is_visible("toolbar_main"));
    CHECK_FALSE(prefs.is_visible("toolbar_search"));
}

TEST_CASE("ControlVisibilityPrefs - toggle", "[personalization][visibility]")
{
    ControlVisibilityPrefs prefs;
    prefs.register_cluster("toolbar_main", "Main Toolbar", true);

    prefs.toggle("toolbar_main");
    CHECK_FALSE(prefs.is_visible("toolbar_main"));
    CHECK(prefs.has_user_modifications());

    prefs.toggle("toolbar_main");
    CHECK(prefs.is_visible("toolbar_main"));
}

TEST_CASE("ControlVisibilityPrefs - visible and hidden lists", "[personalization][visibility]")
{
    ControlVisibilityPrefs prefs;
    prefs.register_cluster("a", "A", true);
    prefs.register_cluster("b", "B", false);
    prefs.register_cluster("c", "C", true);

    auto visible = prefs.visible_clusters();
    CHECK(visible.size() == 2);

    auto hidden = prefs.hidden_clusters();
    CHECK(hidden.size() == 1);
    CHECK(hidden[0] == "b");
}

TEST_CASE("ControlVisibilityPrefs - reset to defaults", "[personalization][visibility]")
{
    ControlVisibilityPrefs prefs;
    prefs.register_cluster("toolbar", "Toolbar", true);

    prefs.set_visible("toolbar", false);
    CHECK_FALSE(prefs.is_visible("toolbar"));
    CHECK(prefs.user_modified_count() == 1);

    prefs.reset_to_defaults();
    CHECK(prefs.is_visible("toolbar"));
    CHECK_FALSE(prefs.has_user_modifications());
}

TEST_CASE("ControlVisibilityPrefs - reset single cluster", "[personalization][visibility]")
{
    ControlVisibilityPrefs prefs;
    prefs.register_cluster("a", "A", true);
    prefs.register_cluster("b", "B", true);

    prefs.set_visible("a", false);
    prefs.set_visible("b", false);
    CHECK(prefs.user_modified_count() == 2);

    prefs.reset_cluster("a");
    CHECK(prefs.is_visible("a"));
    CHECK_FALSE(prefs.is_visible("b"));
    CHECK(prefs.user_modified_count() == 1);
}
