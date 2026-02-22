# Phase 03 -- Theme Engine V2 Semantic Token Architecture

## Objective

Evolve the existing `ThemeEngine` from a flat color token system into a hierarchical, scoped token architecture where every UI surface has its own semantic token namespace (e.g., `editor.background`, `sidebar.background`, `activityBar.activeBorder`). This enables VS Code-compatible theme definitions, extension-contributed theme customization points, CSS-variable-like token inheritance, and built-in accessibility validation tools (contrast ratio checking, color blindness simulation).

## Prerequisites

- Phase 01 (Design System Foundation) -- for `ColorUtils` (HSL conversions, contrast ratio), `ColorPaletteGenerator`, `DesignTokenRegistry`.
- Phase 02 (Icon Library) -- icon colors reference theme tokens.

## Deliverables

- `ScopedTokenMap` -- hierarchical token storage with dot-notation keys.
- `ThemeContributionPoint` -- extension API for registering custom tokens.
- `TokenInheritanceResolver` -- CSS-variable-like fallback chain resolution.
- `ContrastValidator` -- WCAG AA/AAA validation for all fg/bg token pairs.
- `ColorBlindnessSimulator` -- simulate protanopia, deuteranopia, tritanopia on any theme.
- `ThemePreviewPanel` -- live preview of theme changes.
- `ThemeEditorPanel` -- in-app theme token editor.
- Catch2 test target: `test_theme_engine_v2`.

## Estimated Complexity

Very High -- redesigns the core theme system while maintaining backward compatibility with all existing token consumers.

---

## Tasks

### Task 1: Define Scoped Token Naming Convention

**Description:** Establish the hierarchical token naming scheme that mirrors VS Code's `workbench.colorCustomizations` structure. Define the full taxonomy of scoped token names across all UI surfaces.

**Key Implementation Details:**
- Naming pattern: `surface.property` or `surface.element.property`.
- Surface scopes:
  - `editor.*` -- editor background, foreground, line highlight, selection, caret
  - `editorGutter.*` -- gutter background, error/warning/info markers
  - `sidebar.*` -- sidebar background, foreground, border
  - `activityBar.*` -- background, foreground, badge, active border
  - `statusBar.*` -- background, foreground, debugging, noFolder
  - `tab.*` -- active/inactive bg/fg, border, modified indicator
  - `panel.*` -- bottom panel background, foreground, border
  - `titleBar.*` -- active/inactive background, foreground
  - `breadcrumb.*` -- foreground, focus foreground, separator
  - `notification.*` -- background, foreground, border by level
  - `input.*` -- background, foreground, border, placeholder
  - `button.*` -- background, foreground, hover, secondary
  - `list.*` -- active/inactive selection, hover, focus
  - `tree.*` -- indent guides, expand/collapse handles
  - `scrollbar.*` -- track, thumb, hover
  - `minimap.*` -- background, selection highlight
  - `diffEditor.*` -- inserted/removed background
- Define as an enum class `ScopedToken` with string conversion.
- Provide `auto token_name(ScopedToken t) -> std::string` and `auto parse_token(const std::string& name) -> std::optional<ScopedToken>`.
- Total: ~120 scoped tokens.

**Files Affected:**
- `src/core/ScopedToken.h` (new)
- `src/core/ScopedToken.cpp` (new)

**Acceptance Criteria:**
- All 120+ tokens have unique string representations.
- Round-trip: `parse_token(token_name(t))` returns `t`.
- Token names follow the dot-notation convention.

**Dependencies:** None.

---

### Task 2: Implement ScopedTokenMap

**Description:** Create a storage class that maps scoped token names to color values, supporting hierarchical lookup with fallback chains.

**Key Implementation Details:**
- Class: `ScopedTokenMap` in `src/core/ScopedTokenMap.h`
- Internal storage: `std::unordered_map<std::string, wxColour>` for explicit overrides.
- Method: `void set(const std::string& token_name, const wxColour& color)`.
- Method: `auto get(const std::string& token_name) const -> std::optional<wxColour>` -- direct lookup only.
- Method: `auto resolve(const std::string& token_name) const -> wxColour` -- lookup with fallback chain.
- Method: `void set_fallback(const std::string& token_name, const std::string& fallback_token)` -- define inheritance.
- Method: `auto all_tokens() const -> std::vector<std::pair<std::string, wxColour>>`.
- Method: `void clear()`.
- Method: `auto merge(const ScopedTokenMap& overlay) -> void` -- overlay overrides base.

