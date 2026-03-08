# Phase 13 -- Language Pack Core And Web Coverage

## Objective
Add advanced highlighting for core and web languages with practical parity targets.

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
1. Audit current c and c++ highlighting depth implementation and document gaps against v16 targets.
2. Define contract/schema for c and c++ highlighting depth with migration notes and edge cases.
3. Implement c and c++ highlighting depth core refactor with backward-compatible adapter paths.
4. Add automated tests for c and c++ highlighting depth including failure-path coverage.
5. Publish c and c++ highlighting depth documentation and phase acceptance checklist.
6. Audit current csharp and java highlighting depth implementation and document gaps against v16 targets.
7. Define contract/schema for csharp and java highlighting depth with migration notes and edge cases.
8. Implement csharp and java highlighting depth core refactor with backward-compatible adapter paths.
9. Add automated tests for csharp and java highlighting depth including failure-path coverage.
10. Publish csharp and java highlighting depth documentation and phase acceptance checklist.
11. Audit current javascript and typescript highlighting depth implementation and document gaps against v16 targets.
12. Define contract/schema for javascript and typescript highlighting depth with migration notes and edge cases.
13. Implement javascript and typescript highlighting depth core refactor with backward-compatible adapter paths.
14. Add automated tests for javascript and typescript highlighting depth including failure-path coverage.
15. Publish javascript and typescript highlighting depth documentation and phase acceptance checklist.
16. Audit current jsx and tsx highlighting depth implementation and document gaps against v16 targets.
17. Define contract/schema for jsx and tsx highlighting depth with migration notes and edge cases.
18. Implement jsx and tsx highlighting depth core refactor with backward-compatible adapter paths.
19. Add automated tests for jsx and tsx highlighting depth including failure-path coverage.
20. Publish jsx and tsx highlighting depth documentation and phase acceptance checklist.
21. Audit current html and xml highlighting depth implementation and document gaps against v16 targets.
22. Define contract/schema for html and xml highlighting depth with migration notes and edge cases.
23. Implement html and xml highlighting depth core refactor with backward-compatible adapter paths.
24. Add automated tests for html and xml highlighting depth including failure-path coverage.
25. Publish html and xml highlighting depth documentation and phase acceptance checklist.
26. Audit current css scss and sass highlighting depth implementation and document gaps against v16 targets.
27. Define contract/schema for css scss and sass highlighting depth with migration notes and edge cases.
28. Implement css scss and sass highlighting depth core refactor with backward-compatible adapter paths.
29. Add automated tests for css scss and sass highlighting depth including failure-path coverage.
30. Publish css scss and sass highlighting depth documentation and phase acceptance checklist.
31. Audit current json jsonc highlighting depth implementation and document gaps against v16 targets.
32. Define contract/schema for json jsonc highlighting depth with migration notes and edge cases.
33. Implement json jsonc highlighting depth core refactor with backward-compatible adapter paths.
34. Add automated tests for json jsonc highlighting depth including failure-path coverage.
35. Publish json jsonc highlighting depth documentation and phase acceptance checklist.
36. Audit current yaml toml ini highlighting depth implementation and document gaps against v16 targets.
37. Define contract/schema for yaml toml ini highlighting depth with migration notes and edge cases.
38. Implement yaml toml ini highlighting depth core refactor with backward-compatible adapter paths.
39. Add automated tests for yaml toml ini highlighting depth including failure-path coverage.
40. Publish yaml toml ini highlighting depth documentation and phase acceptance checklist.
41. Audit current markdown fenced block language detection implementation and document gaps against v16 targets.
42. Define contract/schema for markdown fenced block language detection with migration notes and edge cases.
43. Implement markdown fenced block language detection core refactor with backward-compatible adapter paths.
44. Add automated tests for markdown fenced block language detection including failure-path coverage.
45. Publish markdown fenced block language detection documentation and phase acceptance checklist.
46. Audit current diff and patch highlighting support implementation and document gaps against v16 targets.
47. Define contract/schema for diff and patch highlighting support with migration notes and edge cases.
48. Implement diff and patch highlighting support core refactor with backward-compatible adapter paths.
49. Add automated tests for diff and patch highlighting support including failure-path coverage.
50. Publish diff and patch highlighting support documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
