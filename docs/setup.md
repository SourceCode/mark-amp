# Setup Guide

> Configuration, environment, and settings reference for MarkAmp v2.3.16.

---

## Configuration System

MarkAmp uses a **YAML-based configuration** with hierarchical scoping:

1. **Built-in defaults** — 60+ default values in `Config::set_defaults()`
2. **Global config file** — User-level settings
3. **Workspace config** — Per-workspace overrides via `ScopedConfig`
4. **Runtime overrides** — Transient changes from the Settings Panel

### Config File Locations

| Platform | Path                                                |
| -------- | --------------------------------------------------- |
| macOS    | `~/Library/Application Support/MarkAmp/config.yaml` |
| Windows  | `%APPDATA%\MarkAmp\config.yaml`                     |
| Linux    | `~/.config/MarkAmp/config.yaml`                     |

### Core Settings Reference

| Key                        | Type   | Default            | Description                              |
| -------------------------- | ------ | ------------------ | ---------------------------------------- |
| `theme`                    | string | `"midnight-neon"`  | Active theme ID                          |
| `view_mode`                | string | `"split"`          | Default view mode (editor/split/preview) |
| `sidebar_visible`          | bool   | `true`             | Sidebar visibility                       |
| `sidebar_width`            | int    | `250`              | Sidebar width in pixels                  |
| `font_size`                | int    | `14`               | Editor font size                         |
| `font_family`              | string | `"JetBrains Mono"` | Editor font family                       |
| `word_wrap`                | bool   | `true`             | Word wrap enabled                        |
| `line_numbers`             | bool   | `true`             | Show line numbers                        |
| `auto_save`                | bool   | `false`            | Auto-save on focus loss                  |
| `auto_save_delay`          | int    | `1000`             | Auto-save delay in ms                    |
| `tab_size`                 | int    | `4`                | Tab width                                |
| `insert_spaces`            | bool   | `true`             | Insert spaces instead of tabs            |
| `minimap_enabled`          | bool   | `false`            | Show code minimap                        |
| `smooth_scrolling`         | bool   | `true`             | Smooth scrolling animation               |
| `sticky_scroll`            | bool   | `true`             | Sticky heading scroll                    |
| `bracket_matching`         | bool   | `true`             | Highlight matching brackets              |
| `auto_closing_brackets`    | bool   | `true`             | Auto-close brackets                      |
| `cursor_blinking`          | bool   | `true`             | Cursor blinking animation                |
| `cursor_surrounding_lines` | int    | `5`                | Lines around cursor                      |
| `scroll_beyond_last_line`  | bool   | `true`             | Scroll below last line                   |
| `render_whitespace`        | string | `"selection"`      | Whitespace rendering mode                |

### Settings Panel

Open the settings UI:

- **Menu:** Edit → Settings
- **Shortcut:** Ctrl+, (macOS: ⌘,)
- **Command Palette:** "Preferences: Open Settings"

Features:

- Searchable/filterable with category grouping
- Modified indicators (●) for non-default values
- Import/export settings to Markdown YAML frontmatter
- Collapsible category groups

## Version Management

Version is synchronized across two files:

| File             | Field                                          |
| ---------------- | ---------------------------------------------- |
| `CMakeLists.txt` | `project(MarkAmp VERSION X.Y.Z LANGUAGES CXX)` |
| `vcpkg.json`     | `"version-semver": "X.Y.Z"`                    |

Both must be updated together when bumping the version.

## Build Configuration

### CMake Variables

| Variable                    | Default      | Description                  |
| --------------------------- | ------------ | ---------------------------- |
| `MARKAMP_BUILD_TESTS`       | `ON` (debug) | Build test targets           |
| `MARKAMP_ENABLE_ASAN`       | `OFF`        | AddressSanitizer             |
| `MARKAMP_ENABLE_UBSAN`      | `OFF`        | UndefinedBehaviorSanitizer   |
| `MARKAMP_ENABLE_TSAN`       | `OFF`        | ThreadSanitizer              |
| `MARKAMP_ENABLE_LSAN`       | `OFF`        | LeakSanitizer                |
| `MARKAMP_ENABLE_CLANG_TIDY` | `OFF`        | Static analysis during build |
| `MARKAMP_ENABLE_LTO`        | `OFF`        | Link-time optimization       |
| `MARKAMP_ENABLE_TRACY`      | `OFF`        | Tracy profiler integration   |
| `MARKAMP_PROFILE_BUILD`     | `OFF`        | Profile build mode           |

## Secrets & Credential Management

- **No credentials are stored in the repository**
- Extension API keys are stored in `ExtensionStorage` (encrypted at rest via `EncryptionService`)
- Cloud sync credentials use `KeyManager` for secure key storage
- Vault encryption keys are managed through `VaultEncryptionManager`
- All secrets use platform-native keychains where available
