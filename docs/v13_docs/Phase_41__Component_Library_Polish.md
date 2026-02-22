# Phase 41 -- Component Library Polish

## Objective

Redesign every basic UI control to achieve visual and interaction parity with VSCode and WebStorm. Replace ad-hoc wxWidgets native controls with custom-drawn, theme-aware components that use the existing ControlStateTracker, ThemeAwareWindow, and ThemeEngine infrastructure. Every control variant (primary, secondary, ghost, danger) must render pixel-perfect across all ControlStateFlag combinations (normal, hover, pressed, focused, disabled, selected) and must be testable without a running wxWidgets event loop.

## Prerequisites

- Phase 40 complete (all prior UI model layers finalized)
- `/Users/ryanrentfro/code/markamp/src/ui/ControlState.h` -- ControlStateTracker and IndexedControlState
- `/Users/ryanrentfro/code/markamp/src/ui/ThemeAwareWindow.h` -- ThemeAwareWindow base class
- `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.h` -- ThemeEngine with cached wxBrush/wxPen/wxFont
- `/Users/ryanrentfro/code/markamp/src/ui/BevelPanel.h` -- BevelPanel for inset/outset effects

## VSCode / WebStorm Reference Behavior

- Buttons: 4px border-radius, 2px focus ring with accent color offset, 120ms hover transition
- Text inputs: 1px border that brightens on focus, icon left/right slots, inline validation message
- Dropdowns: Overlay popup with search field, keyboard navigation, grouped sections
- Checkboxes: 14x14px box with animated checkmark SVG, indeterminate dash
- Toggle switches: 26x14px pill shape with sliding dot, 150ms ease-out transition
- Sliders: Thin track (2px), circular thumb (12px), value tooltip on drag
- Progress bars: 4px height, indeterminate shimmer animation, circular variant with stroke-dasharray
- Badges: Pill-shaped with min-width 18px, numeric and dot variants
- Tooltips: 200ms show delay, 8px offset, max-width 300px, optional rich content
- Tree view items: 22px row height, expand chevron, icon, label, inline action buttons on hover

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedTextInput.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedTextInput.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedCheckbox.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedCheckbox.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedRadio.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedRadio.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedToggleSwitch.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedToggleSwitch.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedSlider.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedSlider.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedProgressBar.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedProgressBar.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/Badge.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/Badge.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/SegmentedControl.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/SegmentedControl.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedColorPicker.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedColorPicker.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/NumberStepper.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/NumberStepper.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/TreeViewItem.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/TreeViewItem.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ComponentLibrary.h` | Create |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_component_library.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Define Component Variant Enum and Token Map

**Description:** Create a shared enum for button/control variants (Primary, Secondary, Ghost, Danger, Link) and a mapping table that resolves each variant + ControlStateFlags combination to ThemeEngine color tokens. This eliminates per-component color logic and ensures all controls derive their appearance from one canonical source.

**Implementation Details:** Define `ControlVariant` enum in a new `ComponentLibrary.h` header. Create a `VariantTokenResolver` class with a static method `resolve(ControlVariant, ControlStateFlags) -> VariantTokens` where `VariantTokens` is a struct containing `bg_token`, `fg_token`, `border_token`, and `focus_ring_token` (all `ThemeColorToken` values). Populate the mapping using the existing `ThemeColorToken` enum values from `ThemeEngine.h`. The resolver must handle all 6 ControlStateFlags (including combined states like hover+focused).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ComponentLibrary.h` (create)

**Acceptance Criteria:**
- VariantTokenResolver compiles and resolves all variant+state combinations without fallthrough
- Unit test verifies that Primary+kHover returns accent_primary_hover tokens
- Unit test verifies that Danger+kDisabled returns the muted danger tokens
- No raw color values anywhere in the resolver -- all values are ThemeColorToken enums

**Dependencies:** None (foundational task)

---

### Task 02 -- Implement ThemedButton with Four Variants

