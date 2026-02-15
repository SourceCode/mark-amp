# Phase 36: Multi-Tier Safe Mode

## Metadata

| Field | Value |
|---|---|
| Phase ID | 36 |
| Prerequisites | Phase 04 (crash dump), Phase 31 (atomic writer for crash counter) |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 2 modified, 1 test |
| PRD Sections | PI-45 (safe mode startup), PII-36 (multi-tier safe mode), PII-37 (automatic safe mode trigger) |

---

## Objective

Implement safe mode startup with multiple tiers (`--safe`, `--safe-no-extensions`, `--safe-no-preview`, `--safe-minimal-ui`) and automatic safe mode trigger after 3 crashes within 5 minutes. Persist a crash counter across sessions.

---

## Background

The PRD mandates safe mode startup (PI-45), multi-tier safe mode (PII-36), and automatic safe mode trigger (PII-37). When MarkAmp crashes repeatedly, it should automatically restart in safe mode to break the crash loop. Safe mode disables features in tiers — from just disabling extensions (lightest) to minimal UI (heaviest). This gives both users and support tools a way to isolate problems.

---

## Scope

### Tasks

1. **Create `src/core/SafeMode.h` / `SafeMode.cpp`**:
   ```cpp
   namespace markamp::core {

   enum class SafeModeTier {
       Normal,          // Full functionality
       NoExtensions,    // Extensions disabled, everything else normal
       NoPreview,       // Extensions + preview disabled
       MinimalUI,       // Minimal: editor only, no sidebar, no minimap
       Full             // All of the above + default theme
   };

   struct CrashCounterState {
       size_t crash_count{0};
       std::chrono::system_clock::time_point first_crash;
       std::chrono::system_clock::time_point last_crash;
   };

   class SafeMode {
   public:
       explicit SafeMode(std::filesystem::path state_dir);  // ~/.markamp/

       // Parse command-line flags
       auto parse_flags(int argc, char* argv[]) -> SafeModeTier;

       // Check if auto-safe-mode should be triggered
       [[nodiscard]] auto should_auto_trigger() -> bool;

       // Get the effective safe mode tier
       [[nodiscard]] auto tier() const -> SafeModeTier;

       // Record a crash (called from crash barrier)
       void record_crash();

       // Record clean exit (resets crash counter)
       void record_clean_exit();

       // Load/save crash counter
       auto load_crash_counter() -> Result<void>;
       auto save_crash_counter() -> Result<void>;

       // Query what's disabled at current tier
       [[nodiscard]] auto extensions_enabled() const -> bool;
       [[nodiscard]] auto preview_enabled() const -> bool;
       [[nodiscard]] auto sidebar_enabled() const -> bool;
       [[nodiscard]] auto minimap_enabled() const -> bool;
       [[nodiscard]] auto use_default_theme() const -> bool;

   private:
       SafeModeTier tier_{SafeModeTier::Normal};
       CrashCounterState crash_counter_;
       std::filesystem::path state_dir_;

       static constexpr size_t kCrashThreshold = 3;
       static constexpr auto kCrashWindow = std::chrono::minutes{5};
   };

   } // namespace markamp::core
   ```

   **Safe mode tiers:**
   | Tier | Flag | Extensions | Preview | Sidebar | Minimap | Theme |
   |---|---|---|---|---|---|---|
   | Normal | (none) | Yes | Yes | Yes | Yes | User |
   | NoExtensions | `--safe-no-extensions` | No | Yes | Yes | Yes | User |
   | NoPreview | `--safe-no-preview` | No | No | Yes | Yes | User |
   | MinimalUI | `--safe-minimal-ui` | No | No | No | No | Default |
   | Full | `--safe` | No | No | No | No | Default |

   **Crash counter:**
   - Persisted to `~/.markamp/crash_counter.json`
   - Incremented on unclean shutdown (crash barrier activates)
   - Reset on clean exit (`MarkAmpApp::OnExit()`)
   - If 3 crashes within 5 minutes → auto-trigger full safe mode

