# Phase 38: Subsystem Fault Domains

## Metadata

| Field | Value |
|---|---|
| Phase ID | 38 |
| Prerequisites | Phase 30 (error reporting), Phase 37 (subsystem reset) |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 0 modified, 1 test |
| PRD Sections | PII-1 (subsystem fault domains), PII-47 (no cross-subsystem direct coupling) |

---

## Objective

Explicitly define fault domain boundaries so that failure in one subsystem (rendering, extensions, markdown parsing, etc.) cannot cascade to others. Each fault domain tracks health status, error rate, and can be independently degraded or reset.

---

## Background

The PRD mandates subsystem fault domains (PII-1) and no cross-subsystem direct coupling (PII-47). A fault domain is an isolation boundary — errors within a domain are contained within that domain. Cross-domain communication happens exclusively via the EventBus. This phase codifies the fault domains that have been implicitly forming through Phases 01-37 and adds health tracking.

---

## Scope

### Tasks

1. **Create `src/core/FaultDomain.h` / `FaultDomain.cpp`**:
   ```cpp
   namespace markamp::core {

   enum class DomainHealth {
       Healthy,    // All operations normal
       Degraded,   // Some errors, but functional
       Faulted,    // Non-functional, needs reset
       Isolated    // Read-only, pending recovery
   };

   struct FaultDomainMetrics {
       SubsystemId subsystem;
       DomainHealth health{DomainHealth::Healthy};
       size_t error_count{0};
       size_t consecutive_errors{0};
       std::chrono::steady_clock::time_point last_error;
       std::string last_error_message;
       std::chrono::steady_clock::time_point last_healthy;
   };

   class FaultDomain {
   public:
       explicit FaultDomain(SubsystemId subsystem);

       // Record an error in this domain
       void record_error(const Error& error);

       // Record a successful operation (resets consecutive error count)
       void record_success();

       // Get current health status
       [[nodiscard]] auto health() const -> DomainHealth;

       // Get metrics
       [[nodiscard]] auto metrics() const -> FaultDomainMetrics;

       // Manually set health (for reset/recovery)
       void set_health(DomainHealth health);

       // Check if domain is operational
       [[nodiscard]] auto is_operational() const -> bool;

       [[nodiscard]] auto subsystem() const -> SubsystemId;

   private:
       SubsystemId subsystem_;
       FaultDomainMetrics metrics_;
       mutable std::mutex mutex_;

       void update_health();

       static constexpr size_t kDegradedThreshold = 5;    // errors in 1 min
       static constexpr size_t kFaultedThreshold = 10;     // consecutive errors
   };

   } // namespace markamp::core
   ```
   - **Health transitions**:
     - Healthy → Degraded: 5+ errors within 1 minute
     - Degraded → Faulted: 10+ consecutive errors
     - Faulted → Isolated: manual or automatic after timeout
     - Any → Healthy: successful operation after reset

2. **Create `src/core/FaultDomainRegistry.h` / `FaultDomainRegistry.cpp`**:
   ```cpp
   namespace markamp::core {

   class FaultDomainRegistry {
   public:
       FaultDomainRegistry();

       // Get a fault domain by subsystem
       [[nodiscard]] auto domain(SubsystemId subsystem) -> FaultDomain&;

       // Get all domains
       [[nodiscard]] auto all_domains() const -> std::vector<const FaultDomain*>;

       // Get unhealthy domains
       [[nodiscard]] auto unhealthy_domains() const -> std::vector<const FaultDomain*>;

       // Register error for a subsystem
       void record_error(SubsystemId subsystem, const Error& error);

       // Register success for a subsystem
       void record_success(SubsystemId subsystem);

       // Get overall system health
       [[nodiscard]] auto system_health() const -> DomainHealth;

       // Subscribe to health change events
       void set_event_bus(EventBus& bus);

   private:
       std::unordered_map<SubsystemId, FaultDomain> domains_;
       EventBus* bus_{nullptr};

       void init_domains();
       void on_health_change(SubsystemId subsystem, DomainHealth old_health, DomainHealth new_health);
   };

   } // namespace markamp::core
   ```
   - Pre-registered fault domains:
     1. **Rendering**: HtmlRenderer, CodeBlockRenderer, ThemeEngine
     2. **ExtensionHost**: PluginManager, all extensions
     3. **MarkdownEngine**: MarkdownParser, Md4cWrapper
     4. **YAMLParsing**: Config, ExtensionManifest
     5. **Workspace**: VaultService, DocumentFileSystem
     6. **Network**: HttpClient, GalleryService
     7. **Editor**: EditorPanel, UndoManager, ClipboardService
     8. **Search**: SearchEngine, IncrementalSearcher
   - Emits `DomainHealthChangedEvent` when any domain's health changes

