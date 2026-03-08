# Phase 08 -- Domain Surface Icon Completion

## Objective
Complete icon migration for all domain surfaces: canvas, graph, notebooks, AV, git, tasks, AI, extensions, presentations.

## Context
This phase is part of the v16 UI/UX overhaul program focused on replacing all icons with a VS Code Material Icon Theme-style system and elevating syntax highlighting/theme quality across macOS, Windows, and Linux.

## Definitions
- **Canonical Icon ID:** Stable string key used in code (for example, ).
- **Icon Manifest:** Versioned metadata file mapping file/folder/context keys to icon assets.
- **Scope Token:** TextMate-like token scope such as  or  used for syntax theme mapping.
- **Language Pack:** Bundle containing grammar, injection rules, and language metadata.
- **Quality Gate:** A test or metric threshold that must pass before merge/release.

## Reference Files (Full Paths)
- \
- \
- \
- \
- \

## Implementation Details For AI Coding Agents
- Preserve backward compatibility by introducing adapter layers before removing legacy icon IDs or syntax classes.
- When replacing icons, prefer manifest-driven lookup over inline SVG literals or emoji.
- Add deterministic tests for mapping completeness, missing asset handling, and fallback behavior.
- Keep all newly introduced color/styling values theme-tokenized; avoid hardcoded colors.
- For cross-platform work, explicitly test font fallback and rendering parity on macOS/Windows/Linux runners.

## Example
\: command icon resolves to emoji from \.
\: command icon resolves via canonical ID -> manifest entry -> SVG asset -> themed render pipeline.

## Tasks
1. Audit current canvas tool and object icon migration implementation and document gaps against v16 targets.
2. Define contract/schema for canvas tool and object icon migration with migration notes and edge cases.
3. Implement canvas tool and object icon migration core refactor with backward-compatible adapter paths.
4. Add automated tests for canvas tool and object icon migration including failure-path coverage.
5. Publish canvas tool and object icon migration documentation and phase acceptance checklist.
6. Audit current graph controls and node-type icon migration implementation and document gaps against v16 targets.
7. Define contract/schema for graph controls and node-type icon migration with migration notes and edge cases.
8. Implement graph controls and node-type icon migration core refactor with backward-compatible adapter paths.
9. Add automated tests for graph controls and node-type icon migration including failure-path coverage.
10. Publish graph controls and node-type icon migration documentation and phase acceptance checklist.
11. Audit current notebook and cell-action icon migration implementation and document gaps against v16 targets.
12. Define contract/schema for notebook and cell-action icon migration with migration notes and edge cases.
13. Implement notebook and cell-action icon migration core refactor with backward-compatible adapter paths.
14. Add automated tests for notebook and cell-action icon migration including failure-path coverage.
15. Publish notebook and cell-action icon migration documentation and phase acceptance checklist.
16. Audit current AV database view icon migration implementation and document gaps against v16 targets.
17. Define contract/schema for AV database view icon migration with migration notes and edge cases.
18. Implement AV database view icon migration core refactor with backward-compatible adapter paths.
19. Add automated tests for AV database view icon migration including failure-path coverage.
20. Publish AV database view icon migration documentation and phase acceptance checklist.
21. Audit current source control and git state icon migration implementation and document gaps against v16 targets.
22. Define contract/schema for source control and git state icon migration with migration notes and edge cases.
23. Implement source control and git state icon migration core refactor with backward-compatible adapter paths.
24. Add automated tests for source control and git state icon migration including failure-path coverage.
25. Publish source control and git state icon migration documentation and phase acceptance checklist.
26. Audit current task and calendar icon migration implementation and document gaps against v16 targets.
27. Define contract/schema for task and calendar icon migration with migration notes and edge cases.
28. Implement task and calendar icon migration core refactor with backward-compatible adapter paths.
29. Add automated tests for task and calendar icon migration including failure-path coverage.
30. Publish task and calendar icon migration documentation and phase acceptance checklist.
31. Audit current AI panel and action icon migration implementation and document gaps against v16 targets.
32. Define contract/schema for AI panel and action icon migration with migration notes and edge cases.
33. Implement AI panel and action icon migration core refactor with backward-compatible adapter paths.
34. Add automated tests for AI panel and action icon migration including failure-path coverage.
35. Publish AI panel and action icon migration documentation and phase acceptance checklist.
36. Audit current extension browser and lifecycle icon migration implementation and document gaps against v16 targets.
37. Define contract/schema for extension browser and lifecycle icon migration with migration notes and edge cases.
38. Implement extension browser and lifecycle icon migration core refactor with backward-compatible adapter paths.
39. Add automated tests for extension browser and lifecycle icon migration including failure-path coverage.
40. Publish extension browser and lifecycle icon migration documentation and phase acceptance checklist.
41. Audit current presentation/slide icon migration implementation and document gaps against v16 targets.
42. Define contract/schema for presentation/slide icon migration with migration notes and edge cases.
43. Implement presentation/slide icon migration core refactor with backward-compatible adapter paths.
44. Add automated tests for presentation/slide icon migration including failure-path coverage.
45. Publish presentation/slide icon migration documentation and phase acceptance checklist.
46. Audit current search/find/replace icon migration implementation and document gaps against v16 targets.
47. Define contract/schema for search/find/replace icon migration with migration notes and edge cases.
48. Implement search/find/replace icon migration core refactor with backward-compatible adapter paths.
49. Add automated tests for search/find/replace icon migration including failure-path coverage.
50. Publish search/find/replace icon migration documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