2. **Modify `src/app/MarkAmpApp.cpp`**:
   - Initialize SafeMode early in `OnInit()`:
     ```cpp
     safe_mode_.load_crash_counter();
     auto tier = safe_mode_.parse_flags(argc_, argv_);
     if (safe_mode_.should_auto_trigger()) {
         tier = SafeModeTier::Full;
         MARKAMP_LOG_WARN("Auto-entering safe mode after repeated crashes");
     }
     ```
   - Apply tier restrictions: skip extension loading, disable preview panel, etc.
   - Show safe mode notification banner if not in Normal tier
   - On clean exit: `safe_mode_.record_clean_exit()`

3. **Modify `src/core/CrashBarrier.cpp`**:
   - In crash path: call `safe_mode_.record_crash()` and `safe_mode_.save_crash_counter()`
   - This ensures crash count is persisted even during crash

4. **Create `tests/unit/test_safe_mode.cpp`**:
   - TEST_CASE: "parse_flags returns Normal with no flags"
   - TEST_CASE: "parse_flags returns Full with --safe"
   - TEST_CASE: "parse_flags returns NoExtensions with --safe-no-extensions"
   - TEST_CASE: "Crash counter increments on record_crash"
   - TEST_CASE: "Crash counter resets on record_clean_exit"
   - TEST_CASE: "Auto-trigger after 3 crashes in 5 minutes"
   - TEST_CASE: "No auto-trigger if crashes outside window"
   - TEST_CASE: "Crash counter persists to file"
   - TEST_CASE: "Crash counter loads from file"
   - TEST_CASE: "extensions_enabled false at NoExtensions tier"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/SafeMode.h` |
| Create | `src/core/SafeMode.cpp` |
| Modify | `src/app/MarkAmpApp.cpp` |
| Modify | `src/core/CrashBarrier.cpp` |
| Create | `tests/unit/test_safe_mode.cpp` |

---

## Implementation Notes

- **Crash counter persistence**: Use atomic write (Phase 31) for the crash counter file. Format:
  ```json
  {"crash_count": 3, "first_crash": "2026-02-15T10:30:00Z", "last_crash": "2026-02-15T10:32:00Z"}
  ```
- **Crash counter in crash path**: The crash barrier must save the crash counter BEFORE generating the crash dump (which may itself fail). Use minimal I/O — just the counter file.
- **Auto-trigger logic**: If `crash_count >= 3` AND `last_crash - first_crash <= 5 minutes`, trigger safe mode. The window resets when crashes stop for >5 minutes.
- **Clean exit recording**: Called from `MarkAmpApp::OnExit()`. This resets the counter, breaking the crash loop detection.
- **Safe mode notification**: Show a prominent banner at the top of the editor: "MarkAmp is running in Safe Mode. Some features are disabled. [Learn More] [Exit Safe Mode]"
- **Exit safe mode**: "Exit Safe Mode" restarts the app without --safe flags. The clean restart also resets the crash counter.
- **Flag parsing**: Parse command-line args before wxWidgets initialization to determine safe mode tier early.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] `--safe` flag launches with all restrictions (extensions + preview + sidebar disabled, default theme)
- [ ] `--safe-no-extensions` launches with only extensions disabled
- [ ] Crash counter persists across sessions
- [ ] 3 rapid crashes within 5 minutes trigger auto-safe-mode on next launch
- [ ] Crashes outside the window do not trigger auto-safe-mode
- [ ] Clean exit resets crash counter
- [ ] Safe mode notification shown to user
- [ ] Feature queries (`extensions_enabled()`, etc.) reflect current tier
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test flag parsing with each individual flag and combinations
- Test crash counter persistence: write, restart, read back
- Test auto-trigger with simulated rapid crashes
- Test auto-trigger NOT triggered with spread-out crashes
- Test clean exit reset
- Test feature queries at each tier level
- Integration test: verify app starts with extensions disabled in safe mode
