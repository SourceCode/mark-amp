# Phase 39: Enterprise Runtime Protections

## Metadata

| Field | Value |
|---|---|
| Phase ID | 39 |
| Prerequisites | Phase 22 (plugin isolation), Phase 33 (telemetry) |
| Estimated Complexity | High |
| Estimated File Count | 6 created, 0 modified, 1 test |
| PRD Sections | PII-39 (runtime policy engine), PII-40 (extension network interceptor), PII-41 (immutable execution mode), PII-42 (extension resource attribution) |

---

## Objective

Add runtime policy enforcement, extension network interception, immutable execution mode, and per-extension resource attribution for enterprise deployments that require strict control over extension behavior and system configuration.

---

## Background

The PRD mandates a runtime policy enforcement engine (PII-39), extension network interceptor (PII-40), immutable execution mode (PII-41), and extension resource attribution (PII-42). Enterprise environments need the ability to lock down configuration, prevent extensions from making network calls without permission, track resource usage per extension, and enforce organizational policies. These features are off by default and activated via command-line flags or config.

---

## Scope

### Tasks

1. **Create `src/core/RuntimePolicy.h` / `RuntimePolicy.cpp`**:
   ```cpp
   namespace markamp::core {

   struct PolicyConfig {
       bool immutable_config{false};        // Prevent config changes
       bool disable_extension_install{false}; // Prevent installing new extensions
       bool enforce_network_permissions{true}; // Require kNetwork for HTTP
       size_t max_memory_per_extension_mb{256}; // Per-extension memory cap
       std::vector<std::string> blocked_extensions; // Extension IDs to block
       std::vector<std::string> allowed_extensions; // Allowlist mode (if set, only these load)
   };

   class RuntimePolicy {
   public:
       explicit RuntimePolicy(PolicyConfig config = {});

       // Check if a specific action is allowed
       [[nodiscard]] auto is_config_mutable() const -> bool;
       [[nodiscard]] auto is_extension_install_allowed() const -> bool;
       [[nodiscard]] auto is_extension_allowed(std::string_view extension_id) const -> bool;

       // Enforce on actions (returns error if blocked)
       [[nodiscard]] auto enforce_config_set(std::string_view key) -> Result<void>;
       [[nodiscard]] auto enforce_extension_install(std::string_view extension_id) -> Result<void>;

       // Load policy from config or CLI flags
       auto load_from_config(const Config& config) -> Result<void>;
       auto parse_flags(int argc, char* argv[]) -> Result<void>;

       [[nodiscard]] auto config() const -> const PolicyConfig&;

   private:
       PolicyConfig config_;
   };

   } // namespace markamp::core
   ```
   - **`--immutable` flag**: Prevents `Config::set()`, disables extension install/uninstall
   - **Extension blocklist/allowlist**: Extensions on blocklist are never loaded. In allowlist mode, only listed extensions load.
   - **Policy violations**: Logged and surfaced via ErrorReportingService

2. **Create `src/core/NetworkInterceptor.h` / `NetworkInterceptor.cpp`**:
   ```cpp
   namespace markamp::core {

   class NetworkInterceptor {
   public:
       // Check if an extension is allowed to make a network call
       [[nodiscard]] auto allow_request(
           std::string_view extension_id,
           std::string_view url,
           std::string_view method
       ) -> Result<void>;

       // Set the extension manifest registry for permission checking
       void set_manifest_registry(/* reference to manifest storage */);

       // Get blocked request log
       [[nodiscard]] auto blocked_requests() const -> std::vector<std::string>;

   private:
       std::vector<std::string> blocked_log_;
       mutable std::mutex mutex_;
   };

   } // namespace markamp::core
   ```
   - Block outbound network calls from extensions that don't have `kNetwork` permission
   - Log blocked requests for auditing
   - System-originated network calls (update checks, gallery) are always allowed