**Files Affected:**
- `src/core/ScopedTokenMap.h` (new)
- `src/core/ScopedTokenMap.cpp` (new)

**Acceptance Criteria:**
- `set("editor.background", blue); get("editor.background")` returns blue.
- `set_fallback("panel.background", "editor.background"); resolve("panel.background")` returns editor.background value when panel.background is not set.
- Circular fallback chains are detected and return a default color (black).

**Dependencies:** None.

---

### Task 3: Implement Token Inheritance Resolver

**Description:** Build the fallback chain resolution engine that mimics CSS custom property inheritance. When a scoped token has no explicit value, it walks the inheritance chain until a value is found.

**Key Implementation Details:**
- Class: `TokenInheritanceResolver` in `src/core/TokenInheritanceResolver.h`
- Stores a directed acyclic graph of fallback relationships.
- Method: `void define_fallback(const std::string& token, const std::string& fallback)`.
- Method: `auto resolve(const std::string& token, const ScopedTokenMap& values) const -> wxColour`.
- Cycle detection: if a chain exceeds 10 levels of indirection, log an error and return the base app background.
- Default inheritance chains (built-in):
  - `sidebar.background` falls back to `editor.background`.
  - `panel.background` falls back to `editor.background`.
  - `activityBar.background` falls back to `editor.background`.
  - `tab.activeBackground` falls back to `editor.background`.
  - `statusBar.background` falls back to `editor.background`.
  - All `*.foreground` tokens fall back to `editor.foreground`.

**Files Affected:**
- `src/core/TokenInheritanceResolver.h` (new)
- `src/core/TokenInheritanceResolver.cpp` (new)

**Acceptance Criteria:**
- Setting only `editor.background` and resolving `sidebar.background` returns the editor background.
- Explicitly setting `sidebar.background` overrides the fallback.
- A 10-level circular chain returns the default color without crashing.

**Dependencies:** Task 2.

---

### Task 4: Bridge Scoped Tokens to Existing ThemeColorToken Enum

**Description:** Create a mapping layer that connects the new scoped token names to the existing `ThemeColorToken` enum, ensuring backward compatibility. Existing code that uses `ThemeEngine::color(ThemeColorToken::SidebarBg)` continues to work, but the value now comes from the scoped token map.

**Key Implementation Details:**
- Create a bidirectional mapping: `ThemeColorToken <-> std::string (scoped token name)`.
  - `ThemeColorToken::SidebarBg` <-> `"sidebar.background"`
  - `ThemeColorToken::ActivityBarBg` <-> `"activityBar.background"`
  - `ThemeColorToken::TabActiveBg` <-> `"tab.activeBackground"`
  - etc. for all existing tokens.
- In `ThemeEngine::rebuild_cache()`:
  1. First, populate the `ScopedTokenMap` from the theme file.
  2. Then, for each `ThemeColorToken`, resolve the corresponding scoped token.
  3. Store the resolved value in the existing `flat_colours_` array.
- New themes can define either scoped names or old token names; the bridge handles both.

**Files Affected:**
- `src/core/ThemeEngine.h` (modify -- add `ScopedTokenMap` and `TokenInheritanceResolver` members)
- `src/core/ThemeEngine.cpp` (modify -- populate scoped map in rebuild_cache)
- `src/core/ScopedTokenBridge.h` (new -- mapping table)
- `src/core/ScopedTokenBridge.cpp` (new)

**Acceptance Criteria:**
- Existing themes (using `ThemeColorToken` names) continue to work unchanged.
- New themes (using scoped dot-notation names) are correctly resolved.
- `ThemeEngine::color(ThemeColorToken::SidebarBg)` returns the same value as `scoped_map.resolve("sidebar.background")`.

**Dependencies:** Tasks 2, 3.

---

### Task 5: Implement Theme Contribution Points for Extensions

**Description:** Allow extensions to register new scoped tokens that they own. This enables extension-contributed UI panels to have themeable colors without modifying the core token list.

**Key Implementation Details:**
- Struct: `ThemeContributionPoint { std::string token_name; wxColour default_dark; wxColour default_light; std::string description; std::string fallback_token; }`.
- Class: `ThemeContributionRegistry` in `src/core/ThemeContributionRegistry.h`
- Method: `void register_contribution(const ThemeContributionPoint& point)`.
- Method: `auto all_contributions() const -> std::vector<ThemeContributionPoint>`.
- Method: `auto get_default(const std::string& token_name, bool dark_mode) const -> std::optional<wxColour>`.
- When resolving tokens, check contribution registry after the theme file and before the inheritance fallback.
- Extensions register contributions during their `activate()` lifecycle hook.

