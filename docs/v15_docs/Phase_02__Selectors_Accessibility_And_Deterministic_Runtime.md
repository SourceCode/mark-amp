# Phase 02 -- Selectors, Accessibility, and Deterministic Runtime

## Objective
Make the app reliably automatable by hardening selectors, accessibility metadata, startup behavior, and fixture determinism.

## Tasks
1. Audit existing `SetName` coverage across top-level windows and panels.
2. Add missing stable names for app shell controls.
3. Add missing stable names for editor and tab controls.
4. Add missing stable names for file tree and explorer controls.
5. Add missing stable names for activity bar controls.
6. Add missing stable names for settings dialog controls.
7. Add missing stable names for command palette controls.
8. Add missing stable names for panel area tabs.
9. Add missing stable names for terminal and debug panel controls.
10. Add missing stable names for extension browser controls.
11. Add missing stable names for theme gallery controls.
12. Add missing stable names for search/find-replace controls.
13. Add missing stable names for graph view controls.
14. Add missing stable names for canvas workspace controls.
15. Add missing stable names for notebook controls.
16. Add missing stable names for AV (database) controls.
17. Add selector uniqueness checker across registered surfaces.
18. Add selector lint rule to prevent dynamic-label selectors.
19. Add selector contract snapshot file in version control.
20. Add unit test validating required selector presence.
21. Add unit test validating no duplicate selectors.
22. Add unit test validating reserved selector prefix rules.
23. Add deterministic startup flag `MARKAMP_E2E=1` behavior checks.
24. Disable first-run wizard in E2E mode.
25. Disable onboarding tours/tooltips in E2E mode.
26. Disable non-deterministic notifications in E2E mode.
27. Enforce deterministic theme in E2E mode.
28. Enforce deterministic window size and position in E2E mode.
29. Enforce deterministic default workspace in E2E mode.
30. Enforce deterministic autosave cadence in E2E mode.
31. Add isolated user config directory for E2E sessions.
32. Add isolated cache directory for E2E sessions.
33. Add isolated extension storage for E2E sessions.
34. Add fixture copy utility for per-test temporary workspaces.
35. Add fixture reset utility for failed-run cleanup.
36. Add deterministic clock abstraction for time-sensitive flows.
37. Add deterministic random-seed support for UI behavior.
38. Add deterministic network stubs for optional online features.
39. Add Appium capability profiles for local vs CI runs.
40. Add capability validation script with fail-fast diagnostics.
41. Add preflight script for macOS accessibility permission checks.
42. Add preflight script for required host tools and versions.
43. Add session bootstrap checks for app binary existence.
44. Add session teardown checks to kill stale app instances.
45. Add screenshot baseline directories by platform + suite.
46. Add synthetic test fixture content for editor workflows.
47. Add synthetic test fixture content for canvas workflows.
48. Add synthetic test fixture content for graph/notebook workflows.
49. Validate deterministic runtime via repeated-run consistency test.
50. Freeze deterministic runtime contract for v15 phase handoff.
