# Phase 18 -- Attribute View Column Types (17 Key Types)

**Priority:** High (required for all AV rendering phases)
**Estimated Scope:** ~6 new files, ~4 modified files
**Dependencies:** Phase 17 (Attribute View Data Model)

## Objective

Implement the full type system for all 17 Attribute View column types (KeyTypes). Each type requires specific value representation, input validation, output formatting, comparison logic for sorting, and filter operator support. This phase transforms the raw data model from Phase 17 into a fully typed, validated, and formattable column system.

The type system serves as the foundation for:
- Cell rendering (Phase 19) -- determines how each cell displays its value
- Filtering and sorting (Phase 20) -- determines valid operators and comparison semantics
- Gallery/Kanban card fields (Phases 21-22) -- determines field display on cards
- Relations/Rollups (Phase 23) -- determines aggregation and linking behavior

## Prerequisites

- **Phase 17** -- Attribute View Core Data Model (`AVKeyType`, `AVValue`, `AVValueVariant`, all per-type structs)

## SiYuan Source Reference

| SiYuan File | Purpose | Key Functions |
|---|---|---|
| `kernel/av/value.go` | `Value.String(format)`, all `Value*` sub-structs, type-specific string conversion | `func (value *Value) String(...)` per type |
| `kernel/av/sort.go` | Comparison function dispatching by type | `func (value *Value) Compare(other *Value)` |
| `kernel/av/filter.go` | Filter operator definitions per type, filter evaluation | `func (value *Value) Filter(operator, operand)` |
| `kernel/av/key.go` | `NumberFormat` formatting, `SelectOption` color mapping | `func (key *Key) FormatNumber(v float64)` |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/core/av/AVColumnType.h` | `markamp::core::av` | `IAVColumnTypeHandler` interface and `AVColumnTypeRegistry` |
| `src/core/av/AVColumnType.cpp` | `markamp::core::av` | Registry implementation with all 17 handler registrations |
| `src/core/av/AVFormatters.h` | `markamp::core::av` | Number formatting, date formatting, display string utilities |
| `src/core/av/AVFormatters.cpp` | `markamp::core::av` | Implementation of all formatting logic |
| `src/core/av/AVValidators.h` | `markamp::core::av` | Per-type validation functions |
| `src/core/av/AVValidators.cpp` | `markamp::core::av` | Validation implementations |
| `tests/unit/test_av_column_types.cpp` | (anonymous) | Catch2 tests |

### Files to Modify

| File | Change |
|---|---|
| `src/core/av/AVValue.h` | Add `format()` and `compare()` method declarations |
| `src/core/av/AVValue.cpp` | Implement formatting and comparison delegating to column type handlers |
| `src/CMakeLists.txt` | Add new `.cpp` sources |
| `tests/CMakeLists.txt` | Add `test_av_column_types` test target |

## Data Structures to Implement

### File: `src/core/av/AVColumnType.h`

```cpp
#pragma once

#include "AVTypes.h"
#include "AVValue.h"

