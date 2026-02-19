// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/SettingsControlModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_controls() -> SettingsControlModel
{
    SettingsControlModel model;
    model.add_constraint(
        {"editor.fontSize", SettingValueType::kInteger, "14", 8, 72, 0.0, 0.0, {}});
    model.add_constraint(
        {"editor.lineHeight", SettingValueType::kDouble, "1.5", 0, 0, 1.0, 3.0, {}});
    model.add_constraint(
        {"editor.wordWrap", SettingValueType::kBoolean, "false", 0, 0, 0.0, 0.0, {}});
    model.add_constraint({"editor.cursorStyle",
                          SettingValueType::kChoice,
                          "line",
                          0,
                          0,
                          0.0,
                          0.0,
                          {"line", "block", "underline"}});
    model.add_constraint({"editor.color", SettingValueType::kColor, "#ffffff", 0, 0, 0.0, 0.0, {}});
    return model;
}

// ── Phase 12 Task 1: Per-type validation ────────────────────────────

TEST_CASE("Integer validation within range", "[settings-ctrl][validate]")
{
    auto model = make_test_controls();
    REQUIRE(model.validate("editor.fontSize", "14").is_valid);
    REQUIRE_FALSE(model.validate("editor.fontSize", "4").is_valid);
    REQUIRE_FALSE(model.validate("editor.fontSize", "abc").is_valid);
}

TEST_CASE("Double validation within range", "[settings-ctrl][validate]")
{
    auto model = make_test_controls();
    REQUIRE(model.validate("editor.lineHeight", "1.5").is_valid);
    REQUIRE_FALSE(model.validate("editor.lineHeight", "0.5").is_valid);
}

TEST_CASE("Boolean validation", "[settings-ctrl][validate]")
{
    auto model = make_test_controls();
    REQUIRE(model.validate("editor.wordWrap", "true").is_valid);
    REQUIRE_FALSE(model.validate("editor.wordWrap", "yes").is_valid);
}

TEST_CASE("Choice validation", "[settings-ctrl][validate]")
{
    auto model = make_test_controls();
    REQUIRE(model.validate("editor.cursorStyle", "block").is_valid);
    REQUIRE_FALSE(model.validate("editor.cursorStyle", "invalid").is_valid);
}

TEST_CASE("Color validation", "[settings-ctrl][validate]")
{
    auto model = make_test_controls();
    REQUIRE(model.validate("editor.color", "#ff0000").is_valid);
    REQUIRE_FALSE(model.validate("editor.color", "red").is_valid);
}

// ── Phase 12 Task 2: Clamping ───────────────────────────────────────

TEST_CASE("Int clamping", "[settings-ctrl][clamp]")
{
    auto model = make_test_controls();
    REQUIRE(model.clamp_int("editor.fontSize", 4) == 8);
    REQUIRE(model.clamp_int("editor.fontSize", 100) == 72);
    REQUIRE(model.clamp_int("editor.fontSize", 14) == 14);
}

TEST_CASE("Double clamping", "[settings-ctrl][clamp]")
{
    auto model = make_test_controls();
    REQUIRE(model.clamp_double("editor.lineHeight", 0.5) == 1.0);
    REQUIRE(model.clamp_double("editor.lineHeight", 5.0) == 3.0);
}

// ── Phase 12 Task 3: Staged changes ─────────────────────────────────

TEST_CASE("Stage and review changes", "[settings-ctrl][staged]")
{
    auto model = make_test_controls();
    model.stage_change("editor.fontSize", "14", "16");
    model.stage_change("editor.wordWrap", "false", "true");

    REQUIRE(model.pending_count() == 2);
    REQUIRE(model.has_pending("editor.fontSize"));
}

TEST_CASE("Staging same setting replaces previous", "[settings-ctrl][staged]")
{
    auto model = make_test_controls();
    model.stage_change("editor.fontSize", "14", "16");
    model.stage_change("editor.fontSize", "14", "18");

    REQUIRE(model.pending_count() == 1);
    REQUIRE(model.staged_changes()[0].new_value == "18");
}

// ── Phase 12 Task 4: Per-setting revert ─────────────────────────────

TEST_CASE("Revert single setting", "[settings-ctrl][revert]")
{
    auto model = make_test_controls();
    model.stage_change("editor.fontSize", "14", "16");
    model.stage_change("editor.wordWrap", "false", "true");

    model.revert("editor.fontSize");
    REQUIRE(model.pending_count() == 1);
    REQUIRE_FALSE(model.has_pending("editor.fontSize"));
}

TEST_CASE("Discard all clears everything", "[settings-ctrl][revert]")
{
    auto model = make_test_controls();
    model.stage_change("editor.fontSize", "14", "16");
    model.stage_change("editor.wordWrap", "false", "true");

    model.discard_all();
    REQUIRE(model.pending_count() == 0);
}

TEST_CASE("Default value retrieval", "[settings-ctrl][default]")
{
    auto model = make_test_controls();
    REQUIRE(model.default_value("editor.fontSize") == "14");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