**Files Affected:**
- `src/core/ThemeContributionRegistry.h` (new)
- `src/core/ThemeContributionRegistry.cpp` (new)
- `src/core/ThemeEngine.cpp` (modify -- consult registry during resolution)

**Acceptance Criteria:**
- An extension registers `"myExtension.panel.background"` with dark default #2D2D2D.
- In a dark theme, `resolve("myExtension.panel.background")` returns #2D2D2D.
- A user theme that explicitly sets `"myExtension.panel.background"` overrides the default.

**Dependencies:** Tasks 2, 3, 4.

---

### Task 6: Implement Contrast Ratio Validator

**Description:** Build a validator that checks all foreground/background token pairs in a theme against WCAG 2.1 contrast ratio requirements. Reports which pairs fail AA (4.5:1) or AAA (7:1) levels.

**Key Implementation Details:**
- Class: `ContrastValidator` in `src/ui/ContrastValidator.h`
- Struct: `ContrastViolation { std::string fg_token; std::string bg_token; float ratio; std::string level; }`.
- Method: `auto validate(const ScopedTokenMap& tokens, const TokenInheritanceResolver& resolver) const -> std::vector<ContrastViolation>`.
- Internal: knows which tokens are foreground/background pairs:
  - `("editor.foreground", "editor.background")`
  - `("sidebar.foreground", "sidebar.background")`
  - `("activityBar.foreground", "activityBar.background")`
  - `("statusBar.foreground", "statusBar.background")`
  - `("tab.activeForeground", "tab.activeBackground")`
  - `("breadcrumb.foreground", "sidebar.background")`
  - etc. (~30 pairs)
- Uses `ColorUtils::contrast_ratio()` from Phase 01.

**Files Affected:**
- `src/ui/ContrastValidator.h` (new)
- `src/ui/ContrastValidator.cpp` (new)

**Acceptance Criteria:**
- Validating the high-contrast theme returns zero violations.
- Validating a badly-configured theme with gray-on-gray text returns violations.
- Each violation includes the numeric ratio and which level it fails (AA, AAA, or both).

**Dependencies:** Phase 01 Task 11 (ColorUtils).

---

### Task 7: Implement Color Blindness Simulator

**Description:** Build a simulator that transforms a theme's color palette to show how it appears under common color vision deficiencies. This helps theme authors ensure their themes are accessible.

**Key Implementation Details:**
- Class: `ColorBlindnessSimulator` in `src/ui/ColorBlindnessSimulator.h`
- Enum: `ColorVisionType { kNormal, kProtanopia, kDeuteranopia, kTritanopia, kAchromatopsia }`.
- Method: `auto simulate(const wxColour& color, ColorVisionType type) const -> wxColour`.
- Method: `auto simulate_theme(const ScopedTokenMap& tokens, ColorVisionType type) const -> ScopedTokenMap` -- transforms all colors.
- Uses Brettel/Machado simulation matrices for physiologically accurate transforms.
- Protanopia matrix (red-blind): standard 3x3 LMS transform.
- Deuteranopia matrix (green-blind): standard 3x3 LMS transform.
- Tritanopia matrix (blue-blind): standard 3x3 LMS transform.

**Files Affected:**
- `src/ui/ColorBlindnessSimulator.h` (new)
- `src/ui/ColorBlindnessSimulator.cpp` (new)

**Acceptance Criteria:**
- `simulate(pure_red, kProtanopia)` returns a desaturated/shifted color (not pure red).
- `simulate(color, kNormal)` returns the original color.
- `simulate_theme` transforms all tokens in the map.

**Dependencies:** Phase 01 Task 11 (ColorUtils).

---

### Task 8: Theme File Format V2

**Description:** Extend the YAML frontmatter theme file format to support scoped tokens, contribution overrides, and metadata about the theme's base type and contrast level.

**Key Implementation Details:**
- New YAML keys in theme frontmatter:
  - `base: dark | light | high-contrast`
  - `scoped_colors:` -- maps scoped token names to hex values.
  - `token_overrides:` -- maps `ThemeColorToken` names to hex values (backward compat).
  - `contribution_overrides:` -- maps extension-contributed token names to hex values.
  - `contrast_level: normal | high`
  - `author:`, `version:`, `description:` metadata fields.