**Description:** Create a custom-drawn button control extending ThemeAwareWindow that supports Primary, Secondary, Ghost, and Danger variants. Must render with 4px rounded corners, use ControlStateTracker for state management, and draw focus rings using the ThemeAwareWindow::DrawFocusRing method.

**Implementation Details:** ThemedButton inherits ThemeAwareWindow. Constructor takes `ControlVariant`, `wxString label`, and optional `wxBitmapBundle icon`. OnPaint draws the rounded rectangle background using `wxGraphicsContext::CreateRoundedRectangle()`, text centered with `wxDC::DrawLabel()`, and icon to the left of text (8px gap). Mouse events delegate to the internal ControlStateTracker. Emit `wxCommandEvent(wxEVT_BUTTON)` on mouse-up-inside. Support `SetEnabled(false)` which delegates to `tracker_.set_disabled()`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp` (create)

**Acceptance Criteria:**
- Button renders correctly in all 4 variants across all 6 ControlStateFlags
- Focus ring appears when button is focused via keyboard Tab
- Click fires wxEVT_BUTTON event
- Disabled state grays out text and ignores mouse events
- Hover transition uses wxTimer for 120ms interpolation

**Dependencies:** Task 01

---

### Task 03 -- Implement Icon Button with Hover and Press States

**Description:** Create a compact icon-only button (24x24px default, configurable) for toolbars and inline actions. Uses the same ControlStateTracker but renders only an icon with a subtle background on hover and a depressed background on press.

**Implementation Details:** IconButton inherits ThemeAwareWindow. Constructor takes `wxBitmapBundle icon` and optional `ControlVariant`. Default size is 24x24. OnPaint renders: transparent background normally, `bg_hover` on hover (4px rounded rect), `bg_pressed` on press. Icon is centered in the control. Supports tooltip text via `SetToolTip()`. Includes a toggle mode (`set_toggle(bool)`) where selected state renders with accent background.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h` (extend with IconButton class)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp` (extend)

**Acceptance Criteria:**
- Icon renders centered and crisp at 16x16 within 24x24 bounds
- Hover shows subtle background with 4px corner radius
- Toggle mode shows persistent accent background when selected
- Tooltip appears after 500ms hover delay

**Dependencies:** Task 01

---

### Task 04 -- Implement Link Button

**Description:** Create a text-only button that renders as a clickable link with underline on hover, matching VSCode's "Learn More" style links.

**Implementation Details:** LinkButton inherits ThemeAwareWindow. Renders text using accent_primary color. On hover, draws underline 1px below text baseline. Cursor changes to kHand via `SetControlCursor()`. No background fill. Font uses the standard UI font at the same size. Fires `wxEVT_BUTTON` on click.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.h` (extend with LinkButton class)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedButton.cpp` (extend)

**Acceptance Criteria:**
- Link renders with accent color and no background
- Underline appears on hover and disappears on leave
- Cursor changes to hand on hover
- Accessible name reads as "link" role

**Dependencies:** Task 01

---

### Task 05 -- Implement ThemedTextInput with Icon Slots and Validation

**Description:** Create a text input control that renders a single-line text field with optional left icon, right icon, clear button, and inline validation state (error/warning/success border colors and message text below).

**Implementation Details:** ThemedTextInput inherits ThemeAwareWindow and wraps an internal wxTextCtrl (hidden borders). Layout: [icon_left 20px | text area | clear_btn 20px | icon_right 20px]. Border is drawn manually: 1px `border_default` normally, `accent_primary` on focus, `danger` on error, `warning` on warning. Below the input, a 12px-font validation message appears when `set_validation_state()` is called. Clear button (x icon) appears when text is non-empty and `set_clearable(true)` is set.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedTextInput.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedTextInput.cpp` (create)

**Acceptance Criteria:**
- Input renders with 1px border that transitions to accent on focus
- Left and right icon slots render at 16x16 within 20px gutters
- Clear button appears when text is non-empty and clearable is true
- Validation error shows red border and error message below
- Tab key moves focus to next control

**Dependencies:** Task 01

---

### Task 06 -- Implement Searchable Dropdown with Grouped Items

