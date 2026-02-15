# Phase 40 -- API Webhooks and Templates API

## Objective

Implement a local REST API server (running within the desktop application) that exposes board operations, a webhook event dispatch system for external tool integration, and a programmatic templates API for CRUD operations on board templates. This enables automation, external tool integration, and programmatic board manipulation.

## Prerequisites

- Phase 05 (Board, BoardSerializer)
- Phase 35 (BoardTemplateLibrary)
- Phase 39 (BoardManager, VersionHistory)
- Existing HttpClient infrastructure

## Feature References (PRD)

- PRD #30: Templates API
- PRD #89: API Webhooks
- PRD #67: Live Data Widgets (webhook-driven data updates)
- PRD #84: Custom Keyboard Shortcuts (API-configurable)

## Data Structures to Implement

### File: `src/canvas/CanvasAPI.h`

```cpp
#pragma once

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

class Board;
class BoardManager;
class BoardTemplateLibrary;
class VersionHistory;

/// HTTP response to return from an API endpoint.
struct APIResponse
{
    int status_code{200};
    std::string content_type{"application/json"};
    std::string body;

    [[nodiscard]] static auto ok(const std::string& json) -> APIResponse;
    [[nodiscard]] static auto created(const std::string& json) -> APIResponse;
    [[nodiscard]] static auto bad_request(const std::string& message) -> APIResponse;
    [[nodiscard]] static auto not_found(const std::string& message) -> APIResponse;
    [[nodiscard]] static auto server_error(const std::string& message) -> APIResponse;
};

/// HTTP request data.
struct APIRequest
{
    std::string method; // GET, POST, PUT, DELETE
    std::string path;
    std::string body;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> headers;
};

/// Route handler type.
using RouteHandler = std::function<APIResponse(const APIRequest&)>;

/// Local REST API server for canvas operations.
class CanvasAPIServer
{
public:
    CanvasAPIServer(BoardManager& board_manager,
                     BoardTemplateLibrary& template_library,
                     VersionHistory& version_history);

    /// Start the API server on the given port.
    auto start(int port = 8765) -> bool;
    /// Stop the server.
    auto stop() -> void;
    [[nodiscard]] auto is_running() const -> bool;
    [[nodiscard]] auto port() const -> int;

    /// Register a custom route.
    auto register_route(const std::string& method, const std::string& path,
                         RouteHandler handler) -> void;

    /// Set the active board (for board-scoped operations).
    auto set_active_board(Board* board) -> void;

private:
    BoardManager& board_manager_;
    BoardTemplateLibrary& template_library_;
    VersionHistory& version_history_;
    Board* active_board_{nullptr};
    int port_{8765};
    bool running_{false};

    struct Route
    {
        std::string method;
        std::string path_pattern;
        RouteHandler handler;
    };
    std::vector<Route> routes_;

    auto register_default_routes() -> void;

    // --- Built-in route handlers ---
    auto handle_list_boards(const APIRequest& req) -> APIResponse;
    auto handle_get_board(const APIRequest& req) -> APIResponse;
    auto handle_create_board(const APIRequest& req) -> APIResponse;
    auto handle_get_objects(const APIRequest& req) -> APIResponse;
    auto handle_add_object(const APIRequest& req) -> APIResponse;
    auto handle_list_templates(const APIRequest& req) -> APIResponse;
    auto handle_apply_template(const APIRequest& req) -> APIResponse;
    auto handle_get_history(const APIRequest& req) -> APIResponse;
};

} // namespace markamp::canvas
```

