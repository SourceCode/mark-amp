# Toolbar and Context Menus Test Cases

Test cases derived from **v9**, **v13**: Toolbar actions, Context Menu system.

## TC-TOOLBAR-001: Main Toolbar

| ID           | Test Case                | Unit                     | E2E | Regression |
| ------------ | ------------------------ | ------------------------ | --- | ---------- |
| TOOLBAR-001a | Toolbar renders          | ✅ `test_toolbar_v2.cpp` | ❌  | ❌         |
| TOOLBAR-001b | View mode toggle buttons | ✅ `test_toolbar_v2.cpp` | ❌  | ❌         |
| TOOLBAR-001c | Save button              | ✅ `test_toolbar_v2.cpp` | ❌  | ❌         |
| TOOLBAR-001d | Theme gallery button     | ✅ `test_toolbar_v2.cpp` | ❌  | ❌         |
| TOOLBAR-001e | Settings button          | ✅ `test_toolbar_v2.cpp` | ❌  | ❌         |

## TC-TOOLBAR-002: Context Menus

| ID           | Test Case                           | Unit                          | E2E | Regression |
| ------------ | ----------------------------------- | ----------------------------- | --- | ---------- |
| TOOLBAR-002a | Context menu renders on right-click | ✅ `test_context_menu_v2.cpp` | ❌  | ❌         |
| TOOLBAR-002b | Sub-menu nesting works              | ✅ `test_context_menu_v2.cpp` | ❌  | ❌         |
| TOOLBAR-002c | Disabled items shown correctly      | ✅ `test_context_menu_v2.cpp` | ❌  | ❌         |
| TOOLBAR-002d | Menu dismisses on Escape            | ✅ `test_context_menu_v2.cpp` | ❌  | ❌         |
| TOOLBAR-002e | Action callback fires on click      | ✅ `test_context_menu_v2.cpp` | ❌  | ❌         |

## Coverage Summary

| Category      | Unit      | E2E      | Regression |
| ------------- | --------- | -------- | ---------- |
| Toolbar       | 5/5 ✅    | 0/5 ❌   | 0/5 ❌     |
| Context Menus | 5/5 ✅    | 0/5 ❌   | 0/5 ❌     |
| **Total**     | **10/10** | **0/10** | **0/10**   |
