# Phase 02 — Block ID Generation & Timestamp System

## Objective

Implement SiYuan-compatible block ID generation. SiYuan uses a timestamp-based ID format: `YYYYMMDDHHmmss-xxxxxxx` where the prefix is a creation timestamp and the suffix is a 7-character random lowercase alphanumeric string. This ensures globally unique, chronologically sortable IDs without any coordination or central authority. The ID system is used for blocks, notebooks, documents, and all other entities in the knowledge base.

## Prerequisites

- Phase 01 (Block Data Model) — Block struct uses the ID format defined here

## SiYuan Source Reference

- `kernel/util/id.go` — Core ID generation functions: `NewNodeID()`, `TimeFromID()`, `IsPeerID()`. Uses `time.Now().Format("20060102150405")` for timestamp prefix and `gulu.Rand.String(7)` for random suffix.
- `kernel/treenode/node.go` — `ResetNodeID()` which regenerates IDs for pasted/duplicated blocks
- `kernel/util/id.go` line-by-line: ID format is exactly `time.Now().Format("20060102150405") + "-" + randStr(7)` where randStr uses charset `0123456789abcdefghijklmnopqrstuvwxyz`
- Block IDs are always 22 characters: 14 digits + 1 hyphen + 7 alphanumeric = 22 total

## MarkAmp Integration Points

- New header: `src/core/BlockID.h`
- New source: `src/core/BlockID.cpp`
- Used by: Block creation (Phase 01), Notebook creation (Phase 03), Document creation (Phase 05), Transaction system (Phase 07)
- No dependency on EventBus or Config

## Data Structures to Implement