**Description:** Create a dropdown/select control that opens a floating popup with a search field at the top, grouped items with section headers, and keyboard navigation (Arrow keys, Enter to select, Escape to close).

**Implementation Details:** ThemedDropdown inherits ThemeAwareWindow. The closed state renders like a text input with a chevron-down icon on the right. On click or Alt+Down, a `wxPopupTransientWindow` opens below (or above if near bottom edge). Popup contains: a ThemedTextInput for search (optional, enabled via `set_searchable(true)`), a scrollable list of items rendered via custom paint. Items support: `GroupedDropdownItem { string group; string label; string value; wxBitmapBundle icon; }`. Groups render as non-selectable header rows with bold text. Multi-select mode stores a `std::vector<string>` of selected values and renders checkmarks.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedDropdown.cpp` (create)

**Acceptance Criteria:**
- Dropdown opens popup on click with correct positioning
- Search field filters items in real time (case-insensitive substring match)
- Arrow keys navigate items, Enter selects, Escape closes
- Group headers render as bold non-selectable separators
- Multi-select mode allows multiple checked items

**Dependencies:** Task 05 (reuses ThemedTextInput for search)

---

### Task 07 -- Implement ThemedCheckbox with Indeterminate State

**Description:** Create a custom-drawn checkbox with three visual states: unchecked, checked (checkmark), and indeterminate (horizontal dash). The box animates between states with a 100ms scale transition.

**Implementation Details:** ThemedCheckbox inherits ThemeAwareWindow. Renders a 14x14 box with 2px border-radius. Unchecked: border only. Checked: filled with accent_primary, white checkmark drawn via wxGraphicsPath (3 line segments). Indeterminate: filled with accent_primary, white horizontal dash (8px wide, 2px tall, centered). Label text renders 6px to the right of the box. State cycles: unchecked -> checked -> (indeterminate if `set_tristate(true)`) -> unchecked. Publishes `wxCommandEvent(wxEVT_CHECKBOX)` on state change.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedCheckbox.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedCheckbox.cpp` (create)

**Acceptance Criteria:**
- Checkbox renders 14x14 box with correct checkmark geometry
- Tristate mode cycles through all three states
- Focus ring renders around the box when focused via Tab
- Label click toggles the checkbox state
- Disabled state renders with 50% opacity

**Dependencies:** Task 01

---

### Task 08 -- Implement ThemedRadio Button Group

**Description:** Create a radio button control where one item in a group is selected at a time. Renders as a 14px circle with an inner filled dot (6px) when selected.

**Implementation Details:** ThemedRadio inherits ThemeAwareWindow. Renders a 14px diameter circle with 1px border. When selected, draws a filled 6px circle centered inside. Radio buttons within the same `radio_group` name automatically deselect siblings when one is selected. Implementation: a static `std::unordered_map<string, std::vector<ThemedRadio*>>` tracks groups. On selection, iterate group members and deselect others. Label renders 6px to the right.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedRadio.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedRadio.cpp` (create)

**Acceptance Criteria:**
- Radio renders as circle with inner dot when selected
- Only one radio in a group can be selected at a time
- Arrow keys cycle selection within a group when focused
- Focus ring renders around the circle
- Group name correctly scopes mutual exclusion

**Dependencies:** Task 01

---

### Task 09 -- Implement Toggle Switch

**Description:** Create a pill-shaped toggle switch (26x14px) with a sliding dot that animates between on/off positions with a 150ms ease-out transition.

**Implementation Details:** ThemedToggleSwitch inherits ThemeAwareWindow. Background is a rounded rectangle (7px radius). Off state: `bg_inactive` background, dot at left. On state: `accent_primary` background, dot at right. Dot is 10px circle with 2px padding from edges. Animation uses `wxTimer` at 16ms intervals, interpolating dot position with ease-out curve (`1 - pow(1 - t, 3)`). Publishes `wxCommandEvent(wxEVT_TOGGLEBUTTON)` on toggle.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedToggleSwitch.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedToggleSwitch.cpp` (create)

