# Phase 01 -- Design System Foundation

## Objective

Establish a centralized, code-driven design system that governs every visual dimension of the MarkAmp Studio UI: color tokens, typography scale, spacing grid, elevation/shadow system, component size variants, and density settings. This phase replaces the current ad-hoc approach -- where each control hard-codes its own sizes, paddings, and colors -- with a single source of truth that all subsequent phases depend on.

## Prerequisites

- None. This is the foundational phase; all other v13 phases depend on it.
- Existing code: `ThemeEngine` (color tokens), `LayoutMetrics` (density profiles), `ControlState` (state flags), `MotionModel` (easing curves).

## Deliverables

- `DesignTokenRegistry` -- runtime registry mapping token names to resolved values.
- `TypographyScale` -- type ramp with semantic names (caption, body, subtitle, title, headline).
- `SpacingGrid` -- 4px-based spacing scale with named stops.
- `ElevationSystem` -- shadow definitions for layered surfaces.
- `ComponentSizeVariant` enum and resolver -- compact/default/comfortable sizing for all controls.
- Integration with existing `ThemeEngine` and `LayoutMetrics`.
- Catch2 test target: `test_design_system`.

## Estimated Complexity

High -- touches the foundation that every UI control will consume going forward.

---

## Tasks

### Task 1: Define the DesignToken Value Type

**Description:** Create a variant value type that can represent any design token value: color (wxColour), dimension (int pixels), font spec, spacing, opacity (float), or shadow definition. This is the atomic unit stored in the registry.

**Key Implementation Details:**
- Class: `DesignTokenValue` in `src/ui/DesignTokenValue.h`
- Use `std::variant<int, float, wxColour, wxFont, ShadowSpec>` as the backing store.
- Provide typed accessors: `as_int()`, `as_float()`, `as_colour()`, `as_font()`, `as_shadow()`.
- All accessors return `std::optional` to handle type mismatches without exceptions.
- Include a `kind()` enum accessor: `kInt`, `kFloat`, `kColour`, `kFont`, `kShadow`.

**Files Affected:**
- `src/ui/DesignTokenValue.h` (new)
- `src/ui/DesignTokenValue.cpp` (new)

**Acceptance Criteria:**
- Construct a `DesignTokenValue` from each supported type.
- `as_int()` returns `std::nullopt` when the value is a colour (and vice versa).
- `kind()` correctly identifies the stored type.

**Dependencies:** None.

---

### Task 2: Define the ShadowSpec Structure

**Description:** Create a data structure representing a single box shadow, used by the elevation system. Each shadow has offset-x, offset-y, blur radius, spread radius, and color with opacity.

**Key Implementation Details:**
- Struct: `ShadowSpec` in `src/ui/ElevationSystem.h`
- Fields: `int offset_x`, `int offset_y`, `int blur_radius`, `int spread_radius`, `wxColour color`, `float opacity`.
- Provide a convenience constructor and a `Render(wxGraphicsContext& gc, const wxRect& bounds)` method that applies the shadow using `gc.SetBrush` with a gradient fill simulating the shadow.
- Define `ElevationLevel` enum: `kNone`, `kLow`, `kMedium`, `kHigh`, `kOverlay`.

**Files Affected:**
- `src/ui/ElevationSystem.h` (new)
- `src/ui/ElevationSystem.cpp` (new)

**Acceptance Criteria:**
- `ShadowSpec` can be default-constructed and compared for equality.
- `ElevationLevel` has exactly 5 values.
- Render method produces no crash when given a zero-size rect.

**Dependencies:** None.

---

### Task 3: Create the ElevationSystem Class

**Description:** Map each `ElevationLevel` to one or more `ShadowSpec` layers. Provide a single call to render the elevation shadow for any rect, and support theme-aware shadow colors (dark themes use lighter glows, light themes use darker shadows).

**Key Implementation Details:**
- Class: `ElevationSystem` in `src/ui/ElevationSystem.h` / `.cpp`
- Method: `void Render(wxGraphicsContext& gc, const wxRect& bounds, ElevationLevel level) const`
- Method: `void SetDarkMode(bool dark)` -- switches shadow color palette.
- Internal table: `std::array<std::vector<ShadowSpec>, 5>` indexed by `ElevationLevel`.
- `kNone` = empty vector (no shadow).
- `kLow` = 1 layer, 2px blur, 1px y-offset.
- `kMedium` = 2 layers, 4px + 8px blur.
- `kHigh` = 3 layers, 8px + 16px + 24px blur.
- `kOverlay` = full drop shadow, 32px blur.

**Files Affected:**
- `src/ui/ElevationSystem.h`
- `src/ui/ElevationSystem.cpp`

