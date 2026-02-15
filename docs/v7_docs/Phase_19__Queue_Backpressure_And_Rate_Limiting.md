# Phase 19: Queue Backpressure and Rate Limiting

## Metadata

| Field | Value |
|---|---|
| Phase ID | 19 |
| Prerequisites | Phase 05 (EventBus hardening) |
| Estimated Complexity | High |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PI-26 (queue backpressure), PII-8 (dynamic backpressure scaling), PII-25 (rate-limited event flood protection) |

---

## Objective

Add backpressure to the EventBus and all async pipelines. Implement event coalescing for high-frequency events (scroll, cursor, content changes) and rate limiting for flood protection against malicious or misbehaving plugins.

---

## Background

The PRD mandates queue backpressure (PI-26), dynamic backpressure scaling (PII-8), and rate-limited event flood protection (PII-25). High-frequency UI events (scroll at 60fps, typing at 100+ chars/sec) generate thousands of events per second. Without coalescing, these events queue up and cause memory pressure and processing delays. Malicious plugins could also flood the event bus, denying service to legitimate handlers.

---

## Scope

### Tasks

1. **Create `src/core/Backpressure.h` / `Backpressure.cpp`**:
   ```cpp
   namespace markamp::core {

   // Event coalescing: for high-frequency events, keep only the latest
   class EventCoalescer {
   public:
       // Register an event type for coalescing
       template<typename EventType>
       void register_coalescable();

       // Register with custom key extractor (e.g., per-file coalescing)
       template<typename EventType, typename KeyFn>
       void register_coalescable(KeyFn key_extractor);

       // Check if an event should be coalesced
       template<typename EventType>
       auto should_coalesce(const EventType& event) -> bool;

       // Coalescing debounce threshold
       void set_debounce_threshold(std::chrono::milliseconds threshold);

   private:
       std::chrono::milliseconds debounce_threshold_{std::chrono::milliseconds{16}};
       // Type-erased coalescing state
       struct CoalesceState;
       std::unordered_map<std::type_index, std::unique_ptr<CoalesceState>> states_;
   };

   // Rate limiter: prevent event flooding from plugins
   class RateLimiter {
   public:
       // Set max events per second per event type per source
       void set_rate_limit(size_t max_events_per_second);

       // Check if an event from a source should be allowed
       [[nodiscard]] auto allow(std::type_index event_type,
                                 std::string_view source_id) -> bool;

       // Get current rate for diagnostics
       [[nodiscard]] auto current_rate(std::type_index event_type,
                                        std::string_view source_id) -> size_t;

   private:
       size_t max_per_second_{1000};
       struct RateState {
           size_t count{0};
           std::chrono::steady_clock::time_point window_start;
       };
       std::unordered_map<std::string, RateState> rates_;
   };

   // Backpressure metrics
   struct BackpressureMetrics {
       size_t total_coalesced{0};
       size_t total_rate_limited{0};
       size_t current_queue_pressure{0};  // 0-100%
   };

   } // namespace markamp::core
   ```

   **Coalescing rules:**
   | Event Type | Coalescing Key | Behavior |
   |---|---|---|
   | CursorPositionChangedEvent | (none — global) | Keep only latest |
   | EditorScrollChangedEvent | (none — global) | Keep only latest |
   | FileContentChangedEvent | file_id | Keep only latest per file |
   | ThemeChangedEvent | (none — global) | Keep only latest |
   | Debounce threshold | 16ms | Events within 16ms window coalesced |

2. **Modify `src/core/EventBus.h` / `EventBus.cpp`**:
   - Integrate `EventCoalescer` into `queue()` method
   - Integrate `RateLimiter` for plugin-sourced events
   - When coalescing: replace queued event with latest, increment coalesced count
   - When rate-limited: drop event, log warning, increment rate_limited count
   - Add `set_event_coalescer()` and `set_rate_limiter()` methods
   - Add backpressure metrics to `EventBusMetrics`

3. **Create `tests/unit/test_backpressure.cpp`**:
   - TEST_CASE: "Coalescer keeps only latest scroll event"
   - TEST_CASE: "Coalescer keeps latest per-file content event"
   - TEST_CASE: "Coalescer respects debounce threshold"
   - TEST_CASE: "Events outside debounce window not coalesced"
   - TEST_CASE: "RateLimiter allows events under limit"
   - TEST_CASE: "RateLimiter blocks events over limit"
   - TEST_CASE: "RateLimiter resets per-second window"
   - TEST_CASE: "10000 scroll events reduced to ~60 dispatches"
   - TEST_CASE: "Normal event flow unaffected by coalescing"
   - TEST_CASE: "BackpressureMetrics tracks coalesced and rate-limited counts"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/Backpressure.h` |
| Create | `src/core/Backpressure.cpp` |
| Modify | `src/core/EventBus.h` |
| Modify | `src/core/EventBus.cpp` |
| Create | `tests/unit/test_backpressure.cpp` |

---

## Implementation Notes

- **Coalescing mechanism**: In the EventBus queue, mark coalescable events. When a new coalescable event arrives and there's already one of the same type in the queue within the debounce window, replace the queued event rather than adding a new one.
- **Per-file coalescing**: `FileContentChangedEvent` should be coalesced per `file_id`. Two content changes to different files should not be coalesced. Use a key extractor function for this.
- **Rate limiting**: Use a sliding window counter. Track event count per type per source ID (extension ID for plugin events, "system" for internal events). Reset when the 1-second window expires.
- **Dynamic backpressure (PII-8)**: When queue depth exceeds 75% of max, increase debounce threshold to 32ms. When queue depth exceeds 90%, increase to 64ms. This automatically reduces event throughput under load.
- **Plugin source identification**: Plugin events should carry a source ID. Enhance `EventBus::publish()` with optional `source_id` parameter.
- **Backward compatibility**: Default behavior (no coalescing, no rate limiting) for events not registered as coalescable. Existing code unaffected.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] 10,000 scroll events per second reduced to ~60 dispatches (coalesced)
- [ ] Per-file content events coalesced independently (different files not merged)
- [ ] Rate limiter blocks >1000 events/sec per event type from plugins
- [ ] Rate-limited events logged with warning including source ID
- [ ] Dynamic backpressure increases debounce under high queue pressure
- [ ] Backpressure metrics exposed (coalesced count, rate-limited count, queue pressure)
- [ ] Normal event flow unaffected (non-coalescable events pass through)
- [ ] Existing EventBus API backward compatible
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test coalescing with rapid event emission (simulate 10K events/sec)
- Test per-file coalescing with interleaved file events
- Test rate limiter with burst patterns
- Test dynamic backpressure by simulating high queue depth
- Verify metrics accuracy
- Run under TSan to verify thread safety of coalescing state
