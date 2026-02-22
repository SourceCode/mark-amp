# Phase 02 -- Icon Library And Vector Rendering Pipeline

## Objective

Replace the current programmatic icon drawing (per-control `DrawCodeIcon`, `DrawGearIcon`, etc. methods using raw `wxGraphicsContext` calls) with a centralized SVG-based icon library and rendering pipeline. Every icon in the application -- activity bar, toolbar, file tree, status bar, panel headers, tabs, breadcrumbs -- will be drawn from a single icon registry that supports size variants, theme-aware colorization, caching, and high-DPI rendering.

## Prerequisites

- Phase 01 (Design System Foundation) -- for `ComponentSizeResolver` icon size queries, `ThemeColorToken` for icon colors, `DesignTokenRegistry` for token access.

## Deliverables

- `SvgParser` -- lightweight SVG path parser (subset: path, circle, rect, line, polyline).
- `IconRenderer` -- renders parsed SVG data to wxBitmap at arbitrary size and color.
- `IconRegistry` -- name-to-SVG-data mapping with lazy loading.
- `FileTypeIconResolver` -- maps file extensions to icon names.
- `IconCache` -- size+color+DPI keyed bitmap cache.
- Built-in icon set covering all current UI needs (80+ icons).
- Catch2 test target: `test_icon_library`.

## Estimated Complexity

High -- requires SVG path parsing, bitmap caching, and integration across all UI controls.

---

## Tasks

### Task 1: Define the SVG Path Data Model

**Description:** Create data structures representing a minimal SVG document sufficient for icon rendering. Support only the subset needed for icons: path elements (with d attribute), circles, rects, lines, and polylines.

**Key Implementation Details:**
- Namespace: `markamp::ui::svg`
- Struct `SvgPath { std::string d; }` -- SVG path data string.
- Struct `SvgCircle { float cx, cy, r; }`.
- Struct `SvgRect { float x, y, width, height, rx, ry; }`.
- Struct `SvgLine { float x1, y1, x2, y2; }`.
- Struct `SvgPolyline { std::vector<std::pair<float,float>> points; }`.
- Variant: `SvgElement = std::variant<SvgPath, SvgCircle, SvgRect, SvgLine, SvgPolyline>`.
- Struct `SvgDocument { float viewbox_width; float viewbox_height; std::vector<SvgElement> elements; float stroke_width; }`.

**Files Affected:**
- `src/ui/svg/SvgTypes.h` (new)

**Acceptance Criteria:**
- All structs are default-constructible and have value semantics.
- `SvgDocument` can hold a mix of element types.
- `stroke_width` defaults to 1.5.

**Dependencies:** None.

---

### Task 2: Implement the SVG Path Parser

**Description:** Parse SVG path `d` attribute strings into a sequence of drawing commands that `wxGraphicsPath` can execute. Support commands: M, L, H, V, C, S, Q, T, A, Z (both absolute and relative).

**Key Implementation Details:**
- Class: `SvgPathParser` in `src/ui/svg/SvgPathParser.h`
- Method: `auto parse(const std::string& d) -> std::vector<PathCommand>`.
- Enum `PathCommandType`: `kMoveTo`, `kLineTo`, `kHLineTo`, `kVLineTo`, `kCubicTo`, `kSmoothCubicTo`, `kQuadTo`, `kSmoothQuadTo`, `kArcTo`, `kClose`.
- Struct `PathCommand { PathCommandType type; bool relative; std::vector<float> params; }`.
- Handle multiple coordinate pairs per command (implicit repeat).
- Handle whitespace and comma-separated values.

**Files Affected:**
- `src/ui/svg/SvgPathParser.h` (new)
- `src/ui/svg/SvgPathParser.cpp` (new)

**Acceptance Criteria:**
- Parses `"M10 10 L20 20 Z"` into 3 commands.
- Parses `"M0,0C10,10,20,0,30,10"` correctly (comma-separated).
- Handles relative commands: `"m10 10 l5 5"`.
- Handles implicit LineTo after MoveTo: `"M0 0 10 10"` produces MoveTo + LineTo.

**Dependencies:** Task 1.

---

### Task 3: Implement the Minimal SVG Document Parser

**Description:** Parse a minimal SVG XML string into an `SvgDocument`. Only handle the elements needed for icons (path, circle, rect, line, polyline, svg root with viewBox).

