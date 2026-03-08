# Command Palette Test Cases

Test cases derived from **v9**, **v13**: Command Palette UI and execution.

## TC-CMDPAL-001: Command Palette

| ID          | Test Case                           | Unit                          | E2E                                  | Regression |
| ----------- | ----------------------------------- | ----------------------------- | ------------------------------------ | ---------- |
| CMDPAL-001a | Opens via Cmd+Shift+P               | ➖                            | ✅ `command_palette_execute.spec.ts` | ✅         |
| CMDPAL-001b | Dismisses on Escape                 | ➖                            | ✅ `command_palette_execute.spec.ts` | ✅         |
| CMDPAL-001c | Shell stable after open/close       | ➖                            | ✅ `command_palette_execute.spec.ts` | ✅         |
| CMDPAL-001d | Rapid open/close cycles don't crash | ➖                            | ✅ `command_palette_execute.spec.ts` | ✅         |
| CMDPAL-001e | Shows command list on open          | ✅ `test_command_palette.cpp` | ❌                                   | ❌         |
| CMDPAL-001f | Fuzzy search filters commands       | ✅ `test_command_palette.cpp` | ❌                                   | ❌         |
| CMDPAL-001g | Execute selected command            | ✅ `test_command_palette.cpp` | ❌                                   | ❌         |
| CMDPAL-001h | Recent commands prioritized         | ✅ `test_command_palette.cpp` | ❌                                   | ❌         |
| CMDPAL-001i | Contextual commands available       | ✅ `test_command_palette.cpp` | ❌                                   | ❌         |

## Coverage Summary

| Category        | Unit    | E2E     | Regression |
| --------------- | ------- | ------- | ---------- |
| Command Palette | 5/9 ✅  | 4/9 ✅  | 4/9 ✅     |
| **Total**       | **5/9** | **4/9** | **4/9**    |