- Parser: extend existing theme parsing in `ThemeRegistry` to read the new keys.
- Fallback: if a theme only has `token_overrides`, it works exactly as before.

**Files Affected:**
- `src/core/ThemeRegistry.h` (modify -- add scoped color parsing)
- `src/core/ThemeRegistry.cpp` (modify -- parse new YAML keys)
- `src/core/Theme.h` (modify -- add `ScopedTokenMap` and metadata fields)

**Acceptance Criteria:**
- A V2 theme file with `scoped_colors` loads correctly.
- A V1 theme file (no `scoped_colors`) still loads correctly.
- `Theme::base` correctly identifies dark/light/high-contrast.

**Dependencies:** Tasks 1, 2.

---

### Task 9: Dark/Light/High-Contrast Base Theme Templates

**Description:** Create three fully-specified base theme templates using the V2 format with all ~120 scoped tokens explicitly defined. These serve as the canonical starting point for all custom themes.

**Key Implementation Details:**
- `themes/v2-dark.md`: Catppuccin-Mocha-inspired dark palette with all scoped tokens.
- `themes/v2-light.md`: Catppuccin-Latte-inspired light palette.
- `themes/v2-high-contrast.md`: Pure black/white/yellow with maximum contrast.
- Each file defines all scoped tokens from Task 1.
- Validated against `ContrastValidator` during development -- high-contrast must pass AAA; dark and light must pass AA.

**Files Affected:**
- `themes/v2-dark.md` (new)
- `themes/v2-light.md` (new)
- `themes/v2-high-contrast.md` (new)

**Acceptance Criteria:**
- All three themes load without errors.
- `ContrastValidator::validate()` returns no AA violations for dark/light themes.
- High-contrast theme returns no AAA violations.

**Dependencies:** Tasks 1, 6, 8.

---

### Task 10: Theme Preview Mode

**Description:** Implement a non-destructive preview mode that temporarily applies a theme so the user can see how it looks before committing. Canceling the preview reverts to the previous theme. Enhance the existing `ThemeEngine::preview_theme()` to work with V2 scoped tokens.

**Key Implementation Details:**
- The existing `ThemeEngine::preview_theme()` and `cancel_preview()` already exist but only work with the old token system.
- Extend to also swap the `ScopedTokenMap` during preview.
- Store both the old `ScopedTokenMap` and old `Theme` on the undo stack.
- Add a visual indicator that preview mode is active (e.g., a dismissable banner at the top of the editor area).
- Publish `ThemePreviewStarted` and `ThemePreviewEnded` events.
- Auto-cancel preview after 30 seconds if the user does not commit.

**Files Affected:**
- `src/core/ThemeEngine.h` (modify -- extend preview to include ScopedTokenMap)
- `src/core/ThemeEngine.cpp` (modify)
- `src/core/Events.h` (modify -- add preview events)

**Acceptance Criteria:**
- Entering preview mode changes all UI colors immediately.
- Canceling preview restores all colors to the previous state.
- Auto-cancel fires after the timeout.

**Dependencies:** Tasks 4, 8.

---

### Task 11: Theme Editor Panel -- Token Browser

**Description:** Create an in-app panel that displays all scoped tokens with their current resolved colors, organized by surface group. Users can browse tokens and see which are explicitly set vs. inherited.

**Key Implementation Details:**
- Class: `ThemeTokenBrowser` in `src/ui/ThemeTokenBrowser.h`
- Groups tokens by surface prefix (editor, sidebar, activityBar, etc.).
- Each token row shows: token name, color swatch, hex value, inheritance source (if inherited).
- Search/filter by token name.
- Click a token to see its full inheritance chain.
- Uses `ScopedTokenMap::all_tokens()` and `TokenInheritanceResolver` for chain display.

**Files Affected:**
- `src/ui/ThemeTokenBrowser.h` (new)
- `src/ui/ThemeTokenBrowser.cpp` (new)

**Acceptance Criteria:**
- Panel displays all ~120 tokens grouped by surface.
- Inherited tokens show the source token name.
- Filtering works for partial name matches.

**Dependencies:** Tasks 1, 2, 3, 4.

---

### Task 12: Theme Editor Panel -- Color Picker

**Description:** Create an inline color picker that allows users to modify individual token values in real-time and see the changes applied live via preview mode.