**Acceptance Criteria:**
- `Render` with `kNone` is a no-op (no drawing calls).
- Shadow color changes when `SetDarkMode` is toggled.
- Each level produces the documented number of shadow layers.

**Dependencies:** Task 2.

---

### Task 4: Define the Typography Scale

**Description:** Create a semantic typography scale that maps named slots to font configurations (family, size, weight, line height). Each slot adjusts automatically based on the active density profile.

**Key Implementation Details:**
- Enum: `TypeSlot` -- `kCaption`, `kBody`, `kBodyStrong`, `kSubtitle`, `kTitle`, `kHeadline`, `kMono`, `kMonoSmall`.
- Class: `TypographyScale` in `src/ui/TypographyScale.h`
- Struct: `TypeSpec { std::string family; int size_pt; int weight; int line_height_px; }`.
- Method: `auto resolve(TypeSlot slot) const -> TypeSpec` -- returns spec for current density.
- Method: `auto font(TypeSlot slot) const -> wxFont` -- returns cached wxFont.
- Method: `void set_density(DensityProfile profile)` -- adjusts sizes: compact (-1pt), default (base), comfortable (+1pt).
- Method: `void set_base_sans(const std::string& family)` and `set_base_mono(const std::string& family)` for user font overrides.
- Default sans: "Inter" (fallback "SF Pro", "Segoe UI", "Noto Sans").
- Default mono: "JetBrains Mono" (fallback "Menlo", "Consolas", "Noto Mono").

**Files Affected:**
- `src/ui/TypographyScale.h` (new)
- `src/ui/TypographyScale.cpp` (new)

**Acceptance Criteria:**
- `resolve(kBody)` returns a 13pt font at default density, 12pt at compact, 14pt at comfortable.
- `font(kMono)` returns a monospace wxFont.
- Setting `set_base_sans("Custom")` changes the family for all non-mono slots.

**Dependencies:** None.

---

### Task 5: Define the Spacing Grid

**Description:** Create a 4px-base spacing scale with named stops, used as the canonical source for all margins, paddings, and gaps throughout the UI. Controls should never hard-code pixel values for spacing.

**Key Implementation Details:**
- Enum: `SpacingToken` -- `kNone` (0), `kXxs` (2), `kXs` (4), `kSm` (8), `kMd` (12), `kLg` (16), `kXl` (24), `kXxl` (32), `kXxxl` (48).
- Class: `SpacingGrid` in `src/ui/SpacingGrid.h`
- Method: `static constexpr auto resolve(SpacingToken token) -> int` -- compile-time resolution.
- Method: `auto scaled(SpacingToken token) const -> int` -- runtime resolution adjusted by density profile.
- Density scaling: compact = 0.75x, default = 1.0x, comfortable = 1.25x (rounded to nearest even integer).
- Provide convenience: `auto gap() -> int` (returns `kSm` scaled), `auto section_gap() -> int` (returns `kXl` scaled).

**Files Affected:**
- `src/ui/SpacingGrid.h` (new)
- `src/ui/SpacingGrid.cpp` (new)

**Acceptance Criteria:**
- `SpacingGrid::resolve(SpacingToken::kMd)` returns 12 at compile time.
- At compact density, `scaled(kLg)` returns 12 (16 * 0.75 = 12).
- At comfortable density, `scaled(kSm)` returns 10 (8 * 1.25 = 10).

**Dependencies:** None.

---

### Task 6: Define Component Size Variant System

**Description:** Create a system that provides standard dimensions (height, min-width, icon size, padding) for three density variants: compact, default, and comfortable. Every interactive control (buttons, inputs, tabs, tree rows) queries this system rather than hard-coding sizes.

**Key Implementation Details:**
- Enum: `ComponentKind` -- `kButton`, `kInput`, `kTab`, `kTreeRow`, `kListRow`, `kToolbarButton`, `kActivityBarSlot`, `kStatusBarSegment`, `kBreadcrumbSegment`, `kPanelHeader`.
- Struct: `ComponentMetrics { int height; int min_width; int icon_size; int padding_h; int padding_v; }`.
- Class: `ComponentSizeResolver` in `src/ui/ComponentSizeResolver.h`
- Method: `auto resolve(ComponentKind kind) const -> ComponentMetrics` -- returns metrics for current density.
- Internally stores a `std::array<std::array<ComponentMetrics, kKindCount>, 3>` table indexed by [density][kind].
- Singleton accessor: `static auto get() -> ComponentSizeResolver&`.

**Files Affected:**
- `src/ui/ComponentSizeResolver.h` (new)
- `src/ui/ComponentSizeResolver.cpp` (new)

**Acceptance Criteria:**
- `resolve(kButton)` returns height 28 at compact, 32 at default, 36 at comfortable.
- `resolve(kTab)` returns height 32 at compact, 36 at default, 40 at comfortable.
- All padding values are multiples of 2.

