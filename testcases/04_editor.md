# Text Editor Test Cases

Test cases derived from **v1 Phase 12, 20**, **v9**, **v13**: Text Editor, Line Numbers, Word Wrap, Large Files, Editor QoL.

## TC-EDITOR-001: Core Editor

| ID          | Test Case                    | Unit                 | E2E                     | Regression |
| ----------- | ---------------------------- | -------------------- | ----------------------- | ---------- |
| EDITOR-001a | Editor panel renders         | ✅ `test_editor.cpp` | 🟡 (page object exists) | ❌         |
| EDITOR-001b | Text input and display       | ✅ `test_editor.cpp` | ❌                      | ❌         |
| EDITOR-001c | Cursor movement (arrow keys) | ✅ `test_editor.cpp` | ❌                      | ❌         |
| EDITOR-001d | Text selection (shift+arrow) | ✅ `test_editor.cpp` | ❌                      | ❌         |
| EDITOR-001e | Copy/paste operations        | ✅ `test_editor.cpp` | ❌                      | ❌         |
| EDITOR-001f | Cut operation                | ✅ `test_editor.cpp` | ❌                      | ❌         |
| EDITOR-001g | Select all (Cmd+A)           | ✅ `test_editor.cpp` | ❌                      | ❌         |

## TC-EDITOR-002: Undo/Redo

| ID          | Test Case                      | Unit                 | E2E | Regression |
| ----------- | ------------------------------ | -------------------- | --- | ---------- |
| EDITOR-002a | Undo typing (Cmd+Z)            | ✅ `test_editor.cpp` | ❌  | ❌         |
| EDITOR-002b | Redo (Cmd+Shift+Z)             | ✅ `test_editor.cpp` | ❌  | ❌         |
| EDITOR-002c | Multiple undo steps            | ✅ `test_editor.cpp` | ❌  | ❌         |
| EDITOR-002d | Undo stack cleared on new file | ✅ `test_editor.cpp` | ❌  | ❌         |

## TC-EDITOR-003: Line Numbers & Gutter

| ID          | Test Case                             | Unit                           | E2E | Regression |
| ----------- | ------------------------------------- | ------------------------------ | --- | ---------- |
| EDITOR-003a | Line numbers display                  | ✅ `test_editor.cpp`           | ❌  | ❌         |
| EDITOR-003b | Line numbers update on content change | ✅ `test_editor.cpp`           | ❌  | ❌         |
| EDITOR-003c | Gutter click selects line             | ➖                             | ❌  | ❌         |
| EDITOR-003d | Git blame gutter annotations          | ✅ `test_git_blame_gutter.cpp` | ❌  | ❌         |

## TC-EDITOR-004: Editor Enhancements

| ID          | Test Case                   | Unit                     | E2E | Regression |
| ----------- | --------------------------- | ------------------------ | --- | ---------- |
| EDITOR-004a | Word wrap toggle            | ✅ `test_editor.cpp`     | ❌  | ❌         |
| EDITOR-004b | Large file handling (>10MB) | ✅ `test_editor.cpp`     | ❌  | ❌         |
| EDITOR-004c | Auto-indent                 | ✅ `test_editor_qol.cpp` | ❌  | ❌         |
| EDITOR-004d | Bracket matching            | ✅ `test_editor_qol.cpp` | ❌  | ❌         |
| EDITOR-004e | Code folding                | ✅ `test_editor_qol.cpp` | ❌  | ❌         |
| EDITOR-004f | Minimap rendering           | ✅ `test_minimap.cpp`    | ❌  | ❌         |

## TC-EDITOR-005: Syntax Highlighting

| ID          | Test Case                         | Unit                             | E2E | Regression |
| ----------- | --------------------------------- | -------------------------------- | --- | ---------- |
| EDITOR-005a | Tree-sitter highlighting active   | ✅ `test_syntax_highlighter.cpp` | ❌  | ❌         |
| EDITOR-005b | Theme-aware highlight colors      | ✅ `test_syntax_highlighter.cpp` | ❌  | ❌         |
| EDITOR-005c | 30+ language support              | ✅ `test_syntax_highlighter.cpp` | ❌  | ❌         |
| EDITOR-005d | Highlight updates on theme change | ✅ `test_async_highlighter.cpp`  | ❌  | ❌         |

## TC-EDITOR-006: File Save Operations

| ID          | Test Case                          | Unit                 | E2E                                | Regression |
| ----------- | ---------------------------------- | -------------------- | ---------------------------------- | ---------- |
| EDITOR-006a | Save file (Cmd+S)                  | ✅ `test_editor.cpp` | 🟡 `create_edit_save_note.spec.ts` | ❌         |
| EDITOR-006b | Save new/untitled file             | ✅ `test_editor.cpp` | ❌                                 | ❌         |
| EDITOR-006c | Dirty indicator on unsaved changes | ✅ `test_editor.cpp` | ❌                                 | ❌         |
| EDITOR-006d | Save confirmation on close         | ✅ `test_editor.cpp` | ❌                                 | ❌         |

## Coverage Summary

| Category         | Unit      | E2E      | Regression |
| ---------------- | --------- | -------- | ---------- |
| Core Editor      | 7/7 ✅    | 0/7 ❌   | 0/7 ❌     |
| Undo/Redo        | 4/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| Line Numbers     | 3/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| Enhancements     | 6/6 ✅    | 0/6 ❌   | 0/6 ❌     |
| Syntax Highlight | 4/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| File Save        | 4/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| **Total**        | **28/29** | **0/29** | **0/29**   |
