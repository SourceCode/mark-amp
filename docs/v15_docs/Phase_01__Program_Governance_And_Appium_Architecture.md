# Phase 01 -- Program Governance and Appium Architecture

## Objective
Create the operating model, standards, and technical architecture for a large-scale Appium `mac2` E2E program.

## Tasks
1. Define E2E mission, scope, and non-goals for MarkAmp UI coverage.
2. Define Appium-vs-Catch2 boundary for UI, service, and integration test ownership.
3. Build an application workflow inventory covering all user-facing surfaces.
4. Rank workflows by risk, usage, and regression history.
5. Define severity model for E2E defects (P0-P3) and triage SLA.
6. Define test-tier taxonomy: smoke, critical, workflow, regression, resilience.
7. Define suite runtime budgets for local and CI execution.
8. Define flake thresholds and auto-quarantine rules.
9. Define baseline pass-rate and quality gate metrics.
10. Define required failure artifacts for every failing test.
11. Establish test naming conventions by area and intent.
12. Establish folder structure for specs, page objects, helpers, fixtures.
13. Establish selector naming convention `ma.<surface>.<control>`.
14. Establish deterministic-data policy for all tests.
15. Establish test isolation policy (workspace, config, cache, temp files).
16. Establish retry policy by suite tier.
17. Establish timeout policy by interaction type.
18. Establish startup/shutdown policy for app and Appium sessions.
19. Establish environment matrix for local and CI runs.
20. Establish macOS host prerequisites checklist.
21. Define quality checklist for adding any new E2E test.
22. Define review checklist for selector additions and renames.
23. Define review checklist for wait logic and anti-flake patterns.
24. Create master coverage map linking product areas to test suites.
25. Create mapping of existing integration tests to missing UI flows.
26. Define incident process for catastrophic E2E regressions.
27. Define waiver process for temporary gate bypasses.
28. Define test data privacy and secret-handling policy.
29. Define logging and redaction policy for captured artifacts.
30. Define branch strategy for test updates with feature rollout.
31. Define ownership model by subsystem (UI, Canvas, Extensions, etc.).
32. Define escalation path for persistent flake clusters.
33. Define bug filing template with repro + artifact attachments.
34. Define “definition of done” for each phase deliverable.
35. Define monthly quality scorecard inputs and outputs.
36. Define KPIs: coverage %, defect escape, mean time to diagnose.
37. Define PR gate rules for smoke and critical workflow suites.
38. Define nightly full-suite execution policy.
39. Define release-candidate certification checklist.
40. Define rollback criteria when E2E gates fail near release.
41. Create architecture decision record for Appium `mac2` stack choice.
42. Create architecture decision record for WDIO + TypeScript runner.
43. Create architecture decision record for page-object standards.
44. Create architecture decision record for fixture lifecycle strategy.
45. Create architecture decision record for artifact storage retention.
46. Publish developer onboarding doc for running E2E locally.
47. Publish maintainer doc for evolving suites safely.
48. Publish test authoring examples for core workflow patterns.
49. Run governance review with engineering owners.
50. Lock v15 Appium architecture baseline and sign off.
