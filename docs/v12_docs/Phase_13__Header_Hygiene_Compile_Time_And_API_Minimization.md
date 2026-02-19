# Phase 13: Header Hygiene Compile Time And API Minimization

## Goal
Reduce compile-time coupling via header hygiene, forward declarations, and API minimization.

## Why This Phase Now
High header volume and broad includes increase build time and encourage transitive dependency leakage.

## Primary Target Files/Areas
- \/Users\/ryanrentfro\/code\/markamp\/src\/core\n- \/Users\/ryanrentfro\/code\/markamp\/src\/ui\n- \/Users\/ryanrentfro\/code\/markamp\/src\/canvas\n- \/Users\/ryanrentfro\/code\/markamp\/CMakeLists.txt

## Tasks (22)
1. Run a focused architecture inventory for header/API surface reduction and capture current ownership gaps.
2. Identify the top coupling points and rank them by change-risk and blast radius.
3. Define explicit module responsibilities and non-responsibilities for header/API surface reduction.
4. Document public APIs for each module and mark internal-only APIs.
5. Remove cross-layer includes that violate the target dependency direction.
6. Introduce stable abstractions/interfaces for external dependencies.
7. Split oversized classes/functions in header/API surface reduction into cohesive components.
8. Extract duplicated logic into reusable domain services/utilities.
9. Replace ad-hoc condition chains with strategy/policy objects where appropriate.
10. Standardize naming and folder placement for header/API surface reduction artifacts.
11. Add constructor-injected dependencies and reduce hidden global access.
12. Add guardrails for invariants with assertions or validated factories.
13. Normalize error handling to one approach (Result/Error or exception policy).
14. Add or update unit tests for each extracted component.
15. Add integration tests to verify module collaboration boundaries.
16. Add negative-path tests for invalid state and contract violations.
17. Add benchmarking/latency checks on the hot paths touched in this phase.
18. Add logging/telemetry points at module boundaries for regressions.
19. Update docs with diagrams and before/after dependency maps.
20. Create migration notes for downstream callers impacted by refactors.
21. Run full build + targeted test suite; record failures and remediation tasks.
22. Publish phase closeout: decisions, deferred debt, and entry criteria for next phase.

## Completion Gates
- All tasks completed or explicitly deferred with rationale.
- Boundary tests and regression tests pass for modified modules.
- No new layering violations introduced by this phase.
- ADR/design notes for key tradeoffs are written.
