# Phase 30: Error Reporting Service and Toast Notifications

## Metadata

| Field | Value |
|---|---|
| Phase ID | 30 |
| Prerequisites | Phase 01 (error types), Phase 03 (structured logging) |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 0 modified, 1 test |
| PRD Sections | PI-41 (central error reporting service), PI-42 (user-facing non-blocking error toasts), PII-3 (graceful feature degradation) |

---

## Objective

Build the central error reporting service that aggregates errors from all subsystems, deduplicates them, and presents user-facing non-blocking error toasts. This is the user-visible surface of the entire resilience system.

---

## Background

The PRD mandates a central error reporting service (PI-41), user-facing non-blocking error toasts (PI-42), and graceful feature degradation (PII-3). Currently errors are logged but not surfaced to the user in a structured way. The user has no visibility into extension failures, rendering issues, or configuration problems. This phase provides a single structured error sink with UI feedback.

---

## Scope

### Tasks

1. **Create `src/core/ErrorReportingService.h` / `ErrorReportingService.cpp`**:
   ```cpp
   namespace markamp::core {

   class ErrorReportingService {
   public:
       explicit ErrorReportingService(EventBus& bus, StructuredLogger& logger);

       // Report an error (logged only)
       void report(Error error);

       // Report an error and show toast to user
       void report_and_notify(Error error);

       // Get recent errors
       [[nodiscard]] auto recent_errors(size_t count = 50) const
           -> std::vector<Error>;

       // Get error count per subsystem
       [[nodiscard]] auto error_count_by_subsystem() const
           -> std::unordered_map<SubsystemId, size_t>;

       // Get total error count
       [[nodiscard]] auto total_error_count() const -> size_t;

       // Clear error history
       void clear();

       // Deduplication window (errors with same code+subsystem within window are merged)
       void set_dedup_window(std::chrono::seconds window);

   private:
       EventBus& bus_;
       StructuredLogger& logger_;
       BoundedVector<Error> error_history_;
       std::chrono::seconds dedup_window_{5};
       mutable std::mutex mutex_;

       auto is_duplicate(const Error& error) const -> bool;
   };

   } // namespace markamp::core
   ```
   - **Deduplication**: Errors with the same `ErrorCode` and `SubsystemId` within 5 seconds are merged (count incremented, not duplicated in history)
   - **History**: Bounded to 1000 entries (BoundedVector from Phase 15)
   - **Events**: Emits `ErrorReportedEvent` on each new error (for Health Panel, telemetry)

2. **Create `src/ui/ErrorToast.h` / `ErrorToast.cpp`**:
   ```cpp
   namespace markamp::ui {

   class ErrorToast : public wxPanel {
   public:
       ErrorToast(wxWindow* parent);

       // Show a toast notification
       void show_info(std::string_view message, std::chrono::seconds duration = std::chrono::seconds{5});
       void show_warning(std::string_view message, std::chrono::seconds duration = std::chrono::seconds{7});
       void show_error(std::string_view message, std::chrono::seconds duration = std::chrono::seconds{10});

       // Dismiss current toast
       void dismiss();

   private:
       wxStaticText* message_text_;
       wxTimer auto_dismiss_timer_;
       std::queue<std::pair<std::string, std::chrono::seconds>> pending_;

       void on_timer(wxTimerEvent& event);
       void show_next();
   };

   } // namespace markamp::ui
   ```
   - **Non-blocking**: Toast appears as overlay, does not block UI interaction
   - **Auto-dismiss**: Info = 5s, Warning = 7s, Error = 10s (configurable)
   - **Queue**: Multiple toasts queued and shown sequentially
   - **Styling**: Color-coded by severity (blue=info, yellow=warning, red=error)
   - **Position**: Bottom-right corner of the main window

3. **Create `tests/unit/test_error_reporting.cpp`**:
   - TEST_CASE: "report adds error to history"
   - TEST_CASE: "report_and_notify emits ErrorReportedEvent"
   - TEST_CASE: "Duplicate errors within window are merged"
   - TEST_CASE: "Duplicate errors outside window are separate"
   - TEST_CASE: "recent_errors returns latest N errors"
   - TEST_CASE: "error_count_by_subsystem aggregates correctly"
   - TEST_CASE: "Error history bounded to 1000 entries"
   - TEST_CASE: "clear resets history and counts"
   - TEST_CASE: "Total error count is accurate"
   - TEST_CASE: "Thread-safe concurrent reporting"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ErrorReportingService.h` |
| Create | `src/core/ErrorReportingService.cpp` |
| Create | `src/ui/ErrorToast.h` |
| Create | `src/ui/ErrorToast.cpp` |
| Create | `tests/unit/test_error_reporting.cpp` |

---

## Implementation Notes

- **Constructor injection**: `ErrorReportingService` takes `EventBus&` and `StructuredLogger&` via constructor, following the MarkAmp convention.
- **Deduplication logic**: Compare `ErrorCode` and `SubsystemId` of the new error against recent errors within the dedup window. If a match is found, increment a counter on the existing error rather than adding a new entry.
- **Toast UI**: Use wxWidgets `wxPanel` positioned as an overlay. Use `wxTimer` for auto-dismiss. The toast should be semi-transparent and should not steal focus.
- **Toast queue**: If multiple errors arrive rapidly, queue them and show one at a time. Each toast has its own duration.
- **Feature degradation (PII-3)**: When a subsystem reports multiple errors, the ErrorReportingService can emit `FeatureDisabledEvent` to disable the affected feature. This is wired in Phase 38 (fault domains).
- **Thread safety**: `ErrorReportingService` is called from any thread. The mutex protects the error history. Toast display must be posted to the UI thread via `wxTheApp->CallAfter()`.
- **Event**: Define `ErrorReportedEvent` in `Events.h`:
  ```cpp
  MARKAMP_DECLARE_EVENT(ErrorReportedEvent, Error error;)
  ```
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Duplicate errors within 5 seconds deduplicated (count incremented, not duplicated)
- [ ] Duplicate errors outside 5 seconds treated as separate
- [ ] Toast appears without blocking UI interaction
- [ ] Toast auto-dismisses after configured duration (5s info, 7s warning, 10s error)
- [ ] Multiple toasts queued and shown sequentially
- [ ] Error history bounded to 1000 entries
- [ ] `error_count_by_subsystem()` aggregates correctly
- [ ] Thread-safe concurrent error reporting
- [ ] `ErrorReportedEvent` emitted on new errors
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test deduplication with same error reported multiple times within window
- Test deduplication with different errors (should not deduplicate)
- Test history bounding with >1000 errors
- Test concurrent reporting from multiple threads
- Test error count aggregation
- Toast UI tested manually (wxWidgets UI tests are complex)
- Verify ErrorReportedEvent emission via EventBus subscription