**Key Implementation Details:**
- Class: `ThemeColorPicker` in `src/ui/ThemeColorPicker.h`
- HSL color wheel + saturation/lightness sliders.
- Hex input field for direct value entry.
- Opacity slider.
- Live preview: changes are applied via `ThemeEngine::preview_theme()` in real-time.
- "Apply" commits the change; "Cancel" reverts.
- Shows contrast ratio against the paired background/foreground token.

**Files Affected:**
- `src/ui/ThemeColorPicker.h` (new)
- `src/ui/ThemeColorPicker.cpp` (new)

**Acceptance Criteria:**
- Dragging the color wheel changes the previewed color in real-time.
- Contrast ratio updates as the color changes.
- "Cancel" reverts to the original color.

**Dependencies:** Tasks 10, 11.

---

### Task 13: Theme Editor Panel -- Export Modified Theme

**Description:** Allow users to export their modified token values as a new V2 theme file. Only the explicitly changed tokens are saved (not the inherited defaults), keeping theme files minimal.

**Key Implementation Details:**
- Method on `ThemeTokenBrowser`: `auto export_modifications() const -> std::string` -- generates YAML frontmatter.
- Only includes tokens that differ from the base theme's resolved values.
- Prompts for theme name, author, and base type.
- Saves to the user's themes directory.
- Registers the new theme in `ThemeRegistry`.

**Files Affected:**
- `src/ui/ThemeTokenBrowser.cpp` (modify -- add export functionality)
- `src/core/ThemeRegistry.cpp` (modify -- add registration of user themes)

**Acceptance Criteria:**
- Modifying 3 tokens and exporting produces a theme file with only those 3 overrides.
- The exported theme loads correctly and shows the modifications.
- Base theme type is correctly set in the exported file.

**Dependencies:** Tasks 8, 11, 12.

---

### Task 14: Theme Import from VS Code JSON

**Description:** Implement an importer that reads VS Code `.json` theme files (the `workbench.colorCustomizations` format) and converts them to MarkAmp V2 theme files.

**Key Implementation Details:**
- Class: `VSCodeThemeImporter` in `src/core/VSCodeThemeImporter.h`
- Method: `auto import_theme(const std::filesystem::path& json_path) -> std::optional<Theme>`.
- Maps VS Code token names to MarkAmp scoped token names:
  - `"editor.background"` -> `"editor.background"` (same)
  - `"sideBar.background"` -> `"sidebar.background"`
  - `"activityBar.background"` -> `"activityBar.background"`
  - etc.
- Handles VS Code `tokenColors` array for syntax highlighting.
- Unmapped VS Code tokens are logged as warnings but not discarded (stored as raw overrides).
- Supports both `workbench.colorCustomizations` and full theme JSON formats.

**Files Affected:**
- `src/core/VSCodeThemeImporter.h` (new)
- `src/core/VSCodeThemeImporter.cpp` (new)

**Acceptance Criteria:**
- Importing the VS Code "One Dark Pro" JSON produces a loadable MarkAmp theme.
- Background, foreground, and accent colors are correctly mapped.
- Unmapped tokens produce warnings but do not cause import failure.

**Dependencies:** Tasks 1, 8.

---

### Task 15: Contrast Validation UI

**Description:** Integrate the contrast validator into the theme editor, showing real-time accessibility feedback as the user edits tokens.

**Key Implementation Details:**
- In `ThemeTokenBrowser`, add a "Validate Accessibility" button.
- Shows a scrollable list of `ContrastViolation` entries with severity indicators.
- Each violation shows the fg/bg pair, their color swatches, the ratio, and the target (AA/AAA).
- Clicking a violation navigates to the offending token in the browser.
- A summary badge shows total violations count.
- Optionally, show inline warnings next to individual tokens that participate in violations.

**Files Affected:**
- `src/ui/ThemeTokenBrowser.h` (modify -- add validation UI)
- `src/ui/ThemeTokenBrowser.cpp` (modify)

**Acceptance Criteria:**
- Clicking "Validate" shows all violations.
- High-contrast theme shows zero violations.
- Modifying a color that fixes a violation removes it from the list.

**Dependencies:** Tasks 6, 11, 12.

---

### Task 16: Color Blindness Simulation UI

**Description:** Add a dropdown in the theme editor to preview how the entire UI looks under different color vision deficiencies.