**Acceptance Criteria:**
- Toggle renders as pill with sliding dot
- Animation completes in 150ms with ease-out curve
- Click anywhere on the control toggles state
- Spacebar toggles when focused
- Disabled state renders at 40% opacity and ignores input

**Dependencies:** Task 01

---

### Task 10 -- Implement ThemedSlider with Value Tooltip

**Description:** Create a horizontal slider with a thin track (2px), circular thumb (12px diameter), and a floating value tooltip that appears during drag.

**Implementation Details:** ThemedSlider inherits ThemeAwareWindow. Track renders as a 2px horizontal line with rounded ends. Filled portion (left of thumb) uses accent_primary, unfilled uses bg_inactive. Thumb is a 12px circle filled with accent_primary, 1px white border. On mouse-down on thumb, begin drag. During drag, show a floating tooltip 8px above the thumb displaying the current value (formatted via a configurable `std::function<string(double)>` formatter). Value range defaults to 0.0-1.0 but is configurable. Step increments supported via arrow keys.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedSlider.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedSlider.cpp` (create)

**Acceptance Criteria:**
- Slider renders track and thumb correctly
- Dragging thumb updates value in real time
- Tooltip appears during drag showing formatted value
- Arrow keys increment/decrement by step value
- Click on track jumps thumb to click position

**Dependencies:** Task 01

---

### Task 11 -- Implement Linear and Circular Progress Bars

**Description:** Create progress bar controls in both linear (horizontal bar) and circular (ring) variants. Linear supports determinate (0-100%) and indeterminate (shimmer animation) modes. Circular renders as a stroke ring with percentage text in center.

**Implementation Details:** ThemedProgressBar inherits ThemeAwareWindow. Constructor takes `ProgressBarStyle::Linear` or `ProgressBarStyle::Circular`. Linear: 4px height, bg_inactive track, accent_primary fill, rounded ends. Indeterminate mode animates a 30% width highlight sweeping left-to-right using wxTimer. Circular: configurable diameter (default 48px), 4px stroke width, bg_inactive track arc, accent_primary fill arc from 12-o'clock clockwise. Center text shows percentage using `wxDC::DrawText` centered.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedProgressBar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedProgressBar.cpp` (create)

**Acceptance Criteria:**
- Linear bar fills proportionally with smooth animation
- Indeterminate shimmer animates continuously at 60fps
- Circular ring draws correct arc from 0% to 100%
- Center text displays percentage in circular mode
- set_value(0.5) renders 50% fill

**Dependencies:** Task 01

---

### Task 12 -- Implement Badge Component

**Description:** Create a badge component that renders as a small pill-shaped indicator for counts (numeric badge) or status (dot badge). Used on tabs, tree items, notification bell, and sidebar buttons.

**Implementation Details:** Badge inherits ThemeAwareWindow. Two modes: `BadgeMode::Numeric` (shows count, min-width 18px, pill shape) and `BadgeMode::Dot` (8px circle, no text). Numeric badge auto-sizes to fit text width + 8px padding. Font is 10px bold. Background color is configurable (default: `accent_primary` for info, `danger` for error, `warning` for warnings). Text color is always white. Badge truncates at "99+" for counts over 99.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Badge.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/Badge.cpp` (create)

**Acceptance Criteria:**
- Numeric badge renders pill shape with count text
- Dot badge renders as 8px circle
- Count > 99 displays "99+"
- Badge repaints when count changes
- Badge renders correctly at all theme colors

**Dependencies:** Task 01

---

### Task 13 -- Implement Chip/Tag Component

**Description:** Create a chip/tag component for displaying and managing labels, tags, and filter tokens. Supports removable chips (with X button), clickable chips, and color-coded chips.

**Implementation Details:** ChipTag inherits ThemeAwareWindow. Renders as a rounded rectangle (12px radius, 24px height). Contains: optional icon (14x14), label text, optional remove button (12px X icon on the right). Background color is either derived from a hash of the label text (for auto-color) or explicitly set. Remove button appears on hover and fires `wxCommandEvent` with custom ID. Chips can be marked as `selected` which adds a 2px accent border.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ChipTag.cpp` (create)