**Key Implementation Details:**
- Class: `SvgDocumentParser` in `src/ui/svg/SvgDocumentParser.h`
- Method: `auto parse(const std::string& svg_xml) -> std::optional<SvgDocument>`.
- Use a lightweight XML parser (tinyxml2 or manual tokenizer for the subset needed).
- Extract viewBox from root `<svg>` element.
- Parse `<path d="..."/>`, `<circle cx="..." cy="..." r="..."/>`, etc.
- Ignore unsupported elements (gradients, filters, text, clipPath).
- Extract `stroke-width` from root or element style attributes.

**Files Affected:**
- `src/ui/svg/SvgDocumentParser.h` (new)
- `src/ui/svg/SvgDocumentParser.cpp` (new)

**Acceptance Criteria:**
- Parses a valid SVG string with a single path into `SvgDocument` with one element.
- Returns `std::nullopt` for malformed XML.
- Correctly extracts viewBox dimensions.

**Dependencies:** Tasks 1, 2.

---

### Task 4: Implement the Icon Renderer

**Description:** Render an `SvgDocument` to a `wxBitmap` at a specified target size and color. This is the core rendering engine for all icons.

**Key Implementation Details:**
- Class: `IconRenderer` in `src/ui/IconRenderer.h`
- Method: `auto render(const SvgDocument& doc, int target_size, const wxColour& color, float dpi_scale = 1.0) const -> wxBitmap`.
- Process:
  1. Create a `wxBitmap` at `target_size * dpi_scale` pixels.
  2. Create a `wxMemoryDC` and `wxGraphicsContext`.
  3. Compute scale factor: `target_size / doc.viewbox_width` (assuming square viewbox; handle non-square with aspect-fit).
  4. For each `SvgElement`:
     - `SvgPath`: convert `PathCommand` sequence to `wxGraphicsPath` calls.
     - `SvgCircle`: `gc->DrawEllipse(...)`.
     - `SvgRect`: `gc->DrawRoundedRectangle(...)` if rx/ry > 0.
     - `SvgLine`: `gc->StrokeLine(...)`.
     - `SvgPolyline`: `gc->StrokeLines(...)`.
  5. Apply `color` as both stroke and fill color (icons are monochrome).
  6. Set stroke width = `doc.stroke_width * scale`.
- Method: `auto render_to_dc(wxGraphicsContext& gc, const SvgDocument& doc, const wxRect& target, const wxColour& color) const -> void` -- render directly to an existing graphics context (for inline icon drawing without bitmap allocation).

**Files Affected:**
- `src/ui/IconRenderer.h` (new)
- `src/ui/IconRenderer.cpp` (new)

**Acceptance Criteria:**
- A 24x24 render of a simple path icon produces a non-empty bitmap.
- A 48x48 render at 2x DPI produces a 96x96 pixel bitmap.
- Rendering a circle SVG produces output centered in the bitmap.

**Dependencies:** Tasks 1, 2, 3.

---

### Task 5: Implement the Icon Cache

**Description:** Cache rendered icon bitmaps by a composite key of (icon_name, size, color_hash, dpi_scale) to avoid redundant rendering. Icons are re-rendered only on theme change (color changes) or DPI change.

**Key Implementation Details:**
- Class: `IconCache` in `src/ui/IconCache.h`
- Key struct: `IconCacheKey { std::string name; int size; uint32_t color_hash; int dpi_scale_x100; }` with custom hash and equality.
- Method: `auto get(const IconCacheKey& key) -> wxBitmap*` -- returns cached bitmap or nullptr.
- Method: `void put(const IconCacheKey& key, wxBitmap bitmap)`.
- Method: `void invalidate_all()` -- called on theme change.
- Method: `void invalidate_color(uint32_t old_color_hash)` -- selective invalidation.
- Method: `auto stats() const -> CacheStats { size_t entries; size_t memory_bytes; }`.
- Use `std::unordered_map<IconCacheKey, wxBitmap, KeyHash>`.
- Maximum cache size: 500 entries (LRU eviction).

**Files Affected:**
- `src/ui/IconCache.h` (new)
- `src/ui/IconCache.cpp` (new)

**Acceptance Criteria:**
- Storing and retrieving the same key returns the same bitmap pointer.
- `invalidate_all()` clears all entries.
- Cache does not exceed 500 entries after inserting 600.

**Dependencies:** None.

---

### Task 6: Build the Icon Registry

