/// @file test_component_library.cpp
/// @brief Phase 41 — Unit tests for component library models.

#include "ui/Badge.h"
#include "ui/ChipTag.h"
#include "ui/ComponentVariants.h"
#include "ui/NumberStepper.h"
#include "ui/SegmentedControl.h"
#include "ui/ThemedCheckbox.h"
#include "ui/ThemedDropdown.h"
#include "ui/ThemedProgressBar.h"
#include "ui/ThemedRadio.h"
#include "ui/ThemedSlider.h"
#include "ui/ThemedTextInput.h"
#include "ui/ThemedToggleSwitch.h"
#include "ui/TreeViewItem.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp;
using namespace markamp::ui;

// ============================================================================
// VariantTokenResolver
// ============================================================================

TEST_CASE("VariantTokenResolver: primary normal", "[component_library]")
{
    auto tokens = VariantTokenResolver::resolve(ControlVariant::kPrimary, 0);
    REQUIRE(tokens.background == core::ThemeColorToken::AccentPrimary);
    REQUIRE(tokens.foreground == core::ThemeColorToken::AccentSecondary);
    REQUIRE(tokens.opacity == 1.0F);
}

TEST_CASE("VariantTokenResolver: disabled opacity", "[component_library]")
{
    auto flags = static_cast<ControlStateFlags>(ControlStateFlag::kDisabled);
    auto tokens = VariantTokenResolver::resolve(ControlVariant::kPrimary, flags);
    REQUIRE(tokens.opacity == 0.5F);
    REQUIRE(tokens.foreground == core::ThemeColorToken::ControlFgDisabled);
}

TEST_CASE("VariantTokenResolver: ghost hover", "[component_library]")
{
    auto flags = static_cast<ControlStateFlags>(ControlStateFlag::kHover);
    auto bg = VariantTokenResolver::background_for(ControlVariant::kGhost, flags);
    REQUIRE(bg == core::ThemeColorToken::HoverBg);
}

TEST_CASE("VariantTokenResolver: danger normal", "[component_library]")
{
    auto tokens = VariantTokenResolver::resolve(ControlVariant::kDanger, 0);
    REQUIRE(tokens.foreground == core::ThemeColorToken::AccentSecondary);
}

TEST_CASE("VariantTokenResolver: focused border", "[component_library]")
{
    auto flags = static_cast<ControlStateFlags>(ControlStateFlag::kFocused);
    auto border = VariantTokenResolver::border_for(ControlVariant::kSecondary, flags);
    REQUIRE(border == core::ThemeColorToken::FocusRingColor);
}

// ============================================================================
// TextInputModel
// ============================================================================

TEST_CASE("TextInputModel: value and placeholder", "[component_library]")
{
    TextInputModel model;
    model.set_placeholder("Enter name...");
    REQUIRE(model.placeholder() == "Enter name...");
    REQUIRE(model.is_empty());

    model.set_value("Hello");
    REQUIRE(model.value() == "Hello");
    REQUIRE_FALSE(model.is_empty());
    REQUIRE(model.char_count() == 5);
}

TEST_CASE("TextInputModel: max length truncation", "[component_library]")
{
    TextInputModel model;
    model.set_max_length(5);
    model.set_value("Hello World");
    REQUIRE(model.value() == "Hello");
    REQUIRE(model.char_count() == 5);
}

TEST_CASE("TextInputModel: validation state", "[component_library]")
{
    TextInputModel model;
    REQUIRE(model.validation_state() == TextInputValidation::kNone);

    model.set_validation(TextInputValidation::kError, "Invalid email");
    REQUIRE(model.validation_state() == TextInputValidation::kError);
    REQUIRE(model.validation_message() == "Invalid email");
}

// ============================================================================
// DropdownModel
// ============================================================================

TEST_CASE("DropdownModel: items and selection", "[component_library]")
{
    DropdownModel model;
    model.set_items({
        {.id = "a", .label = "Alpha"},
        {.id = "b", .label = "Beta"},
        {.id = "c", .label = "Charlie"},
    });

    REQUIRE(model.item_count() == 3);
    REQUIRE(model.selected_index() == -1);

    model.set_selected_index(1);
    REQUIRE(model.selected_index() == 1);
    REQUIRE(model.selected_item()->label == "Beta");
}

TEST_CASE("DropdownModel: search filtering", "[component_library]")
{
    DropdownModel model;
    model.set_items({
        {.id = "a", .label = "Apple"},
        {.id = "b", .label = "Banana"},
        {.id = "c", .label = "Apricot"},
    });

    model.set_search_query("ap");
    auto filtered = model.filtered_items();
    REQUIRE(filtered.size() == 2);
    REQUIRE(filtered[0].label == "Apple");
    REQUIRE(filtered[1].label == "Apricot");
}