**Acceptance Criteria:**
- Chip renders with correct rounded rectangle and label
- Remove X button appears on hover (not on disabled chips)
- Auto-color mode generates consistent colors from label hash
- Selected state shows accent border
- Chip truncates label with ellipsis beyond max-width

**Dependencies:** Task 01

---

### Task 14 -- Implement Rich Tooltip with Delay

**Description:** Create a tooltip system that supports plain text, rich content (title + description + optional shortcut hint), configurable show/hide delays, and proper positioning (flip when near screen edges).

**Implementation Details:** RichTooltip is a `wxPopupTransientWindow` managed by a singleton `TooltipManager`. Controls call `TooltipManager::show(wxWindow* anchor, TooltipContent content, int delay_ms)`. TooltipContent struct contains: `title` (bold), `description` (normal), `shortcut` (monospace, right-aligned). The popup renders with a 1px border, 8px padding, max-width 300px, and positions 8px below the anchor. A wxTimer delays show by `delay_ms` (default 200ms). Moving the mouse to another control cancels the pending tooltip. Hide delay is 100ms to allow mouse-to-tooltip movement for interactive tooltips.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/RichTooltip.cpp` (create)

**Acceptance Criteria:**
- Tooltip appears after configured delay (default 200ms)
- Tooltip repositions when near screen edges (flip above or to left)
- Rich content renders with title (bold), description, shortcut (monospace)
- Moving mouse away dismisses tooltip after 100ms
- Only one tooltip visible at a time (singleton enforcement)

**Dependencies:** Task 01

---

### Task 15 -- Implement Segmented Control

**Description:** Create a segmented control (button group) where one segment is selected at a time, used for view mode switching (e.g., Inline/Side-by-Side in diff view, Grid/List in file browser).

**Implementation Details:** SegmentedControl inherits ThemeAwareWindow. Constructor takes `std::vector<SegmentItem>` where each item has `label`, `icon`, and `value`. Renders as a horizontal strip of equal-width segments separated by 1px dividers. Selected segment has accent background. Hover shows subtle highlight. Keyboard: Left/Right arrow keys change selection. Fires `wxCommandEvent` with the selected value on change.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SegmentedControl.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/SegmentedControl.cpp` (create)

**Acceptance Criteria:**
- Segments render with equal width and 1px dividers
- Selected segment shows accent background
- Arrow keys cycle selection with wrap-around
- Click on segment selects it and fires change event
- Segments support icon-only, text-only, or icon+text

**Dependencies:** Task 01

---

### Task 16 -- Implement Color Picker

**Description:** Create a color picker control with a color swatch button that opens a popup containing a hue/saturation square, lightness slider, hex input, and recent colors strip.

