# Phase 06 -- Navigation Activity And Sidebar Icon Overhaul

## Objective
Replace activity/navigation/sidebar icons with coherent Material-style assets and interaction states.

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
1. Audit current activity bar icon set replacement implementation and document gaps against v16 targets.
2. Define contract/schema for activity bar icon set replacement with migration notes and edge cases.
3. Implement activity bar icon set replacement core refactor with backward-compatible adapter paths.
4. Add automated tests for activity bar icon set replacement including failure-path coverage.
5. Publish activity bar icon set replacement documentation and phase acceptance checklist.
6. Audit current sidebar header icon set replacement implementation and document gaps against v16 targets.
7. Define contract/schema for sidebar header icon set replacement with migration notes and edge cases.
8. Implement sidebar header icon set replacement core refactor with backward-compatible adapter paths.
9. Add automated tests for sidebar header icon set replacement including failure-path coverage.
10. Publish sidebar header icon set replacement documentation and phase acceptance checklist.
11. Audit current panel header icon set replacement implementation and document gaps against v16 targets.
12. Define contract/schema for panel header icon set replacement with migration notes and edge cases.
13. Implement panel header icon set replacement core refactor with backward-compatible adapter paths.
14. Add automated tests for panel header icon set replacement including failure-path coverage.
15. Publish panel header icon set replacement documentation and phase acceptance checklist.
16. Audit current workspace navigation icon coherence implementation and document gaps against v16 targets.
17. Define contract/schema for workspace navigation icon coherence with migration notes and edge cases.
18. Implement workspace navigation icon coherence core refactor with backward-compatible adapter paths.
19. Add automated tests for workspace navigation icon coherence including failure-path coverage.
20. Publish workspace navigation icon coherence documentation and phase acceptance checklist.
21. Audit current hover/active/disabled icon states implementation and document gaps against v16 targets.
22. Define contract/schema for hover/active/disabled icon states with migration notes and edge cases.
23. Implement hover/active/disabled icon states core refactor with backward-compatible adapter paths.
24. Add automated tests for hover/active/disabled icon states including failure-path coverage.
25. Publish hover/active/disabled icon states documentation and phase acceptance checklist.
26. Audit current selected/unselected icon contrast implementation and document gaps against v16 targets.
27. Define contract/schema for selected/unselected icon contrast with migration notes and edge cases.
28. Implement selected/unselected icon contrast core refactor with backward-compatible adapter paths.
29. Add automated tests for selected/unselected icon contrast including failure-path coverage.
30. Publish selected/unselected icon contrast documentation and phase acceptance checklist.
31. Audit current badge overlay and icon composition implementation and document gaps against v16 targets.
32. Define contract/schema for badge overlay and icon composition with migration notes and edge cases.
33. Implement badge overlay and icon composition core refactor with backward-compatible adapter paths.
34. Add automated tests for badge overlay and icon composition including failure-path coverage.
35. Publish badge overlay and icon composition documentation and phase acceptance checklist.
36. Audit current keyboard-focus visual indicators on icons implementation and document gaps against v16 targets.
37. Define contract/schema for keyboard-focus visual indicators on icons with migration notes and edge cases.
38. Implement keyboard-focus visual indicators on icons core refactor with backward-compatible adapter paths.
39. Add automated tests for keyboard-focus visual indicators on icons including failure-path coverage.
40. Publish keyboard-focus visual indicators on icons documentation and phase acceptance checklist.
41. Audit current tooltips and accessible naming for icons implementation and document gaps against v16 targets.
42. Define contract/schema for tooltips and accessible naming for icons with migration notes and edge cases.
43. Implement tooltips and accessible naming for icons core refactor with backward-compatible adapter paths.
44. Add automated tests for tooltips and accessible naming for icons including failure-path coverage.
45. Publish tooltips and accessible naming for icons documentation and phase acceptance checklist.
46. Audit current platform-specific spacing and alignment implementation and document gaps against v16 targets.
47. Define contract/schema for platform-specific spacing and alignment with migration notes and edge cases.
48. Implement platform-specific spacing and alignment core refactor with backward-compatible adapter paths.
49. Add automated tests for platform-specific spacing and alignment including failure-path coverage.
50. Publish platform-specific spacing and alignment documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