#include <expected>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core::av
{

// ============================================================================
// Filter Operator Enumeration
// ============================================================================

enum class AVFilterOperator
{
    // Universal operators
    IsEmpty,
    IsNotEmpty,

    // Text / URL / Email / Phone operators
    IsEqual,
    IsNotEqual,
    Contains,
    DoesNotContain,
    StartsWith,
    EndsWith,

    // Number operators
    NumEqual,
    NumNotEqual,
    GreaterThan,
    GreaterThanOrEqual,
    LessThan,
    LessThanOrEqual,

    // Date operators
    DateEqual,
    DateBefore,
    DateAfter,
    DateOnOrBefore,
    DateOnOrAfter,
    DateBetween,
    DateIsRelative,    // Past week, this month, next year, etc.

    // Select operators
    SelectIs,
    SelectIsNot,

    // Checkbox operators
    IsTrue,
    IsFalse,

    // Relation operators
    RelationContains,
    RelationDoesNotContain
};

[[nodiscard]] auto filter_operator_to_string(AVFilterOperator op) -> std::string;
[[nodiscard]] auto string_to_filter_operator(const std::string& str) -> AVFilterOperator;

// ============================================================================
// Sort Order
// ============================================================================

enum class AVSortOrder
{
    Ascending,
    Descending
};

// ============================================================================
// Validation Result
// ============================================================================

struct AVValidationResult
{
    bool valid{true};
    std::string error_message;

    [[nodiscard]] static auto ok() -> AVValidationResult
    {
        return {true, {}};
    }

    [[nodiscard]] static auto error(std::string msg) -> AVValidationResult
    {
        return {false, std::move(msg)};
    }
};

// ============================================================================
// Column Type Handler Interface
// ============================================================================

/// Interface for type-specific behavior. One handler instance per AVKeyType.
/// Implements the Strategy pattern: the registry dispatches to the correct
/// handler based on the column type.
class IAVColumnTypeHandler
{
public:
    virtual ~IAVColumnTypeHandler() = default;

    /// The key type this handler is responsible for.
    [[nodiscard]] virtual auto key_type() const -> AVKeyType = 0;

    /// Validate that a value is well-formed for this column type.
    [[nodiscard]] virtual auto validate(const AVValue& value) const
        -> AVValidationResult = 0;

    /// Format a value for display. Uses the key's configuration (e.g. NumberFormat).
    [[nodiscard]] virtual auto format(const AVValue& value,
                                       const AVKey& key) const -> std::string = 0;

    /// Compare two values for sorting. Returns <0, 0, or >0.
    [[nodiscard]] virtual auto compare(const AVValue& a,
                                        const AVValue& b) const -> int = 0;

    /// Evaluate a filter condition against a value.
    [[nodiscard]] virtual auto filter(const AVValue& value,
                                       AVFilterOperator op,
                                       const AVValue& operand) const -> bool = 0;

    /// Return the set of filter operators supported by this type.
    [[nodiscard]] virtual auto supported_operators() const
        -> std::vector<AVFilterOperator> = 0;

    /// Convert a raw string input into a typed AVValueVariant.
    /// Used for cell editing: user types a string, we parse it into the correct type.
    [[nodiscard]] virtual auto parse_input(const std::string& input,
                                            const AVKey& key) const
        -> std::expected<AVValueVariant, std::string> = 0;

    /// Return a default empty value for this type.
    [[nodiscard]] virtual auto default_value() const -> AVValueVariant = 0;
};

// ============================================================================
// Column Type Registry
// ============================================================================

/// Singleton registry mapping AVKeyType -> IAVColumnTypeHandler.
/// Populated at startup with all 17 built-in type handlers.
class AVColumnTypeRegistry
{
public:
    AVColumnTypeRegistry();

    /// Get the handler for a key type. Never returns nullptr for valid types.
    [[nodiscard]] auto get_handler(AVKeyType type) const -> const IAVColumnTypeHandler*;

    /// Convenience: format a value using its key's type.
    [[nodiscard]] auto format_value(const AVValue& value,
                                     const AVKey& key) const -> std::string;

    /// Convenience: compare two values of the same type.
    [[nodiscard]] auto compare_values(const AVValue& a,
                                       const AVValue& b,
                                       AVKeyType type) const -> int;

    /// Convenience: validate a value against its key's type.
    [[nodiscard]] auto validate_value(const AVValue& value,
                                       const AVKey& key) const -> AVValidationResult;

    /// Convenience: evaluate a filter.
    [[nodiscard]] auto evaluate_filter(const AVValue& value,
                                        AVFilterOperator op,
                                        const AVValue& operand,
                                        AVKeyType type) const -> bool;

private:
    std::unordered_map<AVKeyType, std::unique_ptr<IAVColumnTypeHandler>> handlers_;

    void register_all_handlers();
};

} // namespace markamp::core::av
```

### File: `src/core/av/AVFormatters.h`

```cpp
#pragma once

#include "AVTypes.h"
#include "AVValue.h"

#include <cstdint>
#include <string>

