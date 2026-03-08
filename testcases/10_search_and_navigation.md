# Search and Navigation Test Cases

Test cases derived from **v9**, **v13**: Search, Find/Replace, Quick Open, Outline.

## TC-SEARCH-001: Search Panel

| ID          | Test Case                          | Unit                       | E2E                     | Regression |
| ----------- | ---------------------------------- | -------------------------- | ----------------------- | ---------- |
| SEARCH-001a | Search panel opens via Cmd+Shift+F | ✅ `test_search_panel.cpp` | 🟡 (shortcut stability) | ❌         |
| SEARCH-001b | Search finds text across files     | ✅ `test_search_panel.cpp` | ❌                      | ❌         |
| SEARCH-001c | Search results list renders        | ✅ `test_search_panel.cpp` | ❌                      | ❌         |
| SEARCH-001d | Click result navigates to file     | ✅ `test_search_panel.cpp` | ❌                      | ❌         |
| SEARCH-001e | Regex search mode                  | ✅ `test_search_panel.cpp` | ❌                      | ❌         |
| SEARCH-001f | Case-sensitive toggle              | ✅ `test_search_panel.cpp` | ❌                      | ❌         |
| SEARCH-001g | Whole word toggle                  | ✅ `test_search_panel.cpp` | ❌                      | ❌         |

## TC-SEARCH-002: Find and Replace (Editor)

| ID          | Test Case                 | Unit                          | E2E | Regression |
| ----------- | ------------------------- | ----------------------------- | --- | ---------- |
| SEARCH-002a | Find bar opens via Cmd+F  | ✅ `test_find_replace_v2.cpp` | ❌  | ❌         |
| SEARCH-002b | Find next match           | ✅ `test_find_replace_v2.cpp` | ❌  | ❌         |
| SEARCH-002c | Find previous match       | ✅ `test_find_replace_v2.cpp` | ❌  | ❌         |
| SEARCH-002d | Replace single occurrence | ✅ `test_find_replace_v2.cpp` | ❌  | ❌         |
| SEARCH-002e | Replace all occurrences   | ✅ `test_find_replace_v2.cpp` | ❌  | ❌         |
| SEARCH-002f | Match count display       | ✅ `test_find_replace_v2.cpp` | ❌  | ❌         |

## TC-SEARCH-003: Quick Open

| ID          | Test Case                | Unit                     | E2E | Regression |
| ----------- | ------------------------ | ------------------------ | --- | ---------- |
| SEARCH-003a | Quick Open via Cmd+P     | ✅ `test_quick_open.cpp` | ❌  | ❌         |
| SEARCH-003b | File fuzzy search        | ✅ `test_quick_open.cpp` | ❌  | ❌         |
| SEARCH-003c | Recent files prioritized | ✅ `test_quick_open.cpp` | ❌  | ❌         |
| SEARCH-003d | Select file opens it     | ✅ `test_quick_open.cpp` | ❌  | ❌         |

## TC-SEARCH-004: Outline Panel

| ID          | Test Case                         | Unit                        | E2E | Regression |
| ----------- | --------------------------------- | --------------------------- | --- | ---------- |
| SEARCH-004a | Outline panel shows headings      | ✅ `test_outline_panel.cpp` | ❌  | ❌         |
| SEARCH-004b | Click heading navigates           | ✅ `test_outline_panel.cpp` | ❌  | ❌         |
| SEARCH-004c | Outline updates on content change | ✅ `test_outline_panel.cpp` | ❌  | ❌         |

## Coverage Summary

| Category     | Unit      | E2E      | Regression |
| ------------ | --------- | -------- | ---------- |
| Search Panel | 7/7 ✅    | 0/7 ❌   | 0/7 ❌     |
| Find/Replace | 6/6 ✅    | 0/6 ❌   | 0/6 ❌     |
| Quick Open   | 4/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| Outline      | 3/3 ✅    | 0/3 ❌   | 0/3 ❌     |
| **Total**    | **20/20** | **0/20** | **0/20**   |