**Description:** Create the central registry that maps icon names (strings) to their SVG data. Icons can be registered programmatically or loaded from SVG files on disk. The registry is the single lookup point for the entire application.

**Key Implementation Details:**
- Class: `IconRegistry` in `src/ui/IconRegistry.h`
- Method: `void register_icon(const std::string& name, const SvgDocument& doc)`.
- Method: `void register_icon(const std::string& name, const std::string& svg_xml)` -- parses and registers.
- Method: `auto get(const std::string& name) const -> const SvgDocument*` -- returns nullptr if not found.
- Method: `auto has(const std::string& name) const -> bool`.
- Method: `auto all_names() const -> std::vector<std::string>`.
- Method: `void load_from_directory(const std::filesystem::path& dir)` -- loads all `.svg` files, using filename (without extension) as the icon name.
- Singleton accessor: `static auto get() -> IconRegistry&`.
- Store as `std::unordered_map<std::string, SvgDocument>`.

**Files Affected:**
- `src/ui/IconRegistry.h` (new)
- `src/ui/IconRegistry.cpp` (new)

**Acceptance Criteria:**
- Registering an icon by name and retrieving it returns the same SVG data.
- `load_from_directory` loads all SVG files from a test directory.
- `all_names()` returns the correct count after registration.

**Dependencies:** Tasks 1, 3.

---

### Task 7: Create the Convenience Icon Drawing API

**Description:** Build a high-level API that combines the registry, renderer, and cache into a single call: "draw icon X at this position with this size and color." This is the API that all UI controls will use.

**Key Implementation Details:**
- Class: `IconDrawer` in `src/ui/IconDrawer.h`
- Constructor: `IconDrawer(IconRegistry& registry, IconRenderer& renderer, IconCache& cache, core::ThemeEngine& theme)`.
- Method: `auto bitmap(const std::string& name, int size, const wxColour& color) -> wxBitmap` -- returns cached or freshly rendered bitmap.
- Method: `auto bitmap(const std::string& name, int size, core::ThemeColorToken color_token) -> wxBitmap` -- resolves color from theme.
- Method: `void draw(wxGraphicsContext& gc, const std::string& name, const wxRect& target, const wxColour& color)` -- draws directly to GC (cache-bypassing for dynamic cases).
- Method: `void draw(wxGraphicsContext& gc, const std::string& name, const wxRect& target, core::ThemeColorToken color_token)`.
- Method: `void invalidate_theme_cache()` -- called on theme change.
- Automatically detects DPI scale from the current display.

**Files Affected:**
- `src/ui/IconDrawer.h` (new)
- `src/ui/IconDrawer.cpp` (new)

**Acceptance Criteria:**
- Calling `bitmap("file-text", 16, wxColour(255,255,255))` twice returns the same cached bitmap.
- After `invalidate_theme_cache()`, the next call re-renders.
- `draw()` on a `wxGraphicsContext` produces visible output.

**Dependencies:** Tasks 4, 5, 6.

---

### Task 8: Define the Core Icon Set -- Activity Bar Icons

**Description:** Create SVG icon definitions for all activity bar items: Explorer, Search, Source Control, Run/Debug, Extensions, Graph, Settings, Account, plus existing MarkAmp-specific items (Notebooks, Canvas, AI, Flashcards, Git, Tasks, Database, Presentation).

**Key Implementation Details:**
- Create SVG files in `assets/icons/activity/` directory.
- Icons should be designed on a 24x24 viewbox with 1.5px stroke width.
- Style: outlined/linear (not filled), matching VS Code's Codicon style.
- Icon names: `"explorer"`, `"search"`, `"source-control"`, `"run-debug"`, `"extensions"`, `"graph"`, `"settings"`, `"account"`, `"notebooks"`, `"canvas"`, `"ai"`, `"flashcards"`, `"git"`, `"tasks"`, `"database"`, `"presentation"`.
- Register all in `IconRegistry` during app initialization.

**Files Affected:**
- `assets/icons/activity/*.svg` (16 new files)
- `src/app/MarkAmpApp.cpp` (modify -- register icons on startup)

**Acceptance Criteria:**
- All 16 icons render correctly at 24x24.
- All icons are visually distinguishable at 16x16.
- Each icon file is under 2KB.

**Dependencies:** Task 6.

---

### Task 9: Define the Core Icon Set -- Toolbar Icons

