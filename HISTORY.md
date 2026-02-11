# MarkAmp Release History

## v1.1.3 — 2026-02-11

### Highlights

Major feature release adding editor QoL improvements, command palette, YAML-based configuration, Lucide icon integration, native macOS window controls, and markdown-based theme loading.

### Added

- **Editor QoL Features**: Zoom in/out (mouse wheel + keyboard shortcuts), trim trailing whitespace, accurate word/line/character counts in status bar, Zen Mode (distraction-free editing), session restore on startup
- **Command Palette**: Fuzzy-searchable command launcher (Ctrl+Shift+P) for quick access to all editor actions
- **Breadcrumb Bar**: Document path navigation breadcrumbs above the editor
- **Snippet System**: Insertable code/text snippets from the command palette
- **Native macOS Window Controls**: Replaced custom window chrome with macOS native traffic-light controls; UI layout adapts to control positions
- **File Tree with Lucide Icons**: SVG icon rendering via nanosvg for file-type icons in the sidebar tree
- **Markdown Theme Loading**: Themes can now be defined in Markdown files with YAML frontmatter via `ThemeLoader`
- **YAML Configuration**: Migrated configuration format from JSON to Markdown/YAML frontmatter using `yaml-cpp`
- **Builtin Theme Color Tokens**: Extended all 8 built-in themes with editor-specific and UI tokens (editor_bg, editor_fg, editor_selection, editor_line_number, editor_cursor, editor_gutter, list_hover, list_selected, scrollbar_thumb, scrollbar_track)
- **Preview Panel Zoom**: Text size in preview panel adjusts via zoom controls
- **Layout Manager Enhancements**: Sidebar toggle and zen-mode layout support
- **SplitView Improvements**: Programmatic sash positioning and minimum pane sizes
- **Event System Expansion**: New events for zoom, zen mode, session restore, and theme changes
- **StatusBarPanel Enhancements**: Real-time document statistics (words, lines, characters) and encoding display
- **Startup Panel & Recent Workspaces**: New landing panel with recent workspace history

### Changed

- Simplified `tests/CMakeLists.txt` — removed 390+ lines of legacy test targets in favor of streamlined test definitions
- Updated `vcpkg.json` to include `yaml-cpp` dependency
- Refactored `FileTreeCtrl` for icon-aware rendering with `nanosvg`
- Enhanced `EditorPanel` with 400+ lines of new functionality
- Expanded `MainFrame` with 450+ lines for command palette, session restore, and toolbar integration

### Fixed

- Heading rendering bug: visible `#` characters no longer appear in preview
- Shutdown crash caused by `EventBus`/`Subscription` lifetime issue
- Compilation errors in editor control tests

---

## v1.0.2 — 2026-02-11

- Initial release with full feature set (see CHANGELOG.md for details)

## v1.0.0 — 2026-02-11

- Project inception and initial commit
