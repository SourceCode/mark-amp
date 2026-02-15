# Phase 17: Hot Path Branch Elimination & Strategy Resolution

## Metadata

| Field | Value |
|---|---|
| Phase ID | 17 |
| Prerequisites | Phase 08 |
| Estimated Complexity | Medium |
| Estimated File Count | 3 created, 5 modified |
| PRD Sections | 4.3 Hot Path Micro-Optimization, 4.4 Branch Elimination |

---

## Objective

Eliminate runtime branching in inner loops by pre-resolving configuration branches into function pointers/strategy objects at config-load time. Apply `[[likely]]`/`[[unlikely]]` attributes to remaining necessary branches.

---

## Background

The PRD mandates converting `if (config.feature_enabled)` patterns in inner loops into "precomputed function pointers, strategy pattern resolved at config-load time." Runtime branches in tight loops defeat branch prediction and waste instruction cache. By resolving configuration decisions once (when config changes) and storing the result as a function pointer, the inner loop becomes a direct call with no branch.

---

## Scope

### Tasks

1. **Audit hot paths for config-check branching**:
   - Scan rendering loops, syntax highlighting, event dispatch for patterns like:
     - `if (config.show_minimap)` in layout calculations
     - `if (config.word_wrap)` in line measurement
     - `if (config.render_line_highlight)` in paint
     - `if (config.enable_syntax_highlighting)` in tokenization
   - Document all instances found

2. **Create `src/core/ResolvedConfig.h` / `ResolvedConfig.cpp`**:
   - Pre-computed strategy table resolved once on Config change
   - Stores function pointers/std::function for configurable behavior:
     - `line_measure_fn`: word-wrap vs no-wrap measurement function
     - `paint_line_highlight_fn`: highlight active line or no-op
     - `syntax_tokenize_fn`: full tokenization or no-op
   - `resolve()` method called when `ConfigChangedEvent` fires
   - Subscribe to `ConfigChangedEvent` via EventBus
   - All strategy fields are `[[nodiscard]]` queryable

3. **Apply `[[likely]]` / `[[unlikely]]` to remaining branches**:
   - Error-path branches in hot code: `if (ptr == nullptr) [[unlikely]]`
   - Common-case branches: `if (has_subscribers) [[likely]]`
   - Apply to EventBus dispatch, rendering loop, parser
   - Only where profiling data (Phase 08/09) indicates benefit

4. **Apply `MARKAMP_HOT` / `MARKAMP_COLD` function attributes**:
   - `MARKAMP_HOT` (`__attribute__((hot))`) on frequently-called functions
   - `MARKAMP_COLD` (`__attribute__((cold))`) on error handlers, rarely-called paths
   - Apply using existing `CompilerHints.h` if available, or create macros

5. **Create `benchmarks/bench_branch_elimination.cpp`**:
   - `BM_ConfigBranch_Runtime`: measure with runtime config checks
   - `BM_ConfigBranch_Resolved`: measure with pre-resolved function pointers
   - `BM_BranchHints_Likely`: measure with [[likely]] annotations
   - Parameterize by loop iteration count

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `src/core/ResolvedConfig.h` |
| Create | `src/core/ResolvedConfig.cpp` |
| Modify | `src/core/Config.h` |
| Modify | `src/core/Config.cpp` |
| Modify | `src/rendering/HtmlRenderer.cpp` |
| Create | `benchmarks/bench_branch_elimination.cpp` |
| Modify | `benchmarks/CMakeLists.txt` |
| Modify | `src/CMakeLists.txt` |

---

## Implementation Notes

- Strategy pattern with function pointers: store a `void(*)(const Line&, RenderContext&)` for each configurable rendering behavior. On config change, update the pointer to either the "enabled" or "disabled" implementation.
- `std::function` has overhead from type erasure. For maximum performance, use raw function pointers or templates where the set of implementations is known at compile time.
- `[[likely]]`/`[[unlikely]]` (C++20) hint to the compiler about expected branch direction. They should only be applied where the branch ratio is heavily skewed (>90% one way).
- Be conservative with hot/cold attributes — the compiler's own analysis is usually good. Apply only where profiling shows clear benefit.
- The `ResolvedConfig` object should be created once and updated on config change. It should be accessible from hot paths without going through Config (which may have its own lookup overhead).

---

## Acceptance Criteria

- [ ] Zero config-check branches remain in identified hot loops (replaced by strategy calls)
- [ ] `ResolvedConfig` updates automatically on `ConfigChangedEvent`
- [ ] `bench_branch_elimination` shows measurable improvement for strategy pattern
- [ ] `[[likely]]`/`[[unlikely]]` applied to all error-path branches in hot code
- [ ] `MARKAMP_HOT`/`MARKAMP_COLD` applied to identified functions
- [ ] All existing tests pass (no behavioral changes)
- [ ] No performance regression in any existing benchmark

---

## Testing Strategy

- Run bench_branch_elimination and verify improvement
- Run full benchmark suite to verify no regressions
- Run full test suite to verify correctness
- Verify ResolvedConfig updates correctly when config values change
