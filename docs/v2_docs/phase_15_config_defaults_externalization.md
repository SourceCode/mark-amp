# Phase 15: Config Defaults Externalization

**Priority:** Medium
**Estimated Scope:** ~4 files affected
**Dependencies:** None

## Objective

Extract the 40+ hardcoded configuration defaults from `Config::apply_defaults()` into a data-driven JSON/YAML defaults file, making defaults discoverable, documentable, and modifiable without recompilation.

## Background/Context

`Config::apply_defaults()` in `/Users/ryanrentfro/code/markamp/src/core/Config.cpp` (lines 59-192) contains 40+ individual `if (!data_["key"]) { data_["key"] = value; }` blocks. Each default is hardcoded in C++, meaning:

1. **Adding a new config key** requires modifying C++ source, recompiling, and testing
2. **Discovering available config keys** requires reading C++ source -- there is no single manifest of all keys, their types, defaults, and descriptions
3. **The SettingsPanel** must separately enumerate all known settings using `SettingDefinition` structs that duplicate the default values from Config.cpp
4. **No validation** -- Config.cpp does not verify that a loaded value has the correct type for a given key

The current defaults are organized by release phase with comments:
- Lines 59-109: Original defaults (theme, view_mode, font_size, etc.)
- Lines 114-150: R14-R15 defaults (auto_indent, indent_guides, bracket_matching, etc.)
- Lines 151-192: R22 defaults (editor.cursor_blinking, editor.cursor_width, etc.)

### Proposed Solution

Create a `config_defaults.json` file that serves as the single source of truth for all configuration keys, their types, default values, and descriptions. Config.cpp reads this file at startup and applies defaults from it.

## Detailed Tasks

### Task 1: Create config_defaults.json

**File:** New file `/Users/ryanrentfro/code/markamp/resources/config_defaults.json`

```json
{
  "$schema": "config_defaults_schema",
  "version": 1,
  "defaults": [
    {
      "key": "theme",
      "type": "string",
      "default": "midnight-neon",
      "description": "Active theme name",
      "category": "Appearance"
    },
    {
      "key": "view_mode",
      "type": "string",
      "default": "split",
      "description": "Editor layout mode (editor, preview, split)",
      "choices": ["editor", "preview", "split"],
      "category": "Editor"
    },
    {
      "key": "sidebar_visible",
      "type": "boolean",
      "default": true,
      "description": "Show the file tree sidebar",
      "category": "Appearance"
    },
    {
      "key": "font_size",
      "type": "integer",
      "default": 14,
      "description": "Editor font size in points",
      "min": 8,
      "max": 72,
      "category": "Editor"
    },
    {
      "key": "word_wrap",
      "type": "boolean",
      "default": true,
      "description": "Enable word wrapping in the editor",
      "category": "Editor"
    },
    {
      "key": "auto_save",
      "type": "boolean",
      "default": false,
      "description": "Automatically save files after changes",
      "category": "Editor"
    },
    {
      "key": "show_line_numbers",
      "type": "boolean",
      "default": true,
      "description": "Show line numbers in the editor gutter",
      "category": "Editor"
    },
    {
      "key": "highlight_current_line",
      "type": "boolean",
      "default": true,
      "description": "Highlight the line containing the cursor",
      "category": "Editor"
    },
    {
      "key": "show_whitespace",
      "type": "boolean",
      "default": false,
      "description": "Render whitespace characters",
      "category": "Editor"
    },
    {
      "key": "tab_size",
      "type": "integer",
      "default": 4,
      "description": "Number of spaces per tab",
      "min": 1,
      "max": 16,
      "category": "Editor"
    },
    {
      "key": "show_minimap",
      "type": "boolean",
      "default": false,
      "description": "Show minimap overview",
      "category": "Editor"
    },
    {
      "key": "last_workspace",
      "type": "string",
      "default": "",
      "description": "Path to the last opened workspace",
      "category": "Internal"
    },
    {
      "key": "last_open_files",
      "type": "string",
      "default": "",
      "description": "Semicolon-separated list of last open files",
      "category": "Internal"
    },
    {
      "key": "auto_indent",
      "type": "boolean",
      "default": true,
      "description": "Automatically indent new lines",
      "category": "Editor"
    },
    {
      "key": "indent_guides",
      "type": "boolean",
      "default": true,
      "description": "Show indent guide lines",
      "category": "Editor"
    },
    {
      "key": "bracket_matching",
      "type": "boolean",
      "default": true,
      "description": "Highlight matching brackets",
      "category": "Editor"
    },
    {
      "key": "code_folding",
      "type": "boolean",
      "default": true,
      "description": "Enable code folding controls",
      "category": "Editor"
    },
    {
      "key": "edge_column",
      "type": "integer",
      "default": 80,
      "description": "Column position for the edge ruler",
      "category": "Editor"
    },
    {
      "key": "font_family",
      "type": "string",
      "default": "Menlo",
      "description": "Editor font family name",
      "category": "Editor"
    },
    {
      "key": "auto_save_interval_seconds",
      "type": "integer",
      "default": 60,
      "description": "Auto-save interval in seconds",
      "min": 5,
      "max": 3600,
      "category": "Editor"
    },
    {
      "key": "show_status_bar",
      "type": "boolean",
      "default": true,
      "description": "Show the status bar at the bottom",
      "category": "Appearance"
    },
    {
      "key": "show_tab_bar",
      "type": "boolean",
      "default": true,
      "description": "Show the tab bar",
      "category": "Appearance"
    },
    {
      "key": "editor.cursor_blinking",
      "type": "string",
      "default": "blink",
      "description": "Cursor blink style",
      "choices": ["blink", "smooth", "phase", "expand", "solid"],
      "category": "Editor"
    },
    {
      "key": "editor.cursor_width",
      "type": "integer",
      "default": 2,
      "description": "Cursor width in pixels",
      "min": 1,
      "max": 10,
      "category": "Editor"
    },
    {
      "key": "editor.mouse_wheel_zoom",
      "type": "boolean",
      "default": false,
      "description": "Enable Ctrl+Wheel zoom in the editor",
      "category": "Editor"
    },
    {
      "key": "editor.word_wrap_column",
      "type": "integer",
      "default": 80,
      "description": "Column at which to wrap text when word wrap is enabled",
      "min": 40,
      "max": 200,
      "category": "Editor"
    },
    {
      "key": "editor.line_height",
      "type": "integer",
      "default": 0,
      "description": "Extra line height in pixels (0 = auto)",
      "category": "Editor"
    },
    {
      "key": "editor.letter_spacing",
      "type": "double",
      "default": 0.0,
      "description": "Extra letter spacing in pixels",
      "category": "Editor"
    },
    {
      "key": "editor.padding_top",
      "type": "integer",
      "default": 0,
      "description": "Extra padding above editor content in pixels",
      "category": "Editor"
    },
    {
      "key": "editor.padding_bottom",
      "type": "integer",
      "default": 0,
      "description": "Extra padding below editor content in pixels",
      "category": "Editor"
    },
    {
      "key": "editor.bracket_pair_colorization",
      "type": "boolean",
      "default": false,
      "description": "Colorize matching bracket pairs",
      "category": "Editor"
    },
    {
      "key": "syntax.dim_whitespace",
      "type": "boolean",
      "default": false,
      "description": "Dim whitespace characters in syntax highlighting",
      "category": "Syntax"
    }
  ]
}
```

