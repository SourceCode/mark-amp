# Phase 01 -- Program Discovery And UI Baseline Audit

## Objective
Build a complete, evidence-driven baseline for current UI quality, icon usage, and syntax-highlighting behavior.

## Context
This phase is part of the v16 UI/UX overhaul focused on replacing all icons with a VS Code Material Icon Theme-style system and advancing syntax highlighting/theme quality across macOS, Windows, and Linux.

## Definitions
- Canonical Icon ID: Stable string key used in code (example: icon.file.ts).
- Icon Manifest: Versioned metadata file mapping file/folder/context keys to icon assets.
- Scope Token: TextMate-like token scope such as source.ts or keyword.control used for syntax theme mapping.
- Language Pack: Bundle containing grammar, injection rules, and language metadata.
- Quality Gate: A test or metric threshold that must pass before merge or release.

## Reference Files (Full Paths)
- /Users/ryanrentfro/code/markamp/docs/v14_docs/Phase_01__Appium_Mac2_Strategy_And_Baseline.md
- /Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp
- /Users/ryanrentfro/code/markamp/src/ui/IconLibrary.cpp
- /Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp
- /Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp

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
1. Audit current icon inventory across shell surfaces implementation and document gaps against v16 targets.
2. Define contract/schema for icon inventory across shell surfaces with migration notes and edge cases.
3. Implement icon inventory across shell surfaces core refactor with backward-compatible adapter paths.
4. Add automated tests for icon inventory across shell surfaces including failure-path coverage.
5. Publish icon inventory across shell surfaces documentation and phase acceptance checklist.
6. Audit current file/folder icon coverage matrix implementation and document gaps against v16 targets.
7. Define contract/schema for file/folder icon coverage matrix with migration notes and edge cases.
8. Implement file/folder icon coverage matrix core refactor with backward-compatible adapter paths.
9. Add automated tests for file/folder icon coverage matrix including failure-path coverage.
10. Publish file/folder icon coverage matrix documentation and phase acceptance checklist.
11. Audit current command and toolbar icon quality audit implementation and document gaps against v16 targets.
12. Define contract/schema for command and toolbar icon quality audit with migration notes and edge cases.
13. Implement command and toolbar icon quality audit core refactor with backward-compatible adapter paths.
14. Add automated tests for command and toolbar icon quality audit including failure-path coverage.
15. Publish command and toolbar icon quality audit documentation and phase acceptance checklist.
16. Audit current syntax token class completeness audit implementation and document gaps against v16 targets.
17. Define contract/schema for syntax token class completeness audit with migration notes and edge cases.
18. Implement syntax token class completeness audit core refactor with backward-compatible adapter paths.
19. Add automated tests for syntax token class completeness audit including failure-path coverage.
20. Publish syntax token class completeness audit documentation and phase acceptance checklist.
21. Audit current theme-token consumption audit implementation and document gaps against v16 targets.
22. Define contract/schema for theme-token consumption audit with migration notes and edge cases.
23. Implement theme-token consumption audit core refactor with backward-compatible adapter paths.
24. Add automated tests for theme-token consumption audit including failure-path coverage.
25. Publish theme-token consumption audit documentation and phase acceptance checklist.
26. Audit current platform rendering parity baseline implementation and document gaps against v16 targets.
27. Define contract/schema for platform rendering parity baseline with migration notes and edge cases.
28. Implement platform rendering parity baseline core refactor with backward-compatible adapter paths.
29. Add automated tests for platform rendering parity baseline including failure-path coverage.
30. Publish platform rendering parity baseline documentation and phase acceptance checklist.
31. Audit current accessibility contrast and focus audit implementation and document gaps against v16 targets.
32. Define contract/schema for accessibility contrast and focus audit with migration notes and edge cases.
33. Implement accessibility contrast and focus audit core refactor with backward-compatible adapter paths.
34. Add automated tests for accessibility contrast and focus audit including failure-path coverage.
35. Publish accessibility contrast and focus audit documentation and phase acceptance checklist.
36. Audit current test coverage baseline for UI visuals implementation and document gaps against v16 targets.
37. Define contract/schema for test coverage baseline for UI visuals with migration notes and edge cases.
38. Implement test coverage baseline for UI visuals core refactor with backward-compatible adapter paths.
39. Add automated tests for test coverage baseline for UI visuals including failure-path coverage.
40. Publish test coverage baseline for UI visuals documentation and phase acceptance checklist.
41. Audit current performance baseline for icon/theme rendering implementation and document gaps against v16 targets.
42. Define contract/schema for performance baseline for icon/theme rendering with migration notes and edge cases.
43. Implement performance baseline for icon/theme rendering core refactor with backward-compatible adapter paths.
44. Add automated tests for performance baseline for icon/theme rendering including failure-path coverage.
45. Publish performance baseline for icon/theme rendering documentation and phase acceptance checklist.
46. Audit current risk register and dependency map implementation and document gaps against v16 targets.
47. Define contract/schema for risk register and dependency map with migration notes and edge cases.
48. Implement risk register and dependency map core refactor with backward-compatible adapter paths.
49. Add automated tests for risk register and dependency map including failure-path coverage.
50. Publish risk register and dependency map documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
