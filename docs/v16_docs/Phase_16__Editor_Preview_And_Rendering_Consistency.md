# Phase 16 -- Editor Preview And Rendering Consistency

## Objective
Guarantee consistent syntax and style rendering between editor, preview, and exported outputs.

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
1. Audit current editor token rendering parity implementation and document gaps against v16 targets.
2. Define contract/schema for editor token rendering parity with migration notes and edge cases.
3. Implement editor token rendering parity core refactor with backward-compatible adapter paths.
4. Add automated tests for editor token rendering parity including failure-path coverage.
5. Publish editor token rendering parity documentation and phase acceptance checklist.
6. Audit current preview token rendering parity implementation and document gaps against v16 targets.
7. Define contract/schema for preview token rendering parity with migration notes and edge cases.
8. Implement preview token rendering parity core refactor with backward-compatible adapter paths.
9. Add automated tests for preview token rendering parity including failure-path coverage.
10. Publish preview token rendering parity documentation and phase acceptance checklist.
11. Audit current line highlight behavior parity implementation and document gaps against v16 targets.
12. Define contract/schema for line highlight behavior parity with migration notes and edge cases.
13. Implement line highlight behavior parity core refactor with backward-compatible adapter paths.
14. Add automated tests for line highlight behavior parity including failure-path coverage.
15. Publish line highlight behavior parity documentation and phase acceptance checklist.
16. Audit current selection and active-line styling parity implementation and document gaps against v16 targets.
17. Define contract/schema for selection and active-line styling parity with migration notes and edge cases.
18. Implement selection and active-line styling parity core refactor with backward-compatible adapter paths.
19. Add automated tests for selection and active-line styling parity including failure-path coverage.
20. Publish selection and active-line styling parity documentation and phase acceptance checklist.
21. Audit current font ligature and fallback parity implementation and document gaps against v16 targets.
22. Define contract/schema for font ligature and fallback parity with migration notes and edge cases.
23. Implement font ligature and fallback parity core refactor with backward-compatible adapter paths.
24. Add automated tests for font ligature and fallback parity including failure-path coverage.
25. Publish font ligature and fallback parity documentation and phase acceptance checklist.
26. Audit current code block header and language badge parity implementation and document gaps against v16 targets.
27. Define contract/schema for code block header and language badge parity with migration notes and edge cases.
28. Implement code block header and language badge parity core refactor with backward-compatible adapter paths.
29. Add automated tests for code block header and language badge parity including failure-path coverage.
30. Publish code block header and language badge parity documentation and phase acceptance checklist.
31. Audit current copy button and action styling parity implementation and document gaps against v16 targets.
32. Define contract/schema for copy button and action styling parity with migration notes and edge cases.
33. Implement copy button and action styling parity core refactor with backward-compatible adapter paths.
34. Add automated tests for copy button and action styling parity including failure-path coverage.
35. Publish copy button and action styling parity documentation and phase acceptance checklist.
36. Audit current exported html token class parity implementation and document gaps against v16 targets.
37. Define contract/schema for exported html token class parity with migration notes and edge cases.
38. Implement exported html token class parity core refactor with backward-compatible adapter paths.
39. Add automated tests for exported html token class parity including failure-path coverage.
40. Publish exported html token class parity documentation and phase acceptance checklist.
41. Audit current pdf export code style parity implementation and document gaps against v16 targets.
42. Define contract/schema for pdf export code style parity with migration notes and edge cases.
43. Implement pdf export code style parity core refactor with backward-compatible adapter paths.
44. Add automated tests for pdf export code style parity including failure-path coverage.
45. Publish pdf export code style parity documentation and phase acceptance checklist.
46. Audit current print view syntax style parity implementation and document gaps against v16 targets.
47. Define contract/schema for print view syntax style parity with migration notes and edge cases.
48. Implement print view syntax style parity core refactor with backward-compatible adapter paths.
49. Add automated tests for print view syntax style parity including failure-path coverage.
50. Publish print view syntax style parity documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
