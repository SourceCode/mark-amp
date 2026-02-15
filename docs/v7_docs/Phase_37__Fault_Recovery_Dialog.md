# Phase 37: Fault Recovery Dialog

## Metadata

| Field | Value |
|---|---|
| Phase ID | 37 |
| Prerequisites | Phase 30 (error reporting), Phase 36 (safe mode) |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 0 modified, 1 test |
| PRD Sections | PII-38 (fault recovery dialog), PII-4 (self-healing state reset) |

---

## Objective

Build the fault recovery dialog that appears when a subsystem resets or the app recovers from a crash. The dialog provides the user with diagnostic information and recovery options: continue, restart in safe mode, view logs, or reset a specific subsystem.

---

## Background

The PRD mandates a fault recovery dialog (PII-38) and self-healing state reset (PII-4). When a subsystem (rendering, extension host, etc.) encounters repeated failures, the system needs to inform the user and offer recovery options. Without this dialog, the user sees mysterious behavior (features stop working) with no way to understand or fix the situation.

---

## Scope

### Tasks

1. **Create `src/ui/FaultRecoveryDialog.h` / `FaultRecoveryDialog.cpp`**:
   ```cpp
   namespace markamp::ui {

   class FaultRecoveryDialog : public wxDialog {
   public:
       enum class Action {
           Continue,        // Dismiss and keep working
           RestartSafe,     // Relaunch with --safe
           ViewLogs,        // Open log file
           ResetSubsystem,  // Reinitialize the faulted subsystem
           ExitApp          // Close the application
       };

       FaultRecoveryDialog(wxWindow* parent,
                           const core::SubsystemId subsystem,
                           const std::string& error_summary,
                           const std::vector<core::Error>& recent_errors);

       [[nodiscard]] auto selected_action() const -> Action;

   private:
       Action action_{Action::Continue};

       void build_ui(core::SubsystemId subsystem,
                     const std::string& error_summary,
                     const std::vector<core::Error>& recent_errors);

       void on_continue(wxCommandEvent& event);
       void on_restart_safe(wxCommandEvent& event);
       void on_view_logs(wxCommandEvent& event);
       void on_reset_subsystem(wxCommandEvent& event);
       void on_exit(wxCommandEvent& event);
   };

   } // namespace markamp::ui
   ```
   - **Dialog layout**:
     - Title: "Fault Recovery — {Subsystem Name}"
     - Icon: Warning icon
     - Error summary: 1-3 sentence description of what happened
     - Recent errors: Scrollable list of last 10 errors from the subsystem
     - Buttons: [Continue] [Restart in Safe Mode] [View Logs] [Reset {Subsystem}]
   - **Restart in Safe Mode**: Uses `wxExecute()` to relaunch the app with `--safe` flag, then exits current instance
   - **View Logs**: Opens the structured log file in the system's default text editor
   - **Reset Subsystem**: Calls `SubsystemReset::reset()` for the affected subsystem

2. **Create `src/core/SubsystemReset.h` / `SubsystemReset.cpp`**:
   ```cpp
   namespace markamp::core {

   class SubsystemReset {
   public:
       // Register a reset handler for a subsystem
       void register_reset_handler(SubsystemId subsystem,
                                    std::function<Result<void>()> handler);

       // Reset a specific subsystem
       [[nodiscard]] auto reset(SubsystemId subsystem) -> Result<void>;

       // Check if a subsystem has a reset handler
       [[nodiscard]] auto can_reset(SubsystemId subsystem) const -> bool;

   private:
       std::unordered_map<SubsystemId, std::function<Result<void>()>> handlers_;
   };

   } // namespace markamp::core
   ```
   - **Reset handlers registered by subsystems:**
     - ExtensionHost: deactivate all → clear state → reactivate healthy (Phase 26)
     - Rendering: clear theme cache → reload theme → repaint
     - Config: reload config from disk → apply defaults for missing values
     - Workspace: reindex files → rebuild caches
   - Reset is non-destructive: it reinitializes the subsystem without losing user data

3. **Create `tests/unit/test_fault_recovery.cpp`**:
   - TEST_CASE: "SubsystemReset can_reset returns true for registered subsystem"
   - TEST_CASE: "SubsystemReset can_reset returns false for unregistered"
   - TEST_CASE: "SubsystemReset reset calls handler"
   - TEST_CASE: "SubsystemReset reset returns error if handler fails"
   - TEST_CASE: "FaultRecoveryDialog default action is Continue"
   - TEST_CASE: "FaultRecoveryDialog shows error summary"
   - TEST_CASE: "FaultRecoveryDialog shows recent errors"
   - TEST_CASE: "Multiple subsystem resets can be registered"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/ui/FaultRecoveryDialog.h` |
| Create | `src/ui/FaultRecoveryDialog.cpp` |
| Create | `src/core/SubsystemReset.h` |
| Create | `src/core/SubsystemReset.cpp` |
| Create | `tests/unit/test_fault_recovery.cpp` |

---

## Implementation Notes

- **Modal dialog**: `FaultRecoveryDialog` is a modal dialog (`wxDialog::ShowModal()`). It blocks the current flow until the user makes a choice. This is appropriate because the subsystem is in a faulted state.
- **Error summary**: Generated by the error reporting service (Phase 30). Include: subsystem name, error count, last error message.
- **Recent errors**: Display the last 10 errors from the faulted subsystem. Each shows: timestamp, error code, message (truncated to 100 chars).
- **Restart in Safe Mode**: Use `wxExecute()` to launch a new instance with `--safe` flag. Then call `wxTheApp->ExitMainLoop()` to close the current instance. On macOS, use `wxExecute()` with the app bundle path.
- **View Logs**: Open `~/.markamp/logs/markamp.json` with `wxLaunchDefaultApplication()`.
- **Reset handler isolation**: Wrap the reset handler in a try/catch. If the reset itself fails, log the error and offer "Restart in Safe Mode" as the only remaining option.
- **Self-healing (PII-4)**: The subsystem reset IS the self-healing mechanism. It detects corruption, resets to clean state, and reinitializes.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] Dialog appears when subsystem fault detected
- [ ] Dialog shows subsystem name, error summary, and recent errors
- [ ] "Continue" dismisses the dialog and resumes operation
- [ ] "Restart in Safe Mode" relaunches the app with --safe flag
- [ ] "View Logs" opens the log file in system editor
- [ ] "Reset {Subsystem}" reinitializes the faulted subsystem
- [ ] Subsystem reset is non-destructive (no data loss)
- [ ] Reset failure offers safe mode restart as fallback
- [ ] All 8+ test cases pass

---

## Testing Strategy

- Test SubsystemReset registration and invocation
- Test SubsystemReset error handling (handler that throws)
- Test FaultRecoveryDialog construction with various error states
- Dialog UI tested manually (wxWidgets dialog testing is complex)
- Test restart in safe mode path (verify correct flags passed)
- Test view logs path (verify correct file path used)