**Key Implementation Details:**
- In `ThemeTokenBrowser`, add a "Simulate" dropdown with options: Normal, Protanopia, Deuteranopia, Tritanopia, Achromatopsia.
- Selecting an option transforms all tokens via `ColorBlindnessSimulator::simulate_theme()`.
- Applied as a preview (non-destructive; switching back to Normal reverts).
- Shows a banner: "Simulating: Protanopia" to remind the user.

**Files Affected:**
- `src/ui/ThemeTokenBrowser.cpp` (modify -- add simulation dropdown and logic)

**Acceptance Criteria:**
- Selecting Protanopia visually transforms the UI colors.
- Selecting Normal restores original colors.
- Simulation can be combined with contrast validation.

**Dependencies:** Tasks 7, 10, 11.

---

### Task 17: Per-Surface Token Application in LayoutManager

**Description:** Update `LayoutManager` to apply scoped tokens to each zone rather than a single global theme. Each child panel gets its correct surface-specific background/foreground from the scoped token map.

**Key Implementation Details:**
- `sidebar_panel_` background: `resolve("sidebar.background")`.
- `content_panel_` background: `resolve("editor.background")`.
- `statusbar_panel_` background: `resolve("statusBar.background")`.
- `bottom_panel_notebook_` background: `resolve("panel.background")`.
- On theme change, re-apply surface-specific colors to each zone.
- Use the `TokenInheritanceResolver` so unset surface tokens fall back gracefully.

**Files Affected:**
- `src/ui/LayoutManager.cpp` (modify -- apply per-surface tokens in CreateLayout and OnThemeChanged)

**Acceptance Criteria:**
- Sidebar, editor, panel, and status bar can each have different background colors.
- A theme that only sets `editor.background` still colors all surfaces correctly via fallback.
- Changing only `sidebar.background` does not affect the editor background.

**Dependencies:** Tasks 2, 3, 4.

---

### Task 18: Per-Surface Token Application in ActivityBar

**Description:** Update `ActivityBar` to use scoped tokens for all its color decisions.

**Key Implementation Details:**
- Background: `resolve("activityBar.background")`.
- Icon foreground: `resolve("activityBar.foreground")`.
- Active icon indicator: `resolve("activityBar.activeBorder")`.
- Badge background: `resolve("activityBar.badge.background")`.
- Badge foreground: `resolve("activityBar.badge.foreground")`.

**Files Affected:**
- `src/ui/ActivityBar.cpp` (modify -- replace ThemeColorToken usage with scoped resolution)

**Acceptance Criteria:**
- Activity bar colors change independently when its scoped tokens are customized.
- Fallback to editor colors works when activity bar tokens are not set.

**Dependencies:** Tasks 4, 17.

---

### Task 19: Per-Surface Token Application in TabBar

**Description:** Update `TabBar` to use scoped tokens.

**Key Implementation Details:**
- Active tab background: `resolve("tab.activeBackground")`.
- Inactive tab background: `resolve("tab.inactiveBackground")`.
- Active tab foreground: `resolve("tab.activeForeground")`.
- Inactive tab foreground: `resolve("tab.inactiveForeground")`.
- Modified indicator: `resolve("tab.modifiedBorder")`.
- Tab border: `resolve("tab.border")`.