### File: `src/canvas/WebhookDispatcher.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <chrono>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::canvas
{

struct WebhookEndpoint
{
    std::string id;
    std::string name;
    std::string url;
    std::vector<std::string> events; // Event type names to listen for
    bool active{true};
    std::string secret;              // For HMAC signature
};

struct WebhookDelivery
{
    std::string webhook_id;
    std::string event_type;
    std::string payload;
    int status_code{0};
    std::chrono::system_clock::time_point timestamp;
    bool success{false};
};

class WebhookDispatcher
{
public:
    explicit WebhookDispatcher(core::EventBus& event_bus);

    /// Register a webhook endpoint.
    auto register_webhook(const WebhookEndpoint& endpoint) -> void;
    auto remove_webhook(const std::string& webhook_id) -> void;
    auto update_webhook(const WebhookEndpoint& endpoint) -> void;

    [[nodiscard]] auto all_webhooks() const -> const std::vector<WebhookEndpoint>&;

    /// Get delivery history for a webhook.
    [[nodiscard]] auto delivery_history(const std::string& webhook_id) const
        -> std::vector<WebhookDelivery>;

    /// Manually trigger a test delivery.
    auto test_webhook(const std::string& webhook_id) -> WebhookDelivery;

    /// Start listening for canvas events and dispatching.
    auto start() -> void;
    auto stop() -> void;

    /// Save/load webhook config.
    auto save_config(const std::filesystem::path& path) const -> void;
    auto load_config(const std::filesystem::path& path) -> void;

private:
    core::EventBus& event_bus_;
    std::vector<WebhookEndpoint> webhooks_;
    std::vector<WebhookDelivery> delivery_log_;

    auto dispatch(const std::string& event_type, const std::string& payload) -> void;
    auto send_http_post(const std::string& url, const std::string& payload,
                         const std::string& secret) -> int;
};

} // namespace markamp::canvas
```

### File: `src/canvas/LiveDataWidget.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <chrono>
#include <string>

namespace markamp::canvas
{

class LiveDataWidget : public CanvasObject
{
public:
    LiveDataWidget();

    [[nodiscard]] auto data_url() const -> const std::string&;
    auto set_data_url(const std::string& url) -> void;

    [[nodiscard]] auto display_format() const -> const std::string&;
    auto set_display_format(const std::string& fmt) -> void;

    [[nodiscard]] auto refresh_interval_seconds() const -> int;
    auto set_refresh_interval_seconds(int seconds) -> void;

    [[nodiscard]] auto current_data() const -> const std::string&;
    auto set_current_data(const std::string& data) -> void;

    [[nodiscard]] auto last_refresh() const -> std::chrono::system_clock::time_point;
    auto set_last_refresh(std::chrono::system_clock::time_point t) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto set_dimensions(double w, double h) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string data_url_;
    std::string display_format_{"{{value}}"};
    int refresh_interval_{60};
    std::string current_data_;
    std::chrono::system_clock::time_point last_refresh_;
    double width_{200.0};
    double height_{100.0};
};

} // namespace markamp::canvas
```

## REST API Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/boards` | List all boards |
| GET | `/api/boards/:id` | Get board details |
| POST | `/api/boards` | Create a new board |
| GET | `/api/boards/:id/objects` | List objects in board |
| POST | `/api/boards/:id/objects` | Add object to board |
| PUT | `/api/boards/:id/objects/:oid` | Update object |
| DELETE | `/api/boards/:id/objects/:oid` | Remove object |
| GET | `/api/templates` | List templates |
| POST | `/api/templates/:id/apply` | Apply template (creates board) |
| GET | `/api/boards/:id/history` | List version snapshots |
| POST | `/api/boards/:id/history/restore/:vid` | Restore snapshot |
| GET | `/api/webhooks` | List webhooks |
| POST | `/api/webhooks` | Register webhook |
| DELETE | `/api/webhooks/:id` | Remove webhook |
| POST | `/api/webhooks/:id/test` | Test webhook delivery |

## Key Functions to Implement

1. `CanvasAPIServer::start()` -- Start an HTTP server on the specified port using cpp-httplib (already a project dependency from v4 REST API phase). Register default routes.

2. `CanvasAPIServer::handle_get_objects()` -- Serialize all objects in the active board to JSON array.

