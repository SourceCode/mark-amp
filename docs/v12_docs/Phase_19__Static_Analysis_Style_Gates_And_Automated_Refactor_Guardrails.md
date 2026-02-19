# Phase 19: Static Analysis Style Gates And Automated Refactor Guardrails

## Goal
Strengthen automated quality gates for complexity, duplication, layering, and style conformance.

## Why This Phase Now
DRY and separation goals must be enforced by CI checks, not only manual review.

## Primary Target Files/Areas
- \/Users\/ryanrentfro\/code\/markamp\/.clang-tidy\n- \/Users\/ryanrentfro\/code\/markamp\/.clang-format\n- \/Users\/ryanrentfro\/code\/markamp\/scripts\/lint.sh\n- \/Users\/ryanrentfro\/code\/markamp\/scripts\/check_catch_all.sh

## Tasks (22)
1. Run a focused architecture inventory for automated quality gates and capture current ownership gaps.
2. Identify the top coupling points and rank them by change-risk and blast radius.
3. Define explicit module responsibilities and non-responsibilities for automated quality gates.
4. Document public APIs for each module and mark internal-only APIs.
5. Remove cross-layer includes that violate the target dependency direction.
6. Introduce stable abstractions/interfaces for external dependencies.
7. Split oversized classes/functions in automated quality gates into cohesive components.
8. Extract duplicated logic into reusable domain services/utilities.
9. Replace ad-hoc condition chains with strategy/policy objects where appropriate.
10. Standardize naming and folder placement for automated quality gates artifacts.
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
