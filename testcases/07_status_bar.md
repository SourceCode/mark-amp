# Status Bar Test Cases

Test cases derived from **v1 Phase 23**, **v10**: Status Bar items and interactions.

## TC-STATUS-001: Status Bar Display

| ID          | Test Case                             | Unit                     | E2E                                | Regression |
| ----------- | ------------------------------------- | ------------------------ | ---------------------------------- | ---------- |
| STATUS-001a | Status bar exists when workspace open | ✅ `test_status_bar.cpp` | 🟡 `status_bar_validation.spec.ts` | ❌         |
| STATUS-001b | Shows READY state                     | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001c | Shows encoding (UTF-8)                | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001d | Shows EOL mode (LF)                   | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001e | Shows indent mode (Spaces: 4)         | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001f | Shows view mode (SRC)                 | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001g | Shows cursor position (LN/COL)        | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001h | Shows theme name                      | ✅ `test_status_bar.cpp` | ❌                                 | ❌         |
| STATUS-001i | Status bar min height ≥ 22px          | ➖                       | ❌                                 | ❌         |

## TC-STATUS-002: Status Bar Interactions

| ID          | Test Case                        | Unit | E2E | Regression |
| ----------- | -------------------------------- | ---- | --- | ---------- |
| STATUS-002a | Click encoding → encoding picker | ➖   | ❌  | ❌         |
| STATUS-002b | Click EOL → EOL picker           | ➖   | ❌  | ❌         |
| STATUS-002c | Click view mode → mode switch    | ➖   | ❌  | ❌         |
| STATUS-002d | Click theme name → theme gallery | ➖   | ❌  | ❌         |

## Coverage Summary

| Category     | Unit     | E2E      | Regression |
| ------------ | -------- | -------- | ---------- |
| Display      | 8/9 ✅   | 0/9 ❌   | 0/9 ❌     |
| Interactions | 0/4 ➖   | 0/4 ❌   | 0/4 ❌     |
| **Total**    | **8/13** | **0/13** | **0/13**   |
