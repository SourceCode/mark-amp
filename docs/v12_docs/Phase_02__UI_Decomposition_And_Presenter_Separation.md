# Phase 02: UI Decomposition And Presenter Separation

## Goal
Break down monolithic UI classes into composable presentation and interaction layers.

## Why This Phase Now
Large UI units like EditorPanel/MainFrame/LayoutManager concentrate rendering, state, and command logic in single classes.

## Primary Target Files/Areas
- \/Users\/ryanrentfro\/code\/markamp\/src\/ui\/EditorPanel.cpp\n- \/Users\/ryanrentfro\/code\/markamp\/src\/ui\/MainFrame.cpp\n- \/Users\/ryanrentfro\/code\/markamp\/src\/ui\/LayoutManager.cpp\n- \/Users\/ryanrentfro\/code\/markamp\/src\/ui\/SettingsPanel.cpp

## Tasks (22)
1. Run a focused architecture inventory for UI presentation and interaction separation and capture current ownership gaps.
2. Identify the top coupling points and rank them by change-risk and blast radius.
3. Define explicit module responsibilities and non-responsibilities for UI presentation and interaction separation.
4. Document public APIs for each module and mark internal-only APIs.
5. Remove cross-layer includes that violate the target dependency direction.
6. Introduce stable abstractions/interfaces for external dependencies.
7. Split oversized classes/functions in UI presentation and interaction separation into cohesive components.
8. Extract duplicated logic into reusable domain services/utilities.
9. Replace ad-hoc condition chains with strategy/policy objects where appropriate.
10. Standardize naming and folder placement for UI presentation and interaction separation artifacts.
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
