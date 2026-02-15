# Phase 05: EventBus Resilience Hardening

## Metadata

| Field | Value |
|---|---|
| Phase ID | 05 |
| Prerequisites | Phase 01 (error types), Phase 03 (structured logging), Phase 04 (crash barrier) |
| Estimated Complexity | Medium |
| Estimated File Count | 1 created, 2 modified, 1 test |
| PRD Sections | PI-3 (EventBus subscriber isolation), PI-4 (UI message loop guard), PI-16 partial (bounded event queue) |

---

## Objective

Harden the EventBus — the backbone of all inter-component communication — with universal subscriber isolation, bounded queue, subscriber identification in error logs, per-handler error counting, and automatic handler disable after consecutive failures. Currently `publish()` and `publish_fast()` have basic try/catch but `queued_events_` is unbounded and error logs lack subscriber identification.

---

## Background

The EventBus is the most critical infrastructure in MarkAmp. Every inter-component communication flows through it. The PRD mandates that every subscriber invocation is individually wrapped (PI-3), the UI message loop is guarded (PI-4), and event queues are bounded (PI-16). A single misbehaving subscriber must never prevent other subscribers from receiving events.

---

## Scope

### Tasks

1. **Modify `src/core/EventBus.h`**:
   - Add optional subscriber name to `subscribe()`:
     ```cpp
     template<typename EventType>
     auto subscribe(std::function<void(const EventType&)> handler,
                    std::string_view subscriber_name = "anonymous") -> SubscriptionId;
     ```
   - Add bounded queue configuration:
     ```cpp
     void set_max_queue_size(size_t max_size);  // Default 4096
     ```
   - Add per-subscriber error tracking:
     ```cpp
     struct SubscriberMetrics {
         std::string name;
         size_t events_received{0};
         size_t errors{0};
         size_t consecutive_errors{0};
         bool disabled{false};
     };
     ```
   - Add configurable error threshold for auto-disable:
     ```cpp
     void set_max_consecutive_errors(size_t max_errors);  // Default 10
     ```

2. **Modify `src/core/EventBus.cpp`**:
   - Enhance `publish()` and `publish_fast()`:
     - Each handler invocation individually wrapped in try/catch
     - On handler exception: log subscriber name, event type name, exception message using structured logger
     - Increment `consecutive_errors` counter; reset to 0 on success
     - Disable handler after exceeding `max_consecutive_errors_`
     - Log warning when handler disabled: "Subscriber '{name}' disabled after {N} consecutive errors for event {type}"
   - Bound `queued_events_`:
     - If queue at capacity when `queue()` called, drop oldest event
     - Log warning: "Event queue overflow: dropping oldest event (queue_depth={N})"
   - Add `re_enable_subscriber(SubscriptionId id)` method

3. **Create `src/core/EventBusMetrics.h`**:
   ```cpp
   namespace markamp::core {

   struct EventBusMetrics {
       size_t total_events_dispatched{0};
       size_t total_errors{0};
       size_t total_dropped_events{0};
       size_t current_queue_depth{0};
       size_t peak_queue_depth{0};
       size_t disabled_subscribers{0};
       std::vector<SubscriberMetrics> subscriber_metrics;
   };

   } // namespace markamp::core
   ```
   - Add `auto metrics() const -> EventBusMetrics` to EventBus

4. **Create `tests/unit/test_eventbus_resilience.cpp`**:
   - TEST_CASE: "Throwing handler does not prevent other handlers from executing"
   - TEST_CASE: "Subscriber name appears in error log"
   - TEST_CASE: "Error count incremented per handler failure"
   - TEST_CASE: "Consecutive error count resets on success"
   - TEST_CASE: "Handler disabled after N consecutive failures"
   - TEST_CASE: "Disabled handler can be re-enabled"
   - TEST_CASE: "Queue overflow drops oldest event"
   - TEST_CASE: "Queue overflow logs warning"
   - TEST_CASE: "Queue respects max size boundary"
   - TEST_CASE: "Metrics reflect dispatched/errored/dropped counts"
   - TEST_CASE: "publish_fast isolation matches publish isolation"
   - TEST_CASE: "Existing subscribe/publish API backward compatible"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `src/core/EventBus.h` |
| Modify | `src/core/EventBus.cpp` |
| Create | `src/core/EventBusMetrics.h` |
| Create | `tests/unit/test_eventbus_resilience.cpp` |

---

## Implementation Notes

- **Backward compatibility is critical**: The existing `subscribe()` signature must continue to work. The subscriber name parameter has a default value of `"anonymous"`.
- **Queue bounding**: `queued_events_` is currently a `std::vector<>`. Replace with a `std::deque<>` or circular buffer with a maximum size. When full, `pop_front()` before `push_back()`.
- **Handler isolation**: Each handler invocation must be in its own try/catch block. Do NOT wrap multiple handlers in a single try/catch — that defeats isolation.
- **Error logging**: Use `MARKAMP_LOG_WARN_S(SubsystemId::Editor, ...)` for handler errors. Include the subscriber name, event type (via `typeid().name()` demangled), and exception `what()`.
- **Auto-disable**: When a handler reaches the error threshold, set its `disabled` flag. Skip disabled handlers during dispatch. This prevents a broken handler from flooding logs.
- **Metrics**: Expose metrics for Phase 34 (telemetry spans) and Phase 38 (fault domain health tracking).
- **Thread safety**: EventBus already uses a mutex. Ensure metrics access is also thread-safe.

---

## Acceptance Criteria

- [ ] Throwing handler does not prevent other handlers from executing
- [ ] Subscriber name appears in structured error log on handler failure
- [ ] Handler error count tracked per subscriber
- [ ] Handler auto-disabled after 10 consecutive failures (configurable)
- [ ] Disabled handler logs warning message
- [ ] Disabled handler can be manually re-enabled
- [ ] Queue bounded to configurable maximum (default 4096)
- [ ] Queue overflow drops oldest events and logs warning
- [ ] EventBusMetrics exposes dispatched/errored/dropped counts
- [ ] Existing `subscribe()`/`publish()` API backward compatible (no breaking changes)
- [ ] All 12+ test cases pass

---

## Testing Strategy

- Test with intentionally throwing handlers mixed with healthy handlers
- Verify handler execution order preserved (throwing handler doesn't reorder)
- Test queue overflow with rapid event publishing
- Test metrics accuracy with known event counts
- Verify backward compatibility with existing event subscription patterns
- Run under TSan to verify thread safety of new metrics code