3. `CanvasAPIServer::handle_add_object()` -- Parse JSON body, use BoardSerializer to create object, add to board.

4. `WebhookDispatcher::dispatch()` -- For each webhook that subscribes to the event type: POST the payload to the webhook URL. Include HMAC signature in header. Log delivery.

5. `WebhookDispatcher::start()` -- Subscribe to canvas events (ObjectAdded, ObjectRemoved, ObjectModified, BoardSaved). On each event, serialize to JSON payload and dispatch.

6. `LiveDataWidget` rendering: Show current_data formatted with display_format. Show a refresh indicator. Auto-refresh using a timer.

## Test Cases

File: `tests/unit/test_canvas_api.cpp`

1. **API response helpers** -- Verify ok(), bad_request(), not_found() format.
2. **Route registration** -- Register route, verify handler called.
3. **List boards** -- Mock boards, verify JSON response.
4. **Get objects** -- Board with objects, verify JSON array.
5. **Add object** -- POST object JSON, verify added to board.

File: `tests/unit/test_webhook_dispatcher.cpp`

6. **Register webhook** -- Register, verify in list.
7. **Remove webhook** -- Remove, verify removed.
8. **Delivery log** -- Dispatch event, verify delivery logged.
9. **Webhook config save/load** -- Save, load, verify round-trip.

File: `tests/unit/test_live_data_widget.cpp`

10. **Construction** -- Verify defaults.
11. **Set data URL** -- Set, verify.
12. **JSON round-trip** -- Serialize/deserialize.
13. **Refresh interval** -- Set, verify.

## Acceptance Criteria

- [ ] Local REST API server with 15 endpoints
- [ ] Board CRUD via API
- [ ] Object CRUD via API
- [ ] Template listing and application via API
- [ ] Version history access via API
- [ ] Webhook registration, dispatch, and delivery logging
- [ ] HMAC signature for webhook payloads
- [ ] Webhook config persistence
- [ ] LiveDataWidget with auto-refresh from URL
- [ ] Custom keyboard shortcut configuration (via config, not API)
- [ ] All 13 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CanvasAPI.h` | REST API server |
| CREATE | `src/canvas/CanvasAPI.cpp` | API server + route handlers |
| CREATE | `src/canvas/WebhookDispatcher.h` | Webhook management + dispatch |
| CREATE | `src/canvas/WebhookDispatcher.cpp` | Webhook implementation |
| CREATE | `src/canvas/LiveDataWidget.h` | Live data widget object |
| CREATE | `src/canvas/LiveDataWidget.cpp` | Widget implementation |
| CREATE | `src/canvas/LiveDataWidgetRenderer.h` | Widget rendering |
| CREATE | `src/canvas/LiveDataWidgetRenderer.cpp` | Render implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register LiveDataWidget factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_canvas_api.cpp` | 5 tests |
| CREATE | `tests/unit/test_webhook_dispatcher.cpp` | 4 tests |
| CREATE | `tests/unit/test_live_data_widget.cpp` | 4 tests |

## Architecture Notes

- The REST API uses cpp-httplib which is already available in the project. The server runs on a background thread; handlers post work to the main thread via EventBus::queue() for thread safety.
- The API server only binds to localhost (127.0.0.1) by default for security. An option to bind to all interfaces can be added later.
- Webhooks use HTTP POST with JSON payloads. The HMAC-SHA256 signature is included in an `X-MarkAmp-Signature` header.
- LiveDataWidget fetches from an HTTP endpoint on a timer. The fetched data is stored as a string and rendered using a simple template format ({{value}}).
- Custom keyboard shortcuts (PRD #84) are handled by the existing ShortcutManager's remap_shortcut() and save_keybindings(). No new code needed beyond ensuring the settings UI exposes these functions.

## Estimated Complexity

**XL** -- REST API server with 15 endpoints, webhook dispatch with HMAC, live data widget with auto-refresh, 13 tests.
