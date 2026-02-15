# Phase 17: Async Task Cancellation Tokens

## Metadata

| Field | Value |
|---|---|
| Phase ID | 17 |
| Prerequisites | Phase 06 (thread ownership) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 4 modified, 1 test |
| PRD Sections | PI-27 (async task cancellation tokens), PII-14 (safe task cancellation enforcement) |

---

## Objective

Implement a universal `CancellationToken` pattern for all async operations, replacing the per-class `std::atomic<bool>` pattern currently used in `AsyncFileLoader`. The token supports cancellation callbacks, thread-safe cancellation, and multiple consumers sharing one token.

---

## Background

The PRD mandates async task cancellation tokens (PI-27) and safe task cancellation enforcement (PII-14). Currently `AsyncFileLoader` uses a raw `std::atomic<bool> cancelled_` field. This pattern is duplicated in `AsyncHighlighter` and other async components. A unified `CancellationToken`/`CancellationTokenSource` pattern (inspired by .NET's pattern) provides a consistent, composable cancellation mechanism.

---

## Scope

### Tasks

1. **Create `src/core/CancellationToken.h` / `CancellationToken.cpp`**:
   ```cpp
   namespace markamp::core {

   class CancellationToken;

   class CancellationTokenSource {
   public:
       CancellationTokenSource();
       ~CancellationTokenSource();

       // Non-copyable, movable
       CancellationTokenSource(const CancellationTokenSource&) = delete;
       CancellationTokenSource& operator=(const CancellationTokenSource&) = delete;
       CancellationTokenSource(CancellationTokenSource&&) noexcept;
       CancellationTokenSource& operator=(CancellationTokenSource&&) noexcept;

       // Get a token linked to this source
       [[nodiscard]] auto token() const -> CancellationToken;

       // Cancel all associated tokens
       void cancel();

       // Check if cancelled
       [[nodiscard]] auto is_cancelled() const -> bool;

   private:
       struct Impl;
       std::shared_ptr<Impl> impl_;
   };

   class CancellationToken {
   public:
       // Check if cancellation has been requested
       [[nodiscard]] auto is_cancelled() const -> bool;

       // Throw if cancelled (for use in loops)
       void throw_if_cancelled() const;

       // Register a callback to be called on cancellation
       // Returns a subscription that unregisters on destruction
       [[nodiscard]] auto on_cancelled(std::function<void()> callback) -> std::uint64_t;

       // Unregister a callback
       void remove_callback(std::uint64_t id);

       // Static none token (never cancelled)
       static auto none() -> CancellationToken;

   private:
       friend class CancellationTokenSource;
       struct Impl;
       std::shared_ptr<const Impl> impl_;
       explicit CancellationToken(std::shared_ptr<const Impl> impl);
   };

   } // namespace markamp::core
   ```
   - `CancellationTokenSource` owns the cancellation state
   - `CancellationToken` is a lightweight, copyable view
   - Cancellation is thread-safe (internal atomic flag)
   - Callbacks fire synchronously on the thread that calls `cancel()`
   - `throw_if_cancelled()` throws `CancellationException` (derived from `std::exception`)
   - `CancellationToken::none()` returns a token that never gets cancelled (for optional cancellation parameters)

2. **Modify `src/core/AsyncFileLoader.h` / `AsyncFileLoader.cpp`**:
   - Replace `std::atomic<bool> cancelled_` with `CancellationTokenSource`
   - Accept `CancellationToken` in async methods
   - Check `token.is_cancelled()` at regular intervals during file loading

3. **Modify `src/core/AsyncHighlighter.h` / (if exists, AsyncHighlighter.cpp)**:
   - Adopt CancellationToken for highlight operations

4. **Create `tests/unit/test_cancellation_token.cpp`**:
   - TEST_CASE: "Token starts uncancelled"
   - TEST_CASE: "Token reflects source cancellation"
   - TEST_CASE: "Multiple tokens from same source"
   - TEST_CASE: "Callback fires on cancellation"
   - TEST_CASE: "Multiple callbacks fire on cancellation"
   - TEST_CASE: "Callback removed before cancellation does not fire"
   - TEST_CASE: "throw_if_cancelled throws when cancelled"
   - TEST_CASE: "throw_if_cancelled does not throw when active"
   - TEST_CASE: "CancellationToken::none() never cancels"
   - TEST_CASE: "Token is thread-safe (concurrent cancel + check)"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/CancellationToken.h` |
| Create | `src/core/CancellationToken.cpp` |
| Modify | `src/core/AsyncFileLoader.h` |
| Modify | `src/core/AsyncFileLoader.cpp` |
| Modify | `src/core/AsyncHighlighter.h` |
| Modify | `src/core/AsyncHighlighter.cpp` |
| Create | `tests/unit/test_cancellation_token.cpp` |

---

## Implementation Notes

- **Shared state**: `CancellationTokenSource::Impl` contains an `std::atomic<bool>` for the cancelled flag and a `std::vector<std::function<void()>>` for callbacks. Protect the callback list with a `std::mutex`.
- **Callback invocation**: When `cancel()` is called, acquire mutex, copy callbacks, release mutex, then invoke callbacks. This prevents deadlocks from callbacks that unregister themselves.
- **Token lifetime**: Tokens hold a `shared_ptr` to the impl, so tokens remain valid even after the source is destroyed (they just read the atomic flag).
- **CancellationException**: Define as `class CancellationException : public std::exception` with `what()` returning "Operation cancelled". Catch this in crash barriers.
- **Backward compatibility**: AsyncFileLoader's public API should accept an optional `CancellationToken` parameter with default `CancellationToken::none()`.
- **Performance**: `is_cancelled()` is a single atomic load — suitable for tight loops.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Token cancellation is thread-safe (atomic reads/writes)
- [ ] Callback fires synchronously on cancellation
- [ ] Multiple callbacks from different threads work correctly
- [ ] Callback removal prevents firing
- [ ] `throw_if_cancelled()` throws `CancellationException` when cancelled
- [ ] `CancellationToken::none()` never cancels
- [ ] AsyncFileLoader uses CancellationToken instead of raw atomic
- [ ] Multiple consumers can share one token
- [ ] Token remains valid after source is destroyed
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test single-threaded cancellation flow
- Test multi-threaded cancellation (cancel from one thread, check from another)
- Test callback ordering and removal
- Stress test: create 1000 tokens from one source, cancel, verify all reflect cancellation
- Test AsyncFileLoader integration: start load, cancel, verify clean shutdown
- Run under TSan to verify thread safety
