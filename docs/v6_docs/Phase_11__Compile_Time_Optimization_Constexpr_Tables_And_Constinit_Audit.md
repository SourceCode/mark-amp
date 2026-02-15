# Phase 11: Compile-Time Optimization -- constexpr Tables & constinit Audit

## Metadata

| Field | Value |
|---|---|
| Phase ID | 11 |
| Prerequisites | Phase 03 |
| Estimated Complexity | Medium |
| Estimated File Count | 2 created, 4 modified |
| PRD Sections | 3.1 Minimize Dynamic Initialization, 4.6 Compile-Time Optimizations |

---

## Objective

Eliminate runtime table construction by converting static lookup tables to constexpr/constinit and auditing all translation units for non-trivial static initialization.

---

## Background

The PRD mandates: "Eliminate global non-trivial static objects. Replace static initialization with: constinit, function-local statics, constexpr initialization. Avoid dynamic allocation during static construction." Non-trivial static initialization runs before `main()`, adding invisible startup latency. Every `static std::vector`, `static std::map`, or `static std::string` at file scope represents hidden startup cost.

---

## Scope

### Tasks

1. **Create `scripts/audit_static_init.sh`**:
   - Uses `nm` (or `objdump`) to find global constructors in object files
   - Searches for `__cxx_global_var_init` symbols
   - Reports each translation unit with non-trivial static initialization
   - Produces sorted report by number of global constructors
   - Exit code 0 if no non-trivial constructors found in `src/core/`, 1 otherwise

2. **Audit all source files for static initialization patterns**:
   - Search for: `static std::vector`, `static std::map`, `static std::unordered_map`, `static std::string`, `static std::set`
   - Search for: static objects with constructors (non-trivial types)
   - Document all findings

3. **Convert identified tables** (priority targets):

   **`src/core/BuiltInThemes.cpp`** (if present):
   - Convert theme definitions to `constexpr std::array` of theme data
   - Use `constinit` for any mutable state

   **`src/core/SyntaxHighlighter.cpp`**:
   - Convert keyword tables to `constexpr std::array`
   - Use sorted array + binary search instead of `unordered_set`

   **`src/core/Config.cpp`**:
   - Convert hardcoded default values to `constexpr` array of key-value pairs
   - Verify JSON defaults are loaded at runtime (not as statics)

   **`src/core/Events.h`**:
   - Ensure event name string constants are `constexpr` or `constinit`
   - No runtime std::string construction for event type names

4. **Convert all identified patterns to safe alternatives**:
   - `static std::vector<T>` -> `constexpr std::array<T, N>` (if size known) or function-local static
   - `static std::map<K,V>` -> `constexpr` sorted array + binary search
   - `static std::string` -> `constexpr std::string_view` or `constinit const char*`
   - `static` singletons -> function-local static (lazy init on first use)

5. **Create `tests/unit/test_constexpr_tables.cpp`**:
   - Verify converted tables are evaluated at compile time using `static_assert` where possible
   - Test lookup operations on constexpr arrays
   - Verify sorted arrays maintain order

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `scripts/audit_static_init.sh` |
| Modify | `src/core/BuiltInThemes.cpp` |
| Modify | `src/core/SyntaxHighlighter.cpp` |
| Modify | `src/core/Config.cpp` |
| Create | `tests/unit/test_constexpr_tables.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `constinit` (C++20) guarantees constant initialization but allows mutation after. Use for mutable file-scope state.
- `constexpr` (C++11+) guarantees compile-time evaluation. Use for immutable lookup tables.
- `std::string_view` is `constexpr`-friendly. Replace all `static const std::string` with `constexpr std::string_view`.
- For sorted arrays, use `std::is_sorted()` in a `static_assert` to verify compile-time sort order.
- The `nm` command on macOS uses `nm -g` to list global symbols. Filter for `__cxx_global_var_init`.
- Some wxWidgets types cannot be `constexpr` (e.g., `wxColour`). These must remain as function-local statics.

---

## Acceptance Criteria

- [ ] `audit_static_init.sh` reports zero non-trivial global constructors in `src/core/`
- [ ] All converted tables use `constexpr` or `constinit`
- [ ] `test_constexpr_tables` validates compile-time evaluation (uses `static_assert`)
- [ ] No `static std::vector`, `static std::map`, `static std::string` at file scope in `src/core/`
- [ ] No functional regression (all existing tests pass)
- [ ] Keyword/config lookups still work correctly after conversion

---

## Testing Strategy

- Run `audit_static_init.sh` on debug build before and after changes
- Build and run all unit tests
- Run test_constexpr_tables to verify compile-time evaluation
- Verify application startup (no crashes from removed static state)
