# Phase 07 -- Toolbar Panel Status And Command Icons

## Objective
Overhaul toolbar, panel, status bar, and command palette icons to a unified high-quality system.

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
1. Audit current toolbar icon replacement and spacing implementation and document gaps against v16 targets.
2. Define contract/schema for toolbar icon replacement and spacing with migration notes and edge cases.
3. Implement toolbar icon replacement and spacing core refactor with backward-compatible adapter paths.
4. Add automated tests for toolbar icon replacement and spacing including failure-path coverage.
5. Publish toolbar icon replacement and spacing documentation and phase acceptance checklist.
6. Audit current panel tab icon replacement implementation and document gaps against v16 targets.
7. Define contract/schema for panel tab icon replacement with migration notes and edge cases.
8. Implement panel tab icon replacement core refactor with backward-compatible adapter paths.
9. Add automated tests for panel tab icon replacement including failure-path coverage.
10. Publish panel tab icon replacement documentation and phase acceptance checklist.
11. Audit current status bar icon replacement implementation and document gaps against v16 targets.
12. Define contract/schema for status bar icon replacement with migration notes and edge cases.
13. Implement status bar icon replacement core refactor with backward-compatible adapter paths.
14. Add automated tests for status bar icon replacement including failure-path coverage.
15. Publish status bar icon replacement documentation and phase acceptance checklist.
16. Audit current command palette icon replacement implementation and document gaps against v16 targets.
17. Define contract/schema for command palette icon replacement with migration notes and edge cases.
18. Implement command palette icon replacement core refactor with backward-compatible adapter paths.
19. Add automated tests for command palette icon replacement including failure-path coverage.
20. Publish command palette icon replacement documentation and phase acceptance checklist.
21. Audit current command contribution icon id normalization implementation and document gaps against v16 targets.
22. Define contract/schema for command contribution icon id normalization with migration notes and edge cases.
23. Implement command contribution icon id normalization core refactor with backward-compatible adapter paths.
24. Add automated tests for command contribution icon id normalization including failure-path coverage.
25. Publish command contribution icon id normalization documentation and phase acceptance checklist.
26. Audit current context menu icon replacement implementation and document gaps against v16 targets.
27. Define contract/schema for context menu icon replacement with migration notes and edge cases.
28. Implement context menu icon replacement core refactor with backward-compatible adapter paths.
29. Add automated tests for context menu icon replacement including failure-path coverage.
30. Publish context menu icon replacement documentation and phase acceptance checklist.
31. Audit current floating toolbar icon replacement implementation and document gaps against v16 targets.
32. Define contract/schema for floating toolbar icon replacement with migration notes and edge cases.
33. Implement floating toolbar icon replacement core refactor with backward-compatible adapter paths.
34. Add automated tests for floating toolbar icon replacement including failure-path coverage.
35. Publish floating toolbar icon replacement documentation and phase acceptance checklist.
36. Audit current notification/action icon replacement implementation and document gaps against v16 targets.
37. Define contract/schema for notification/action icon replacement with migration notes and edge cases.
38. Implement notification/action icon replacement core refactor with backward-compatible adapter paths.
39. Add automated tests for notification/action icon replacement including failure-path coverage.
40. Publish notification/action icon replacement documentation and phase acceptance checklist.
41. Audit current problems/debug/output icon replacement implementation and document gaps against v16 targets.
42. Define contract/schema for problems/debug/output icon replacement with migration notes and edge cases.
43. Implement problems/debug/output icon replacement core refactor with backward-compatible adapter paths.
44. Add automated tests for problems/debug/output icon replacement including failure-path coverage.
45. Publish problems/debug/output icon replacement documentation and phase acceptance checklist.
46. Audit current fallback glyph elimination implementation and document gaps against v16 targets.
47. Define contract/schema for fallback glyph elimination with migration notes and edge cases.
48. Implement fallback glyph elimination core refactor with backward-compatible adapter paths.
49. Add automated tests for fallback glyph elimination including failure-path coverage.
50. Publish fallback glyph elimination documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
