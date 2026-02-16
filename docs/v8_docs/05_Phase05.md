Phase 5 Prompt (Settings IA Overhaul + Menu Access)

1. Refactor the settings system into a JetBrains-style hierarchy by introducing a catalog model in `/Volumes/SecondDrive/code2/mark-amp/src/core/SettingsCatalog.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/SettingsCatalog.cpp` with `group`, `subgroup`, `setting_id`, `scope`, `type`, `default`, `description`, `keywords`, and `restart_required` metadata; migrate built-in settings registration out of `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp` into this catalog.

2. Add extension-contributed settings ingestion to the catalog by wiring `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginManager.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/PluginManager.cpp` (`get_all_setting_contributions`) into catalog build, so plugin settings appear in the same tree as core settings.

3. Replace the current flat settings panel UX with a split settings editor in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`: left navigation tree (JetBrains-style categories), center settings form, top search/filter row, and bottom action row (`Apply`, `OK`, `Cancel`, `Reset`).

4. Add scoped configuration tabs (`Application`, `Workspace`, `Project`) to `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp` and implement scope-aware read/write in `/Volumes/SecondDrive/code2/mark-amp/src/core/Config.h` and `/Volumes/SecondDrive/code2/mark-amp/src/core/Config.cpp` so users can configure global and workspace/project-specific behavior.

5. Change settings mutation behavior from immediate-write to staged edits by implementing a pending-change buffer in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`; only commit on `Apply`/`OK`, discard on `Cancel`, and publish `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h` `SettingsBatchChangedEvent` with all changed keys.

6. Extend `/Volumes/SecondDrive/code2/mark-amp/src/core/Events.h` `SettingsOpenRequestEvent` to optionally carry `setting_id`, `query`, and `scope`, enabling deep links from commands and UI controls directly to a specific setting.

7. Mount settings as a first-class surface by creating `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsDialog.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsDialog.cpp` (wrapping `SettingsPanel`) and integrating open/close lifecycle in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.h` and `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.

8. Add explicit menu access in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` `createMenuBar()` using a dedicated settings menu action (`Preferences...` on macOS via `wxID_PREFERENCES`, `Settings...` on Windows/Linux) and bind it to open settings with platform shortcuts (`Cmd+,` macOS, `Ctrl+,` others).

9. Add command-palette and shortcut integration in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp` (`RegisterDefaultShortcuts`, `RegisterPaletteCommands`) for `Open Settings`, plus direct commands for `Open Settings: Editor`, `Open Settings: Appearance`, and `Open Settings: Plugins`.

10. Fix broken settings entry points by changing `/Volumes/SecondDrive/code2/mark-amp/src/ui/Toolbar.cpp` settings button behavior to emit `SettingsOpenRequestEvent` (not `ActivityBarSelectionEvent`) and add a corresponding handler path in `/Volumes/SecondDrive/code2/mark-amp/src/ui/MainFrame.cpp`.

11. Bring the settings UI to full theme fidelity and accessibility in `/Volumes/SecondDrive/code2/mark-amp/src/ui/SettingsPanel.cpp`: remove hard-coded destructive colors, enforce token-based semantic colors, add keyboard tree navigation, focus rings, and high-contrast-safe selected/hover states.

12. Add coverage in `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_settings_catalog.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_settings_panel.cpp`, `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_mainframe_menu.cpp`, and update `/Volumes/SecondDrive/code2/mark-amp/tests/unit/test_theme_engine.cpp` for staged apply/cancel semantics, scope resolution, deep-link open behavior, menu/shortcut wiring, and theme-safe settings rendering; acceptance is menu-accessible settings on all platforms, JetBrains-style organization, and passing build/tests with `cmake --preset debug`, `cmake --build /Volumes/SecondDrive/code2/mark-amp/build/debug -j$(sysctl -n hw.ncpu)`, and `cd /Volumes/SecondDrive/code2/mark-amp/build/debug && ctest --output-on-failure`.
