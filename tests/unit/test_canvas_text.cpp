/**
 * @file test_canvas_text.cpp
 * @brief Phase 45: Tests for CanvasTextEditor and TextStylePreset.
 */

#include "canvas/CanvasTextEditor.h"
#include "canvas/TextStylePreset.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

// ═══════════════════════════════════════════════════════
// CanvasTextEditor
// ═══════════════════════════════════════════════════════

TEST_CASE("CanvasTextEditor - initial state", "[canvas][text]")
{
    CanvasTextEditor editor;
    CHECK(editor.state() == TextEditState::kIdle);
    CHECK(CanvasTextEditor::state_name(TextEditState::kIdle) == "idle");
    CHECK(editor.text_length() == 0);
}

TEST_CASE("CanvasTextEditor - edit lifecycle", "[canvas][text]")
{
    CanvasTextEditor editor;
    editor.set_text("hello");
    editor.begin_edit();
    CHECK(editor.state() == TextEditState::kEditing);

    editor.set_text("hello world");
    editor.commit();
    CHECK(editor.state() == TextEditState::kCommitted);
    CHECK(editor.text() == "hello world");
}

TEST_CASE("CanvasTextEditor - cancel reverts", "[canvas][text]")
{
    CanvasTextEditor editor;
    editor.set_text("original");
    editor.begin_edit();
    editor.set_text("modified");
    editor.cancel();

    CHECK(editor.state() == TextEditState::kCancelled);
    CHECK(editor.text() == "original");
}

TEST_CASE("CanvasTextEditor - typography", "[canvas][text]")
{
    CanvasTextEditor editor;
    TypographyProps props;
    props.font_family = "Inter";
    props.font_size = 18.0;
    props.font_weight = 600;
    props.alignment = TextAlignment::kCenter;
    editor.set_typography(props);

    CHECK(editor.typography().font_family == "Inter");
    CHECK(editor.typography().font_size == 18.0);
    CHECK(editor.typography().font_weight == 600);
}

TEST_CASE("CanvasTextEditor - size mode", "[canvas][text]")
{
    CanvasTextEditor editor;
    CHECK(editor.size_mode() == TextSizeMode::kAutoSize);

    editor.set_size_mode(TextSizeMode::kFixedBox);
    CHECK(editor.size_mode() == TextSizeMode::kFixedBox);

    editor.set_box_size(300.0, 100.0);
    CHECK(editor.box_width() == 300.0);
    CHECK(editor.box_height() == 100.0);
}

TEST_CASE("CanvasTextEditor - estimated height", "[canvas][text]")
{
    CanvasTextEditor editor;
    editor.set_text("A long text that should need some wrapping in a normal-sized box.");
    editor.set_box_size(200.0, 100.0);

    double height = editor.estimated_height();
    CHECK(height > 0.0);
}

// ═══════════════════════════════════════════════════════
// TextPreset
// ═══════════════════════════════════════════════════════

TEST_CASE("TextPreset - type name", "[canvas][text]")
{
    TextPreset preset;
    preset.type = TextPresetType::kHeading;
    CHECK(preset.type_name() == "heading");

    preset.type = TextPresetType::kCode;
    CHECK(preset.type_name() == "code");
}

// ═══════════════════════════════════════════════════════
// TextStylePreset
// ═══════════════════════════════════════════════════════

TEST_CASE("TextStylePreset - standard presets", "[canvas][text]")
{
    TextStylePreset presets;
    presets.register_standard_presets();

    CHECK(presets.preset_count() == 4);
    CHECK(presets.find_preset("heading") != nullptr);
    CHECK(presets.find_preset("body") != nullptr);
    CHECK(presets.find_preset("callout") != nullptr);
    CHECK(presets.find_preset("code") != nullptr);
}

TEST_CASE("TextStylePreset - active preset", "[canvas][text]")
{
    TextStylePreset presets;
    presets.register_standard_presets();

    CHECK(presets.set_active("heading"));
    CHECK(presets.active().font_size == 24.0);
    CHECK(presets.active().font_weight == 700);
}

TEST_CASE("TextStylePreset - preset not found", "[canvas][text]")
{
    TextStylePreset presets;
    CHECK_FALSE(presets.set_active("nonexistent"));
}