**Description:** Create SVG icons for all toolbar actions: Source view, Split view, Preview view, Save, Themes/Palette, Settings/Gear, Focus mode.

**Key Implementation Details:**
- Create SVG files in `assets/icons/toolbar/`.
- 24x24 viewbox, 1.5px stroke.
- Icon names: `"view-source"`, `"view-split"`, `"view-preview"`, `"save"`, `"palette"`, `"gear"`, `"focus-mode"`.
- These replace the existing `DrawCodeIcon`, `DrawColumnsIcon`, `DrawEyeIcon`, `DrawSaveIcon`, `DrawPaletteIcon`, `DrawGearIcon`, `DrawFocusIcon` methods.

**Files Affected:**
- `assets/icons/toolbar/*.svg` (7 new files)

**Acceptance Criteria:**
- Each icon matches the visual intent of the current programmatic drawing.
- Icons render crisply at 20x20 and 24x24.

**Dependencies:** Task 6.

---

### Task 10: Define the Core Icon Set -- File Type Icons

**Description:** Create SVG icons for common file types and a default file/folder icon. These will be used in the file tree, tab bar, and breadcrumb bar.

**Key Implementation Details:**
- Create SVG files in `assets/icons/filetypes/`.
- Icon names: `"file-default"`, `"file-markdown"`, `"file-json"`, `"file-yaml"`, `"file-html"`, `"file-css"`, `"file-js"`, `"file-ts"`, `"file-py"`, `"file-cpp"`, `"file-h"`, `"file-rust"`, `"file-go"`, `"file-java"`, `"file-image"`, `"file-svg"`, `"file-pdf"`, `"file-config"`, `"file-git"`, `"file-lock"`.
- Folder icons: `"folder-default"`, `"folder-open"`, `"folder-src"`, `"folder-docs"`, `"folder-test"`, `"folder-assets"`, `"folder-config"`.
- 16x16 viewbox for file type icons (used at small sizes).

**Files Affected:**
- `assets/icons/filetypes/*.svg` (27 new files)

**Acceptance Criteria:**
- File type icons are visually distinct by type at 16x16.
- Folder icons clearly read as folders at 16x16.
- Default icons are used when no specific match exists.

**Dependencies:** Task 6.

---

### Task 11: Implement FileTypeIconResolver

**Description:** Map file extensions to icon names. Given a filename or path, return the appropriate icon name for rendering in the file tree, tabs, and breadcrumbs.

**Key Implementation Details:**
- Class: `FileTypeIconResolver` in `src/ui/FileTypeIconResolver.h`
- Method: `auto resolve(const std::string& filename) const -> std::string` -- returns icon name.
- Method: `auto resolve_folder(const std::string& folder_name) const -> std::string` -- returns folder icon name.
- Internal maps:
  - Extension map: `.md` -> `"file-markdown"`, `.json` -> `"file-json"`, etc.
  - Special filename map: `"Makefile"` -> `"file-config"`, `.gitignore` -> `"file-git"`, etc.
  - Folder name map: `"src"` -> `"folder-src"`, `"docs"` -> `"folder-docs"`, etc.
- Default: `"file-default"` for unknown extensions, `"folder-default"` for unknown folders.

**Files Affected:**
- `src/ui/FileTypeIconResolver.h` (new)
- `src/ui/FileTypeIconResolver.cpp` (new)

**Acceptance Criteria:**
- `resolve("readme.md")` returns `"file-markdown"`.
- `resolve("unknown.xyz")` returns `"file-default"`.
- `resolve_folder("src")` returns `"folder-src"`.
- `resolve_folder("random")` returns `"folder-default"`.

**Dependencies:** None.

---

### Task 12: Define the Core Icon Set -- Panel and Action Icons

**Description:** Create SVG icons for panel headers, action buttons, and common UI affordances used across all panels.

**Key Implementation Details:**
- Create SVG files in `assets/icons/actions/`.
- Icons: `"close"`, `"close-all"`, `"add"`, `"remove"`, `"refresh"`, `"filter"`, `"sort-asc"`, `"sort-desc"`, `"collapse-all"`, `"expand-all"`, `"maximize"`, `"restore"`, `"chevron-right"`, `"chevron-down"`, `"chevron-left"`, `"chevron-up"`, `"ellipsis"`, `"pin"`, `"unpin"`, `"split-horizontal"`, `"split-vertical"`, `"lock"`, `"unlock"`, `"copy"`, `"search"`, `"clear"`, `"warning"`, `"error"`, `"info"`, `"success"`, `"terminal"`, `"output"`, `"problems"`, `"debug-console"`.
- 16x16 viewbox for small action icons.

