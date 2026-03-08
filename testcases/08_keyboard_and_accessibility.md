# Keyboard and Accessibility Test Cases

Test cases derived from **v1 Phases 24-25**, **v10**: Keyboard shortcuts and accessibility.

## TC-KEY-001: Keyboard Shortcuts

| ID       | Test Case                     | Unit | E2E                                    | Regression |
| -------- | ----------------------------- | ---- | -------------------------------------- | ---------- |
| KEY-001a | Cmd+Shift+P → command palette | ➖   | ✅ `command_palette_execute.spec.ts`   | ✅         |
| KEY-001b | Cmd+, → settings              | ➖   | ✅ `settings_toggle_roundtrip.spec.ts` | ✅         |
| KEY-001c | Cmd+N → new file              | ➖   | ❌                                     | ❌         |
| KEY-001d | Cmd+O → open file             | ➖   | ❌                                     | ❌         |
| KEY-001e | Cmd+S → save file             | ➖   | ❌                                     | ❌         |
| KEY-001f | Cmd+W → close tab             | ➖   | ❌                                     | ❌         |
| KEY-001g | Cmd+B → toggle sidebar        | ➖   | ✅ `filetree_open_and_edit.spec.ts`    | ✅         |
| KEY-001h | Cmd+J → toggle panel          | ➖   | ❌                                     | ❌         |
| KEY-001i | Cmd+Z → undo                  | ➖   | ❌                                     | ❌         |
| KEY-001j | Cmd+Shift+Z → redo            | ➖   | ❌                                     | ❌         |
| KEY-001k | Cmd+Shift+E → explorer        | ➖   | ✅ `activity_bar_navigation.spec.ts`   | ✅         |
| KEY-001l | Cmd+Shift+F → search          | ➖   | ✅ `activity_bar_navigation.spec.ts`   | ✅         |
| KEY-001m | Cmd+Shift+Return → zen mode   | ➖   | ❌                                     | ❌         |
| KEY-001n | Ctrl+G → go to line           | ➖   | ❌                                     | ❌         |
| KEY-001o | Escape → dismiss overlay      | ➖   | ✅ `keyboard_shortcuts.spec.ts`        | ✅         |

## TC-KEY-002: Accessibility

| ID       | Test Case                          | Unit                        | E2E                                  | Regression |
| -------- | ---------------------------------- | --------------------------- | ------------------------------------ | ---------- |
| KEY-002a | All ma.\* identifiers discoverable | ✅ `test_accessibility.cpp` | ✅ `selector_contract_audit.spec.ts` | ✅         |
| KEY-002b | No broken accessibility elements   | ✅ `test_accessibility.cpp` | ✅ `accessibility_audit.spec.ts`     | ✅         |
| KEY-002c | Tab navigation between panels      | ✅ `test_accessibility.cpp` | ❌                                   | ❌         |
| KEY-002d | Screen reader text for buttons     | ✅ `test_accessibility.cpp` | ❌                                   | ❌         |
| KEY-002e | Focus ring visible on keyboard nav | ➖                          | ❌                                   | ❌         |

## Coverage Summary

| Category      | Unit     | E2E      | Regression |
| ------------- | -------- | -------- | ---------- |
| Shortcuts     | 0/15 ➖  | 6/15 ✅  | 6/15 ✅    |
| Accessibility | 4/5 ✅   | 2/5 ✅   | 2/5 ✅     |
| **Total**     | **4/20** | **8/20** | **8/20**   |