TEST_CASE("DropdownModel: groups", "[component_library]")
{
    DropdownModel model;
    model.set_items({
        {.id = "a", .label = "Alpha", .group = "Greek"},
        {.id = "b", .label = "Beta", .group = "Greek"},
        {.id = "1", .label = "One", .group = "Numbers"},
    });

    auto groups = model.groups();
    REQUIRE(groups.size() == 2);
    REQUIRE(groups[0] == "Greek");
    REQUIRE(groups[1] == "Numbers");
}

TEST_CASE("DropdownModel: highlight navigation", "[component_library]")
{
    DropdownModel model;
    model.set_items({
        {.id = "a", .label = "A"},
        {.id = "b", .label = "B", .is_separator = true},
        {.id = "c", .label = "C"},
    });

    model.set_open(true);
    REQUIRE(model.highlight_index() == 0);
    model.move_highlight(1);
    // Should skip separator
    REQUIRE(model.highlight_index() == 2);
}

TEST_CASE("DropdownModel: open/close state", "[component_library]")
{
    DropdownModel model;
    model.set_items({{.id = "a", .label = "A"}});
    REQUIRE_FALSE(model.is_open());
    model.set_open(true);
    REQUIRE(model.is_open());

    model.set_search_query("test");
    model.set_open(true); // re-opening clears search
    REQUIRE(model.search_query().empty());
}

// ============================================================================
// CheckboxModel
// ============================================================================

TEST_CASE("CheckboxModel: toggle binary", "[component_library]")
{
    CheckboxModel model;
    REQUIRE(model.state() == CheckState::kUnchecked);
    model.toggle(false);
    REQUIRE(model.state() == CheckState::kChecked);
    model.toggle(false);
    REQUIRE(model.state() == CheckState::kUnchecked);
}

TEST_CASE("CheckboxModel: toggle tri-state", "[component_library]")
{
    CheckboxModel model;
    model.toggle(true);
    REQUIRE(model.state() == CheckState::kChecked);
    model.toggle(true);
    REQUIRE(model.state() == CheckState::kIndeterminate);
    REQUIRE(model.is_indeterminate());
    model.toggle(true);
    REQUIRE(model.state() == CheckState::kUnchecked);
}

// ============================================================================
// RadioGroupModel
// ============================================================================

TEST_CASE("RadioGroupModel: selection", "[component_library]")
{
    RadioGroupModel model;
    model.set_items({
        {.id = "a", .label = "Alpha"},
        {.id = "b", .label = "Beta"},
        {.id = "c", .label = "Charlie"},
    });

    REQUIRE(model.selected_index() == 0);
    model.set_selected_index(2);
    REQUIRE(model.selected_index() == 2);
    REQUIRE(model.selected_item()->label == "Charlie");
}

TEST_CASE("RadioGroupModel: next/previous", "[component_library]")
{
    RadioGroupModel model;
    model.set_items({
        {.id = "a", .label = "A"},
        {.id = "b", .label = "B", .is_disabled = true},
        {.id = "c", .label = "C"},
    });

    model.select_next();
    // Should skip disabled item
    REQUIRE(model.selected_index() == 2);
    model.select_previous();
    REQUIRE(model.selected_index() == 0);
}

// ============================================================================
// ToggleSwitchModel
// ============================================================================

TEST_CASE("ToggleSwitchModel: toggle", "[component_library]")
{
    ToggleSwitchModel model;
    REQUIRE_FALSE(model.is_on());
    model.toggle();
    REQUIRE(model.is_on());
    model.toggle();
    REQUIRE_FALSE(model.is_on());
}

TEST_CASE("ToggleSwitchModel: set_on", "[component_library]")
{
    ToggleSwitchModel model;
    model.set_on(true);
    REQUIRE(model.is_on());
    model.set_on(false);
    REQUIRE_FALSE(model.is_on());
}

// ============================================================================
// SliderModel
// ============================================================================

TEST_CASE("SliderModel: value and range", "[component_library]")
{
    SliderModel model;
    model.set_range(0, 100);
    model.set_value(50);
    REQUIRE(model.value() == 50.0);
    REQUIRE(model.normalized() == 0.5);
}

TEST_CASE("SliderModel: clamp", "[component_library]")
{
    SliderModel model;
    model.set_range(10, 20);
    model.set_value(30);
    REQUIRE(model.value() == 20.0);
    model.set_value(5);
    REQUIRE(model.value() == 10.0);
}

TEST_CASE("SliderModel: increment/decrement", "[component_library]")
{
    SliderModel model;
    model.set_range(0, 10);
    model.set_step(2);
    model.set_value(4);
    model.increment();
    REQUIRE(model.value() == 6.0);
    model.decrement();
    REQUIRE(model.value() == 4.0);
}

TEST_CASE("SliderModel: snap_to_step", "[component_library]")
{
    SliderModel model;
    model.set_range(0, 100);
    model.set_step(10);
    model.set_value(27);
    model.snap_to_step();
    REQUIRE(model.value() == 30.0);
}

