# Phase 15: Test Architecture Fixture Reuse And Contract Tests

## Goal
Improve test organization, shared fixtures, and contract tests for refactor safety.

## Why This Phase Now
Refactoring at this scale requires reusable fixtures and boundary-focused tests to avoid brittle behavior coupling.

## Primary Target Files/Areas
- \/Users\/ryanrentfro\/code\/markamp\/tests\/unit\n- \/Users\/ryanrentfro\/code\/markamp\/src\/core\n- \/Users\/ryanrentfro\/code\/markamp\/src\/ui\n- \/Users\/ryanrentfro\/code\/markamp\/src\/canvas

## Tasks (22)
1. Run a focused architecture inventory for test architecture and boundary contracts and capture current ownership gaps.
2. Identify the top coupling points and rank them by change-risk and blast radius.
3. Define explicit module responsibilities and non-responsibilities for test architecture and boundary contracts.
4. Document public APIs for each module and mark internal-only APIs.
5. Remove cross-layer includes that violate the target dependency direction.
6. Introduce stable abstractions/interfaces for external dependencies.
7. Split oversized classes/functions in test architecture and boundary contracts into cohesive components.
8. Extract duplicated logic into reusable domain services/utilities.
9. Replace ad-hoc condition chains with strategy/policy objects where appropriate.
10. Standardize naming and folder placement for test architecture and boundary contracts artifacts.
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
