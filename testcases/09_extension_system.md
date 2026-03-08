# Extension System Test Cases

Test cases derived from **v2**: Plugin/Extension infrastructure, service wiring, event system.

## TC-EXT-001: Extension Services

| ID       | Test Case                       | Unit                                | E2E | Regression |
| -------- | ------------------------------- | ----------------------------------- | --- | ---------- |
| EXT-001a | Extension service instantiation | ✅ `test_extension_integration.cpp` | ❌  | ✅         |
| EXT-001b | PluginContext population        | ✅ `test_extension_integration.cpp` | ❌  | ✅         |
| EXT-001c | Feature guard integration       | ✅ `test_extension_integration.cpp` | ❌  | ✅         |
| EXT-001d | Built-in plugin activation      | ✅ `test_extension_integration.cpp` | ❌  | ✅         |
| EXT-001e | Extension event bus             | ✅ `test_event_bus.cpp`             | ❌  | ✅         |

## TC-EXT-002: Extension UI Panels

| ID       | Test Case                        | Unit                             | E2E | Regression |
| -------- | -------------------------------- | -------------------------------- | --- | ---------- |
| EXT-002a | Extensions browser panel renders | ✅ `test_extensions_browser.cpp` | ❌  | ❌         |
| EXT-002b | Extension card display           | ✅ `test_extensions_browser.cpp` | ❌  | ❌         |
| EXT-002c | Extension detail panel           | ✅ `test_extensions_browser.cpp` | ❌  | ❌         |
| EXT-002d | Output panel renders             | ✅ `test_output_panel.cpp`       | ❌  | ❌         |
| EXT-002e | Problems panel renders           | ✅ `test_problems_panel.cpp`     | ❌  | ❌         |

## Coverage Summary

| Category  | Unit      | E2E      | Regression |
| --------- | --------- | -------- | ---------- |
| Services  | 5/5 ✅    | 0/5 ❌   | 5/5 ✅     |
| UI Panels | 5/5 ✅    | 0/5 ❌   | 0/5 ❌     |
| **Total** | **10/10** | **0/10** | **5/10**   |
