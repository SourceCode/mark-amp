# Phase 10: EventBus publish_fast & Queue Processing Fix

**Priority:** High
**Estimated Scope:** ~4 files affected
**Dependencies:** None

## Objective

Fix the `publish_fast()` method to actually be lock-free (currently identical to `publish()`), and wire `process_queued()` / `drain_fast_queue()` into the application's idle handler.

## Background/Context

### Problem 1: publish_fast() is not fast
The `publish_fast()` template in EventBus.h (line 163-193) is documented as "bypasses the mutex by atomically loading the handler snapshot" but actually takes a `std::lock_guard lock(mutex_)` -- making it identical to `publish()`. The COW pattern with `shared_ptr` would allow a true lock-free read if implemented correctly.

### Problem 2: Queued events never processed
`EventBus::queue()` stores events in `queued_events_` for later delivery, and `drain_fast_queue()` processes the SPSC fast queue. But neither `process_queued()` nor `drain_fast_queue()` is ever called from any application idle handler or main loop. Queued events silently disappear.

## Detailed Tasks

### Task 1: Make publish_fast() truly lock-free

**File:** `/Users/ryanrentfro/code/markamp/src/core/EventBus.h`

Replace the lock_guard in publish_fast with an atomic load pattern:
```cpp
template <typename T>
    requires std::derived_from<T, Event>
void EventBus::publish_fast(const T& event)
{
    // True lock-free: use atomic load of the shared_ptr snapshot.
    // The COW pattern guarantees the snapshot is always valid.
    std::shared_ptr<HandlerList> snapshot;
    {
        // Atomic load of the shared_ptr -- no mutex
        // Requires changing handlers_ to use std::atomic<std::shared_ptr<HandlerList>>
        // OR using a separate atomic snapshot pointer for the fast path.
        snapshot = load_handlers<T>();
    }
    if (snapshot)
    {
        for (const auto& entry : *snapshot)
        {
            try { entry.handler(event); }
            catch (const std::exception& ex) {
                MARKAMP_LOG_WARN("EventBus fast handler threw: {}", ex.what());
            }
        }
    }
}
```

The cleanest approach is to use `std::atomic<std::shared_ptr<HandlerList>>` (C++20) or maintain a separate `std::shared_ptr` snapshot that is atomically swapped on subscribe/unsubscribe:
```cpp
// In EventBus private:
std::unordered_map<std::type_index, std::atomic<std::shared_ptr<HandlerList>>> fast_handlers_;
```

If `std::atomic<shared_ptr>` is not available on the target compilers, use a seqlock or reader-optimized lock pattern.

### Task 2: Wire process_queued() into application idle handler

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

Override `wxApp::OnIdle()`:
```cpp
void MarkAmpApp::OnIdle(wxIdleEvent& event)
{
    if (event_bus_)
    {
        event_bus_->process_queued();
        event_bus_->drain_fast_queue();
    }
    event.Skip();
}
```

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.h`

Add declaration:
```cpp
void OnIdle(wxIdleEvent& event);
```

### Task 3: Add event table binding for idle

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

Use `Bind()` in OnInit():
```cpp
Bind(wxEVT_IDLE, &MarkAmpApp::OnIdle, this);
```

### Task 4: Verify SPSC queue is functional

**File:** `/Users/ryanrentfro/code/markamp/src/core/EventBus.cpp`

Verify that `drain_fast_queue()` correctly processes all pending items:
```cpp
void EventBus::drain_fast_queue()
{
    std::function<void()> task;
    while (fast_queue_.try_dequeue(task))
    {
        if (task) task();
    }
}
```

## Acceptance Criteria

1. publish_fast() does not acquire the global mutex
2. process_queued() is called every idle cycle
3. drain_fast_queue() is called every idle cycle
4. Events queued via queue() are delivered within one idle cycle
5. No race conditions or crashes under concurrent publishing
6. All existing tests pass

## Testing Requirements

- Add test case: queue an event, call process_queued(), verify delivery
- Add test case: publish_fast() delivers to subscribers
- Stress test: concurrent publish_fast() from worker thread
- Existing test suite passes
