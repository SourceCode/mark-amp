# Phase 03 -- Icon Asset Ingestion And Build Pipeline

## Objective
Create deterministic asset ingestion, validation, and packaging pipelines for the new icon set.

## Context
This phase is part of the v16 UI/UX overhaul focused on replacing all icons with a VS Code Material Icon Theme-style system and advancing syntax highlighting/theme quality across macOS, Windows, and Linux.

## Definitions
- Canonical Icon ID: Stable string key used in code (example: icon.file.ts).
- Icon Manifest: Versioned metadata file mapping file/folder/context keys to icon assets.
- Scope Token: TextMate-like token scope such as source.ts or keyword.control used for syntax theme mapping.
- Language Pack: Bundle containing grammar, injection rules, and language metadata.
- Quality Gate: A test or metric threshold that must pass before merge or release.

## Reference Files (Full Paths)
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconRegistry.cpp
- /Users/ryanrentfro/code/markamp/scripts/build_debug.sh
- /Users/ryanrentfro/code/markamp/CMakeLists.txt
- /Users/ryanrentfro/code/markamp/src/CMakeLists.txt

## Implementation Details For AI Coding Agents
- Preserve backward compatibility by introducing adapter layers before removing legacy icon IDs or syntax classes.
- When replacing icons, prefer manifest-driven lookup over inline SVG literals or emoji.
- Add deterministic tests for mapping completeness, missing asset handling, and fallback behavior.
- Keep all newly introduced styling values theme-tokenized; avoid hardcoded colors.
- For cross-platform work, explicitly test font fallback and rendering parity on macOS, Windows, and Linux runners.

## Example
Old: command icon resolves to emoji from /Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp.
New: command icon resolves via canonical ID -> manifest entry -> SVG asset -> themed render pipeline.

## Tasks
1. Audit current raw SVG asset import pipeline implementation and document gaps against v16 targets.
2. Define contract/schema for raw SVG asset import pipeline with migration notes and edge cases.
3. Implement raw SVG asset import pipeline core refactor with backward-compatible adapter paths.
4. Add automated tests for raw SVG asset import pipeline including failure-path coverage.
5. Publish raw SVG asset import pipeline documentation and phase acceptance checklist.
6. Audit current svg sanitization and linting implementation and document gaps against v16 targets.
7. Define contract/schema for svg sanitization and linting with migration notes and edge cases.
8. Implement svg sanitization and linting core refactor with backward-compatible adapter paths.
9. Add automated tests for svg sanitization and linting including failure-path coverage.
10. Publish svg sanitization and linting documentation and phase acceptance checklist.
11. Audit current manifest generation tooling implementation and document gaps against v16 targets.
12. Define contract/schema for manifest generation tooling with migration notes and edge cases.
13. Implement manifest generation tooling core refactor with backward-compatible adapter paths.
14. Add automated tests for manifest generation tooling including failure-path coverage.
15. Publish manifest generation tooling documentation and phase acceptance checklist.
16. Audit current build-time asset packaging implementation and document gaps against v16 targets.
17. Define contract/schema for build-time asset packaging with migration notes and edge cases.
18. Implement build-time asset packaging core refactor with backward-compatible adapter paths.
19. Add automated tests for build-time asset packaging including failure-path coverage.
20. Publish build-time asset packaging documentation and phase acceptance checklist.
21. Audit current runtime asset lookup indexing implementation and document gaps against v16 targets.
22. Define contract/schema for runtime asset lookup indexing with migration notes and edge cases.
23. Implement runtime asset lookup indexing core refactor with backward-compatible adapter paths.
24. Add automated tests for runtime asset lookup indexing including failure-path coverage.
25. Publish runtime asset lookup indexing documentation and phase acceptance checklist.
26. Audit current cache warming for common icon groups implementation and document gaps against v16 targets.
27. Define contract/schema for cache warming for common icon groups with migration notes and edge cases.
28. Implement cache warming for common icon groups core refactor with backward-compatible adapter paths.
29. Add automated tests for cache warming for common icon groups including failure-path coverage.
30. Publish cache warming for common icon groups documentation and phase acceptance checklist.
31. Audit current asset diffing and change detection implementation and document gaps against v16 targets.
32. Define contract/schema for asset diffing and change detection with migration notes and edge cases.
33. Implement asset diffing and change detection core refactor with backward-compatible adapter paths.
34. Add automated tests for asset diffing and change detection including failure-path coverage.
35. Publish asset diffing and change detection documentation and phase acceptance checklist.
36. Audit current CI integrity checks for icon pack implementation and document gaps against v16 targets.
37. Define contract/schema for CI integrity checks for icon pack with migration notes and edge cases.
38. Implement CI integrity checks for icon pack core refactor with backward-compatible adapter paths.
39. Add automated tests for CI integrity checks for icon pack including failure-path coverage.
40. Publish CI integrity checks for icon pack documentation and phase acceptance checklist.
41. Audit current developer tooling for previewing icons implementation and document gaps against v16 targets.
42. Define contract/schema for developer tooling for previewing icons with migration notes and edge cases.
43. Implement developer tooling for previewing icons core refactor with backward-compatible adapter paths.
44. Add automated tests for developer tooling for previewing icons including failure-path coverage.
45. Publish developer tooling for previewing icons documentation and phase acceptance checklist.
46. Audit current rollback strategy for bad icon bundles implementation and document gaps against v16 targets.
47. Define contract/schema for rollback strategy for bad icon bundles with migration notes and edge cases.
48. Implement rollback strategy for bad icon bundles core refactor with backward-compatible adapter paths.
49. Add automated tests for rollback strategy for bad icon bundles including failure-path coverage.
50. Publish rollback strategy for bad icon bundles documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