// ============================================================================
// ProgressBarModel
// ============================================================================

TEST_CASE("ProgressBarModel: value and percentage", "[component_library]")
{
    ProgressBarModel model;
    model.set_max(200);
    model.set_value(100);
    REQUIRE(model.percentage() == 50);
    REQUIRE(model.normalized() == 0.5);
    REQUIRE_FALSE(model.is_complete());
}

TEST_CASE("ProgressBarModel: complete", "[component_library]")
{
    ProgressBarModel model;
    model.set_max(100);
    model.set_value(100);
    REQUIRE(model.is_complete());
    REQUIRE(model.percentage() == 100);
}

// ============================================================================
// BadgeModel
// ============================================================================

TEST_CASE("BadgeModel: count and display", "[component_library]")
{
    BadgeModel model;
    model.set_count(5);
    REQUIRE(model.display_text() == "5");
    REQUIRE(model.is_visible());
}

TEST_CASE("BadgeModel: max count overflow", "[component_library]")
{
    BadgeModel model;
    model.set_max_count(99);
    model.set_count(150);
    REQUIRE(model.display_text() == "99+");
}

TEST_CASE("BadgeModel: dot only", "[component_library]")
{
    BadgeModel model;
    model.set_dot_only(true);
    model.set_count(5);
    REQUIRE(model.display_text().empty());
    REQUIRE(model.is_visible());
}

TEST_CASE("BadgeModel: zero is invisible", "[component_library]")
{
    BadgeModel model;
    model.set_count(0);
    REQUIRE_FALSE(model.is_visible());
}

// ============================================================================
// ChipTagModel
// ============================================================================

TEST_CASE("ChipTagModel: label and state", "[component_library]")
{
    ChipTagModel model;
    model.set_label("TypeScript");
    model.set_removable(true);
    model.set_selected(true);
    REQUIRE(model.label() == "TypeScript");
    REQUIRE(model.is_removable());
    REQUIRE(model.is_selected());
}

// ============================================================================
// SegmentedControlModel
// ============================================================================

TEST_CASE("SegmentedControlModel: selection", "[component_library]")
{
    SegmentedControlModel model;
    model.set_items({
        {.id = "code", .label = "Code"},
        {.id = "preview", .label = "Preview"},
        {.id = "split", .label = "Split"},
    });

    REQUIRE(model.selected_index() == 0);
    model.set_selected_index(2);
    REQUIRE(model.selected_item()->label == "Split");
    REQUIRE(model.item_count() == 3);
}

// ============================================================================
// NumberStepperModel
// ============================================================================

TEST_CASE("NumberStepperModel: increment/decrement", "[component_library]")
{
    NumberStepperModel model;
    model.set_range(0, 10);
    model.set_step(1);
    model.set_value(5);
    model.increment();
    REQUIRE(model.value() == 6.0);
    model.decrement();
    REQUIRE(model.value() == 5.0);
}

TEST_CASE("NumberStepperModel: clamp at boundaries", "[component_library]")
{
    NumberStepperModel model;
    model.set_range(0, 5);
    model.set_step(1);
    model.set_value(5);
    REQUIRE_FALSE(model.can_increment());
    model.increment();
    REQUIRE(model.value() == 5.0);
}

TEST_CASE("NumberStepperModel: display_text precision", "[component_library]")
{
    NumberStepperModel model;
    model.set_value(3.14159);
    model.set_precision(2);
    REQUIRE(model.display_text() == "3.14");
}

// ============================================================================
// TreeViewItemModel
// ============================================================================

TEST_CASE("TreeViewItemModel: expand/collapse", "[component_library]")
{
    TreeViewItemModel model;
    model.set_expandable(true);
    REQUIRE_FALSE(model.is_expanded());
    model.toggle_expanded();
    REQUIRE(model.is_expanded());
    model.toggle_expanded();
    REQUIRE_FALSE(model.is_expanded());
}

TEST_CASE("TreeViewItemModel: non-expandable cannot toggle", "[component_library]")
{
    TreeViewItemModel model;
    model.set_expandable(false);
    model.toggle_expanded();
    REQUIRE_FALSE(model.is_expanded());
}

TEST_CASE("TreeViewItemModel: indent calculation", "[component_library]")
{
    TreeViewItemModel model;
    model.set_depth(3);
    REQUIRE(model.indent_pixels() == 3 * TreeViewItemModel::kIndentPerLevel);
}

TEST_CASE("TreeViewItemModel: hover actions", "[component_library]")
{
    TreeViewItemModel model;
    model.set_hover_actions({
        {.id = "delete", .icon_name = "trash", .tooltip = "Delete"},
        {.id = "rename", .icon_name = "edit", .tooltip = "Rename"},
    });
    REQUIRE(model.hover_actions().size() == 2);
    REQUIRE(model.hover_actions()[0].id == "delete");
}
