# Preview and Views Test Cases

Test cases derived from **v1 Phases 18-19**, **v9**: Preview, Split View, View Mode Switching.

## TC-VIEW-001: Preview Panel

| ID        | Test Case                           | Unit                        | E2E | Regression |
| --------- | ----------------------------------- | --------------------------- | --- | ---------- |
| VIEW-001a | Preview panel renders markdown HTML | ✅ `test_preview_panel.cpp` | ❌  | ❌         |
| VIEW-001b | Preview theme-aware background      | ✅ `test_preview_panel.cpp` | ❌  | ❌         |
| VIEW-001c | Preview scroll sync with editor     | ✅ `test_preview_panel.cpp` | ❌  | ❌         |
| VIEW-001d | Preview updates on editor change    | ✅ `test_preview_panel.cpp` | ❌  | ❌         |
| VIEW-001e | Preview renders headings correctly  | ✅ `test_preview_panel.cpp` | ❌  | ❌         |

## TC-VIEW-002: Split View

| ID        | Test Case                           | Unit | E2E                                | Regression |
| --------- | ----------------------------------- | ---- | ---------------------------------- | ---------- |
| VIEW-002a | Split mode shows editor + preview   | ➖   | 🟡 `split_view_operations.spec.ts` | ❌         |
| VIEW-002b | SRC mode shows editor only          | ➖   | 🟡 `split_view_operations.spec.ts` | ❌         |
| VIEW-002c | VIEW mode shows preview only        | ➖   | 🟡 `split_view_operations.spec.ts` | ❌         |
| VIEW-002d | Split divider visible in split mode | ➖   | 🟡 `split_view_operations.spec.ts` | ❌         |
| VIEW-002e | Divider drag resizes panels         | ➖   | ❌                                 | ❌         |

## TC-VIEW-003: View Mode Switching

| ID        | Test Case                        | Unit | E2E | Regression |
| --------- | -------------------------------- | ---- | --- | ---------- |
| VIEW-003a | SRC → Split transition           | ➖   | ❌  | ❌         |
| VIEW-003b | Split → VIEW transition          | ➖   | ❌  | ❌         |
| VIEW-003c | VIEW → SRC transition            | ➖   | ❌  | ❌         |
| VIEW-003d | Rapid view mode cycling          | ➖   | ❌  | ❌         |
| VIEW-003e | View mode persists in status bar | ➖   | ❌  | ❌         |

## Coverage Summary

| Category       | Unit     | E2E      | Regression |
| -------------- | -------- | -------- | ---------- |
| Preview        | 5/5 ✅   | 0/5 ❌   | 0/5 ❌     |
| Split View     | 0/5 ➖   | 0/5 🟡   | 0/5 ❌     |
| View Switching | 0/5 ➖   | 0/5 ❌   | 0/5 ❌     |
| **Total**      | **5/15** | **0/15** | **0/15**   |