**Dependencies:** Task 5 (uses SpacingGrid for padding values).

---

### Task 7: Create the DesignTokenRegistry

**Description:** Build the central registry that stores all design tokens by name and resolves them at runtime. This is the top-level aggregator that combines color tokens (from ThemeEngine), typography (from TypographyScale), spacing (from SpacingGrid), elevation, and component sizes into a single queryable interface.

**Key Implementation Details:**
- Class: `DesignTokenRegistry` in `src/ui/DesignTokenRegistry.h`
- Owns: `TypographyScale`, `SpacingGrid`, `ElevationSystem`, `ComponentSizeResolver*` (reference to singleton).
- References: `core::ThemeEngine&` for color tokens.
- Method: `auto color(core::ThemeColorToken token) const -> const wxColour&` -- delegates to ThemeEngine.
- Method: `auto type(TypeSlot slot) const -> TypeSpec` -- delegates to TypographyScale.
- Method: `auto spacing(SpacingToken token) const -> int` -- delegates to SpacingGrid.
- Method: `auto component(ComponentKind kind) const -> ComponentMetrics` -- delegates to ComponentSizeResolver.
- Method: `auto elevation() const -> const ElevationSystem&`.
- Method: `void set_density(DensityProfile profile)` -- propagates to all subsystems.
- Constructor injection: `DesignTokenRegistry(core::ThemeEngine& theme, core::EventBus& bus)`.
- Publishes `DesignSystemDensityChanged` event when density changes.

**Files Affected:**
- `src/ui/DesignTokenRegistry.h` (new)
- `src/ui/DesignTokenRegistry.cpp` (new)

**Acceptance Criteria:**
- After construction, `color(ThemeColorToken::BgApp)` returns the same value as `ThemeEngine::color(BgApp)`.
- `set_density(kCompact)` causes `spacing(kLg)` to return 12.
- `component(kButton).height` changes when density changes.

**Dependencies:** Tasks 1, 3, 4, 5, 6.

---

### Task 8: Integrate Density Profile with Existing LayoutMetrics

**Description:** The existing `LayoutMetrics` singleton already provides density-aware metrics. Refactor it to delegate to `ComponentSizeResolver` and `SpacingGrid` instead of maintaining its own hard-coded tables, eliminating the duplication.

**Key Implementation Details:**
- `LayoutMetrics::row_height()` should delegate to `ComponentSizeResolver::get().resolve(kTreeRow).height`.
- `LayoutMetrics::tab_height()` delegates to `ComponentSizeResolver::get().resolve(kTab).height`.
- `LayoutMetrics::toolbar_height()` delegates to `ComponentSizeResolver::get().resolve(kToolbarButton).height`.
- `LayoutMetrics::control_padding_h()` delegates to `SpacingGrid`.
- `LayoutMetrics::icon_size()` delegates to `ComponentSizeResolver`.
- Remove the hard-coded switch statements currently in `LayoutMetrics.cpp`.
- Keep `LayoutMetrics::set_profile()` but have it forward to `ComponentSizeResolver::get()` and `SpacingGrid`.

**Files Affected:**
- `src/ui/LayoutMetrics.h` (modify -- add dependency on ComponentSizeResolver)
- `src/ui/LayoutMetrics.cpp` (modify -- replace switch bodies with delegation)

**Acceptance Criteria:**
- All existing `LayoutMetrics` return values remain unchanged at `kDefault` density.
- Changing `LayoutMetrics::set_profile(kCompact)` produces different values consistent with `ComponentSizeResolver`.
- No code in `LayoutMetrics.cpp` contains hard-coded pixel values for sizes.

**Dependencies:** Tasks 5, 6.

---

### Task 9: Define Design System Events

**Description:** Declare EventBus events for design system changes so that all controls can react when the user switches density, typography, or color theme.

**Key Implementation Details:**
- In `src/core/Events.h`, add new event types using `MARKAMP_DECLARE_EVENT`:
  - `DensityProfileChanged { DensityProfile new_profile; }`
  - `TypographyScaleChanged { std::string base_sans; std::string base_mono; }`
  - `ElevationModeChanged { bool dark_mode; }`
- Ensure `DesignTokenRegistry::set_density()` publishes `DensityProfileChanged`.
- Ensure `TypographyScale::set_base_sans/mono()` publishes `TypographyScaleChanged`.
- Existing `ThemeChangedEvent` already covers color changes.

**Files Affected:**
- `src/core/Events.h` (modify -- add 3 event declarations)
- `src/ui/DesignTokenRegistry.cpp` (modify -- publish events on changes)

**Acceptance Criteria:**
- Subscribing to `DensityProfileChanged` and calling `set_density(kCompact)` fires the handler exactly once.
- `TypographyScaleChanged` carries the new font family strings.