**Files Affected:**
- `assets/icons/actions/*.svg` (34 new files)

**Acceptance Criteria:**
- All icons render at 12x12, 16x16, and 20x20 without visual artifacts.
- Chevron icons have consistent stroke weight across all directions.

**Dependencies:** Task 6.

---

### Task 13: Define the Core Icon Set -- Status Bar Icons

**Description:** Create SVG icons for status bar segments: git branch, errors, warnings, info, encoding, line ending, language, zoom.

**Key Implementation Details:**
- Create SVG files in `assets/icons/status/`.
- Icons: `"git-branch"`, `"error-circle"`, `"warning-triangle"`, `"info-circle"`, `"check-circle"`, `"encoding"`, `"line-ending"`, `"language"`, `"zoom"`, `"spinner"`.
- 14x14 viewbox (status bar icons are small).

**Files Affected:**
- `assets/icons/status/*.svg` (10 new files)

**Acceptance Criteria:**
- Icons are legible at 14x14 with 1px stroke.
- Error/warning/info icons follow standard color semantics when colorized.

**Dependencies:** Task 6.

---

### Task 14: Integrate Icon Rendering into ActivityBar

**Description:** Replace the current `icon_char` (Unicode/emoji fallback) rendering in `ActivityBar` with SVG icon rendering using the `IconDrawer`.

**Key Implementation Details:**
- Add `IconDrawer*` member to `ActivityBar`.
- In `OnPaint`, replace the text-based icon rendering with `icon_drawer_->draw(gc, icon_name, icon_rect, color)`.
- Map each `ActivityBarItem` enum value to an icon name string.
- Keep `icon_char` as a fallback for when SVG loading fails.
- Icons should be rendered at `ComponentSizeResolver::resolve(kActivityBarSlot).icon_size` (24px default).

**Files Affected:**
- `src/ui/ActivityBar.h` (modify -- add `IconDrawer*` member)
- `src/ui/ActivityBar.cpp` (modify -- replace icon rendering in `OnPaint`)

**Acceptance Criteria:**
- Activity bar shows SVG icons instead of Unicode characters.
- Icons colorize correctly with the theme's `ActivityBarFg` token.
- Active item icon uses `AccentPrimary` color.

**Dependencies:** Tasks 7, 8.

---

### Task 15: Integrate Icon Rendering into Toolbar

**Description:** Replace the seven `DrawXxxIcon` methods in `Toolbar` with `IconDrawer` calls.

**Key Implementation Details:**
- Remove methods: `DrawCodeIcon`, `DrawColumnsIcon`, `DrawEyeIcon`, `DrawSaveIcon`, `DrawPaletteIcon`, `DrawGearIcon`, `DrawFocusIcon`.
- In `DrawButton`, use `icon_drawer_->draw(gc, icon_name, icon_rect, color)`.
- Map `ButtonInfo::icon_type` integers to icon name strings.
- Consider replacing the `icon_type` int with a `std::string icon_name` field.

**Files Affected:**
- `src/ui/Toolbar.h` (modify -- remove Draw*Icon declarations, add IconDrawer*)
- `src/ui/Toolbar.cpp` (modify -- replace drawing code)

**Acceptance Criteria:**
- Toolbar buttons show SVG icons.
- Save button icon animates correctly (pulse effect still works with bitmap icons).
- Compact mode still works (icons-only rendering).

**Dependencies:** Tasks 7, 9.

---

### Task 16: Integrate Icon Rendering into FileTreeCtrl

**Description:** Add file type icons next to file and folder names in the file tree.

**Key Implementation Details:**
- Add `IconDrawer*` and `FileTypeIconResolver*` to `FileTreeCtrl`.
- Before rendering each tree item label, draw the resolved icon at `icon_size_small` (16px default).
- Folder items: use `"folder-default"` when collapsed, `"folder-open"` when expanded.
- Apply semantic coloring: folder icons use `TextMuted`, file icons use `TextMain`.

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (modify)
- `src/ui/FileTreeCtrl.cpp` (modify)

**Acceptance Criteria:**
- Each file in the tree has a file-type-specific icon.
- Folders show open/closed variants based on expansion state.
- Icons are correctly sized and aligned with the text label.