namespace markamp::core::av
{

// ============================================================================
// Number Formatting
// ============================================================================

/// Format a number according to the specified format.
/// Examples:
///   format_number(1234567.89, AVNumberFormat::Commas)   -> "1,234,567.89"
///   format_number(0.75, AVNumberFormat::Percent)         -> "75%"
///   format_number(1234.56, AVNumberFormat::USDollar)     -> "$1,234.56"
[[nodiscard]] auto format_number(double value,
                                  AVNumberFormat format,
                                  const std::string& custom_format = "") -> std::string;

// ============================================================================
// Date Formatting
// ============================================================================

/// Format a Unix timestamp as a human-readable date string.
/// If is_not_time is true, only the date portion is shown.
[[nodiscard]] auto format_date(int64_t timestamp,
                                bool is_not_time = false) -> std::string;

/// Format a date range (start to end).
[[nodiscard]] auto format_date_range(int64_t start,
                                      int64_t end,
                                      bool is_not_time = false) -> std::string;

// ============================================================================
// Select Option Formatting
// ============================================================================

/// Format a single select option as "name" (color is metadata, not displayed as text).
[[nodiscard]] auto format_select(const AVSelectOption& option) -> std::string;

/// Format multiple select options as comma-separated names.
[[nodiscard]] auto format_mselect(const std::vector<AVSelectOption>& options) -> std::string;

// ============================================================================
// URL Formatting
// ============================================================================

/// Generate a "smart URL" display from a full URL.
/// Example: "https://www.example.com/path/to/page?query=1" -> "example.com/.../page"
[[nodiscard]] auto smart_url(const std::string& full_url) -> std::string;

// ============================================================================
// Asset Formatting
// ============================================================================

/// Format an asset entry for display (filename or URL basename).
[[nodiscard]] auto format_asset(const AVValueAssetEntry& asset) -> std::string;

/// Format multiple assets as comma-separated display strings.
[[nodiscard]] auto format_masset(const std::vector<AVValueAssetEntry>& assets) -> std::string;

// ============================================================================
// General Display String
// ============================================================================

/// Master formatting function: dispatches to type-specific formatters.
[[nodiscard]] auto format_value_for_display(const AVValue& value,
                                             const AVKey& key) -> std::string;

} // namespace markamp::core::av
```

### File: `src/core/av/AVValidators.h`

```cpp
#pragma once

#include "AVTypes.h"
#include "AVValue.h"
#include "AVColumnType.h"

#include <string>

namespace markamp::core::av
{

// ============================================================================
// Per-Type Validation Functions
// ============================================================================

/// Validate that a value's variant type matches its declared AVKeyType.
[[nodiscard]] auto validate_type_match(const AVValue& value) -> AVValidationResult;

/// Validate a text value (non-null, within max length).
[[nodiscard]] auto validate_text(const AVValueText& text,
                                  int max_length = 65536) -> AVValidationResult;

/// Validate a number value (finite, within range).
[[nodiscard]] auto validate_number(const AVValueNumber& number) -> AVValidationResult;

/// Validate a date value (valid timestamp, end >= start if has_end_date).
[[nodiscard]] auto validate_date(const AVValueDate& date) -> AVValidationResult;

/// Validate a URL value (well-formed URL syntax).
[[nodiscard]] auto validate_url(const AVValueURL& url) -> AVValidationResult;

/// Validate an email value (basic email format check).
[[nodiscard]] auto validate_email(const AVValueEmail& email) -> AVValidationResult;

/// Validate a phone value (digits, spaces, dashes, plus, parens allowed).
[[nodiscard]] auto validate_phone(const AVValuePhone& phone) -> AVValidationResult;

/// Validate a select value (option name exists in key's options list).
[[nodiscard]] auto validate_select(const AVValueSelect& select,
                                    const AVKey& key) -> AVValidationResult;

/// Validate a multi-select value (all option names exist in key's options list).
[[nodiscard]] auto validate_mselect(const AVValueMSelect& mselect,
                                     const AVKey& key) -> AVValidationResult;

/// Validate asset entries (non-empty name and content).
[[nodiscard]] auto validate_masset(const AVValueMAsset& masset) -> AVValidationResult;

/// Validate a relation value (block IDs are non-empty strings).
[[nodiscard]] auto validate_relation(const AVValueRelation& relation) -> AVValidationResult;

} // namespace markamp::core::av
```

## Key Functions to Implement

### AVColumnType.cpp -- 17 Handler Implementations

Each handler class follows this pattern (shown for TextColumnHandler):

```cpp
class TextColumnHandler : public IAVColumnTypeHandler
{
public:
    [[nodiscard]] auto key_type() const -> AVKeyType override
    {
        return AVKeyType::Text;
    }

