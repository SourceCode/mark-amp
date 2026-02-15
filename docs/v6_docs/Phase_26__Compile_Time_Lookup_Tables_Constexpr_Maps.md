# Phase 26: Compile-Time Lookup Tables -- constexpr Maps

## Metadata

| Field | Value |
|---|---|
| Phase ID | 26 |
| Prerequisites | Phase 21 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 5 modified |
| PRD Sections | 4.6 Compile-Time Optimizations |

---

## Objective

Replace remaining runtime-built lookup tables with constexpr arrays and binary search, eliminating constructor-time allocation. Create a reusable `ConstexprMap` utility.

---

## Background

The PRD mandates: "constexpr parsing for static tables, constexpr lookup maps, consteval for static generation. Replace runtime table builds with compile-time arrays." Phase 11 handled the most critical static initialization, but many lookup tables may still be built at runtime using `unordered_map` or `map`. This phase provides a general-purpose constexpr lookup facility and converts remaining tables.

---

## Scope

### Tasks

1. **Create `src/core/ConstexprMap.h`**:
   - `ConstexprMap<Key, Value, N>` template:
     - Stores `std::array<std::pair<Key, Value>, N>` sorted by key
     - Compile-time sorted construction (verified by `static_assert`)
     - `constexpr auto find(Key) -> const Value*` using binary search
     - `constexpr auto operator[](Key) -> const Value&` with assert on miss
     - `constexpr auto contains(Key) -> bool`
     - O(log N) lookup, zero runtime allocation
   - Helper: `constexpr auto make_constexpr_map(std::pair<K,V>... pairs)` for construction
   - Support `std::string_view` keys for string-based lookups

2. **Convert remaining runtime lookup tables**:

   **MIME type lookups in HtmlRenderer**:
   - File extension -> MIME type mapping
   - Convert from `unordered_map<string, string>` to `ConstexprMap<string_view, string_view, N>`

   **Markdown syntax keywords in SyntaxHighlighter**:
   - Language keyword sets
   - Convert from `unordered_set<string>` to sorted `constexpr std::array<string_view, N>`

   **Config key defaults**:
   - Default value table
   - Convert from runtime map to `ConstexprMap`

   **CSS property mappings in theme system**:
   - CSS name -> internal property mapping
   - Convert to `ConstexprMap`

3. **Create `tests/unit/test_constexpr_map.cpp`**:
   - Test compile-time construction (static_assert)
   - Test find() returns correct values
   - Test find() returns nullptr for missing keys
   - Test contains() for present and absent keys
   - Test with string_view keys
   - Test with integer keys
   - Test empty map
   - Test single-element map

4. **Create `benchmarks/bench_constexpr_lookup.cpp`**:
   - `BM_ConstexprMap_Find`: lookup throughput for constexpr map
   - `BM_UnorderedMap_Find`: lookup throughput for unordered_map (baseline)
   - `BM_StdMap_Find`: lookup throughput for std::map (baseline)
   - Parameterize by map size (10, 50, 200 entries)

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ConstexprMap.h` |
| Modify | `src/rendering/HtmlRenderer.cpp` |
| Modify | `src/core/SyntaxHighlighter.cpp` |
| Modify | `src/core/Config.cpp` |
| Create | `tests/unit/test_constexpr_map.cpp` |
| Create | `benchmarks/bench_constexpr_lookup.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `constexpr` binary search is available since C++20 (`std::lower_bound` is constexpr in C++20).
- For `string_view` keys, comparison uses `operator<=>` which is constexpr for `string_view`.
- The sorted construction can be verified at compile time:
  ```cpp
  static_assert(std::is_sorted(map.data_.begin(), map.data_.end(),
      [](auto& a, auto& b) { return a.first < b.first; }));
  ```
- For small maps (<20 entries), linear search may be faster than binary search due to simpler branch prediction. The benchmark will validate this.
- `constexpr` maps are embedded in the binary's read-only data segment, requiring zero runtime initialization.

---

## Acceptance Criteria

- [ ] All converted tables are `constexpr` (compile-time evaluated)
- [ ] `bench_constexpr_lookup` shows comparable or better performance vs `unordered_map`
- [ ] Zero runtime allocation for table construction
- [ ] `test_constexpr_map` validates lookup correctness for all key types
- [ ] `static_assert` verifies compile-time construction
- [ ] All existing tests pass after table conversion
- [ ] ConstexprMap supports both integral and string_view keys

---

## Testing Strategy

- Run test_constexpr_map for thorough unit validation
- Run bench_constexpr_lookup and compare against runtime map baselines
- Run full test suite for regression checking
- Verify with `static_assert` that maps are constructed at compile time
