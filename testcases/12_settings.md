# Settings UI Test Cases

Test cases derived from **v9**, **v13**: Settings panel, preferences.

## TC-SETTINGS-001: Settings Panel

| ID       | Test Case                    | Unit                         | E2E                                    | Regression |
| -------- | ---------------------------- | ---------------------------- | -------------------------------------- | ---------- |
| SET-001a | Opens via Cmd+,              | ➖                           | ✅ `settings_toggle_roundtrip.spec.ts` | ✅         |
| SET-001b | Dismisses on Escape          | ➖                           | ✅ `settings_toggle_roundtrip.spec.ts` | ✅         |
| SET-001c | Shell stable after toggle    | ➖                           | ✅ `settings_toggle_roundtrip.spec.ts` | ✅         |
| SET-001d | Settings categories visible  | ✅ `test_settings_ui_v2.cpp` | ❌                                     | ❌         |
| SET-001e | Toggle setting changes value | ✅ `test_settings_ui_v2.cpp` | ❌                                     | ❌         |
| SET-001f | Settings persist after close | ✅ `test_settings_ui_v2.cpp` | ❌                                     | ❌         |
| SET-001g | Reset to defaults            | ✅ `test_settings_ui_v2.cpp` | ❌                                     | ❌         |
| SET-001h | Search settings              | ✅ `test_settings_ui_v2.cpp` | ❌                                     | ❌         |

## Coverage Summary

| Category  | Unit    | E2E     | Regression |
| --------- | ------- | ------- | ---------- |
| Settings  | 5/8 ✅  | 3/8 ✅  | 3/8 ✅     |
| **Total** | **5/8** | **3/8** | **3/8**    |