**Dependencies:** Tasks 7, 10, 11.

---

### Task 17: Integrate Icon Rendering into TabBar

**Description:** Show file type icons in tab labels, positioned before the file name.

**Key Implementation Details:**
- In `TabBar::DrawTab`, add icon rendering before the display name text.
- Use `FileTypeIconResolver` to determine the icon name from `TabInfo::file_path`.
- Icon size: `ComponentSizeResolver::resolve(kTab).icon_size` (14-16px).
- Adjust tab text x-offset to account for the icon width + padding.

**Files Affected:**
- `src/ui/TabBar.h` (modify -- add IconDrawer*, FileTypeIconResolver*)
- `src/ui/TabBar.cpp` (modify -- add icon in DrawTab)

**Acceptance Criteria:**
- Each tab shows the correct file type icon.
- Tab width calculation accounts for the icon.
- Modified dot position is not affected by the icon.

**Dependencies:** Tasks 7, 10, 11.

---

### Task 18: Integrate Icon Rendering into StatusBarPanel

**Description:** Add icons to status bar segments: git branch icon, error/warning count icons, encoding icon.

**Key Implementation Details:**
- In `StatusBarPanel::RebuildItems()`, prefix relevant items with icon names.
- Add an `icon_name` field to `StatusBarPanel::StatusItem`.
- In `OnPaint`, render the icon before the text for items that have one.
- Icons: git branch item gets `"git-branch"`, error count gets `"error-circle"`, warning count gets `"warning-triangle"`.

**Files Affected:**
- `src/ui/StatusBarPanel.h` (modify -- add icon_name to StatusItem)
- `src/ui/StatusBarPanel.cpp` (modify -- set icon names, render icons)

**Acceptance Criteria:**
- Git branch segment shows the branch icon.
- Error/warning count segments show severity icons.
- Icons are properly aligned vertically within the 24px bar.

**Dependencies:** Tasks 7, 13.

---

### Task 19: Integrate Icon Rendering into BreadcrumbBar

**Description:** Add file type icons and separator chevrons to the breadcrumb bar.

**Key Implementation Details:**
- Use `FileTypeIconResolver` to add a file type icon for the last breadcrumb segment.
- Replace text-based separators (" > ") with chevron icons (`"chevron-right"`).
- Use `icon_size_small` from the design system.

**Files Affected:**
- `src/ui/BreadcrumbBar.h` (modify)
- `src/ui/BreadcrumbBar.cpp` (modify -- replace text separators, add file icon)

**Acceptance Criteria:**
- Breadcrumb separators are rendered as chevron icons.
- The file name segment has the correct file type icon.
- Clicking chevrons does not trigger segment click callback.

**Dependencies:** Tasks 7, 11, 12.

---

### Task 20: Integrate Icon Rendering into NotificationManager

**Description:** Add severity level icons to notification toasts.

**Key Implementation Details:**
- In `NotificationManager::OnPaint`, render a severity icon on the left side of each toast.
- Map `NotificationLevel::Info` -> `"info-circle"`, `Warning` -> `"warning-triangle"`, `Error` -> `"error-circle"`, `Success` -> `"check-circle"`.
- Icon color matches the toast accent color (from `GetLevelColor`).

**Files Affected:**
- `src/ui/NotificationManager.h` (modify -- add IconDrawer*)
- `src/ui/NotificationManager.cpp` (modify -- draw severity icon in toasts)

**Acceptance Criteria:**
- Each notification toast shows the correct severity icon.
- Icon color matches the notification level color.
- Icon is vertically centered in the toast.

**Dependencies:** Tasks 7, 12.

---

### Task 21: High-DPI Rendering Support

**Description:** Ensure the icon pipeline correctly handles Retina/HiDPI displays. Bitmaps must be rendered at the physical pixel size while maintaining logical pixel positioning.

**Key Implementation Details:**
- Detect DPI scale via `wxWindow::GetDPIScaleFactor()` or `wxDisplay::GetScaleFactor()`.
- In `IconRenderer::render()`, multiply target size by DPI scale for bitmap dimensions.
- In `IconDrawer::bitmap()`, include DPI scale in the cache key.
- When drawing bitmaps to a DC, use `wxDC::DrawBitmap` with the logical (not physical) size rect.
- Platform-specific: on macOS, use `wxBitmap::CreateScaled()` for proper Retina support.

