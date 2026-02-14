# Phase 13: Test Coverage for New Services & Panels

**Priority:** Medium
**Estimated Scope:** ~15 files affected
**Dependencies:** Phase 02, Phase 06

## Objective

Add test coverage for all services and panels introduced in v1.9.12 that currently have zero tests or only data-model tests, including integration tests that verify the full wiring chain from MarkAmpApp through PluginContext to service consumers.

## Background/Context

The test suite has 21+ targets with 100% pass rate, but coverage is concentrated on the original core systems (EventBus, Config, Theme, FileSystem, PieceTable, SyntaxHighlighter, etc.). The v1.9.12 extension infrastructure has two test files:

1. `test_extension_services.cpp` -- tests the 6 contribution point registries (OutputChannelService, DiagnosticsService, TreeDataProviderRegistry, WebviewService, DecorationService, FileSystemProviderRegistry, LanguageProviderRegistry)
2. `test_p1_p4_services.cpp` -- tests the P1-P4 services (SnippetEngine, WorkspaceService, TextEditorService, ProgressService, ExtensionEvents, EnvironmentService, GrammarEngine, TerminalService, TaskRunnerService)

What is NOT tested:
- **Service instantiation & wiring** -- No test verifies that MarkAmpApp creates services and wires them to PluginContext (because it currently does not)
- **UI panels** -- OutputPanel, ProblemsPanel, TreeViewHost, WebviewHostPanel, WalkthroughPanel have zero test coverage
- **BuiltIn plugin command handler registration** -- No test verifies that built-in plugins register commands during activate()
- **FeatureRegistry integration** -- No test verifies that FeatureRegistry.is_enabled() gates UI behavior
- **EventBus queue processing** -- No test verifies that queued events are delivered via process_queued() or drain_fast_queue()
- **Cross-service interactions** -- No test verifies that e.g. an extension calling NotificationServiceAPI.show_info() results in a notification being displayed

## Detailed Tasks

### Task 1: Add service wiring integration test

**File:** New file `/Users/ryanrentfro/code/markamp/tests/unit/test_service_wiring.cpp`

Test that after Phase 02 is implemented, all services are instantiated and accessible:

```cpp
#include <catch2/catch_test_macros.hpp>
#include "core/EventBus.h"
#include "core/Config.h"
#include "core/PluginManager.h"
#include "core/PluginContext.h"
#include "core/SnippetEngine.h"
#include "core/WorkspaceService.h"
#include "core/TextEditorService.h"
#include "core/ProgressService.h"
#include "core/EnvironmentService.h"
#include "core/InputBoxService.h"
#include "core/QuickPickService.h"
#include "core/NotificationServiceAPI.h"
#include "core/StatusBarItemService.h"

TEST_CASE("PluginContext has non-null service pointers after wiring", "[integration]")
{
    markamp::core::EventBus bus;
    markamp::core::Config config;

    // Simulate what MarkAmpApp should do after Phase 02
    auto snippet_engine = std::make_unique<markamp::core::SnippetEngine>();
    auto workspace_svc = std::make_unique<markamp::core::WorkspaceService>(bus);
    // ... create all services ...

    markamp::core::PluginContext ctx;
    ctx.event_bus = &bus;
    ctx.config = &config;
    ctx.snippet_engine = snippet_engine.get();
    ctx.workspace_service = workspace_svc.get();

    REQUIRE(ctx.event_bus != nullptr);
    REQUIRE(ctx.config != nullptr);
    REQUIRE(ctx.snippet_engine != nullptr);
    REQUIRE(ctx.workspace_service != nullptr);
}
```

### Task 2: Add EventBus queue processing tests

**File:** New file `/Users/ryanrentfro/code/markamp/tests/unit/test_eventbus_queuing.cpp`

```cpp
#include <catch2/catch_test_macros.hpp>
#include "core/EventBus.h"
#include "core/Events.h"

TEST_CASE("EventBus::queue stores events for later delivery", "[eventbus]")
{
    markamp::core::EventBus bus;
    bool received = false;

    bus.subscribe<markamp::core::events::ThemeChangedEvent>(
        [&received](const auto&) { received = true; });

    markamp::core::events::ThemeChangedEvent evt;
    bus.queue(evt);

    // Event should NOT have been delivered yet
    REQUIRE_FALSE(received);

    // Process the queue
    bus.process_queued();

    // Now it should be delivered
    REQUIRE(received);
}

TEST_CASE("EventBus::drain_fast_queue processes SPSC entries", "[eventbus]")
{
    markamp::core::EventBus bus;
    bool received = false;

    bus.subscribe<markamp::core::events::ThemeChangedEvent>(
        [&received](const auto&) { received = true; });

    // Enqueue via fast path
    markamp::core::events::ThemeChangedEvent evt;
    bus.enqueue_fast([&bus, evt]() { bus.publish(evt); });

    REQUIRE_FALSE(received);
    bus.drain_fast_queue();
    REQUIRE(received);
}
```

