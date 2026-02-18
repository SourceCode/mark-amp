# Phase 03: Theme Engine Completion

## Overview
MarkAmp has 64 themes and a VsCodeThemeAdapter, but the ThemeEngine (370 lines) is thin relative to the UI surface area. Hard-coded colors persist in multiple UI files, the ThemeScopeMapper (156 lines) is minimal, and theme hot-swap does not reliably update all surfaces. This phase makes the theme system production-complete.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 02 (Config modernization for scoped theme preferences)

## Tasks

### Task 1: Audit and Eliminate All Hard-Coded Colors
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/TabBar.cpp`, `src/ui/Toolbar.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`, `src/ui/SettingsPanel.cpp`, `src/ui/NotificationManager.cpp`, `src/ui/PreviewPanel.cpp`, `src/rendering/MermaidBlockRenderer.cpp`, `src/ui/MainFrame.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Systematic search for all `wxColour(r,g,b)` or `wxColour("#...")` literals that are not sourced from ThemeEngine. Replace each with the appropriate semantic token from ThemeEngine. V8 Phase 1 identified these but implementation may be incomplete.
**Acceptance Criteria:**
- Zero hard-coded RGB color literals in any UI file (except print-only CSS)
- Every color sourced from `ThemeEngine::get_color(token)` or cached brush/pen/font
- Grep verification: `wxColour\(` with non-theme arguments returns zero results

### Task 2: Add Missing Semantic Tokens
**Files:** `src/core/Theme.h`, `src/core/ThemeEngine.h`, `src/core/ThemeEngine.cpp`
**Description:** Add semantic tokens missing from the current token set: `surface_raised`, `surface_overlay`, `focus_ring`, `interactive_hover`, `interactive_active`, `interactive_pressed`, `success`, `warning`, `error`, `info`, `editor_active_line`, `editor_gutter_marker_error/warn/info`, `editor_match_highlight`, `editor_find_hit`, `editor_quick_fix`.
**Acceptance Criteria:**
- All 15+ new tokens defined in Theme.h
- ThemeEngine resolves them from theme data with sensible fallbacks
- Built-in themes updated to provide values for all new tokens
- Test verifies all tokens resolve to non-default colors for each built-in theme