**Files Affected:**
- `src/ui/IconRenderer.cpp` (modify)
- `src/ui/IconDrawer.cpp` (modify)
- `src/ui/IconCache.h` (modify -- include DPI in key)

**Acceptance Criteria:**
- On a 2x Retina display, icons render at 2x physical pixels.
- No blurriness on HiDPI screens.
- Cache key distinguishes between 1x and 2x renders.

**Dependencies:** Tasks 4, 5, 7.

---

### Task 22: Theme Change Cache Invalidation

**Description:** When the theme changes, all cached icon bitmaps must be invalidated because icon colors change with the theme. Wire this to the existing theme change event system.

**Key Implementation Details:**
- Subscribe `IconDrawer` to `ThemeChangedEvent` via EventBus.
- On theme change, call `IconCache::invalidate_all()`.
- Next frame's paint calls will lazily re-render icons with the new theme colors.
- Consider selective invalidation: only invalidate entries whose color token has actually changed.

**Files Affected:**
- `src/ui/IconDrawer.cpp` (modify -- subscribe to theme events)

**Acceptance Criteria:**
- Switching themes causes icons to update colors on the next paint.
- No stale icons visible after a theme switch.
- No crash or race condition during invalidation while rendering.

**Dependencies:** Tasks 5, 7.

---

### Task 23: Icon Font Fallback System

**Description:** When an SVG icon is not found in the registry, fall back to rendering a Unicode character from a known icon font (or system font). This ensures the UI never shows a blank space where an icon should be.

**Key Implementation Details:**
- In `IconDrawer::bitmap()` and `IconDrawer::draw()`, if `IconRegistry::get(name)` returns nullptr:
  1. Look up the name in a fallback map: `std::unordered_map<std::string, std::string>` mapping icon name to Unicode character.
  2. Render the Unicode character using `TypographyScale::font(kMono)` at the target size.
  3. Log a warning via `MARKAMP_LOG_WARN` that the SVG icon is missing.
- Fallback map examples: `"close"` -> U+2715, `"search"` -> U+1F50D, `"settings"` -> U+2699.

**Files Affected:**
- `src/ui/IconDrawer.h` (modify -- add fallback map)
- `src/ui/IconDrawer.cpp` (modify -- implement fallback rendering)

**Acceptance Criteria:**
- Requesting a nonexistent icon name renders a Unicode glyph instead of nothing.
- A warning is logged for each missing SVG icon.
- The fallback character is centered and sized to match the target icon size.

**Dependencies:** Task 7.

---

### Task 24: Icon Preview Developer Tool

**Description:** Create a debug panel (accessible via Command Palette: "Developer: Show Icon Gallery") that displays all registered icons in a grid, useful for development and verification.

**Key Implementation Details:**
- Class: `IconGalleryPanel` in `src/ui/IconGalleryPanel.h`
- Shows all icons from `IconRegistry::all_names()` in a grid.
- Each cell: icon rendered at 16, 24, and 32px sizes, with the icon name below.
- Supports scrolling for large icon sets.
- Shows current theme's `TextMain` color and a color picker to preview icons in arbitrary colors.
- Register command in `MainFrame::RegisterPaletteCommands()`.

**Files Affected:**
- `src/ui/IconGalleryPanel.h` (new)
- `src/ui/IconGalleryPanel.cpp` (new)
- `src/ui/MainFrame.cpp` (modify -- register command)

**Acceptance Criteria:**
- Opening the gallery shows all registered icons.
- Icons display at three size variants.
- The panel scrolls correctly with 80+ icons.

**Dependencies:** Tasks 6, 7.

---

### Task 25: Catch2 Test Suite for Icon Library

**Description:** Write comprehensive unit tests for the icon library components.

**Key Implementation Details:**
- Test file: `tests/unit/test_icon_library.cpp`
- Test target: `test_icon_library` in CMakeLists.txt
- Test sections:
  - `SvgPathParser`: parse basic commands, relative commands, implicit repeats, malformed input.
  - `SvgDocumentParser`: parse complete SVG strings, handle missing viewBox, reject malformed XML.
  - `IconRegistry`: register, lookup, has, all_names, load_from_directory.
  - `FileTypeIconResolver`: extension mapping, special filenames, folder names, default fallback.
  - `IconCache`: put/get, invalidation, LRU eviction, stats.
  - `IconRenderer`: render produces non-zero bitmap, DPI scaling produces correct dimensions.
  - `IconDrawer`: fallback to Unicode on missing icon.