**Dependencies:** Task 7.

---

### Task 10: Create the Theme-Aware Color Palette Generator

**Description:** Build a utility that derives a complete extended palette from the existing 10 base color tokens. Given a base theme, it generates hover states, pressed states, disabled states, selection backgrounds, borders, and focus rings -- reducing the need for manual color definitions in every theme file.

**Key Implementation Details:**
- Class: `ColorPaletteGenerator` in `src/ui/ColorPaletteGenerator.h`
- Method: `auto hover_variant(const wxColour& base) const -> wxColour` -- lightens by 10% in dark mode, darkens by 10% in light mode.
- Method: `auto pressed_variant(const wxColour& base) const -> wxColour` -- shifts by 20%.
- Method: `auto disabled_variant(const wxColour& base) const -> wxColour` -- reduces saturation by 50%, adjusts alpha to 0.5.
- Method: `auto focus_ring(const wxColour& accent) const -> wxColour` -- returns accent at 40% opacity.
- Method: `auto generate_extended_palette(const core::Theme& base) const -> ExtendedPalette` -- produces all derived colors in one call.
- Struct: `ExtendedPalette` contains all generated colors keyed by `ThemeColorToken`.
- Should use HSL color space for transformations.

**Files Affected:**
- `src/ui/ColorPaletteGenerator.h` (new)
- `src/ui/ColorPaletteGenerator.cpp` (new)

**Acceptance Criteria:**
- `hover_variant` of pure white (`#FFFFFF`) in dark mode produces a lighter shade (clamped at white).
- `disabled_variant` always reduces saturation.
- `generate_extended_palette` returns at least 20 derived colors.

**Dependencies:** None.

---

### Task 11: HSL Color Space Utility

**Description:** Implement HSL (Hue, Saturation, Lightness) color space conversion utilities since wxWidgets only provides RGB. These utilities are required by the palette generator and will be used throughout the theme system.

**Key Implementation Details:**
- Struct: `HSLColor { float h; float s; float l; float a; }` where h is [0,360), s/l/a are [0,1].
- Free functions in `src/ui/ColorUtils.h`:
  - `auto rgb_to_hsl(const wxColour& c) -> HSLColor`
  - `auto hsl_to_rgb(const HSLColor& hsl) -> wxColour`
  - `auto lighten(const wxColour& c, float amount) -> wxColour` -- amount is [0,1]
  - `auto darken(const wxColour& c, float amount) -> wxColour`
  - `auto desaturate(const wxColour& c, float amount) -> wxColour`
  - `auto with_alpha(const wxColour& c, float alpha) -> wxColour`
  - `auto contrast_ratio(const wxColour& fg, const wxColour& bg) -> float` -- WCAG 2.1 formula
  - `auto meets_wcag_aa(const wxColour& fg, const wxColour& bg) -> bool` -- ratio >= 4.5
  - `auto meets_wcag_aaa(const wxColour& fg, const wxColour& bg) -> bool` -- ratio >= 7.0

**Files Affected:**
- `src/ui/ColorUtils.h` (new)
- `src/ui/ColorUtils.cpp` (new)

**Acceptance Criteria:**
- Round-trip: `hsl_to_rgb(rgb_to_hsl(color))` returns the original color (within +/-1 per channel).
- `contrast_ratio(black, white)` returns 21.0.
- `meets_wcag_aa(wxColour(100,100,100), wxColour(255,255,255))` returns the correct boolean per WCAG formula.

**Dependencies:** None.

---

### Task 12: Integrate Extended Palette into ThemeEngine

**Description:** When a theme is applied, automatically run the `ColorPaletteGenerator` to fill in any missing derived tokens. The ThemeEngine should use the extended palette to populate tokens like `ControlBgHover`, `ControlBgPressed`, etc., if they are not explicitly defined in the theme file.

**Key Implementation Details:**
- In `ThemeEngine::rebuild_cache()`, after loading the base theme tokens:
  1. Run `ColorPaletteGenerator::generate_extended_palette(current_theme_)`.
  2. For each token in the extended palette, if the token has no explicit value in the theme, use the generated value.
  3. This ensures custom themes only need to define the 10 base tokens; all control state tokens are auto-derived.
- Add `ColorPaletteGenerator` as a member of `ThemeEngine`.

**Files Affected:**
- `src/core/ThemeEngine.h` (modify -- add `ColorPaletteGenerator` member)
- `src/core/ThemeEngine.cpp` (modify -- call generator in `rebuild_cache`)

**Acceptance Criteria:**
- A theme that only defines 10 base tokens still produces valid `ControlBgHover` colors.
- A theme that explicitly defines `ControlBgHover` overrides the generated value.
- `ThemeEngine::missing_tokens()` returns fewer tokens after palette generation.

