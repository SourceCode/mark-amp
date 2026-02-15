# Phase 39 -- Plugin API Enhancement and Developer SDK

## Objective

Enhance the existing plugin system (v1.9.12 extension infrastructure) with a stable, documented Plugin API v2 that exposes all v4 services to extensions. Create an SDK with TypeScript type definitions (for JS/TS plugins), a plugin template generator, and comprehensive API documentation. The goal is to enable community plugin development for the full v4 feature set.

## Prerequisites

- Existing PluginManager, PluginContext, IPlugin interface
- All v4 services registered in PluginContext (Phases 1-38 add service pointers)
- Existing ExtensionManagement infrastructure

## Feature References (PRD)

- PRD #11: Plugin Ecosystem
- PRD #35: Plugin API for Developers

## Data Structures to Implement

### File: `src/core/PluginApiV2.h`

```cpp
#pragma once

#include "PluginContext.h"

#include <expected>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Versioned API surface for plugins. Provides safe, stable access to v4 services.
class PluginApiV2
{
public:
    explicit PluginApiV2(PluginContext& ctx);

    // --- Documents ---
    [[nodiscard]] auto list_documents() const -> std::vector<std::string>;
    [[nodiscard]] auto get_document_content(const std::string& id) const -> std::string;
    [[nodiscard]] auto create_document(const std::string& title, const std::string& content)
        -> std::expected<std::string, std::string>;
    [[nodiscard]] auto update_document(const std::string& id, const std::string& content)
        -> std::expected<void, std::string>;

    // --- Search ---
    [[nodiscard]] auto search(const std::string& query) const -> std::string; // JSON result

    // --- Tags ---
    [[nodiscard]] auto get_tags() const -> std::vector<std::string>;
    [[nodiscard]] auto get_documents_for_tag(const std::string& tag) const
        -> std::vector<std::string>;

    // --- Backlinks ---
    [[nodiscard]] auto get_backlinks(const std::string& document_id) const -> std::string; // JSON

    // --- Tasks ---
    [[nodiscard]] auto get_tasks(const std::string& filter_json = "") const -> std::string; // JSON

    // --- UI ---
    auto show_notification(const std::string& message, const std::string& level = "info") -> void;
    auto register_sidebar_panel(const std::string& id, const std::string& title,
                                 std::function<void()> on_activate) -> void;
    auto register_command(const std::string& id, const std::string& title,
                           std::function<void()> handler) -> void;
    auto register_status_bar_item(const std::string& id, const std::string& text,
                                    std::function<void()> on_click) -> void;

    // --- Events ---
    auto on_event(const std::string& event_name,
                   std::function<void(const std::string& json)> handler)
        -> Subscription;

    // --- Settings ---
    [[nodiscard]] auto get_setting(const std::string& key) const -> std::string;
    auto set_setting(const std::string& key, const std::string& value) -> void;

    // --- Graph ---
    [[nodiscard]] auto get_graph_data() const -> std::string; // JSON

    // --- Kernel (Notebook) ---
    [[nodiscard]] auto execute_code(const std::string& kernel_id, const std::string& code)
        -> std::expected<std::string, std::string>;

    // --- Version ---
    [[nodiscard]] auto api_version() const -> std::string;

private:
    PluginContext& ctx_;

    auto validate_service(const void* service, const std::string& name) const -> void;
};

/// Plugin SDK metadata for discoverability.
struct PluginManifestV2
{
    std::string id;               // publisher.plugin-name
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string min_api_version;  // Minimum PluginApiV2 version required
    std::vector<std::string> activation_events;  // When to activate
    std::vector<std::string> commands;
    std::vector<std::string> settings;
    std::string entry_point;      // Main script/library path
};

/// Template for generating new plugin projects.
struct PluginTemplateConfig
{
    std::string plugin_name;
    std::string author;
    std::string description;
    std::string language;         // "cpp", "lua", "javascript"
    bool include_tests{true};
    bool include_settings{true};
};

class PluginTemplateGenerator
{
public:
    /// Generate a plugin project from template.
    [[nodiscard]] auto generate(const PluginTemplateConfig& config,
                                 const std::filesystem::path& output_dir) const
        -> std::expected<void, std::string>;

private:
    auto generate_manifest(const PluginTemplateConfig& config) const -> std::string;
    auto generate_main_cpp(const PluginTemplateConfig& config) const -> std::string;
    auto generate_cmake(const PluginTemplateConfig& config) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`PluginApiV2` methods** -- Each method wraps calls to the underlying service with null-checks, error handling, and JSON serialization. Provides a stable facade over internal services.

2. **`validate_service(service, name)`** -- Check if the service pointer is non-null. If null, log a warning and throw/return error. Prevents plugins from crashing on missing services.

3. **`on_event(event_name, handler)`** -- Map event name strings to EventBus subscriptions. Serialize event data to JSON before calling the handler. Return Subscription for cleanup.

4. **`PluginTemplateGenerator::generate(config, output_dir)`** -- Create directory structure: manifest.json, main.cpp (or main.js), CMakeLists.txt (for C++), README.md. Fill with boilerplate code using the plugin API.

5. **`generate_main_cpp(config)`** -- Generate a C++ plugin skeleton that implements IPlugin, registers a command, and demonstrates API usage.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginApiCallEvent)
std::string plugin_id;
std::string method;
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_plugin_api_v2.cpp`

1. **API version** -- api_version() returns "2.0".
2. **List documents** -- Via API, returns document list.
3. **Search via API** -- search("test") returns JSON results.
4. **Get tags** -- get_tags() returns tag list.
5. **Create document** -- create_document() returns ID.
6. **Register command** -- register_command() callable without error.
7. **Show notification** -- show_notification() callable.
8. **Get setting** -- get_setting("key") returns configured value.
9. **Missing service** -- Call method when service is null. Verify graceful error.
10. **Template generation** -- Generate plugin template. Verify files created.
11. **Manifest parsing** -- Parse PluginManifestV2 from JSON. Verify fields.
12. **Event subscription** -- Subscribe to event via API. Publish event. Verify callback fired.

## Acceptance Criteria

- [ ] PluginApiV2 provides stable access to all v4 services
- [ ] Null service checks prevent crashes
- [ ] JSON serialization for cross-language compatibility
- [ ] Plugin template generator creates working project skeleton
- [ ] Event subscription via string name works
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/PluginApiV2.h` | PluginApiV2, PluginManifestV2, PluginTemplateGenerator |
| CREATE | `src/core/PluginApiV2.cpp` | Full implementation |
| MODIFY | `src/core/PluginManager.cpp` | Provide PluginApiV2 to plugins during activation |
| MODIFY | `src/core/Events.h` | Add PluginApiCallEvent |
| MODIFY | `src/CMakeLists.txt` | Add PluginApiV2.cpp |
| CREATE | `tests/unit/test_plugin_api_v2.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_plugin_api_v2 target |

## Architecture Notes

- PluginApiV2 is a facade over the raw PluginContext pointers
- All methods validate service pointers before use
- JSON is used as the interchange format for cross-language plugins
- API versioning allows backward compatibility checks
- Constructor injection: PluginApiV2(PluginContext&)

## Estimated Complexity

**L** -- API facade, validation layer, template generator, JSON serialization, 12 tests.