    [[nodiscard]] auto validate(const AVValue& value) const -> AVValidationResult override
    {
        // Verify variant holds AVValueText
        // Call validate_text() from AVValidators
    }

    [[nodiscard]] auto format(const AVValue& value,
                               const AVKey& /*key*/) const -> std::string override
    {
        // Return text content directly
    }

    [[nodiscard]] auto compare(const AVValue& a, const AVValue& b) const -> int override
    {
        // Case-insensitive string comparison
    }

    [[nodiscard]] auto filter(const AVValue& value,
                               AVFilterOperator op,
                               const AVValue& operand) const -> bool override
    {
        // Switch on operator: Contains, StartsWith, EndsWith, etc.
    }

    [[nodiscard]] auto supported_operators() const
        -> std::vector<AVFilterOperator> override
    {
        return {
            AVFilterOperator::IsEqual, AVFilterOperator::IsNotEqual,
            AVFilterOperator::Contains, AVFilterOperator::DoesNotContain,
            AVFilterOperator::StartsWith, AVFilterOperator::EndsWith,
            AVFilterOperator::IsEmpty, AVFilterOperator::IsNotEmpty
        };
    }

    [[nodiscard]] auto parse_input(const std::string& input,
                                    const AVKey& /*key*/) const
        -> std::expected<AVValueVariant, std::string> override
    {
        return AVValueText{input};
    }

