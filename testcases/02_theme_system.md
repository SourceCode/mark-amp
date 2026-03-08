# Theme System Test Cases

Test cases derived from **v1 Phases 07-08, 21-22** and **v8** (UI polish): Theme data model, engine, gallery, import/export, theme fidelity.

## TC-THEME-001: Theme Data Model

| ID         | Test Case                   | Unit                | E2E | Regression |
| ---------- | --------------------------- | ------------------- | --- | ---------- |
| THEME-001a | Theme loads from JSON       | ✅ `test_theme.cpp` | ➖  | ✅         |
| THEME-001b | Theme serializes to JSON    | ✅ `test_theme.cpp` | ➖  | ✅         |
| THEME-001c | All 10 color tokens present | ✅ `test_theme.cpp` | ➖  | ✅         |
| THEME-001d | Invalid JSON rejected       | ✅ `test_theme.cpp` | ➖  | ✅         |
| THEME-001e | 8 built-in themes load      | ✅ `test_theme.cpp` | ❌  | ✅         |

## TC-THEME-002: Theme Engine Runtime Switching

| ID         | Test Case                          | Unit                        | E2E | Regression |
| ---------- | ---------------------------------- | --------------------------- | --- | ---------- |
| THEME-002a | Active theme applies to all panels | ✅ `test_theme_engine.cpp`  | ❌  | ❌         |
| THEME-002b | Theme switch updates sidebar       | ✅ `test_theme_engine.cpp`  | ❌  | ❌         |
| THEME-002c | Theme switch updates editor bg     | ✅ `test_theme_engine.cpp`  | ❌  | ❌         |
| THEME-002d | Theme switch updates status bar    | ✅ `test_theme_engine.cpp`  | ❌  | ❌         |
| THEME-002e | No stale colors after switch       | ✅ `test_visual_polish.cpp` | ❌  | ❌         |
| THEME-002f | Consecutive rapid switches         | ✅ `test_theme_engine.cpp`  | ❌  | ❌         |
| THEME-002g | Theme persists after restart       | ✅ `test_config.cpp`        | ❌  | ❌         |

## TC-THEME-003: Theme Gallery UI

| ID         | Test Case                             | Unit | E2E | Regression |
| ---------- | ------------------------------------- | ---- | --- | ---------- |
| THEME-003a | Gallery modal opens                   | ➖   | ❌  | ❌         |
| THEME-003b | 8 theme previews visible              | ➖   | ❌  | ❌         |
| THEME-003c | Clicking theme preview applies it     | ➖   | ❌  | ❌         |
| THEME-003d | Gallery dismisses on Escape           | ➖   | ❌  | ❌         |
| THEME-003e | Gallery dismisses on background click | ➖   | ❌  | ❌         |

## TC-THEME-004: Theme Import/Export

| ID         | Test Case                          | Unit                | E2E | Regression |
| ---------- | ---------------------------------- | ------------------- | --- | ---------- |
| THEME-004a | Export theme to JSON file          | ✅ `test_theme.cpp` | ❌  | ❌         |
| THEME-004b | Import theme from JSON file        | ✅ `test_theme.cpp` | ❌  | ❌         |
| THEME-004c | Imported theme appears in gallery  | ➖                  | ❌  | ❌         |
| THEME-004d | Invalid import rejected gracefully | ✅ `test_theme.cpp` | ❌  | ❌         |

## TC-THEME-005: Theme Fidelity (v8)

| ID         | Test Case                              | Unit                        | E2E | Regression |
| ---------- | -------------------------------------- | --------------------------- | --- | ---------- |
| THEME-005a | No hard-coded colors in file tree      | ✅ `test_visual_polish.cpp` | ❌  | ❌         |
| THEME-005b | No hard-coded colors in tab bar        | ✅ `test_visual_polish.cpp` | ❌  | ❌         |
| THEME-005c | Explorer selection contrast-safe       | ✅ `test_visual_polish.cpp` | ❌  | ❌         |
| THEME-005d | Sidebar header updates on theme change | ✅ `test_visual_polish.cpp` | ❌  | ❌         |

## Coverage Summary

| Category      | Unit      | E2E      | Regression |
| ------------- | --------- | -------- | ---------- |
| Data Model    | 5/5 ✅    | 0/5 ❌   | 5/5 ✅     |
| Engine        | 7/7 ✅    | 0/7 ❌   | 0/7 ❌     |
| Gallery UI    | 0/5 ➖    | 0/5 ❌   | 0/5 ❌     |
| Import/Export | 3/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| Fidelity      | 4/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| **Total**     | **19/25** | **0/25** | **5/25**   |