### Task 3: Complete Theme Hot-Swap for All Surfaces
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/ActivityBar.cpp`, `src/ui/StatusBarPanel.cpp`, `src/ui/TabBar.cpp`, `src/ui/CommandPalette.cpp`, `src/ui/CanvasWorkspacePanel.cpp`, `src/ui/GraphViewPanel.cpp`
**Description:** Every UI panel must subscribe to `ThemeChangedEvent` and refresh all visual elements. Currently sidebar header/footer, explorer states, and canvas workspace do not fully refresh. Add `OnThemeChanged` handlers to all surfaces.
**Acceptance Criteria:**
- Runtime theme switch updates every visible surface without stale colors
- No flash/flicker during theme transition
- Test: switch theme 10 times rapidly without crash or visual artifact

### Task 4: Expand VsCodeThemeAdapter Coverage
**Files:** `src/core/VsCodeThemeAdapter.h`, `src/core/VsCodeThemeAdapter.cpp`
**Description:** The VsCodeThemeAdapter (210 lines) maps a subset of VSCode theme tokens. Expand coverage to handle: `colors` (editor, sidebar, statusBar, activityBar, tab, notification, badge, minimap, list, menu, input, button, dropdown, scrollbar, selection), `tokenColors` (scope selectors with foreground/fontStyle), and `semanticTokenColors`.
**Acceptance Criteria:**
- VSCode `colors` section: 80%+ of common keys mapped to MarkAmp tokens
- `tokenColors`: TextMate scope selectors mapped to SyntaxHighlighter styles
- `semanticTokenColors`: semantic scopes override TextMate scopes when present
- Unmapped keys logged at DEBUG level with count summary

### Task 5: Add Theme Compatibility Scoring
**Files:** `src/core/VsCodeThemeAdapter.h`, `src/core/VsCodeThemeAdapter.cpp`
**Description:** After importing a VSCode theme, calculate a compatibility score: "full" (90%+), "mapped with fallbacks" (60-90%), "partial" (<60%). Emit `ThemeCompatibilityReportEvent`.
**Acceptance Criteria:**
- Score based on percentage of tokens that map without fallback
- Report includes: supported_tokens, total_tokens, unmapped_keys list
- Event emitted after every import
- Import UI can display the compatibility badge

### Task 6: Add Theme Auto-Repair (Contrast and Missing Tokens)
**Files:** `src/core/ThemeValidator.h`, `src/core/ThemeValidator.cpp`
**Description:** ThemeValidator (226 lines) does basic validation. Add auto-repair capability: fix WCAG contrast violations by adjusting lightness, fill missing tokens by deriving from existing ones (e.g., derive `surface_raised` from `surface` + 10% lighter).
**Acceptance Criteria:**
- `ThemeValidator::repair(theme)` returns a repaired theme with change log
- All text/background pairs meet WCAG AA contrast ratio (4.5:1)
- Missing tokens derived from nearest existing token with documented formula
- Repair is non-destructive (returns copy, not modifying original)

### Task 7: Add Theme Preview Before Apply
**Files:** `src/ui/ThemeGallery.cpp`, `src/ui/ThemePreviewCard.cpp`
**Description:** ThemeGallery should show a live mini-preview of each theme before the user applies it. Render a small card showing editor background, text, sidebar, and accent colors.
**Acceptance Criteria:**
- Each theme card shows a 200x120px preview with actual theme colors
- Preview includes: background, text, accent, sidebar, code block, heading
- Hovering a card shows a larger tooltip preview
- Currently-active theme is visually distinguished

### Task 8: Complete ThemeScopeMapper for Syntax Highlighting
**Files:** `src/core/ThemeScopeMapper.h`, `src/core/ThemeScopeMapper.cpp`
**Description:** ThemeScopeMapper (156 lines) handles basic scope-to-style mapping. Implement full TextMate scope matching priority: most specific scope wins, parent scopes cascade, font styles (bold, italic, underline) from rules are applied.
**Acceptance Criteria:**
- Scope matching follows TextMate priority rules (most specific selector wins)
- Font style flags (bold, italic, underline, strikethrough) are resolved
- Parent scope cascade works (e.g., `source.js` matches `source`)
- Test with real VSCode theme data verifies correct keyword/string/comment coloring

### Task 9: Add Extension-Contributed Theme Discovery
**Files:** `src/core/ExtensionScanner.cpp`, `src/core/ExtensionManagement.cpp`, `src/core/ThemeRegistry.cpp`
**Description:** When an extension is installed that has `contributes.themes`, automatically register those themes in ThemeRegistry. Currently extension themes are not auto-indexed.
**Acceptance Criteria:**
- Installing an extension with themes makes them appear in ThemeGallery
- Uninstalling removes them
- Theme source metadata distinguishes "built-in", "custom", "extension", "vscode-import"
- ThemeRegistry emits event when themes are added/removed

### Task 10: Add Theme Token Editor Live Preview
**Files:** `src/ui/ThemeTokenEditor.cpp`, `src/ui/ThemeTokenEditor.h`
**Description:** The ThemeTokenEditor exists but lacks live preview. Changes to token values should immediately update a preview pane showing how the token affects various UI surfaces.
**Acceptance Criteria:**
- Editing a token value shows real-time preview of affected UI elements
- Preview shows: editor background/text, sidebar, tabs, status bar, code block
- Changes are staged (not committed until user clicks "Apply")
- "Reset" reverts to the theme's original value

### Task 11: Add Theme Persistence Format Version
**Files:** `src/core/loader/ThemeLoader.h`, `src/core/loader/ThemeLoader.cpp`
**Description:** Theme files (YAML frontmatter in Markdown) need a version field for forward compatibility. Add a `theme_format_version` field and migration logic for older formats.
**Acceptance Criteria:**
- New theme files include `theme_format_version: 2`
- Loader detects version and applies migrations for older formats
- Version 1 (current implicit) is auto-migrated to version 2
- Unknown future versions produce a warning but load with best effort

### Task 12: Add Contrast-Safe Selected/Hover States
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/ui/ExtensionsBrowserPanel.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Selection and hover states in list views currently use `AccentPrimary.ChangeLightness(80)` which breaks contrast under some themes. Use theme semantic tokens for list_selected/list_hover with guaranteed contrast.
**Acceptance Criteria:**
- `list_selected_bg`, `list_selected_fg`, `list_hover_bg`, `list_hover_fg` tokens used
- All list views use these tokens consistently
- WCAG AA contrast maintained under all built-in themes
- Test verifies contrast ratios for each built-in theme

### Task 13: Add Theme Export to VSCode Format
**Files:** `src/core/VsCodeThemeAdapter.h`, `src/core/VsCodeThemeAdapter.cpp`
**Description:** Add the reverse mapping: export a MarkAmp theme to VSCode-compatible JSON format. This enables theme authors to share themes across both platforms.
**Acceptance Criteria:**
- `VsCodeThemeAdapter::export_to_vscode(theme)` produces valid VSCode theme JSON
- Round-trip: import VSCode theme, export back, reimport produces equivalent result
- Token colors and semantic highlighting included in export

### Task 14: Add Reduced-Motion Theme Variant
**Files:** `src/core/ThemeEngine.h`, `src/core/ThemeEngine.cpp`
**Description:** When the OS or user preference indicates reduced-motion, ThemeEngine should automatically suppress animation-related tokens (transition durations set to 0, glow/pulse effects disabled).
**Acceptance Criteria:**
- ThemeEngine detects OS reduced-motion preference
- Animation-related token values are overridden to static/zero
- User can also set this explicitly via Config
- Theme switch still works correctly in reduced-motion mode

### Task 15: Add Theme Performance Metrics
**Files:** `src/core/ThemeEngine.cpp`
**Description:** Track and log: theme load time, token resolution time (avg per query), cache hit rate for wxBrush/wxPen/wxFont objects, and theme switch total time. Expose via health panel.
**Acceptance Criteria:**
- Theme load time logged at INFO level
- Average token resolution time available via `ThemeEngine::stats()`
- Cache hit rate tracked and reported
- Theme switch total time (from event to all surfaces updated) measured

### Task 16: Add Theme Inheritance Chain
**Files:** `src/core/Theme.h`, `src/core/loader/ThemeLoader.cpp`
**Description:** Allow themes to inherit from a parent theme, only overriding specific tokens. This reduces theme file size and enables theme families (e.g., "Tokyo Night" base with "Tokyo Night Storm" variant).
**Acceptance Criteria:**
- Theme YAML frontmatter supports `extends: parent-theme-id`
- Child theme only needs to specify overridden tokens
- Inheritance chains resolve correctly (up to 3 levels deep)
- Circular inheritance detected and reported as error

### Task 17: Add FX Token Integration
**Files:** `src/core/Theme.h`, `src/rendering/FxEngine.h`
**Description:** V8 added extensive FX tokens (`window.blur_background`, `text.outer_glow`, etc.) in the Phase 13 docs. Wire these tokens into Theme.h so they are available from theme files and the FX engine reads them from the active theme.
**Acceptance Criteria:**
- All FX tokens from v8 Phase 13 defined in Theme.h
- FxEngine reads token values from ThemeEngine on startup and theme change
- Theme files can specify FX values in YAML frontmatter
- Missing FX tokens fall back to sensible defaults (effects disabled)

### Task 18: Add Theme Unit Test Corpus
**Files:** `tests/unit/test_theme_engine.cpp`, `tests/unit/test_vscode_theme_adapter.cpp`
**Description:** Create a test fixture corpus of 3-5 real VSCode themes (popular open-source themes). Test that import, mapping, contrast validation, and round-trip export all work correctly.
**Acceptance Criteria:**
- Test fixtures for: One Dark Pro, Dracula, Solarized Dark, Nord, GitHub Light
- Each fixture tests: import, token mapping coverage, contrast validation
- Round-trip test: import then export produces valid VSCode theme
- All fixtures pass without crash

### Task 19: Fix Theme Gallery Scrolling and Selection
**Files:** `src/ui/ThemeGallery.cpp`
**Description:** With 64+ themes, the gallery needs smooth scrolling, a search/filter bar, and clear active-theme indication. Add keyboard navigation (arrow keys to browse, Enter to apply, Escape to close).
**Acceptance Criteria:**
- Gallery scrolls smoothly with 64+ theme cards
- Search bar filters themes by name in real-time
- Currently active theme has a visible checkmark/border
- Full keyboard navigation: arrows, enter, escape
- Category filters: built-in, custom, extension, VSCode import

### Task 20: Add Theme Change Undo
**Files:** `src/core/ThemeEngine.cpp`
**Description:** When the user switches themes, store the previous theme ID so they can quickly revert. Add a "Previous Theme" command to command palette and a 5-second undo toast after theme switch.
**Acceptance Criteria:**
- `ThemeEngine::previous_theme_id()` returns the last active theme
- Command palette: "Revert to Previous Theme" command
- After theme switch, 5-second notification with "Undo" action
- Undo reverts to the previous theme

## Testing Requirements
- All 64 built-in themes load without errors
- Theme hot-swap test: rapid switching 20 times without visual artifacts
- VSCode import test: 5 real theme fixtures pass import/mapping
- Contrast validation: all built-in themes meet WCAG AA for text/background
- Theme export round-trip preserves fidelity

## Phase Completion Criteria
- Zero hard-coded colors in UI files
- All semantic tokens defined and used consistently
- Theme hot-swap works reliably for every surface
- VSCode theme import handles 80%+ of tokens
- Theme auto-repair fixes contrast issues
- All tests pass