    [[nodiscard]] auto default_value() const -> AVValueVariant override
    {
        return AVValueText{""};
    }
};
```

Implement all 17 handlers:

1. **BlockColumnHandler** -- Format: display block content text. Compare: by content text. Filter: text operators on content. Parse: block ID string.
2. **TextColumnHandler** -- Format: return content. Compare: case-insensitive. Filter: text operators. Parse: identity.
3. **NumberColumnHandler** -- Format: use `format_number()` with key's `number_format`. Compare: numeric `<=>`. Filter: numeric operators. Parse: `std::stod()` with error handling.
4. **DateColumnHandler** -- Format: use `format_date()` / `format_date_range()`. Compare: by timestamp. Filter: date operators including `DateIsRelative`. Parse: ISO 8601 or natural language date.
5. **SelectColumnHandler** -- Format: option name. Compare: by option name alphabetically. Filter: `SelectIs`/`SelectIsNot`. Parse: match against key's options list.
6. **MSelectColumnHandler** -- Format: comma-separated option names. Compare: by first option name, then count. Filter: `SelectIs` (any match), `SelectIsNot` (none match). Parse: comma-separated names.
7. **URLColumnHandler** -- Format: smart URL display. Compare: by full URL string. Filter: text operators on URL. Parse: URL validation.
8. **EmailColumnHandler** -- Format: email address. Compare: string comparison. Filter: text operators. Parse: email validation.
9. **PhoneColumnHandler** -- Format: phone number. Compare: by digits-only normalized form. Filter: text operators. Parse: phone validation.
10. **MAssetColumnHandler** -- Format: comma-separated filenames. Compare: by first asset name. Filter: `IsEmpty`/`IsNotEmpty`. Parse: not applicable (assets added via UI).
11. **TemplateColumnHandler** -- Format: rendered template content. Compare: by rendered string. Filter: text operators on rendered output. Parse: not applicable (read-only).
12. **CreatedColumnHandler** -- Format: `format_date()`. Compare: by timestamp. Filter: date operators. Parse: not applicable (auto-populated).
13. **UpdatedColumnHandler** -- Same as Created but with update timestamp.
14. **CheckboxColumnHandler** -- Format: "Yes"/"No" or checkbox character. Compare: false < true. Filter: `IsTrue`/`IsFalse`. Parse: "true"/"1"/"yes" -> checked.
15. **RelationColumnHandler** -- Format: comma-separated block content texts. Compare: by first relation content. Filter: `RelationContains`/`RelationDoesNotContain`. Parse: comma-separated block IDs.
16. **RollupColumnHandler** -- Format: depends on underlying rollup calc result. Compare: by computed value. Filter: depends on result type. Parse: not applicable (computed).
17. **LineNumberColumnHandler** -- Format: row index as string. Compare: numeric. Filter: numeric operators. Parse: not applicable (virtual).

### AVFormatters.cpp

18. **`format_number(double, AVNumberFormat, custom_format)`** -- Switch on format enum. Use `std::format` or manual string building for thousand separators, currency symbols, percent conversion. Handle edge cases: NaN, infinity, negative zero.
19. **`format_date(int64_t, bool)`** -- Convert Unix timestamp to `std::chrono::system_clock::time_point`. Format as "YYYY-MM-DD" if `is_not_time`, else "YYYY-MM-DD HH:MM".
20. **`format_date_range(int64_t, int64_t, bool)`** -- Format as "start -> end" or "start" if start == end.
21. **`smart_url(const std::string&)`** -- Parse URL, extract domain, truncate path to last segment.
22. **`format_value_for_display(AVValue, AVKey)`** -- Dispatch to type-specific formatter based on `key.type`.

### AVValidators.cpp

23. **`validate_type_match(AVValue)`** -- Verify the variant alternative index matches the declared `AVKeyType`. E.g., if `type == AVKeyType::Number`, the variant must hold `AVValueNumber`.
24. **`validate_text(AVValueText, max_length)`** -- Check content length against max.
25. **`validate_number(AVValueNumber)`** -- Check `std::isfinite(content)`.
26. **`validate_date(AVValueDate)`** -- Check `content >= 0`, check `content2 >= content` if `has_end_date`.
27. **`validate_url(AVValueURL)`** -- Basic regex: starts with `http://` or `https://`, has a domain.
28. **`validate_email(AVValueEmail)`** -- Basic regex: `^[^@]+@[^@]+\.[^@]+$`.
29. **`validate_phone(AVValuePhone)`** -- Check only `[0-9+() -]` characters.
30. **`validate_select(AVValueSelect, AVKey)`** -- Verify `option.name` exists in `key.options`.
31. **`validate_mselect(AVValueMSelect, AVKey)`** -- Verify all option names exist in `key.options`.
32. **`validate_masset(AVValueMAsset)`** -- Check each asset has non-empty `name` and `content`.
33. **`validate_relation(AVValueRelation)`** -- Check all block IDs are non-empty.

## Events to Add

No new events in this phase. Phase 17 events cover value changes. The column type system is a pure logic layer.

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `av.date_format` | string | `"YYYY-MM-DD"` | Default date display format |
| `av.time_format` | string | `"HH:mm"` | Default time display format |
| `av.number_decimal_places` | int | `2` | Default decimal places for number display |
| `av.checkbox_display` | string | `"symbol"` | Checkbox display mode: "symbol" (checkmark/cross) or "text" (Yes/No) |

## Test Cases

File: `tests/unit/test_av_column_types.cpp`

All tests use Catch2 with `[av][column_types]` tags.

1. **Text type: validate, format, compare** -- Create two text values "Apple" and "Banana". Validate both (should pass). Format should return content as-is. Compare should return negative (Apple < Banana case-insensitive).

2. **Number type: format with all NumberFormats** -- Create number value `1234567.89`. Format with each of the 12 `AVNumberFormat` values. Verify: `Commas` -> `"1,234,567.89"`, `Percent` -> `"123,456,789%"`, `USDollar` -> `"$1,234,567.89"`, `Euro` -> `"1.234.567,89 EUR"`, etc.

3. **Number type: compare and sort** -- Create values 10.0, 2.5, 7.3. Compare pairwise. Verify 2.5 < 7.3 < 10.0. Verify NaN handling (NaN sorts to end).

4. **Date type: format with time and without** -- Create date value for `2024-06-15T14:30:00Z`. Format with `is_not_time=true` -> `"2024-06-15"`. Format with `is_not_time=false` -> `"2024-06-15 14:30"`. Format date range: `"2024-06-15 -> 2024-06-20"`.