**Dependencies:** Tasks 10, 11.

---

### Task 13: Create the DensityProfileSwitcher UI

**Description:** Add a density profile selector to the Settings sidebar panel (or Command Palette) that lets users switch between Compact, Default, and Comfortable. The switch should take effect immediately across all controls.

**Key Implementation Details:**
- Add a Command Palette command: "View: Set Density Profile" with sub-choices.
- Register in `MainFrame::RegisterPaletteCommands()`.
- The command calls `DesignTokenRegistry::set_density(profile)`.
- The density change event propagates to `LayoutMetrics`, which triggers relayout.
- Persist the selected density in `Config` under key `"ui.density"`.
- Load the saved density on startup in `MarkAmpApp::OnInit()`.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- add command registration)
- `src/app/MarkAmpApp.cpp` (modify -- load density from config)
- `src/ui/DesignTokenRegistry.cpp` (modify -- persist to config on change)

**Acceptance Criteria:**
- Running the command "View: Set Density Profile > Compact" visually shrinks all controls.
- Restarting the app restores the previously selected density.
- All three density profiles produce distinct `row_height()` values.

**Dependencies:** Tasks 7, 8, 9.

---

### Task 14: Define Base Theme Presets (Dark, Light, High Contrast)

**Description:** Create three canonical base theme presets that define all token values. These serve as the starting point for user-created themes and ensure the design system has complete coverage.