3. **Create `src/core/ExtensionResourceTracker.h` / `ExtensionResourceTracker.cpp`**:
   ```cpp
   namespace markamp::core {

   struct ExtensionResourceUsage {
       std::string extension_id;
       size_t estimated_memory_bytes{0};
       size_t events_processed{0};
       size_t errors_generated{0};
       std::chrono::milliseconds total_execution_time{0};
       std::chrono::steady_clock::time_point last_activity;
   };

   class ExtensionResourceTracker {
   public:
       // Record resource usage
       void record_event(std::string_view extension_id);
       void record_error(std::string_view extension_id);
       void record_execution_time(std::string_view extension_id,
                                   std::chrono::milliseconds duration);
       void record_memory_estimate(std::string_view extension_id, size_t bytes);

       // Query usage
       [[nodiscard]] auto usage(std::string_view extension_id) const -> ExtensionResourceUsage;
       [[nodiscard]] auto all_usage() const -> std::vector<ExtensionResourceUsage>;
       [[nodiscard]] auto top_consumers(size_t count = 5) const -> std::vector<ExtensionResourceUsage>;

       // Check limits
       [[nodiscard]] auto check_memory_limit(std::string_view extension_id,
                                              size_t limit_bytes) -> bool;

   private:
       std::unordered_map<std::string, ExtensionResourceUsage> usage_;
       mutable std::mutex mutex_;
   };

   } // namespace markamp::core
   ```
   - Track per-extension: memory estimate, event count, error count, execution time
   - Expose via diagnostics panel (Health Panel integration)
   - Flag extensions that exceed memory budget

4. **Create `tests/unit/test_enterprise_runtime.cpp`**:
   - TEST_CASE: "Immutable mode prevents Config::set"
   - TEST_CASE: "Immutable mode prevents extension install"
   - TEST_CASE: "Extension blocklist prevents loading"
   - TEST_CASE: "Extension allowlist allows only listed"
   - TEST_CASE: "NetworkInterceptor blocks without kNetwork permission"
   - TEST_CASE: "NetworkInterceptor allows with kNetwork permission"
   - TEST_CASE: "Blocked requests logged for audit"
   - TEST_CASE: "ResourceTracker tracks events per extension"
   - TEST_CASE: "ResourceTracker tracks errors per extension"
   - TEST_CASE: "top_consumers returns correct order"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/RuntimePolicy.h` |
| Create | `src/core/RuntimePolicy.cpp` |
| Create | `src/core/NetworkInterceptor.h` |
| Create | `src/core/NetworkInterceptor.cpp` |
| Create | `src/core/ExtensionResourceTracker.h` |
| Create | `src/core/ExtensionResourceTracker.cpp` |
| Create | `tests/unit/test_enterprise_runtime.cpp` |

---

## Implementation Notes

- **Immutable mode**: When `--immutable` is passed on the command line, `RuntimePolicy::is_config_mutable()` returns false. `Config::set()` checks this and returns an error. This is for managed/kiosk deployments.
- **Extension blocking**: The blocklist/allowlist is loaded from a policy config file (`~/.markamp/policy.json`) or embedded in the build for enterprise distribution.
- **Network interception**: The interceptor must be called before any HTTP request made by an extension. This requires the extension's HTTP client to route through the interceptor. Use the existing sandbox pattern from Phase 23.
- **Resource tracking**: Memory estimates are approximate — based on the number of objects created via the extension API. Precise tracking would require a custom allocator per extension (too complex for v7).
- **Diagnostics exposure**: ResourceTracker data feeds into the Health Panel (from v6) and the telemetry service (Phase 33). The `top_consumers()` query is useful for the diagnostics UI.
- **Policy violations**: All policy violations are logged via structured logger (Phase 03) and surfaced via ErrorReportingService (Phase 30).
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] `--immutable` flag prevents `Config::set()` and extension install
- [ ] Extension blocklist prevents specified extensions from loading
- [ ] Extension allowlist mode only loads listed extensions
- [ ] Extension without `kNetwork` permission cannot make HTTP calls
- [ ] Blocked network requests logged for audit
- [ ] Per-extension resource usage tracked (events, errors, execution time)
- [ ] `top_consumers()` returns extensions sorted by resource usage
- [ ] Policy violations logged and surfaced via error reporting
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test immutable mode with config set attempts
- Test extension blocklist/allowlist with various configurations
- Test network interceptor with extensions that have/lack kNetwork permission
- Test resource tracker accuracy with known event/error counts
- Test top_consumers ordering
- Test policy loading from config file
- Integration test: verify end-to-end policy enforcement