- Avoid tests that require actual wxWidgets display initialization where possible; mock or use headless bitmap creation.

**Files Affected:**
- `tests/unit/test_icon_library.cpp` (new)
- `CMakeLists.txt` (modify -- add test target)

**Acceptance Criteria:**
- All tests pass with `ctest --output-on-failure`.
- At least 35 test assertions.
- SVG parsing tests use embedded SVG strings (no file I/O dependency).

**Dependencies:** All previous tasks.

---

## Dependency Graph

```
Task 1 (SvgTypes) ─────────────────────────┐
Task 2 (SvgPathParser) ── depends on 1 ────┤
Task 3 (SvgDocumentParser) ── depends on 1,2 ─┤
Task 4 (IconRenderer) ── depends on 1,2,3 ─────┤
Task 5 (IconCache) ────────────────────────────┤
Task 6 (IconRegistry) ── depends on 1,3 ───────┤
Task 7 (IconDrawer) ── depends on 4,5,6 ───────┤
Task 8-13 (Icon assets) ── depend on 6 ────────┤
Task 14 (ActivityBar) ── depends on 7,8 ───────┤
Task 15 (Toolbar) ── depends on 7,9 ───────────┤
Task 16 (FileTree) ── depends on 7,10,11 ──────┤
Task 17 (TabBar) ── depends on 7,10,11 ────────┤
Task 18 (StatusBar) ── depends on 7,13 ────────┤
Task 19 (BreadcrumbBar) ── depends on 7,11,12 ─┤
Task 20 (Notifications) ── depends on 7,12 ────┤
Task 21 (HiDPI) ── depends on 4,5,7 ──────────┤
Task 22 (Theme invalidation) ── depends on 5,7 ┤
Task 23 (Fallback) ── depends on 7 ────────────┤
Task 24 (Gallery) ── depends on 6,7 ──────────┤
Task 25 (Tests) ── depends on all ─────────────┘
```

## Files Created

| File | Type |
|------|------|
| `src/ui/svg/SvgTypes.h` | Header |
| `src/ui/svg/SvgPathParser.h` | Header |
| `src/ui/svg/SvgPathParser.cpp` | Implementation |
| `src/ui/svg/SvgDocumentParser.h` | Header |
| `src/ui/svg/SvgDocumentParser.cpp` | Implementation |
| `src/ui/IconRenderer.h` | Header |
| `src/ui/IconRenderer.cpp` | Implementation |
| `src/ui/IconCache.h` | Header |
| `src/ui/IconCache.cpp` | Implementation |
| `src/ui/IconRegistry.h` | Header |
| `src/ui/IconRegistry.cpp` | Implementation |
| `src/ui/IconDrawer.h` | Header |
| `src/ui/IconDrawer.cpp` | Implementation |
| `src/ui/FileTypeIconResolver.h` | Header |
| `src/ui/FileTypeIconResolver.cpp` | Implementation |
| `src/ui/IconGalleryPanel.h` | Header |
| `src/ui/IconGalleryPanel.cpp` | Implementation |
| `assets/icons/activity/*.svg` | 16 SVG files |
| `assets/icons/toolbar/*.svg` | 7 SVG files |
| `assets/icons/filetypes/*.svg` | 27 SVG files |
| `assets/icons/actions/*.svg` | 34 SVG files |
| `assets/icons/status/*.svg` | 10 SVG files |
| `tests/unit/test_icon_library.cpp` | Test file |

## Files Modified

| File | Change |
|------|--------|
| `src/ui/ActivityBar.h` / `.cpp` | Use IconDrawer for icon rendering |
| `src/ui/Toolbar.h` / `.cpp` | Replace Draw*Icon methods with IconDrawer |
| `src/ui/FileTreeCtrl.h` / `.cpp` | Add file type icons |
| `src/ui/TabBar.h` / `.cpp` | Add file type icons to tabs |
| `src/ui/StatusBarPanel.h` / `.cpp` | Add status icons |
| `src/ui/BreadcrumbBar.h` / `.cpp` | Add chevrons and file icon |
| `src/ui/NotificationManager.h` / `.cpp` | Add severity icons |
| `src/ui/MainFrame.cpp` | Register icon gallery command |
| `src/app/MarkAmpApp.cpp` | Initialize icon registry and load icons |
| `CMakeLists.txt` | Add new sources, asset directory, test target |