```cpp
namespace markamp::core
{

// Thread-safe, globally-unique block ID generator compatible with SiYuan's
// ID format: YYYYMMDDHHmmss-xxxxxxx (22 characters total).
//
// The timestamp prefix enables:
//   - Chronological sorting via lexicographic comparison
//   - Creation time extraction without database lookup
//   - Collision avoidance across devices (timestamp + random)
//
// The random suffix uses charset: 0123456789abcdefghijklmnopqrstuvwxyz
// giving 36^7 = ~78 billion possible suffixes per second.
class BlockIDGenerator
{
public:
    // Generate a new block ID using the current system clock.
    // Thread-safe. Returns a 22-character string.
    [[nodiscard]] static auto generate() -> std::string;

    // Generate a new block ID with a specific timestamp.
    // Useful for importing data with preserved creation times.
    [[nodiscard]] static auto generate(std::chrono::system_clock::time_point timestamp) -> std::string;

    // Parse the creation timestamp from a block ID.
    // Returns nullopt if the ID format is invalid.
    [[nodiscard]] static auto parse_timestamp(const std::string& block_id)
        -> std::optional<std::chrono::system_clock::time_point>;

    // Validate that a string is a well-formed block ID.
    // Checks: length == 22, first 14 chars are digits, char 14 is '-',
    // last 7 chars are [0-9a-z].
    [[nodiscard]] static auto is_valid(const std::string& block_id) -> bool;

    // Extract the 14-character timestamp prefix from a block ID.
    // Returns empty string if block_id is invalid.
    [[nodiscard]] static auto timestamp_prefix(const std::string& block_id) -> std::string;

    // Extract the 7-character random suffix from a block ID.
    // Returns empty string if block_id is invalid.
    [[nodiscard]] static auto random_suffix(const std::string& block_id) -> std::string;

    // Get the current time formatted as a 14-character timestamp string.
    // Format: YYYYMMDDHHmmss (e.g., "20260214153045")
    [[nodiscard]] static auto current_timestamp_string() -> std::string;

    // Format a time_point as a 14-character SiYuan timestamp string.
    [[nodiscard]] static auto format_timestamp(std::chrono::system_clock::time_point tp) -> std::string;

    // Parse a 14-character timestamp string back to a time_point.
    // Returns nullopt if the string is not a valid timestamp.
    [[nodiscard]] static auto parse_timestamp_string(const std::string& ts)
        -> std::optional<std::chrono::system_clock::time_point>;

private:
    // Length constants
    static constexpr int TIMESTAMP_LEN = 14;
    static constexpr int RANDOM_LEN = 7;
    static constexpr int TOTAL_LEN = TIMESTAMP_LEN + 1 + RANDOM_LEN; // 22

    // Character set for random suffix: 0-9 a-z (36 chars, matching SiYuan)
    static constexpr std::string_view CHARSET = "0123456789abcdefghijklmnopqrstuvwxyz";

    // Generate a random alphanumeric string of the given length.
    // Uses thread_local std::mt19937 seeded from std::random_device.
    [[nodiscard]] static auto random_alphanumeric(int length) -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`generate()`** — Get current system time, format as `YYYYMMDDHHmmss`, append `-`, append 7 random chars from `CHARSET`. Use `std::chrono::system_clock::now()` and `std::put_time` or `std::format` (C++20/23). Return the 22-character string.

2. **`generate(timestamp)`** — Same as above but using the provided time_point instead of now().

3. **`parse_timestamp(block_id)`** — Validate format first. Extract first 14 chars. Parse `YYYY=id[0..3]`, `MM=id[4..5]`, `DD=id[6..7]`, `HH=id[8..9]`, `mm=id[10..11]`, `ss=id[12..13]`. Construct `std::tm`, convert to `time_point` via `std::mktime`. Return nullopt on any parsing failure.

4. **`is_valid(block_id)`** — Check: `block_id.size() == 22`, `block_id[14] == '-'`, `block_id[0..13]` are all digits, `block_id[15..21]` are all in CHARSET. Return false on any violation.

5. **`random_alphanumeric(length)`** — Use `thread_local static std::mt19937 rng{std::random_device{}()}`. Create uniform_int_distribution over `[0, CHARSET.size()-1]`. Build string of `length` chars. The thread_local ensures no mutex needed.

6. **`format_timestamp(tp)`** — Convert time_point to `std::time_t`, then to `std::tm` via `localtime_r` (POSIX) or `localtime_s` (Windows). Format as `%Y%m%d%H%M%S`. Return 14-character string.

7. **`current_timestamp_string()`** — Calls `format_timestamp(std::chrono::system_clock::now())`.

8. **`timestamp_prefix(block_id)`** — If `is_valid(block_id)`, return `block_id.substr(0, 14)`. Else return `""`.

9. **`random_suffix(block_id)`** — If `is_valid(block_id)`, return `block_id.substr(15, 7)`. Else return `""`.

10. **`parse_timestamp_string(ts)`** — Parse a 14-char string as `YYYYMMDDHHmmss`, return time_point. Used internally by `parse_timestamp`.

## Implementation Notes

- **Thread safety**: The `thread_local` keyword on the RNG ensures no mutex is needed for concurrent ID generation. Each thread gets its own seeded generator.
- **Charset**: SiYuan uses lowercase alphanumeric only (0-9, a-z). Do NOT include uppercase letters.
- **Timestamp locality**: SiYuan uses local time, not UTC. Match this behavior for compatibility.
- **Performance**: ID generation should be fast enough for batch operations (generating 10,000+ IDs per second).

## Events to Add

- None for this phase.

## Config Keys to Add

- None for this phase.

## Test Cases (Catch2)

File: `tests/unit/test_block_id.cpp`

1. **ID generation produces valid format** — Generate an ID, verify: length == 22, position 14 is '-', first 14 chars are digits, last 7 chars are lowercase alphanumeric.

2. **ID uniqueness (sequential)** — Generate 100 IDs in a tight loop, insert all into a `std::unordered_set`, verify set size == 100 (no duplicates).

3. **ID uniqueness (concurrent)** — Launch 8 threads, each generating 100 IDs. Collect all 800 IDs, verify all unique.

4. **Timestamp extraction matches current time** — Generate an ID, extract timestamp via `parse_timestamp()`, verify it is within 1 second of `std::chrono::system_clock::now()`.

5. **Custom timestamp generation** — Create a specific time_point (e.g., 2024-01-15 10:30:45), generate an ID with it, verify the prefix is `"20240115103045"`.

6. **Validation accepts valid IDs** — Test `is_valid()` with: `"20210808180117-6v0mkxr"`, `"20260214120000-abcdefg"`, `"99991231235959-0000000"`. All should return true.

7. **Validation rejects invalid IDs** — Test `is_valid()` with: `""` (empty), `"short"` (too short), `"20210808180117_6v0mkxr"` (underscore instead of hyphen), `"2021080818011X-6v0mkxr"` (X in timestamp), `"20210808180117-6v0mkXr"` (uppercase X in suffix), `"20210808180117-6v0mkxr!"` (too long). All should return false.

8. **Random suffix uniqueness** — Generate 1000 IDs within the same second (use `generate(fixed_timestamp)`). Verify all are unique (the random suffix differentiates them). There should be zero collisions with 36^7 possible suffixes.

9. **Parse round-trip** — Generate an ID, call `parse_timestamp()` to get time_point, call `format_timestamp()` on that time_point, verify the result matches `timestamp_prefix()` of the original ID.

10. **Lexicographic chronological sorting** — Generate an ID at time T1, sleep 1 second, generate at T2. Verify `id1 < id2` (string comparison). Also test with explicit timestamps 1 minute apart.

11. **Prefix and suffix extraction** — Generate an ID, verify `timestamp_prefix()` returns exactly 14 chars, `random_suffix()` returns exactly 7 chars, and concatenating `prefix + "-" + suffix` equals the original ID.

12. **Invalid ID extraction** — Call `timestamp_prefix("")`, `random_suffix("")`, `parse_timestamp("invalid")` — verify they return empty string / nullopt respectively without crashing.

## Acceptance Criteria

- [ ] Generated IDs match SiYuan format exactly: `YYYYMMDDHHmmss-xxxxxxx` (22 chars)
- [ ] Random suffix uses only charset `0-9a-z` (36 characters)
- [ ] No duplicate IDs even when generating rapidly from multiple threads
- [ ] Thread-safe generation without mutex (uses thread_local RNG)
- [ ] Timestamp extraction is accurate to the second
- [ ] Lexicographic ordering of IDs matches chronological ordering
- [ ] All 12 test cases pass
- [ ] No external dependencies (C++23 standard library only: `<chrono>`, `<random>`, `<string>`, `<optional>`, `<string_view>`)
- [ ] Uses `[[nodiscard]]` on all functions
- [ ] Allman brace style, 4-space indent, trailing return types

## Files to Create/Modify

- CREATE: `src/core/BlockID.h`
- CREATE: `src/core/BlockID.cpp`
- MODIFY: `src/CMakeLists.txt` (add `core/BlockID.cpp` to source list)
- CREATE: `tests/unit/test_block_id.cpp`
- MODIFY: `tests/CMakeLists.txt` (add `test_block_id` target linking Catch2)