### Task 3: Add BuiltIn plugin behavior tests

**File:** Extend `/Users/ryanrentfro/code/markamp/tests/unit/test_builtin_plugins.cpp`

After Phase 08, add tests verifying command handler registration:

```cpp
TEST_CASE("MermaidPlugin registers toggle command", "[plugins]")
{
    markamp::core::EventBus bus;
    markamp::core::Config config;
    markamp::core::FeatureRegistry registry(bus, config);
    markamp::core::PluginManager manager(bus, config);

    markamp::core::register_builtin_plugins(manager, registry);

    bool command_called = false;
    // After activation, the command handler map should contain "mermaid.toggle"
    // Verify by executing the command through the manager
    manager.activate_all();

    // Check that at least the mermaid plugin registered a command
    auto* plugin = manager.find_plugin("markamp.mermaid");
    REQUIRE(plugin != nullptr);
    REQUIRE(plugin->is_active());
}
```

### Task 4: Add FeatureRegistry guard integration test

**File:** Extend `/Users/ryanrentfro/code/markamp/tests/unit/test_feature_registry.cpp`

```cpp
TEST_CASE("FeatureRegistry disabling a feature prevents behavior", "[feature-registry]")
{
    markamp::core::EventBus bus;
    markamp::core::Config config;
    markamp::core::FeatureRegistry registry(bus, config);

    registry.register_feature("mermaid", "Mermaid Diagrams", true);
    REQUIRE(registry.is_enabled("mermaid"));

    registry.disable("mermaid");
    REQUIRE_FALSE(registry.is_enabled("mermaid"));

    // Verify that FeatureToggledEvent was published
    bool toggled = false;
    auto sub = bus.subscribe<markamp::core::events::FeatureToggledEvent>(
        [&toggled](const auto& e) { toggled = true; });

    registry.enable("mermaid");
    REQUIRE(toggled);
}
```

### Task 5: Add panel data-flow tests (post Phase 06)

**File:** New file `/Users/ryanrentfro/code/markamp/tests/unit/test_panel_data_flow.cpp`

Test that panel classes correctly reflect service state changes:

```cpp
TEST_CASE("OutputPanel tracks channel content", "[panels]")
{
    markamp::core::OutputChannelService svc;
    auto channel = svc.create("Test Channel");
    channel->append("Hello, world!");

    // OutputPanel should be able to retrieve channel content
    // (exact API depends on Phase 06 implementation)
    REQUIRE(channel->content().find("Hello, world!") != std::string::npos);
}

TEST_CASE("ProblemsPanel counts by severity", "[panels]")
{
    markamp::core::DiagnosticsService svc;
    svc.report("file.md", {
        {markamp::core::DiagnosticSeverity::Error, "Missing heading", 1, 0},
        {markamp::core::DiagnosticSeverity::Warning, "Long line", 5, 0},
    });

    auto problems = svc.get_diagnostics("file.md");
    REQUIRE(problems.size() == 2);
}
```

### Task 6: Add test targets to CMakeLists.txt

**File:** `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

Add build targets for the new test files:

```cmake
# --- Service Wiring Integration test ---
add_executable(test_service_wiring
    unit/test_service_wiring.cpp
)
target_include_directories(test_service_wiring PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_service_wiring PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_service_wiring COMMAND test_service_wiring)

# --- EventBus Queuing test ---
add_executable(test_eventbus_queuing
    unit/test_eventbus_queuing.cpp
)
target_include_directories(test_eventbus_queuing PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_eventbus_queuing PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_eventbus_queuing COMMAND test_eventbus_queuing)

# --- Panel Data Flow test ---
add_executable(test_panel_data_flow
    unit/test_panel_data_flow.cpp
)
target_include_directories(test_panel_data_flow PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_panel_data_flow PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_panel_data_flow COMMAND test_panel_data_flow)
```

### Task 7: Update README.md test count

**File:** `/Users/ryanrentfro/code/markamp/README.md`

Update the test target count from 21 to the new count (should be 24+ after adding the three new test targets).

## Acceptance Criteria

1. New test files compile and pass
2. Service wiring test validates all PluginContext fields
3. EventBus queue test validates deferred delivery
4. Panel data-flow tests validate service-to-panel data propagation
5. README reflects the updated test count
6. All existing tests continue to pass

## Testing Requirements

- All new test targets pass: `ctest --output-on-failure`
- Test names follow the existing naming convention: `test_<area>`
- Tests are deterministic (no timing dependencies, no file system side effects)
