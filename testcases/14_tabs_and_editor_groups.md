# Tabs and Editor Groups Test Cases

Test cases derived from **v9**, **v13**: Tab Bar, Editor Groups.

## TC-TAB-001: Tab Bar

| ID       | Test Case                   | Unit                  | E2E | Regression |
| -------- | --------------------------- | --------------------- | --- | ---------- |
| TAB-001a | Tab shows filename          | ✅ `test_tab_bar.cpp` | ❌  | ❌         |
| TAB-001b | Active tab highlighted      | ✅ `test_tab_bar.cpp` | ❌  | ❌         |
| TAB-001c | Close tab via X button      | ✅ `test_tab_bar.cpp` | ❌  | ❌         |
| TAB-001d | Close tab via Cmd+W         | ➖                    | ❌  | ❌         |
| TAB-001e | Switch tabs by clicking     | ✅ `test_tab_bar.cpp` | ❌  | ❌         |
| TAB-001f | Tab dirty indicator         | ✅ `test_tab_bar.cpp` | ❌  | ❌         |
| TAB-001g | Tab context menu            | ✅ `test_tab_bar.cpp` | ❌  | ❌         |
| TAB-001h | Tab reorder via drag        | ➖                    | ❌  | ❌         |
| TAB-001i | Many tabs overflow handling | ✅ `test_tab_bar.cpp` | ❌  | ❌         |

## TC-TAB-002: Editor Groups

| ID       | Test Case                 | Unit                       | E2E | Regression |
| -------- | ------------------------- | -------------------------- | --- | ---------- |
| TAB-002a | Split editor horizontally | ✅ `test_editor_group.cpp` | ❌  | ❌         |
| TAB-002b | Split editor vertically   | ✅ `test_editor_group.cpp` | ❌  | ❌         |
| TAB-002c | Close editor group        | ✅ `test_editor_group.cpp` | ❌  | ❌         |

## Coverage Summary

| Category      | Unit      | E2E      | Regression |
| ------------- | --------- | -------- | ---------- |
| Tab Bar       | 7/9 ✅    | 0/9 ❌   | 0/9 ❌     |
| Editor Groups | 3/3 ✅    | 0/3 ❌   | 0/3 ❌     |
| **Total**     | **10/12** | **0/12** | **0/12**   |
