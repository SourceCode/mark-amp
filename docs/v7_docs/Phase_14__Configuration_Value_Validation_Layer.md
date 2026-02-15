# Phase 14: Configuration Value Validation Layer

## Metadata

| Field | Value |
|---|---|
| Phase ID | 14 |
| Prerequisites | Phase 02, Phase 07, Phase 11 |
| Estimated Complexity | Low |
| Estimated File Count | 2 created, 1 modified, 1 test |
| PRD Sections | PI-8 (all external data untrusted), PI-14 (enum validation), PI-13 (numeric clamping) |

---

## Objective

Add comprehensive validation to `Config::set()` to reject invalid values before they are persisted and propagated through the event system. This is the last line of defense against corrupted configuration values.

---

## Background

The PRD mandates that all external data is treated as untrusted (PI-8) and enum strings are validated during deserialization (PI-14). The Config system accepts values from YAML files, extension APIs, user settings UI, and command-line arguments. Each of these is an attack surface. This phase adds a validation layer at the `Config::set()` boundary that rejects invalid values before they can cause downstream failures.

---

## Scope

### Tasks

1. **Create `src/core/ConfigValidator.h` / `ConfigValidator.cpp`**:
   ```cpp
   namespace markamp::core {

   class ConfigValidator {
   public:
       // Validate a config value before setting
       [[nodiscard]] auto validate(std::string_view key, const ConfigValue& value)
           -> Result<ConfigValue>;

       // Register a validation rule for a config key
       void register_rule(std::string_view key, std::function<Result<ConfigValue>(const ConfigValue&)> rule);

       // Load default rules for all known config keys
       void load_default_rules();

   private:
       std::unordered_map<std::string, std::function<Result<ConfigValue>(const ConfigValue&)>> rules_;
   };

   } // namespace markamp::core
   ```

   **Default validation rules:**
   | Config Key | Validation |
   |---|---|
   | `theme` | Non-empty string, max 256 chars |
   | `view_mode` | Enum: `"split"`, `"editor"`, `"preview"` |
   | `font_size` | Integer, clamp 6-128 |
   | `font_family` | Non-empty string, max 256 chars |
   | `tab_size` | Integer, clamp 1-16 |
   | `auto_save` | Boolean |
   | `auto_save_interval_seconds` | Integer, clamp 5-3600 |
   | `word_wrap` | Boolean |
   | `word_wrap_column` | Integer, clamp 20-1000 |
   | `line_numbers` | Boolean |
   | `spell_check` | Boolean |
   | `cursor_style` | Enum: `"block"`, `"line"`, `"underline"` |
   | `cursor_width` | Integer, clamp 1-10 |
   | `cursor_blink` | Boolean |
   | `minimap_enabled` | Boolean |
   | `minimap_side` | Enum: `"right"`, `"left"` |
   | `highlight_current_line` | Boolean |
   | `bracket_matching` | Boolean |
   | `edge_column` | Integer, clamp 1-500 |
   | `zoom_level` | Integer, clamp 25-500 |
   | `letter_spacing` | Double, clamp -5.0 to 20.0 |
   | `line_height` | Double, clamp 0.5 to 5.0 |

2. **Modify `src/core/Config.cpp`**:
   - Integrate `ConfigValidator` into `set_string()`, `set_int()`, `set_double()`, `set_bool()`
   - On validation failure: log warning, reject the value, keep existing value
   - On clamping: log info, accept clamped value
   - Initialize ConfigValidator with default rules during Config construction

3. **Create `tests/unit/test_config_validator.cpp`**:
   - TEST_CASE: "Empty theme ID rejected"
   - TEST_CASE: "Valid theme ID accepted"
   - TEST_CASE: "Invalid view_mode enum rejected"
   - TEST_CASE: "Valid view_mode enum accepted"
   - TEST_CASE: "font_size below min clamped to 6"
   - TEST_CASE: "font_size above max clamped to 128"
   - TEST_CASE: "font_size in range passes through"
   - TEST_CASE: "Invalid cursor_style rejected"
   - TEST_CASE: "Boolean config accepts true/false"
   - TEST_CASE: "Unknown config key passes through without validation"

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ConfigValidator.h` |
| Create | `src/core/ConfigValidator.cpp` |
| Modify | `src/core/Config.cpp` |
| Create | `tests/unit/test_config_validator.cpp` |

---

## Implementation Notes

- **Validation at set(), not get()**: Phase 11 clamps at `get_int()`/`get_double()`. This phase validates at `set()`. Both layers are needed: set() catches invalid values from external sources, get() handles values that were stored before validation was added.
- **Unknown keys pass through**: Config keys not in the validation rules table are accepted without validation. This ensures extensibility — plugins can add custom config keys.
- **Enum validation**: Use `validate_enum_string()` from Phase 02. Config enum values are case-insensitive.
- **Numeric clamping**: Use `clamp_config()` from Phase 11. Clamped values are accepted (not rejected) with a log info.
- **String validation**: Use `validate_string_length()` from Phase 02 and `validate_not_empty()` for required strings.
- **Boolean validation**: Accept `true`/`false` string values (case-insensitive) and actual bool values.
- **Event propagation**: Rejected values should NOT fire `SettingChangedEvent`. Only validated/clamped values propagate.
- Update `src/CMakeLists.txt` and `tests/CMakeLists.txt`.

---

## Acceptance Criteria

- [ ] `config.set("theme", "")` rejected (empty theme ID not persisted)
- [ ] `config.set("view_mode", "banana")` rejected (invalid enum)
- [ ] `config.set("font_size", -5)` clamped to 6 and accepted
- [ ] `config.set("font_size", 72)` accepted without modification
- [ ] Valid values pass through unchanged
- [ ] Unknown config keys accepted without validation
- [ ] Rejected values do not fire SettingChangedEvent
- [ ] Clamped values fire SettingChangedEvent with clamped value
- [ ] Validation logs warning on rejection, info on clamping
- [ ] All 10+ test cases pass

---

## Testing Strategy

- Test each validation rule with valid, invalid, and boundary values
- Test enum validation is case-insensitive
- Test that rejected values don't overwrite existing valid values
- Test that unknown config keys pass through
- Verify event propagation behavior (rejected = no event, clamped = event with clamped value)
- Integration test: load a config file with invalid values, verify app starts with clamped defaults