3. **Create `tests/unit/test_fault_domains.cpp`**:
   - TEST_CASE: "FaultDomain starts Healthy"
   - TEST_CASE: "FaultDomain transitions to Degraded after errors"
   - TEST_CASE: "FaultDomain transitions to Faulted after consecutive errors"
   - TEST_CASE: "FaultDomain returns to Healthy after success"
   - TEST_CASE: "FaultDomainRegistry has all predefined domains"
   - TEST_CASE: "FaultDomainRegistry records errors by subsystem"
   - TEST_CASE: "FaultDomainRegistry reports unhealthy domains"
   - TEST_CASE: "System health reflects worst domain"
   - TEST_CASE: "DomainHealthChangedEvent emitted on transition"
   - TEST_CASE: "is_operational returns true for Healthy and Degraded"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/FaultDomain.h` |
| Create | `src/core/FaultDomain.cpp` |
| Create | `src/core/FaultDomainRegistry.h` |
| Create | `src/core/FaultDomainRegistry.cpp` |
| Create | `tests/unit/test_fault_domains.cpp` |

---

## Implementation Notes

- **Health tracking**: Each fault domain independently tracks errors and determines its health. The registry provides a system-wide view.
- **Cross-domain isolation enforcement**: The EventBus is the ONLY allowed communication path between domains. This is enforced by code review and the architectural convention in CLAUDE.md, not by runtime checks (which would be too expensive).
- **Health change events**: When a domain transitions from one health state to another, emit `DomainHealthChangedEvent`. The UI can subscribe to show health indicators. The error reporting service can subscribe to show toasts.
- **Integration points**:
  - Phase 30 (ErrorReportingService) feeds errors to FaultDomainRegistry
  - Phase 37 (SubsystemReset) uses FaultDomainRegistry to determine what to reset
  - Phase 34 (Telemetry) records domain health as metrics
- **System health**: The overall system health is the worst health of any domain. If one domain is Faulted, the system is Faulted.
- **Graceful degradation**: When a domain is Degraded, disable non-essential features in that domain. When Faulted, disable the entire domain. When Isolated, the domain is read-only.
- **Event**: Define `DomainHealthChangedEvent` in `Events.h`:
  ```cpp
  MARKAMP_DECLARE_EVENT(DomainHealthChangedEvent,
      SubsystemId subsystem;
      DomainHealth old_health;
      DomainHealth new_health;
  )
  ```
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Each predefined fault domain tracked independently
- [ ] Health transitions: Healthy → Degraded → Faulted → Isolated
- [ ] Success operation resets consecutive error count (can return to Healthy)
- [ ] Domain fault does not cascade to other domains
- [ ] Unhealthy domains queryable from registry
- [ ] `DomainHealthChangedEvent` emitted on health transitions
- [ ] System health reflects the worst domain health
- [ ] `is_operational()` returns true for Healthy and Degraded
- [ ] Cross-domain communication only via EventBus (architectural convention)
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test health transitions with controlled error sequences
- Test recovery path: fault → reset → success → healthy
- Test registry with multiple domains in different health states
- Test event emission on health changes
- Test system_health aggregation
- Verify domain isolation (error in one domain doesn't affect another's metrics)