**Implementation Details:** ThemedColorPicker inherits ThemeAwareWindow. Closed state: 24x24 swatch showing current color with a 1px border. On click, opens a `wxPopupTransientWindow` containing: (1) 200x200 SV square (saturation X, value Y) drawn via wxBitmap, (2) 20px-wide vertical hue slider (rainbow gradient), (3) ThemedTextInput for hex value (#RRGGBB), (4) 8-slot recent colors strip. Mouse drag on the SV square or hue slider updates the color in real time and publishes `wxCommandEvent`. The existing `core::Color` class is used for HSV<->RGB conversion.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedColorPicker.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedColorPicker.cpp` (create)

**Acceptance Criteria:**
- Swatch displays current color accurately
- SV square and hue slider render correct gradients
- Hex input accepts valid #RRGGBB and updates picker
- Recent colors strip stores last 8 selected colors
- Color change fires event with new color value

**Dependencies:** Task 05 (ThemedTextInput for hex input), Task 01

---

### Task 17 -- Implement Number Input with Stepper

**Description:** Create a number input with increment/decrement buttons (up/down arrows), configurable min/max/step, and direct text editing with validation.

**Implementation Details:** NumberStepper inherits ThemeAwareWindow. Layout: [text input | up-arrow-btn | down-arrow-btn]. Text input is a ThemedTextInput that validates on blur (clamps to min/max, rounds to nearest step). Up/down buttons are 20x12px each, stacked vertically. Holding a button repeats the increment/decrement with accelerating delay (initial 400ms, then 100ms after 1s). Arrow keys increment/decrement when the text input is focused. Scroll wheel increments/decrements when hovered.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/NumberStepper.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/NumberStepper.cpp` (create)

**Acceptance Criteria:**
- Stepper increments/decrements within min/max bounds
- Holding button auto-repeats with acceleration
- Text input accepts valid numbers and clamps on blur
- Arrow keys and scroll wheel modify value
- Step value is configurable (e.g., 0.1, 1, 10)

**Dependencies:** Task 05 (ThemedTextInput), Task 03 (IconButton)

---

### Task 18 -- Implement Tree View Item with Hover Actions

**Description:** Create a tree view row component that renders with expand chevron, file/folder icon, label text, and inline action buttons that appear on hover. This replaces the ad-hoc rendering in FileTreeCtrl.

**Implementation Details:** TreeViewItem inherits ThemeAwareWindow. Row height is 22px. Layout: [indent (20px * depth) | chevron 16px | icon 16px | label flex | hover_actions]. Chevron rotates 90 degrees when expanded (animated 100ms). Hover actions are a `std::vector<IconButton*>` that appear at the right edge on mouse-enter and hide on mouse-leave. Selection highlight uses accent_primary at 20% opacity. Double-click fires open event. Right-click fires context menu event. Uses IndexedControlState for multi-row state tracking.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TreeViewItem.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/TreeViewItem.cpp` (create)

**Acceptance Criteria:**
- Row renders at 22px height with correct indentation
- Chevron animates rotation on expand/collapse
- Hover actions appear on mouse-enter, hide on mouse-leave
- Selection highlight uses semi-transparent accent
- Right-click opens context menu at correct position

**Dependencies:** Task 03 (IconButton), Task 01

---

### Task 19 -- Add Animation Utility for Smooth Transitions

**Description:** Create a shared animation utility class that components use for hover transitions, toggle slides, and chevron rotations, eliminating per-component wxTimer boilerplate.

**Implementation Details:** Create `AnimationDriver` class in ComponentLibrary.h. It manages a single wxTimer shared across all animated properties. Components register `AnimatedProperty { double from; double to; double current; int duration_ms; EasingFunction easing; Callback on_update; }`. The driver ticks at 16ms (60fps) and advances all active properties. When a property reaches its target, it is removed from the active set. Easing functions: `linear`, `ease_out_cubic`, `ease_in_out_quad`. Components call `driver.animate(property)` to start and `driver.cancel(property)` to abort.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ComponentLibrary.h` (extend)
- `/Users/ryanrentfro/code/markamp/src/ui/ComponentLibrary.cpp` (create)

**Acceptance Criteria:**
- AnimationDriver ticks at 16ms intervals
- Multiple properties animate concurrently without interference
- Easing functions produce correct curves
- Completed animations are cleaned up automatically
- Cancelling an animation snaps to current value

**Dependencies:** None

---

### Task 20 -- Wire Theme Change Propagation to All New Controls

**Description:** Ensure all new controls respond to ThemeChangedEvent by re-resolving their VariantTokens and repainting. Verify that switching themes at runtime updates every control without flicker or stale colors.

**Implementation Details:** Since all new controls inherit ThemeAwareWindow, they already receive OnThemeChanged callbacks. Each control's OnThemeChanged override must: (1) re-resolve VariantTokens via VariantTokenResolver, (2) invalidate cached wxBrush/wxPen objects, (3) call `Refresh()` to trigger repaint. Add a test that creates each control, changes theme, and verifies the paint output uses the new theme's colors.

**Files Affected:**
- All new control .cpp files created in Tasks 02-18
- `/Users/ryanrentfro/code/markamp/tests/unit/test_component_library.cpp` (create)

**Acceptance Criteria:**
- Theme switch updates all controls within one paint cycle
- No stale colors visible after theme change
- Test toggles between Dark and Light theme and validates token resolution
- No memory leaks from cached brush/pen objects

**Dependencies:** Tasks 02-18

---

### Task 21 -- Implement Keyboard Navigation for All Controls

**Description:** Ensure all new controls participate in Tab-order navigation, respond to keyboard activation (Space/Enter for buttons, Arrow keys for sliders/dropdowns), and display focus rings.

**Implementation Details:** Each control calls `SetCanFocus(true)` in its constructor. Tab order is managed by wxWidgets' native tab traversal. When focused, controls call `ThemeAwareWindow::DrawFocusRing(dc)` during paint. Button controls activate on Space/Enter. Slider responds to Left/Right arrows. Dropdown opens on Alt+Down/Space. Checkbox toggles on Space. All keyboard handlers use the existing `wxEVT_KEY_DOWN` binding pattern.

**Files Affected:**
- All new control .h/.cpp files created in Tasks 02-18

**Acceptance Criteria:**
- Tab cycles through all controls in order
- Focus ring is visible on the currently focused control
- Space/Enter activates buttons and checkboxes
- Arrow keys navigate dropdown items and slider values
- Escape closes open popups (dropdown, color picker, tooltip)

**Dependencies:** Tasks 02-18

---

### Task 22 -- Implement Accessibility Annotations for All Controls

**Description:** Add wxAccessible overrides to all new controls so screen readers can announce control type, state, label, and value.

**Implementation Details:** Each control overrides `wxAccessible::GetRole()` (returning appropriate `wxACC_ROLE_*`), `GetName()` (returning the control label), `GetState()` (returning checked/pressed/disabled flags), and `GetValue()` (returning slider position, text input content, etc.). ThemedCheckbox reports `wxACC_STATE_CHECKED` or `wxACC_STATE_MIXED` for indeterminate. ThemedToggleSwitch reports checked/unchecked. Dropdown reports expanded/collapsed.

**Files Affected:**
- All new control .h/.cpp files created in Tasks 02-18

**Acceptance Criteria:**
- VoiceOver (macOS) reads correct role, name, and state for each control
- Checkbox announces "checked", "unchecked", or "mixed"
- Toggle announces "on" or "off"
- Slider announces current value and range
- No controls produce "unknown" role in accessibility inspector

**Dependencies:** Tasks 02-18, Task 21

---

### Task 23 -- Migrate FileTreeCtrl to Use TreeViewItem

**Description:** Refactor the existing FileTreeCtrl to use the new TreeViewItem component for row rendering instead of its current ad-hoc paint code. This validates the component in a real use case and reduces duplicated rendering logic.

**Implementation Details:** Replace the custom `OnPaint` row rendering in FileTreeCtrl with instances of TreeViewItem. The FileTreeCtrl becomes a virtual-scrolling container that creates/recycles TreeViewItem instances for visible rows. Each TreeViewItem receives its data (icon, label, depth, expanded state) via `set_data()`. Hover actions are configured per-item (e.g., New File, New Folder, Delete for folders).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.h` (modify)

**Acceptance Criteria:**
- File tree renders identically to before using TreeViewItem
- Hover actions appear on tree rows
- Expand/collapse animation works on chevron click
- Selection and multi-selection work correctly
- No rendering regressions visible

**Dependencies:** Task 18

---

### Task 24 -- Migrate Settings Panel to Use New Controls

**Description:** Replace native wxWidgets controls in SettingsPanel and SettingsDialog with the new themed controls (ThemedTextInput, ThemedCheckbox, ThemedDropdown, NumberStepper, ThemedToggleSwitch).

**Implementation Details:** Audit all controls in SettingsPanel.cpp and SettingsDialog.cpp. Replace `wxTextCtrl` with ThemedTextInput, `wxCheckBox` with ThemedCheckbox, `wxChoice`/`wxComboBox` with ThemedDropdown, `wxSpinCtrl` with NumberStepper, and boolean toggles with ThemedToggleSwitch. Ensure all controls bind to the Config system correctly and publish changes via EventBus.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (modify)

**Acceptance Criteria:**
- Settings panel renders with all new themed controls
- All settings values persist correctly when changed
- Theme change updates all controls in settings panel
- Tab navigation works through all settings controls
- No functional regressions in settings behavior

**Dependencies:** Tasks 05, 06, 07, 09, 17

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new component source files to CMakeLists.txt and create a comprehensive unit test file that validates rendering logic, state transitions, and event firing for every new control.

**Implementation Details:** Add all new .h and .cpp files to the `add_executable` and `source_group` targets in CMakeLists.txt. Create `test_component_library.cpp` using Catch2. Test sections: (1) VariantTokenResolver mapping correctness, (2) ControlStateTracker transitions for each control, (3) AnimationDriver tick accuracy, (4) Badge count truncation at 99+, (5) Dropdown search filtering, (6) Slider value clamping, (7) NumberStepper min/max/step enforcement, (8) ThemedCheckbox tristate cycling, (9) Radio group mutual exclusion, (10) ChipTag auto-color consistency.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_component_library.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles all new sources without errors
- `ctest --output-on-failure` passes all new test sections
- No undefined symbol errors from missing source files
- source_group entries match add_executable entries for all new files

**Dependencies:** Tasks 01-24

---

### Task 26 -- High-DPI Rendering Validation

**Description:** Verify all new controls render correctly on high-DPI displays (Retina on macOS, 150%/200% scaling on Windows). Ensure icons use wxBitmapBundle for resolution-independent rendering and all pixel measurements account for DPI scaling.

**Implementation Details:** Audit all hardcoded pixel values in new controls (e.g., 14px checkbox, 12px thumb, 4px border-radius). Replace with `FromDIP()` calls where wxWidgets provides DPI-aware conversion. Ensure wxBitmapBundle is used for all icons so @2x variants are selected automatically. Test by running the app at 1x and 2x scale factors and visually comparing control rendering. Add a test helper that simulates different DPI factors.

**Files Affected:**
- All new control .cpp files created in Tasks 02-18

**Acceptance Criteria:**
- Controls render crisp at 1x, 1.5x, and 2x scale factors
- No blurry icons or misaligned borders at Retina resolution
- FromDIP() used for all size constants
- wxBitmapBundle provides appropriate resolution bitmaps

**Dependencies:** Tasks 02-18

---

### Task 27 -- Component Storybook / Gallery Panel

**Description:** Create a developer-only gallery panel that renders every new control in every variant and state, serving as a living style guide and regression testing surface.

**Implementation Details:** Create a ComponentGalleryPanel (debug-only, behind `MARKAMP_DEBUG` preprocessor flag) that arranges all new controls in a scrollable grid. Sections: Buttons (all 4 variants x 6 states), Text Inputs (normal, error, warning, disabled), Dropdowns (single, multi, grouped), Checkboxes (unchecked, checked, indeterminate), Radios, Toggles, Sliders, Progress Bars, Badges, Chips, Tooltips, Segmented Controls, Color Picker, Number Stepper, Tree View Items. This panel is accessible via Command Palette: "Developer: Open Component Gallery".

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ComponentGalleryPanel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ComponentGalleryPanel.cpp` (create)

**Acceptance Criteria:**
- Gallery panel renders all controls in all variants
- Gallery is accessible via Command Palette in debug builds only
- Scrolling through gallery shows no rendering artifacts
- Theme switch updates all gallery controls immediately
- Gallery serves as a visual regression reference

**Dependencies:** Tasks 02-19

## Completion Gates

- All 27 tasks executed or explicitly deferred with rationale
- Every new control renders correctly in all 6 ControlStateFlags
- Theme switching updates all controls without stale colors
- Keyboard Tab navigation traverses all controls
- Accessibility annotations work with VoiceOver
- Unit tests pass for all component logic
- FileTreeCtrl and SettingsPanel migrations show no regressions
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