**Files Affected:**
- `src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Active and inactive tabs can have distinct backgrounds.
- Modified indicator color is theme-controlled.

**Dependencies:** Tasks 4, 17.

---

### Task 20: Per-Surface Token Application in StatusBarPanel

**Description:** Update `StatusBarPanel` to use scoped tokens.

**Key Implementation Details:**
- Background: `resolve("statusBar.background")`.
- Foreground: `resolve("statusBar.foreground")`.
- Debugging mode background: `resolve("statusBar.debuggingBackground")`.
- No folder mode: `resolve("statusBar.noFolderBackground")`.
- Clickable item hover: `resolve("statusBar.prominentHoverBackground")`.

**Files Affected:**
- `src/ui/StatusBarPanel.cpp` (modify)

**Acceptance Criteria:**
- Status bar background changes when debugging mode is active (future integration point).
- Hover state on clickable items uses the correct scoped token.

**Dependencies:** Tasks 4, 17.

---

### Task 21: Per-Surface Token Application in CustomChrome

**Description:** Update `CustomChrome` to use scoped tokens instead of its ad-hoc `chromeBg()`, `textMain()` helper methods.

**Key Implementation Details:**
- Background: `resolve("titleBar.activeBackground")`.
- Foreground: `resolve("titleBar.activeForeground")`.
- Inactive background: `resolve("titleBar.inactiveBackground")`.
- Inactive foreground: `resolve("titleBar.inactiveForeground")`.
- Button hover: `resolve("titleBar.buttonHoverBackground")`.
- Remove the `chromeBg()`, `textMain()`, etc. private helper methods.

**Files Affected:**
- `src/ui/CustomChrome.h` (modify -- remove ad-hoc color helpers)
- `src/ui/CustomChrome.cpp` (modify -- use scoped tokens)

**Acceptance Criteria:**
- Title bar colors are fully theme-controlled via scoped tokens.
- Window focus/unfocus transitions use active/inactive tokens.

**Dependencies:** Tasks 4, 17.

---

### Task 22: Per-Surface Token Application in Bottom Panel

**Description:** Update the bottom panel (Output, Problems, Walkthrough) to use scoped tokens.

**Key Implementation Details:**
- Panel background: `resolve("panel.background")`.
- Panel foreground: `resolve("panel.foreground")`.
- Panel border: `resolve("panel.border")`.
- Panel title foreground: `resolve("panelTitle.activeForeground")`.
- Panel title inactive: `resolve("panelTitle.inactiveForeground")`.
- Panel title border: `resolve("panelTitle.activeBorder")`.

**Files Affected:**
- `src/ui/OutputPanel.cpp` (modify)
- `src/ui/ProblemsPanel.cpp` (modify)
- `src/ui/WalkthroughPanel.cpp` (modify)
- `src/ui/LayoutManager.cpp` (modify -- apply panel tokens to notebook)

**Acceptance Criteria:**
- Bottom panel has independent background color from editor.
- Panel tab active/inactive states use correct scoped tokens.

**Dependencies:** Tasks 4, 17.

---

### Task 23: Scoped Syntax Token Resolution

**Description:** Extend the existing `ThemeScopeMapper` to use the new V2 format's `tokenColors` array, enabling fine-grained syntax highlighting customization per-token-scope (matching VS Code's TextMate scope resolution).

**Key Implementation Details:**
- In `ThemeEngine::populate_scope_mapper()`, also process the V2 theme's `syntax_scopes` section.
- Each entry: `{ scope: "keyword.control", settings: { foreground: "#FF79C6", fontStyle: "bold" } }`.
- Support multi-scope selectors: `"keyword.control, keyword.operator"`.
- Priority: more specific scopes override less specific ones.
- Integrate with `ScopedTokenMap` so syntax tokens like `"syntax.keyword"` can be queried alongside UI tokens.

**Files Affected:**
- `src/core/ThemeScopeMapper.h` (modify)
- `src/core/ThemeScopeMapper.cpp` (modify)
- `src/core/ThemeEngine.cpp` (modify)

**Acceptance Criteria:**
- A V2 theme with `syntax_scopes` correctly colors keywords, strings, comments.
- A more specific scope (e.g., `"keyword.control.python"`) overrides a general one (`"keyword"`).
- Existing V1 syntax tokens still work.

**Dependencies:** Tasks 4, 8.

---

### Task 24: Command Palette Integration

**Description:** Add theme management commands to the Command Palette for quick access.

**Key Implementation Details:**
- Commands:
  - "Preferences: Color Theme" -- opens theme picker with live preview.
  - "Preferences: Open Theme Editor" -- opens the ThemeTokenBrowser panel.
  - "Developer: Validate Theme Contrast" -- runs ContrastValidator and shows results.
  - "Developer: Simulate Color Blindness" -- opens simulation mode picker.
  - "Preferences: Import VS Code Theme" -- opens file picker for JSON import.
- Each command registered in `MainFrame::RegisterPaletteCommands()`.

**Files Affected:**
- `src/ui/MainFrame.cpp` (modify -- register commands)

**Acceptance Criteria:**
- Each command executes the expected action.
- "Color Theme" command shows a filterable list of available themes with live preview on focus.

**Dependencies:** Tasks 10, 11, 14, 15, 16.

---

### Task 25: Catch2 Test Suite for Theme Engine V2

**Description:** Write comprehensive unit tests for all V2 theme engine components.

**Key Implementation Details:**
- Test file: `tests/unit/test_theme_engine_v2.cpp`
- Test target: `test_theme_engine_v2` in CMakeLists.txt
- Test sections:
  - `ScopedToken`: round-trip name/parse, all tokens have names.
  - `ScopedTokenMap`: set/get, merge, all_tokens.
  - `TokenInheritanceResolver`: single fallback, chain fallback, cycle detection.
  - `ScopedTokenBridge`: mapping completeness (every ThemeColorToken has a scoped name).
  - `ContrastValidator`: known violations detected, compliant pairs pass.
  - `ColorBlindnessSimulator`: normal is identity, protanopia shifts red.
  - `ThemeContributionRegistry`: register, get_default, merge with theme.
  - `VSCodeThemeImporter`: valid JSON import, invalid JSON graceful failure.
  - Theme file V2 parsing.

**Files Affected:**
- `tests/unit/test_theme_engine_v2.cpp` (new)
- `CMakeLists.txt` (modify -- add test target)

**Acceptance Criteria:**
- All tests pass.
- At least 45 test assertions.
- No dependency on wxWidgets display initialization.

**Dependencies:** All previous tasks.

---

## Dependency Graph

```
Task 1 (ScopedToken names)
Task 2 (ScopedTokenMap)
Task 3 (TokenInheritanceResolver) ── depends on 2
Task 4 (Bridge to ThemeColorToken) ── depends on 2, 3
Task 5 (ThemeContributionPoints) ── depends on 2, 3
Task 6 (ContrastValidator) ── depends on Phase 01
Task 7 (ColorBlindnessSimulator) ── depends on Phase 01
Task 8 (Theme File V2) ── depends on 1, 2
Task 9 (Base themes) ── depends on 1, 6, 8
Task 10 (Preview mode) ── depends on 4, 8
Task 11 (Token browser) ── depends on 1, 2, 3, 4
Task 12 (Color picker) ── depends on 10, 11
Task 13 (Export) ── depends on 8, 11, 12
Task 14 (VS Code import) ── depends on 1, 8
Task 15 (Contrast UI) ── depends on 6, 11, 12
Task 16 (Color blindness UI) ── depends on 7, 10, 11
Task 17-22 (Per-surface application) ── depend on 4
Task 23 (Syntax scopes) ── depends on 4, 8
Task 24 (Commands) ── depends on 10, 11, 14, 15, 16
Task 25 (Tests) ── depends on all
```

## Files Created

| File | Type |
|------|------|
| `src/core/ScopedToken.h` / `.cpp` | Token naming |
| `src/core/ScopedTokenMap.h` / `.cpp` | Token storage |
| `src/core/TokenInheritanceResolver.h` / `.cpp` | Fallback chains |
| `src/core/ScopedTokenBridge.h` / `.cpp` | Backward compat bridge |
| `src/core/ThemeContributionRegistry.h` / `.cpp` | Extension tokens |
| `src/core/VSCodeThemeImporter.h` / `.cpp` | Import |
| `src/ui/ContrastValidator.h` / `.cpp` | WCAG validation |
| `src/ui/ColorBlindnessSimulator.h` / `.cpp` | CVD simulation |
| `src/ui/ThemeTokenBrowser.h` / `.cpp` | Editor panel |
| `src/ui/ThemeColorPicker.h` / `.cpp` | Color picker |
| `themes/v2-dark.md` | Theme file |
| `themes/v2-light.md` | Theme file |
| `themes/v2-high-contrast.md` | Theme file |
| `tests/unit/test_theme_engine_v2.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/core/ThemeEngine.h` / `.cpp` | ScopedTokenMap + resolver integration |
| `src/core/ThemeRegistry.h` / `.cpp` | V2 format parsing |
| `src/core/Theme.h` | Add ScopedTokenMap and metadata |
| `src/core/ThemeScopeMapper.h` / `.cpp` | V2 syntax scope support |
| `src/core/Events.h` | Preview events |
| `src/ui/LayoutManager.cpp` | Per-surface tokens |
| `src/ui/ActivityBar.cpp` | Scoped tokens |
| `src/ui/TabBar.cpp` | Scoped tokens |
| `src/ui/StatusBarPanel.cpp` | Scoped tokens |
| `src/ui/CustomChrome.h` / `.cpp` | Scoped tokens, remove ad-hoc helpers |
| `src/ui/OutputPanel.cpp` | Scoped tokens |
| `src/ui/ProblemsPanel.cpp` | Scoped tokens |
| `src/ui/MainFrame.cpp` | Register theme commands |
| `CMakeLists.txt` | New sources and test target |
