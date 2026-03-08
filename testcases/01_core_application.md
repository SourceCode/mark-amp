# Core Application Test Cases

Test cases derived from **v1 Phases 01–06**: Project scaffolding, code quality tooling, dependency management, wxWidgets integration, application architecture, custom window chrome.

## TC-CORE-001: Application Launch

| ID        | Test Case                                    | Unit                   | E2E                                 | Regression |
| --------- | -------------------------------------------- | ---------------------- | ----------------------------------- | ---------- |
| CORE-001a | App launches without crash                   | ✅ `test_app_init.cpp` | ✅ `launch_and_shell_ready.spec.ts` | ✅         |
| CORE-001b | Main window is accessible by title "MarkAmp" | ➖                     | ✅ `launch_and_shell_ready.spec.ts` | ✅         |
| CORE-001c | Window has non-zero dimensions ≥800x600      | ➖                     | ✅ `launch_and_shell_ready.spec.ts` | ✅         |
| CORE-001d | No alert/modal dialogs on launch             | ➖                     | ✅ `launch_and_shell_ready.spec.ts` | ✅         |
| CORE-001e | Version string visible in UI                 | ➖                     | ✅ `launch_and_shell_ready.spec.ts` | ✅         |
| CORE-001f | App opens without E2E environment variable   | ✅ `test_app_init.cpp` | ❌                                  | ❌         |
| CORE-001g | Cold start time < 500ms                      | ✅ `test_app_init.cpp` | ❌                                  | ❌         |

## TC-CORE-002: Custom Window Chrome

| ID        | Test Case                                   | Unit                      | E2E              | Regression |
| --------- | ------------------------------------------- | ------------------------- | ---------------- | ---------- |
| CORE-002a | Custom chrome renders (no OS default frame) | ✅ `test_bevel_panel.cpp` | 🟡 (shell ready) | ❌         |
| CORE-002b | Close button works                          | ➖                        | ❌               | ❌         |
| CORE-002c | Minimize button works                       | ➖                        | ❌               | ❌         |
| CORE-002d | Fullscreen button works                     | ➖                        | ❌               | ❌         |
| CORE-002e | Window dragging via title bar               | ➖                        | ❌               | ❌         |
| CORE-002f | Window resizing                             | ➖                        | ❌               | ❌         |
| CORE-002g | Bevel panel effects render correctly        | ✅ `test_bevel_panel.cpp` | ❌               | ❌         |

## TC-CORE-003: Welcome Screen

| ID        | Test Case                                        | Unit | E2E                                   | Regression |
| --------- | ------------------------------------------------ | ---- | ------------------------------------- | ---------- |
| CORE-003a | Welcome screen displays on launch (no workspace) | ➖   | ✅ `launch_and_shell_ready.spec.ts`   | ✅         |
| CORE-003b | "Workspaces" label visible                       | ➖   | ✅ `theme_switch_persistence.spec.ts` | ✅         |
| CORE-003c | Recent workspaces list visible                   | ➖   | ❌                                    | ❌         |
| CORE-003d | "Open Folder" action available                   | ➖   | ❌                                    | ❌         |
| CORE-003e | "New File" action available                      | ➖   | ❌                                    | ❌         |

## TC-CORE-004: Event Bus

| ID        | Test Case                   | Unit                    | E2E | Regression |
| --------- | --------------------------- | ----------------------- | --- | ---------- |
| CORE-004a | Event publish and subscribe | ✅ `test_event_bus.cpp` | ➖  | ✅         |
| CORE-004b | Event unsubscribe           | ✅ `test_event_bus.cpp` | ➖  | ✅         |
| CORE-004c | publish_fast() path         | ✅ `test_event_bus.cpp` | ➖  | ✅         |
| CORE-004d | Queue drain on idle         | ✅ `test_event_bus.cpp` | ➖  | ✅         |
| CORE-004e | No duplicate event delivery | ✅ `test_event_bus.cpp` | ➖  | ✅         |

## TC-CORE-005: Configuration System

| ID        | Test Case                      | Unit                 | E2E | Regression |
| --------- | ------------------------------ | -------------------- | --- | ---------- |
| CORE-005a | Default config loads           | ✅ `test_config.cpp` | ➖  | ✅         |
| CORE-005b | Config persists to disk        | ✅ `test_config.cpp` | ❌  | ❌         |
| CORE-005c | Config roundtrip (save/reload) | ✅ `test_config.cpp` | ❌  | ❌         |
| CORE-005d | Invalid config file handling   | ✅ `test_config.cpp` | ❌  | ❌         |

## Coverage Summary

| Category           | Unit      | E2E      | Regression |
| ------------------ | --------- | -------- | ---------- |
| Application Launch | 2/7 ✅    | 5/7 ✅   | 4/7 ✅     |
| Window Chrome      | 2/7 ✅    | 0/7 ❌   | 0/7 ❌     |
| Welcome Screen     | 0/5 ➖    | 2/5 ✅   | 2/5 ✅     |
| Event Bus          | 5/5 ✅    | 0/5 ➖   | 5/5 ✅     |
| Configuration      | 4/4 ✅    | 0/4 ❌   | 0/4 ❌     |
| **Total**          | **13/28** | **7/28** | **11/28**  |