5. **Select type: validate against key options** -- Create a key with options ["Red", "Blue", "Green"]. Create value with option "Blue" -> validate passes. Create value with option "Purple" -> validate fails with descriptive error.

6. **MSelect type: format and filter** -- Create MSelect value with options ["Tag1", "Tag2"]. Format -> `"Tag1, Tag2"`. Filter `SelectIs("Tag1")` -> true. Filter `SelectIs("Tag3")` -> false. Filter `IsEmpty` -> false.

7. **URL type: validate and smart URL** -- Create URL value `"https://www.example.com/path/to/page?q=1"`. Validate -> pass. Smart URL -> `"example.com/.../page"`. Create invalid URL `"not a url"` -> validate fails.

8. **Email type: validate** -- Valid: `"user@example.com"` -> pass. Invalid: `"user@"` -> fail. Invalid: `"@example.com"` -> fail. Invalid: `""` -> `IsEmpty` filter returns true.

9. **Phone type: validate** -- Valid: `"+1 (555) 123-4567"` -> pass. Invalid: `"abc"` -> fail. Compare: `"+15551234567"` vs `"+15559876543"` by normalized digits.

10. **Checkbox type: compare, format, filter** -- True value: format -> checkmark or "Yes". False value: format -> empty or "No". Compare: false < true. Filter `IsTrue` on true value -> true. Filter `IsFalse` on true value -> false.

11. **All 17 types: parse_input round-trip** -- For each type handler, call `parse_input()` with a representative string. Construct an `AVValue` from the result. Call `format()`. Verify the output is a reasonable display of the input. (Skip Template, Created, Updated, LineNumber, MAsset which are not user-editable.)

12. **Filter operators: text Contains/StartsWith/EndsWith** -- Text value `"Hello World"`. Filter `Contains("World")` -> true. Filter `Contains("xyz")` -> false. Filter `StartsWith("Hello")` -> true. Filter `EndsWith("World")` -> true. Filter `EndsWith("Hello")` -> false.

13. **Filter operators: number comparisons** -- Number value `42.0`. Filter `GreaterThan(40)` -> true. Filter `LessThan(40)` -> false. Filter `NumEqual(42)` -> true. Filter `GreaterThanOrEqual(42)` -> true.

14. **Filter operators: date relative** -- Date value for "today". Filter `DateIsRelative("past_week")` -> true (today is within past week). Filter `DateIsRelative("next_month")` -> false (today is not in the future month). This test should use a fixed "now" timestamp for determinism.

15. **Validation: type mismatch detection** -- Create an `AVValue` with `type = AVKeyType::Number` but `data = AVValueText{"not a number"}`. Call `validate_type_match()`. Verify it returns an error indicating the variant type does not match the declared key type.

## Acceptance Criteria

- [ ] All 17 `IAVColumnTypeHandler` implementations are registered in `AVColumnTypeRegistry`
- [ ] `AVColumnTypeRegistry::get_handler()` returns non-null for all 17 `AVKeyType` values
- [ ] Number formatting produces correct output for all 12 `AVNumberFormat` values
- [ ] Date formatting produces ISO 8601 date strings and handles time/no-time variants
- [ ] `compare()` produces correct ordering for all sortable types
- [ ] `filter()` evaluates all type-specific operators correctly
- [ ] `validate()` catches type mismatches, invalid URLs, invalid emails, out-of-range numbers
- [ ] `parse_input()` converts user-entered strings to correct `AVValueVariant` types
- [ ] All 15 test cases pass
- [ ] No use of `catch(...)` -- all exceptions are typed
- [ ] All public methods have `[[nodiscard]]`
- [ ] Trailing return types used consistently

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/core/av/AVColumnType.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVColumnType.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVFormatters.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVFormatters.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVValidators.h`
- `/Users/ryanrentfro/code/markamp/src/core/av/AVValidators.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_column_types.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/av/AVValue.h` -- Add `format()` and `compare()` declarations that delegate to the registry
- `/Users/ryanrentfro/code/markamp/src/core/av/AVValue.cpp` -- Implement delegation methods
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 3 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_column_types` test target
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 4 new `av.*` config keys