### Task 2: Add JSON defaults loading to Config

**File:** `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

Add a method that reads the defaults file:

```cpp
void Config::apply_defaults()
{
    // Try to load defaults from the bundled JSON file
    auto defaults_path = find_resource("config_defaults.json");
    if (!defaults_path.empty())
    {
        apply_defaults_from_json(defaults_path);
        return;
    }

    // Fallback to hardcoded defaults (kept for safety)
    apply_hardcoded_defaults();
}

void Config::apply_defaults_from_json(const std::filesystem::path& path)
{
    try
    {
        std::ifstream file(path);
        auto j = nlohmann::json::parse(file);

        for (const auto& entry : j["defaults"])
        {
            auto key = entry["key"].get<std::string>();
            if (data_[key]) continue; // User has set this value

            auto type = entry["type"].get<std::string>();
            if (type == "boolean")
                data_[key] = entry["default"].get<bool>();
            else if (type == "integer")
                data_[key] = entry["default"].get<int>();
            else if (type == "double")
                data_[key] = entry["default"].get<double>();
            else if (type == "string")
                data_[key] = entry["default"].get<std::string>();
        }
    }
    catch (const std::exception& e)
    {
        MARKAMP_LOG_WARN("Failed to load config defaults JSON: {}", e.what());
        apply_hardcoded_defaults();
    }
}

void Config::apply_hardcoded_defaults()
{
    // ... existing apply_defaults() body as fallback ...
}
```

### Task 3: Wire SettingsPanel to use the same defaults file

**File:** `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`

The SettingsPanel currently constructs `SettingDefinition` objects with hardcoded defaults. After the JSON defaults file exists, generate `SettingDefinition` entries from the JSON at runtime:

```cpp
auto SettingsPanel::load_setting_definitions() -> std::vector<SettingDefinition>
{
    std::vector<SettingDefinition> defs;
    auto defaults_path = Config::find_resource("config_defaults.json");
    // ... parse JSON and build SettingDefinition vector ...
    return defs;
}
```

This eliminates the duplication between Config.cpp and SettingsPanel.cpp.

### Task 4: Copy defaults file to build directory

**File:** `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

Add a post-build copy command:

```cmake
# Copy config defaults to build tree
add_custom_command(
    TARGET markamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/resources/config_defaults.json"
        "${CMAKE_BINARY_DIR}/resources/config_defaults.json"
    COMMENT "Copying config defaults to build tree"
)
```

For macOS bundles, also copy into the bundle:
```cmake
if(APPLE)
    add_custom_command(
        TARGET markamp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/resources/config_defaults.json"
            "$<TARGET_BUNDLE_DIR:markamp>/Contents/Resources/config_defaults.json"
        COMMENT "Copying config defaults into macOS bundle"
    )
endif()
```

## Acceptance Criteria

1. `config_defaults.json` contains all 40+ config keys with types, defaults, descriptions, and categories
2. `Config::apply_defaults()` reads from the JSON file
3. If the JSON file is missing, hardcoded fallback works
4. SettingsPanel generates its definitions from the same JSON (no duplication)
5. Adding a new config key requires only adding a JSON entry (no C++ changes for simple defaults)
6. All existing tests pass

## Testing Requirements

- Add test: load defaults from JSON, verify all expected keys are set
- Add test: user-set values are NOT overwritten by defaults
- Add test: missing JSON falls back to hardcoded defaults gracefully
- Existing config tests pass unchanged