**Key Implementation Details:**
- Each preset defines all `ThemeColorToken` values explicitly (no reliance on palette generator for base presets).
- Dark preset: dark backgrounds (#1E1E2E), light text (#CDD6F4), blue accent (#89B4FA).
- Light preset: light backgrounds (#EFF1F5), dark text (#4C4F69), blue accent (#1E66F5).
- High Contrast preset: pure black backgrounds (#000000), white text (#FFFFFF), yellow accent (#FFFF00), all borders visible.
- Store as YAML frontmatter Markdown files in `themes/` directory (following existing convention).
- Each file defines color mappings for all tokens including the new V10 control state tokens.

**Files Affected:**
- `themes/dark-v13.md` (new)
- `themes/light-v13.md` (new)
- `themes/high-contrast-v13.md` (new)

**Acceptance Criteria:**
- Each theme file parses without error.
- All `ThemeColorToken` enum values have an explicit mapping (verified by `ThemeEngine::missing_tokens()` returning empty vector).
- High contrast theme passes `meets_wcag_aaa` for all text/background pairs.

**Dependencies:** Tasks 10, 11.

---

### Task 15: Create the DesignSystemContext Convenience Accessor

**Description:** Create a lightweight context object that bundles references to all design system subsystems, passed by reference to controls that need design token access. This avoids each control needing 4-5 separate constructor parameters.

**Key Implementation Details:**
- Struct: `DesignSystemContext` in `src/ui/DesignSystemContext.h`
- Fields:
  - `DesignTokenRegistry& tokens`
  - `core::ThemeEngine& theme`
  - `core::EventBus& events`
- This is a reference-only struct (no ownership). Created once in `MarkAmpApp::OnInit()` and passed down the widget tree.
- Replaces the pattern of passing `(ThemeEngine&, EventBus&)` everywhere with `(DesignSystemContext&)`.
- For backward compatibility, keep existing constructors but add overloads accepting `DesignSystemContext&`.

**Files Affected:**
- `src/ui/DesignSystemContext.h` (new)

**Acceptance Criteria:**
- `DesignSystemContext` compiles as an aggregate with brace initialization.
- Can be passed by reference to any control constructor.
- Accessing `ctx.tokens.spacing(kMd)` returns the correct value.

**Dependencies:** Task 7.

---

### Task 16: Refactor ActivityBar to Use Design System

**Description:** Migrate `ActivityBar` from hard-coded constants to design system queries. This serves as the reference implementation for how all controls should consume the design system.

**Key Implementation Details:**
- Replace `kBarWidth = 48` with `ComponentSizeResolver::get().resolve(kActivityBarSlot).height` for slot height and a computed bar width.
- Replace `kIconSize = 24` with `DesignTokenRegistry::component(kActivityBarSlot).icon_size`.
- Replace `kIconPadding = 12` with `SpacingGrid::scaled(SpacingToken::kMd)`.
- Use `TypographyScale::font(kCaption)` for tooltip text.
- Use `ElevationSystem::Render(gc, bounds, kLow)` for the active indicator glow.
- Use `ThemeEngine::color(ThemeColorToken::ActivityBarBg)` for background (already partially done).

**Files Affected:**
- `src/ui/ActivityBar.h` (modify -- remove hard-coded constexpr where replaced)
- `src/ui/ActivityBar.cpp` (modify -- replace literal values with design system calls)

**Acceptance Criteria:**
- Changing density profile changes the activity bar icon spacing and bar width.
- Visual appearance at default density is indistinguishable from the current implementation.
- No hard-coded pixel values remain for sizes or paddings in ActivityBar.cpp (colors still reference tokens).

**Dependencies:** Tasks 6, 7.

---

### Task 17: Refactor StatusBarPanel to Use Design System

**Description:** Migrate `StatusBarPanel` from hard-coded constants to design system queries.

**Key Implementation Details:**
- Replace `kHeight = 24` with `ComponentSizeResolver::get().resolve(kStatusBarSegment).height`.
- Use `TypographyScale::font(kCaption)` for all status bar text.
- Use `SpacingGrid::scaled(kSm)` for item padding.
- Use `ColorPaletteGenerator` derived tokens for hover states on clickable items.
- Use `ElevationSystem::Render(gc, bounds, kLow)` for the subtle top border shadow.

**Files Affected:**
- `src/ui/StatusBarPanel.h` (modify)
- `src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Status bar height changes with density profile.
- Font size adjusts with density.
- Hover effect on clickable items uses the derived `ControlBgHover` token.

**Dependencies:** Tasks 6, 7, 10.

---

### Task 18: Refactor TabBar to Use Design System

**Description:** Migrate `TabBar` from hard-coded constants to design system queries.

**Key Implementation Details:**
- Replace `kHeight = 36` with `ComponentSizeResolver::get().resolve(kTab).height`.
- Replace `kMaxTabWidth = 200` / `kMinTabWidth = 100` with density-scaled values.
- Replace `kTabPaddingH = 12` with `SpacingGrid::scaled(kMd)`.
- Replace `kCloseButtonSize = 14` with `ComponentSizeResolver::get().resolve(kTab).icon_size`.
- Use `TypographyScale::font(kBody)` for tab labels.
- Use `ThemeColorToken::TabActiveBg` / `TabInactiveBg` for backgrounds (already partially wired).
- Use `ElevationSystem::Render(gc, tab.rect, kLow)` for active tab shadow.

**Files Affected:**
- `src/ui/TabBar.h` (modify)
- `src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Tab height changes with density profile.
- Close button size scales proportionally.
- Active tab has a subtle elevation shadow.

**Dependencies:** Tasks 6, 7.

---

### Task 19: Refactor Toolbar to Use Design System

**Description:** Migrate `Toolbar` from hard-coded constants to design system queries.

**Key Implementation Details:**
- Replace `kHeight = 40` with `ComponentSizeResolver::get().resolve(kToolbarButton).height + SpacingGrid::scaled(kXs) * 2`.
- Use `TypographyScale::font(kBodyStrong)` for button labels.
- Use `SpacingGrid::scaled(kSm)` for button gaps.
- Replace ad-hoc icon drawing sizes with `ComponentSizeResolver::resolve(kToolbarButton).icon_size`.
- Use `ControlStateTracker` pattern consistently for all button hover/press states (partially done already).

**Files Affected:**
- `src/ui/Toolbar.h` (modify)
- `src/ui/Toolbar.cpp` (modify)

**Acceptance Criteria:**
- Toolbar height changes with density profile.
- Button labels use the correct font from the typography scale.
- Icon sizes are consistent with the component size resolver.

**Dependencies:** Tasks 6, 7.

---

### Task 20: Refactor BreadcrumbBar to Use Design System

**Description:** Migrate `BreadcrumbBar` from hard-coded layout to design system queries.

**Key Implementation Details:**
- Use `ComponentSizeResolver::get().resolve(kBreadcrumbSegment).height` for bar height.
- Use `TypographyScale::font(kCaption)` for segment labels.
- Use `SpacingGrid::scaled(kXs)` for separator spacing.
- Use `ThemeColorToken::BreadcrumbFg` / `BreadcrumbFocusFg` for colors (already defined).
- Separator chevron size from `ComponentSizeResolver`.

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (modify)
- `src/ui/BreadcrumbBar.cpp` (modify)

**Acceptance Criteria:**
- Breadcrumb height adjusts with density.
- Font matches the caption slot from typography scale.
- Separator spacing uses the spacing grid.

**Dependencies:** Tasks 6, 7.

---

### Task 21: Density-Aware SplitterBar

**Description:** Update the `SplitterBar` to use design system metrics for its hit zone width and visual line width.

**Key Implementation Details:**
- Replace `kHitWidth = 8` with `SpacingGrid::scaled(kSm)` (8 at default, 6 at compact, 10 at comfortable).
- Visual line width: `DesignTokenRegistry::tokens.spacing(kXxs)` (2px at default).
- Hover animation alpha ramp uses `MotionModel::effective_duration(MotionContext::kHover)`.
- Use `ThemeColorToken::BorderLight` for the splitter line color.

**Files Affected:**
- `src/ui/SplitterBar.h` (modify)
- `src/ui/SplitterBar.cpp` (modify)

**Acceptance Criteria:**
- Hit zone width changes with density.
- Hover animation respects reduced-motion setting.
- Splitter is always visible in high contrast mode.

**Dependencies:** Tasks 5, 7.

---

### Task 22: Design System Documentation Constants Header

**Description:** Create a single header that documents all the design system constants and their relationships, serving as a reference for developers.

**Key Implementation Details:**
- File: `src/ui/DesignSystemConstants.h`
- Contains:
  - Table of all `SpacingToken` values and their pixel equivalents at each density.
  - Table of all `TypeSlot` values and their font specs at each density.
  - Table of all `ComponentKind` values and their metrics at each density.
  - Table of all `ElevationLevel` shadow parameters.
- Implemented as a series of `static_assert` or `constexpr` validation checks.
- Example: `static_assert(SpacingGrid::resolve(SpacingToken::kSm) == 8)`.

**Files Affected:**
- `src/ui/DesignSystemConstants.h` (new)

**Acceptance Criteria:**
- File compiles without errors.
- All static assertions pass.
- Serves as readable documentation for the design system values.

**Dependencies:** Tasks 4, 5, 6.

---

### Task 23: Config Persistence for Design System Settings

**Description:** Persist all user-configurable design system settings to the Config YAML: density profile, custom font families, reduced motion preference.

**Key Implementation Details:**
- Config keys:
  - `ui.density` -- string: "compact", "default", "comfortable"
  - `ui.font.sans` -- string: custom sans family (empty = platform default)
  - `ui.font.mono` -- string: custom mono family (empty = platform default)
  - `ui.reduced_motion` -- bool
  - `ui.elevation.enabled` -- bool (allows disabling shadows entirely for performance)
- Load in `DesignTokenRegistry` constructor from `Config`.
- Save on change via `Config::set()`.

**Files Affected:**
- `src/ui/DesignTokenRegistry.cpp` (modify)
- `src/core/Config.h` (modify -- add keys if not already present)

**Acceptance Criteria:**
- Setting density to compact, restarting, confirms compact density is restored.
- Setting a custom sans font persists and applies on next launch.
- `ui.reduced_motion = true` disables all non-essential animations.

**Dependencies:** Tasks 7, 9.

---

### Task 24: Catch2 Test Suite for Design System

**Description:** Write comprehensive unit tests for all design system components.

**Key Implementation Details:**
- Test file: `tests/unit/test_design_system.cpp`
- Test target: `test_design_system` in CMakeLists.txt
- Test sections:
  - `DesignTokenValue` type safety and variant access.
  - `SpacingGrid` compile-time and runtime resolution at all densities.
  - `TypographyScale` font specs at all densities, font family overrides.
  - `ComponentSizeResolver` metrics at all densities, all component kinds.
  - `ElevationSystem` shadow layer counts per level.
  - `ColorPaletteGenerator` hover/pressed/disabled variants, round-trip HSL.
  - `ColorUtils` contrast ratio calculations, WCAG compliance checks.
  - `DesignTokenRegistry` end-to-end: set density, query typography, query spacing.
  - `LayoutMetrics` delegation (values match `ComponentSizeResolver`).
  - Event publication on density change.

**Files Affected:**
- `tests/unit/test_design_system.cpp` (new)
- `CMakeLists.txt` (modify -- add test target)

**Acceptance Criteria:**
- All tests pass with `ctest --output-on-failure`.
- At least 40 test assertions covering the full surface area.
- No test depends on wxWidgets GUI initialization (use headless-safe constructs).

**Dependencies:** All previous tasks.

---

### Task 25: Migration Guide and Deprecation Markers

**Description:** Document the migration path for existing controls to adopt the design system, and add `[[deprecated]]` attributes to the old hard-coded constants that should be replaced.

**Key Implementation Details:**
- Add `[[deprecated("Use ComponentSizeResolver::resolve(kTab).height")]]` to `TabBar::kHeight`.
- Add `[[deprecated("Use ComponentSizeResolver::resolve(kActivityBarSlot)")]]` to `ActivityBar::kBarWidth`.
- Add `[[deprecated("Use LayoutMetrics via DesignTokenRegistry")]]` to raw pixel constants in other controls.
- Create `docs/v13_docs/MIGRATION_DESIGN_SYSTEM.md` with:
  - Before/after code examples for each control type.
  - Checklist of constants to migrate per file.
  - Timeline: deprecated constants will be removed in v14.

**Files Affected:**
- `src/ui/TabBar.h` (modify -- add deprecation attributes)
- `src/ui/ActivityBar.h` (modify -- add deprecation attributes)
- `src/ui/StatusBarPanel.h` (modify -- add deprecation attributes)
- `src/ui/Toolbar.h` (modify -- add deprecation attributes)
- `docs/v13_docs/MIGRATION_DESIGN_SYSTEM.md` (new)

**Acceptance Criteria:**
- Compiling with deprecation warnings enabled shows warnings for any code still using old constants.
- Migration guide covers at least 10 before/after examples.
- No functional behavior changes from adding deprecation attributes.

**Dependencies:** Tasks 16, 17, 18, 19, 20.

---

## Dependency Graph

```
Task 1 (DesignTokenValue)  ──┐
Task 2 (ShadowSpec)  ────────┤
Task 3 (ElevationSystem) ────┤── depends on 2
Task 4 (TypographyScale) ────┤
Task 5 (SpacingGrid) ────────┤
Task 6 (ComponentSizeResolver) ── depends on 5
Task 7 (DesignTokenRegistry) ── depends on 1, 3, 4, 5, 6
Task 8 (LayoutMetrics refactor) ── depends on 5, 6
Task 9 (Events) ── depends on 7
Task 10 (ColorPaletteGenerator) ── depends on 11
Task 11 (HSL ColorUtils) ──┐
Task 12 (ThemeEngine integration) ── depends on 10, 11
Task 13 (DensitySwitcher UI) ── depends on 7, 8, 9
Task 14 (Base theme presets) ── depends on 10, 11
Task 15 (DesignSystemContext) ── depends on 7
Task 16 (ActivityBar refactor) ── depends on 6, 7
Task 17 (StatusBarPanel refactor) ── depends on 6, 7, 10
Task 18 (TabBar refactor) ── depends on 6, 7
Task 19 (Toolbar refactor) ── depends on 6, 7
Task 20 (BreadcrumbBar refactor) ── depends on 6, 7
Task 21 (SplitterBar refactor) ── depends on 5, 7
Task 22 (Constants header) ── depends on 4, 5, 6
Task 23 (Config persistence) ── depends on 7, 9
Task 24 (Tests) ── depends on all
Task 25 (Migration guide) ── depends on 16-20
```

## Files Created

| File | Type |
|------|------|
| `src/ui/DesignTokenValue.h` | Header |
| `src/ui/DesignTokenValue.cpp` | Implementation |
| `src/ui/ElevationSystem.h` | Header |
| `src/ui/ElevationSystem.cpp` | Implementation |
| `src/ui/TypographyScale.h` | Header |
| `src/ui/TypographyScale.cpp` | Implementation |
| `src/ui/SpacingGrid.h` | Header |
| `src/ui/SpacingGrid.cpp` | Implementation |
| `src/ui/ComponentSizeResolver.h` | Header |
| `src/ui/ComponentSizeResolver.cpp` | Implementation |
| `src/ui/DesignTokenRegistry.h` | Header |
| `src/ui/DesignTokenRegistry.cpp` | Implementation |
| `src/ui/ColorPaletteGenerator.h` | Header |
| `src/ui/ColorPaletteGenerator.cpp` | Implementation |
| `src/ui/ColorUtils.h` | Header |
| `src/ui/ColorUtils.cpp` | Implementation |
| `src/ui/DesignSystemContext.h` | Header |
| `src/ui/DesignSystemConstants.h` | Header |
| `themes/dark-v13.md` | Theme file |
| `themes/light-v13.md` | Theme file |
| `themes/high-contrast-v13.md` | Theme file |
| `tests/unit/test_design_system.cpp` | Test file |
| `docs/v13_docs/MIGRATION_DESIGN_SYSTEM.md` | Documentation |

## Files Modified

| File | Change |
|------|--------|
| `src/core/ThemeEngine.h` | Add ColorPaletteGenerator member |
| `src/core/ThemeEngine.cpp` | Integrate palette generation |
| `src/core/Events.h` | Add 3 design system events |
| `src/ui/LayoutMetrics.h` | Add delegation to ComponentSizeResolver |
| `src/ui/LayoutMetrics.cpp` | Replace hard-coded values |
| `src/ui/ActivityBar.h` / `.cpp` | Use design system |
| `src/ui/StatusBarPanel.h` / `.cpp` | Use design system |
| `src/ui/TabBar.h` / `.cpp` | Use design system |
| `src/ui/Toolbar.h` / `.cpp` | Use design system |
| `src/ui/BreadcrumbBar.h` / `.cpp` | Use design system |
| `src/ui/SplitterBar.h` / `.cpp` | Use design system |
| `src/ui/MainFrame.cpp` | Register density command |
| `src/app/MarkAmpApp.cpp` | Load density on startup |
| `CMakeLists.txt` | Add new sources and test target |
