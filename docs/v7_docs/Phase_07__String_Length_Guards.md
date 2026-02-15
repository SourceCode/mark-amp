# Phase 07: String Length Guards

## Metadata

| Field | Value |
|---|---|
| Phase ID | 07 |
| Prerequisites | Phase 02 (validation utilities) |
| Estimated Complexity | Low |
| Estimated File Count | 2 created, 3 modified, 1 test |
| PRD Sections | PI-9 (length guards on all strings) |

---

## Objective

Enforce maximum length limits on all user-facing strings to prevent memory exhaustion and display corruption. Provide `truncate_safe()` and `validate_display_string()` utilities with centralized length constants for each context.

---

## Background

The PRD mandates length guards on all strings (PI-9) including output channel messages, YAML keys, notification content, and command palette entries. Without length guards, a malicious or corrupted extension/theme could inject megabyte-sized strings into UI components, causing memory exhaustion or rendering hangs.

---

## Scope

### Tasks

1. **Create `src/core/StringGuards.h` / `StringGuards.cpp`**:
   ```cpp
   namespace markamp::core {

   // Length limit constants
   namespace string_limits {
       constexpr size_t kOutputChannelMessage = 1 * 1024 * 1024;   // 1 MB
       constexpr size_t kYamlKey = 256;
       constexpr size_t kNotificationMessage = 4096;
       constexpr size_t kCommandPaletteEntry = 512;
       constexpr size_t kConfigValueString = 64 * 1024;            // 64 KB
       constexpr size_t kExtensionId = 256;
       constexpr size_t kFilePath = 4096;
       constexpr size_t kThemeTokenName = 128;
       constexpr size_t kSearchQuery = 4096;
   }

   // Truncate string to max_len, appending "[truncated]" if truncated
   [[nodiscard]] auto truncate_safe(std::string_view input, size_t max_len) -> std::string;

   // Validate and optionally truncate a display string
   [[nodiscard]] auto validate_display_string(std::string_view input,
                                               size_t max_len,
                                               std::string_view context = "") -> Result<std::string>;

   // Apply length guard in-place (modifies string if too long)
   void apply_length_guard(std::string& value, size_t max_len,
                           std::string_view context = "");

   } // namespace markamp::core
   ```
   - `truncate_safe()`: UTF-8 aware truncation (does not cut in middle of multi-byte character)
   - `validate_display_string()`: returns error if input exceeds limit and cannot be safely truncated
   - `apply_length_guard()`: truncates in-place and logs at DEBUG level

2. **Modify `src/core/OutputChannelService.cpp`**:
   - Apply `string_limits::kOutputChannelMessage` to all incoming messages via `apply_length_guard()`

3. **Modify `src/core/NotificationService.cpp`**:
   - Apply `string_limits::kNotificationMessage` to notification text
   - Apply length guard to notification title (max 256 chars)

4. **Modify `src/core/Command.cpp`**:
   - Apply `string_limits::kCommandPaletteEntry` to command labels and descriptions

5. **Create `tests/unit/test_string_guards.cpp`**:
   - TEST_CASE: "truncate_safe preserves short strings"
   - TEST_CASE: "truncate_safe truncates long strings with suffix"
   - TEST_CASE: "truncate_safe handles UTF-8 correctly"
   - TEST_CASE: "truncate_safe handles empty string"
   - TEST_CASE: "validate_display_string accepts valid strings"
   - TEST_CASE: "validate_display_string reports oversized strings"
   - TEST_CASE: "apply_length_guard modifies oversized string in-place"
   - TEST_CASE: "apply_length_guard preserves short strings"
   - TEST_CASE: "OutputChannel message truncated at 1MB"
   - TEST_CASE: "Notification message truncated at 4096 chars"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/StringGuards.h` |
| Create | `src/core/StringGuards.cpp` |
| Modify | `src/core/OutputChannelService.cpp` |
| Modify | `src/core/NotificationService.cpp` |
| Modify | `src/core/Command.cpp` |
| Create | `tests/unit/test_string_guards.cpp` |

---

## Implementation Notes

- **UTF-8 awareness**: When truncating, check if the cut point is in the middle of a multi-byte UTF-8 sequence. If so, back up to the start of the character. A UTF-8 continuation byte has the pattern `10xxxxxx` (0x80-0xBF).
- **Truncation suffix**: Append `" [truncated]"` (12 chars). The `max_len` includes the suffix, so the actual content limit is `max_len - 12`.
- **Logging**: Use `MARKAMP_LOG_DEBUG` when truncation occurs. Include the context name and original length for diagnostics.
- **Performance**: `apply_length_guard()` should be O(1) for strings under the limit (just a length check).
- **Existing functionality**: These guards are additive. All existing strings within normal limits pass through unchanged.
- Update `src/CMakeLists.txt` to add `StringGuards.cpp`.
- Update `tests/CMakeLists.txt` to add test target.

---

## Acceptance Criteria

- [ ] Strings exceeding limits are truncated with `" [truncated]"` suffix
- [ ] UTF-8 multi-byte characters not broken by truncation
- [ ] Truncation logged at DEBUG level with context name
- [ ] Existing functionality unchanged for normal-length strings
- [ ] OutputChannelService enforces 1 MB message limit
- [ ] NotificationService enforces 4096 char message limit
- [ ] Command palette entries enforce 512 char limit
- [ ] All limit constants defined in `string_limits` namespace
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Unit test each function with valid strings (under limit), boundary strings (at limit), and oversized strings (over limit)
- Test UTF-8 edge cases: multi-byte characters at truncation boundary, mixed ASCII/UTF-8
- Test empty string handling
- Verify log output on truncation
- Integration test with OutputChannelService to verify end-to-end truncation
