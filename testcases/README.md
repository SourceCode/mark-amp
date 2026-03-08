# MarkAmp Test Case Index

This directory contains the comprehensive test case matrix that tracks **unit test**, **E2E (Appium)**, and **regression** coverage for every feature documented across v1–v13 docs.

## Coverage Legend

| Symbol | Meaning                                  |
| ------ | ---------------------------------------- |
| ✅     | Covered — automated tests exist and pass |
| 🟡     | Partial — some sub-scenarios covered     |
| ❌     | Not covered — no automated tests exist   |
| ➖     | N/A — feature not testable at this level |

## Test Case Files

| File                                                                   | Feature Area                                        | Version Docs           |
| ---------------------------------------------------------------------- | --------------------------------------------------- | ---------------------- |
| [01_core_application.md](01_core_application.md)                       | Core App (scaffolding, architecture, window chrome) | v1 01-06               |
| [02_theme_system.md](02_theme_system.md)                               | Theme Engine, Gallery, Import/Export                | v1 07-08, 21-22, v8    |
| [03_layout_and_navigation.md](03_layout_and_navigation.md)             | Layout, Sidebar, File Tree, Activity Bar            | v1 09-10, v9, v10, v13 |
| [04_editor.md](04_editor.md)                                           | Text Editor, Line Numbers, Large Files              | v1 12, 20, v9, v13     |
| [05_markdown_pipeline.md](05_markdown_pipeline.md)                     | Markdown Parsing, GFM, Code Blocks, Mermaid         | v1 13-17               |
| [06_preview_and_views.md](06_preview_and_views.md)                     | Preview, Split View, View Modes                     | v1 18-19, v9           |
| [07_status_bar.md](07_status_bar.md)                                   | Status Bar                                          | v1 23, v10             |
| [08_keyboard_and_accessibility.md](08_keyboard_and_accessibility.md)   | Keyboard Shortcuts, Accessibility                   | v1 24-25, v10          |
| [09_extension_system.md](09_extension_system.md)                       | Extension/Plugin Infrastructure                     | v2                     |
| [10_search_and_navigation.md](10_search_and_navigation.md)             | Search, Find/Replace, Quick Open                    | v9, v13                |
| [11_command_palette.md](11_command_palette.md)                         | Command Palette                                     | v9, v13                |
| [12_settings.md](12_settings.md)                                       | Settings UI                                         | v9, v13                |
| [13_source_control.md](13_source_control.md)                           | Git/Source Control Panel                            | v4, v9, v13            |
| [14_tabs_and_editor_groups.md](14_tabs_and_editor_groups.md)           | Tab Bar, Editor Groups                              | v9, v13                |
| [15_toolbar_and_context_menus.md](15_toolbar_and_context_menus.md)     | Toolbar, Context Menus                              | v9, v13                |
| [16_notifications.md](16_notifications.md)                             | Notification System                                 | v9                     |
| [17_performance_and_reliability.md](17_performance_and_reliability.md) | Performance, error handling                         | v6, v7                 |

## Running Tests

### Unit Tests

```bash
cmake --preset debug
cmake --build build/debug -j$(sysctl -n hw.ncpu)
cd build/debug && ctest --output-on-failure
```

### E2E (Appium)

```bash
cd tests/e2e/appium
npm install
npm run bootstrap    # Start Appium server
npm test             # Run all specs
npm run test:smoke   # Smoke tests only
npm run test:workflows  # Workflow tests
```

### Regression

```bash
npm run test:regression  # Workflows + edge cases
```
