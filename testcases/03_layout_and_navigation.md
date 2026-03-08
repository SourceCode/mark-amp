# Layout and Navigation Test Cases

Test cases derived from **v1 Phases 09-10**, **v9**, **v10**, **v13**: Layout Manager, Sidebar, File Tree, Activity Bar, Panel System.

## TC-LAYOUT-001: Layout Manager

| ID          | Test Case                                            | Unit                 | E2E                                 | Regression |
| ----------- | ---------------------------------------------------- | -------------------- | ----------------------------------- | ---------- |
| LAYOUT-001a | Three-zone layout renders (sidebar, content, status) | ✅ `test_layout.cpp` | 🟡 (shell exists)                   | ❌         |
| LAYOUT-001b | Sidebar toggle via Cmd+B                             | ➖                   | ✅ `filetree_open_and_edit.spec.ts` | ✅         |
| LAYOUT-001c | Sidebar hides/shows correctly                        | ➖                   | ✅ `filetree_open_and_edit.spec.ts` | ✅         |
| LAYOUT-001d | Layout maintains after rapid sidebar toggle          | ➖                   | ✅ `filetree_open_and_edit.spec.ts` | ✅         |
| LAYOUT-001e | Panel area toggle via Cmd+J                          | ➖                   | ❌                                  | ❌         |
| LAYOUT-001f | Window resize reflows layout                         | ➖                   | ❌                                  | ❌         |
| LAYOUT-001g | Layout persists across sessions                      | ✅ `test_layout.cpp` | ❌                                  | ❌         |

## TC-LAYOUT-002: File Tree (Sidebar)

| ID          | Test Case                            | Unit                    | E2E | Regression |
| ----------- | ------------------------------------ | ----------------------- | --- | ---------- |
| LAYOUT-002a | File tree renders after folder open  | ✅ `test_file_tree.cpp` | ❌  | ❌         |
| LAYOUT-002b | Folder expand/collapse               | ✅ `test_file_tree.cpp` | ❌  | ❌         |
| LAYOUT-002c | File selection navigates to editor   | ✅ `test_file_tree.cpp` | ❌  | ❌         |
| LAYOUT-002d | File tree shows .md files            | ✅ `test_file_tree.cpp` | ❌  | ❌         |
| LAYOUT-002e | Context menu on right-click          | ➖                      | ❌  | ❌         |
| LAYOUT-002f | File tree with deeply nested folders | ✅ `test_file_tree.cpp` | ❌  | ❌         |
| LAYOUT-002g | File tree refresh on disk changes    | ✅ `test_file_tree.cpp` | ❌  | ❌         |
| LAYOUT-002h | File tree search/filter              | ✅ `test_file_tree.cpp` | ❌  | ❌         |

## TC-LAYOUT-003: Activity Bar

| ID          | Test Case                                  | Unit                                     | E2E                                  | Regression |
| ----------- | ------------------------------------------ | ---------------------------------------- | ------------------------------------ | ---------- |
| LAYOUT-003a | 15 total items defined (12 top + 3 bottom) | ➖                                       | ✅ `activity_bar_navigation.spec.ts` | ✅         |
| LAYOUT-003b | Explorer shortcut Cmd+Shift+E              | ➖                                       | ✅ `activity_bar_navigation.spec.ts` | ✅         |
| LAYOUT-003c | Search shortcut Cmd+Shift+F                | ➖                                       | ✅ `activity_bar_navigation.spec.ts` | ✅         |
| LAYOUT-003d | No duplicate labels in model               | ➖                                       | ✅ `activity_bar_navigation.spec.ts` | ✅         |
| LAYOUT-003e | Activity bar visible when workspace open   | ✅ `test_activity_bar_accessibility.cpp` | ❌                                   | ❌         |
| LAYOUT-003f | Panel switching via activity bar click     | ➖                                       | ❌                                   | ❌         |
| LAYOUT-003g | Active item highlight                      | ➖                                       | ❌                                   | ❌         |
| LAYOUT-003h | Hover tooltip shows item name              | ➖                                       | ❌                                   | ❌         |

## TC-LAYOUT-004: Panel System

| ID          | Test Case                   | Unit | E2E | Regression |
| ----------- | --------------------------- | ---- | --- | ---------- |
| LAYOUT-004a | Panel toggle via Cmd+J      | ➖   | ❌  | ❌         |
| LAYOUT-004b | Panel resizing via drag     | ➖   | ❌  | ❌         |
| LAYOUT-004c | Panel tab switching         | ➖   | ❌  | ❌         |
| LAYOUT-004d | Panel header action buttons | ➖   | ❌  | ❌         |
| LAYOUT-004e | Panel maximize/restore      | ➖   | ❌  | ❌         |

## Coverage Summary

| Category       | Unit      | E2E      | Regression |
| -------------- | --------- | -------- | ---------- |
| Layout Manager | 2/7 ✅    | 3/7 ✅   | 3/7 ✅     |
| File Tree      | 7/8 ✅    | 0/8 ❌   | 0/8 ❌     |
| Activity Bar   | 1/8 ✅    | 4/8 ✅   | 4/8 ✅     |
| Panel System   | 0/5 ➖    | 0/5 ❌   | 0/5 ❌     |
| **Total**      | **10/28** | **7/28** | **7/28**   |
