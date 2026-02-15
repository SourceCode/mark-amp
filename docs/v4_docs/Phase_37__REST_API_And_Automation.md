# Phase 37 -- REST API and Automation

## Objective

Implement a local HTTP REST API server that allows external tools and scripts to interact with MarkAmp: query documents, execute searches, create/update notes, trigger commands, and manage notebooks programmatically. Enables integration with automation tools, shell scripts, Alfred/Raycast workflows, and custom dashboards. Uses cpp-httplib for the embedded HTTP server.

## Prerequisites

- Phase 02 (VaultService -- document CRUD)
- Phase 05 (SearchEngine -- search)
- Phase 24 (TaskService -- task queries)
- Phase 29 (KernelManager -- notebook execution, optional)

## Feature References (PRD)

- PRD #50: API Integrations (Third-Party)
- PRD Notebook #45: REST API for Automation
- PRD Notebook #29: Notebook as API (Voila)

## Data Structures to Implement

### File: `src/core/RestApiServer.h`

```cpp
#pragma once

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace markamp::core
{

class EventBus;
class Config;
class VaultService;
class SearchEngine;
class TaskService;
class KernelManager;

struct ApiRequest
{
    std::string method;         // GET, POST, PUT, DELETE
    std::string path;
    std::string body;
    std::unordered_map<std::string, std::string> query_params;
    std::unordered_map<std::string, std::string> headers;
};

struct ApiResponse
{
    int status_code{200};
    std::string body;
    std::string content_type{"application/json"};
    std::unordered_map<std::string, std::string> headers;

    static auto ok(const std::string& json_body) -> ApiResponse;
    static auto error(int code, const std::string& message) -> ApiResponse;
    static auto not_found() -> ApiResponse;
    static auto created(const std::string& json_body) -> ApiResponse;
};

class RestApiServer
{
public:
    RestApiServer(EventBus& event_bus, Config& config,
                  VaultService& vault_service,
                  SearchEngine& search_engine,
                  TaskService& task_service);

    /// Start the HTTP server on the configured port.
    [[nodiscard]] auto start(int port = 27123) -> std::expected<void, std::string>;

    /// Stop the HTTP server.
    auto stop() -> void;

    /// Check if the server is running.
    [[nodiscard]] auto is_running() const -> bool;

    /// Get the server port.
    [[nodiscard]] auto port() const -> int;

    /// Register a custom API route (for plugins).
    auto register_route(const std::string& method, const std::string& path,
                         std::function<ApiResponse(const ApiRequest&)> handler) -> void;

private:
    EventBus& event_bus_;
    Config& config_;
    VaultService& vault_service_;
    SearchEngine& search_engine_;
    TaskService& task_service_;

    struct HttpServerImpl;
    std::unique_ptr<HttpServerImpl> server_;
    int port_{27123};
    bool running_{false};

    // Route handlers
    auto handle_list_documents(const ApiRequest& req) -> ApiResponse;
    auto handle_get_document(const ApiRequest& req) -> ApiResponse;
    auto handle_create_document(const ApiRequest& req) -> ApiResponse;
    auto handle_update_document(const ApiRequest& req) -> ApiResponse;
    auto handle_delete_document(const ApiRequest& req) -> ApiResponse;
    auto handle_search(const ApiRequest& req) -> ApiResponse;
    auto handle_list_tasks(const ApiRequest& req) -> ApiResponse;
    auto handle_toggle_task(const ApiRequest& req) -> ApiResponse;
    auto handle_execute_command(const ApiRequest& req) -> ApiResponse;
    auto handle_vault_info(const ApiRequest& req) -> ApiResponse;

    auto setup_routes() -> void;
    auto authenticate(const ApiRequest& req) -> bool;
};

} // namespace markamp::core
```

## API Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | `/api/v1/vault` | Vault info (name, doc count, tags) |
| GET | `/api/v1/documents` | List all documents (paginated) |
| GET | `/api/v1/documents/:id` | Get document content and metadata |
| POST | `/api/v1/documents` | Create new document |
| PUT | `/api/v1/documents/:id` | Update document content |
| DELETE | `/api/v1/documents/:id` | Delete document |
| GET | `/api/v1/search?q=query` | Full-text search |
| GET | `/api/v1/tasks` | List tasks (with filters) |
| POST | `/api/v1/tasks/:id/toggle` | Toggle task completion |
| POST | `/api/v1/commands/:name` | Execute a named command |
| GET | `/api/v1/tags` | List all tags |
| GET | `/api/v1/backlinks/:id` | Get backlinks for a document |

## Key Functions to Implement

1. **`start(port)`** -- Create cpp-httplib Server. Set up all routes. Start listening on localhost:port. Run server in background thread.

2. **`handle_list_documents(req)`** -- Query VaultService::list_documents(). Apply pagination from query params. Return JSON array.

3. **`handle_search(req)`** -- Parse query from `q` param. Build SearchQuery. Execute via SearchEngine. Return JSON results.

4. **`handle_create_document(req)`** -- Parse title and content from JSON body. Create document via VaultService. Return 201 Created with document ID.

5. **`authenticate(req)`** -- Check for Bearer token in Authorization header. Compare to configured API key. Reject if invalid.

6. **`register_route(method, path, handler)`** -- Allow plugins to add custom API endpoints.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ApiServerStartedEvent)
int port{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ApiRequestReceivedEvent)
std::string method;
std::string path;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `api.enabled` | bool | `false` | Enable REST API server |
| `api.port` | int | `27123` | Server port |
| `api.api_key` | string | `""` | API key for authentication |
| `api.cors_origins` | string | `"*"` | CORS allowed origins |

## Test Cases

File: `tests/unit/test_rest_api.cpp`

1. **Server starts** -- Start server. Verify is_running()=true.
2. **List documents** -- GET /documents. Verify JSON array returned.
3. **Get document** -- GET /documents/:id. Verify content and metadata.
4. **Create document** -- POST /documents with title/content. Verify 201 and ID returned.
5. **Update document** -- PUT /documents/:id. Verify content updated.
6. **Delete document** -- DELETE /documents/:id. Verify 204.
7. **Search** -- GET /search?q=test. Verify search results.
8. **Authentication required** -- Request without API key. Verify 401.
9. **Not found** -- GET /documents/nonexistent. Verify 404.
10. **Custom route** -- Register custom route. Verify it responds.

## Acceptance Criteria

- [ ] HTTP server starts and listens on configured port
- [ ] CRUD operations for documents via REST endpoints
- [ ] Search endpoint returns filtered results
- [ ] Task listing and toggle via API
- [ ] Bearer token authentication
- [ ] CORS headers for cross-origin access
- [ ] Custom route registration for plugins
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/RestApiServer.h` | RestApiServer, ApiRequest, ApiResponse |
| CREATE | `src/core/RestApiServer.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 2 API events |
| MODIFY | `src/core/PluginContext.h` | Add `RestApiServer* api_server{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add RestApiServer.cpp, link cpp-httplib |
| MODIFY | `CMakeLists.txt` | Add cpp-httplib dependency (header-only) |
| CREATE | `tests/unit/test_rest_api.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_rest_api target |

## Estimated Complexity

**L** -- HTTP server, 12 endpoints, authentication, JSON serialization, 10 tests.
