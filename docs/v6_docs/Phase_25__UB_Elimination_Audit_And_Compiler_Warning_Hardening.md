# Phase 25: UB Elimination Audit & Compiler Warning Hardening

## Metadata

| Field | Value |
|---|---|
| Phase ID | 25 |
| Prerequisites | Phase 18 |
| Estimated Complexity | Medium |
| Estimated File Count | 1 created, 5+ modified |
| PRD Sections | 5.3 UB Elimination |

---

## Objective

Systematic audit and elimination of undefined behavior across the codebase. Enable the full compiler warning set specified in the PRD and fix all findings.

---

## Background

The PRD mandates: "No reinterpret_cast on non-trivially compatible types, no unchecked downcasts, no raw pointer ownership ambiguity, no data races." Warning flags required: `-Wall -Wextra -Wconversion -Wshadow -Wpedantic`. The project may already have some of these, but this phase ensures comprehensive coverage and fixes all findings.

---

## Scope

### Tasks

1. **Add missing warnings to `cmake/CompilerWarnings.cmake`**:
   - Verify and add if missing:
     - `-Wshadow` (variable shadowing)
     - `-Wold-style-cast` (C-style casts)
     - `-Woverloaded-virtual` (hidden virtual overloads)
     - `-Wnon-virtual-dtor` (polymorphic classes without virtual destructors)
     - `-Wconversion` (implicit type conversions)
     - `-Wpedantic` (strict standards compliance)
   - Suppress only where absolutely necessary (wxWidgets macro compatibility)

2. **Audit for UB patterns across the codebase**:

   **reinterpret_cast audit**:
   - Search for all `reinterpret_cast` usage
   - Verify each is on trivially compatible types (e.g., char* <-> unsigned char*)
   - Replace unsafe casts with safe alternatives
   - FrameArena/ObjectPool may use reinterpret_cast — verify safety

   **Unchecked downcast audit**:
   - Search for `static_cast<Derived*>(base_ptr)` patterns
   - EventBus handler dispatch may use `static_cast<const EventT&>(base_event)`
   - Add `dynamic_cast` with check in debug mode (`assert(dynamic_cast<T*>(ptr))`)
   - Use `MARKAMP_SAFE_DOWNCAST(ptr, TargetType)` macro: dynamic_cast in debug, static_cast in release

   **Pointer ownership audit**:
   - Document all raw pointer contracts: who owns, who borrows
   - Verify `PluginContext` pointer lifetimes are guaranteed by MarkAmpApp
   - Ensure no dangling pointers from service destruction order

   **Arithmetic overflow audit**:
   - Check for signed integer overflow on user-supplied sizes
   - Check for subtraction underflow (e.g., `size - offset` where offset > size)
   - Add overflow guards where needed

3. **Fix all identified UB**:
   - Apply fixes per the audit findings
   - Use `MARKAMP_SAFE_DOWNCAST` macro for type-safe downcasts
   - Add arithmetic overflow guards
   - Replace unsafe reinterpret_casts

4. **Run full test suite under ASan+UBSan**:
   - Build with `debug-asan` preset (ASan + UBSan)
   - Run all tests
   - Fix every finding

5. **Create `tests/unit/test_ub_edge_cases.cpp`**:
   - Test boundary conditions for arithmetic operations
   - Test safe downcast macro behavior
   - Test pointer validity checks
   - Test with values near integer limits

---

## Key Files

| Action | File Path |
|--------|-----------|
| Modify | `cmake/CompilerWarnings.cmake` |
| Modify | `src/core/FrameArena.h` (ObjectPool safety) |
| Modify | `src/core/EventBus.h` (downcast safety) |
| Modify | Various source files with UB fixes |
| Create | `tests/unit/test_ub_edge_cases.cpp` |
| Modify | `tests/CMakeLists.txt` |

---

## Implementation Notes

- `-Wconversion` can be very noisy, especially with wxWidgets which uses `int` for many parameters. Consider enabling it with specific suppressions for wxWidgets interfaces.
- `MARKAMP_SAFE_DOWNCAST` implementation:
  ```cpp
  #ifndef NDEBUG
  #define MARKAMP_SAFE_DOWNCAST(ptr, Type) \
      ([](auto* p) { assert(dynamic_cast<Type*>(p)); return static_cast<Type*>(p); }(ptr))
  #else
  #define MARKAMP_SAFE_DOWNCAST(ptr, Type) static_cast<Type*>(ptr)
  #endif
  ```
- Arithmetic overflow: use `std::numeric_limits` checks or C++26 `std::add_sat`/`std::sub_sat` (or manual equivalents).
- `-Wold-style-cast` will catch any `(Type)value` casts. Replace with appropriate C++ casts.
- wxWidgets macros may trigger some warnings — use `#pragma` suppression only for wxWidgets headers.

---

## Acceptance Criteria

- [ ] Full test suite passes under ASan+UBSan with zero findings
- [ ] Zero `reinterpret_cast` on non-trivially-compatible types
- [ ] All downcasts are either compile-time safe or runtime-checked (in debug)
- [ ] No `-Wshadow` warnings in `src/` directory
- [ ] No `-Wold-style-cast` warnings in `src/` directory
- [ ] `test_ub_edge_cases` exercises boundary conditions without UB
- [ ] All new warning flags are enabled in `CompilerWarnings.cmake`
- [ ] Pointer ownership contracts documented for all PluginContext fields

---

## Testing Strategy

- Build with `debug-asan` preset and run full test suite
- Build with all new warnings enabled and fix all warnings
- Run test_ub_edge_cases for boundary condition validation
- Run under UBSan specifically to catch integer overflow, null deref, alignment issues
